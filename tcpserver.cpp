#include "tcpserver.h"

TCPServer::TCPServer()
{
    this->MariaDB = new DatabaseManagerMySQL;
}

int TCPServer::Run()
{
    /* creare socket */
    if ((socket_descriptor = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server] Eroare la socket().\n");
        return errno;
    }

    /*setam pentru socket optiunea SO_REUSEADDR */
    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* pregatim structurile de date */
    bzero (&sockServer, sizeof (sockServer));

    /* umplem structura folosita de server */
    sockServer.sin_family = AF_INET;
    sockServer.sin_addr.s_addr = htonl (INADDR_ANY);
    sockServer.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (socket_descriptor, (struct sockaddr *) &sockServer, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server] Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin MAX_CONNECTIONS clienti sa se conecteze */
    if (listen (socket_descriptor, MAX_CONNECTIONS) == -1)
    {
        perror ("[server] Eroare la listen().\n");
        return errno;
    }

    /* completam multimea de descriptori de citire */
    FD_ZERO (&active_fd_list);		/* initial, multimea este vida */
    FD_SET (socket_descriptor, &active_fd_list);		/* includem in multime socketul creat */

    tv.tv_sec = 1;		/* se va astepta un timp de 1 sec. */
    tv.tv_usec = 0;

    /* valoarea maxima a descriptorilor folositi */
    count_used_fd = socket_descriptor;

    printf ("[server] Asteptam la portul %d...\n", PORT);
    fflush (stdout);

    /* servim in mod concurent clientii... */
    std::vector<std::thread*> th_array;
    while (1)
    {
        for(auto tread: th_array)
        {
            tread->join();
        }
        th_array.clear();


        /* ajustam multimea descriptorilor activi (efectiv utilizati) */
        bcopy ((char *) &active_fd_list, (char *) &read_fd_list, sizeof (read_fd_list));

        /* apelul select() */
        if (select (count_used_fd+1, &read_fd_list, NULL, NULL, &tv) < 0)
        {
            perror ("[server] Eroare la select().\n");
            return errno;
        }


        /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
        if (FD_ISSET (socket_descriptor, &read_fd_list))
        {
            /* pregatirea structurii client */
            len = sizeof (sockClient);
            bzero (&sockClient, sizeof (sockClient));

            /* a venit un client, acceptam conexiunea */
            fd_client = accept (socket_descriptor, (struct sockaddr *) &sockClient, (socklen_t *)(&len));

            /* eroare la acceptarea conexiunii de la un client */
            if (fd_client < 0)
            {
                perror ("[server] Eroare la accept().\n");
                continue;
            }
            else
            {
                AddClient(fd_client);
            }
            if (count_used_fd < fd_client) /* ajusteaza valoarea maximului */
            {
                count_used_fd = fd_client;
            }
            /* includem in lista de descriptori activi si acest socket */
            FD_SET (fd_client, &active_fd_list);
            printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",fd_client, conv_addr (sockClient));
            fflush (stdout);
        }


        /* vedem daca e pregatit vreun socket client pentru a trimite raspunsul */
        for (index_fd = 0; index_fd <= count_used_fd; index_fd++)	/* parcurgem multimea de descriptori */
        {
            /* este un socket de citire pregatit? */
            if (index_fd != socket_descriptor && FD_ISSET (index_fd, &read_fd_list))
            {
                th_array.emplace_back(new std::thread(wrapper_request, this, index_fd));
            }
        }
    }
}

char* TCPServer::conv_addr (struct sockaddr_in address)
{
    static char str[25];
    char port[7];

    /* adresa IP a clientului */
    strcpy (str, inet_ntoa (address.sin_addr));
    /* portul utilizat de client */
    bzero (port, 7);
    sprintf (port, ":%d", ntohs (address.sin_port));
    strcat (str, port);
    return (str);
}

int TCPServer::ParseClientRequest(int fd)
{
    ClientManager* client = GetClient(fd);
    if(client == NULL)
    {
          printf("No client with this descriptor!\n");
          fflush(stdout);
          return 0;
    }

    char buffer[100];		/* mesajul */
    int bytes;			/* numarul de octeti cititi/scrisi */
    char msgReceived[100];		//mesajul primit de la client
    char msgSent[100]="";        //mesaj de raspuns pentru client

    bytes = read (fd, msgReceived, sizeof (buffer));
    if (bytes < 0)
    {
        perror ("Eroare la read() de la client.\n");
        return 0;
    }
    else if(bytes == 0)
    {
        printf("[server] Client with descriptor %d disconnected!\n", fd);
        Logout(fd);
        close (client->GetDescriptor());		/* inchidem conexiunea cu clientul */
        FD_CLR (client->GetDescriptor(), &active_fd_list);/* scoatem si din multime */

        clients_list.erase(std::remove(clients_list.begin(), clients_list.end(), client), clients_list.end());
        delete client;
        client =  NULL;


        return 0;
    }
    printf ("[server]Mesajul a fost receptionat...%s\n", msgReceived);

    // TO DO:
    // DATABASE ACTION

    /*pregatim mesajul de raspuns */
    bzero(msgSent, 100);

    int action, id;
    memcpy(&action, msgReceived, 1);

    switch(action)
    {
        case 1:
            if(Register(msgReceived))
            {
                strcat(msgSent, "1");
            }
            else
            {
                strcat(msgSent, "-1");
            }
        break;
        case 2:
            id = Login(msgReceived, fd);
            if(id)
            {
                memcpy(msgSent, &id, sizeof(int));
            }
            else
            {
                strcat(msgSent, "-1");
            }
        break;
        case 3:
            if(Logout(fd))
            {
                strcat(msgSent, "1");
            }
            else
            {
                strcat(msgSent, "-1");
            }
        break;
    }

    printf("[server]Trimitem mesajul inapoi...%s\n", msgSent);

    if (write (fd, msgSent, bytes) < 0)
    {
        perror ("[server] Eroare la write() catre client.\n");
        return 0;
    }

    return bytes;
}

void TCPServer::AddClient(int descriptor)
{
    this->clients_list.emplace_back(new ClientManager(descriptor));
}

ClientManager *TCPServer::GetClient(int descriptor)
{
    for(auto client: clients_list)
    {
        if(client->GetDescriptor() == descriptor)
        {
            return client;
        }
    }
    return NULL;
}

bool TCPServer::Register(char *user_credentials_buffer)
{
    char* temp_email = new char[MAX_INPUT];
    memset(temp_email, 0, MAX_INPUT);

    char* temp_pass = new char[MAX_INPUT];
    memset(temp_pass, 0, MAX_INPUT);

    int index = 1; // should eventually change if there are more than 9 types of requests
    int index02 = 0;
    while(user_credentials_buffer[index] != ',')
    {
        temp_email[index02] = user_credentials_buffer[index];
        index02++;
        index++;
    }

    index++;
    index02 = 0;
    while(user_credentials_buffer[index] != '\0')
    {
        temp_pass[index02] = user_credentials_buffer[index];
        index02++;
        index++;
    }

    bool action;
    action = MariaDB->AddUser(QString(temp_email), QString(temp_pass));

    if(action)
    {
        DatabaseManagerSQLite * SQLiteDB = new DatabaseManagerSQLite();
        action = action && SQLiteDB->CreateConnection(MariaDB->GetUserId(QString(temp_email)));
    }


    delete temp_email;
    delete temp_pass;

    return action;
}

int TCPServer::Login(char *user_credentials_buffer, int fd)
{
    char* temp_email = new char[MAX_INPUT];
    memset(temp_email, 0, MAX_INPUT);

    char* temp_pass = new char[MAX_INPUT];
    memset(temp_pass, 0, MAX_INPUT);

    int index = 1; // should eventually change if there are more than 9 types of requests
    int index02 = 0;
    while(user_credentials_buffer[index] != ',')
    {
        temp_email[index02] = user_credentials_buffer[index];
        index02++;
        index++;
    }

    index++;
    index02 = 0;
    while(user_credentials_buffer[index] != '\0')
    {
        temp_pass[index02] = user_credentials_buffer[index];
        index02++;
        index++;
    }

    bool action;
    action = MariaDB->CheckEmailPassword(QString(temp_email), QString(temp_pass));

    if(action)
    {
        action = MariaDB->GetUserId(QString(temp_email));
        MariaDB->ChangeUserStatus(GetClient(fd)->GetId(), true);
    }


    delete temp_email;
    delete temp_pass;

    GetClient(fd)->SetId(action);
    return action;
}

bool TCPServer::Logout(int fd)
{

    GetClient(fd)->SetLogged(false);
    GetClient(fd)->SetId(0);
    GetClient(fd)->SetSQLiteDB(NULL);
    return MariaDB->ChangeUserStatus(GetClient(fd)->GetId(), false);
}

void wrapper_request(class TCPServer *tcpServer, int descriptor)
{
    tcpServer->ParseClientRequest(descriptor);
}
