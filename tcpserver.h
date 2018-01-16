#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "databasemanagermysql.h"
#include "databasemanagersqlite.h"
#include "clientmanager.h"

#include <vector>
#include <iterator>
#include <thread>

#define PORT 2728
#define MAX_CONNECTIONS 64

extern int errno;		/* eroarea returnata de unele apeluri */
void wrapper_request(class TCPServer *tcpServer, int descriptor);

class TCPServer
{
public:
    TCPServer();
    int Run();

    //threads functions
    int ParseClientRequest(int index_fd); /* realizeaza primirea si retrimiterea unui mesaj unui client */
private:
    struct sockaddr_in sockServer;	/* structurile pentru server si clienti */
    struct sockaddr_in sockClient;
    fd_set read_fd_list;		/* multimea descriptorilor de citire */
    fd_set active_fd_list;		/* multimea descriptorilor activi */
    struct timeval tv;		/* structura de timp pentru select() */
    int socket_descriptor, fd_client;		/* descriptori de socket */
    int optval=1; 			/* optiune folosita pentru setsockopt()*/
    int index_fd;			/* descriptor folosit pentru  parcurgerea listelor de descriptori */
    int count_used_fd;			/* numarul maxim de descriptori */
    int len;			/* lungimea structurii sockaddr_in */

    //databases utilities functions
    void make_sqlite_user_db(char* email);

    char * conv_addr (struct sockaddr_in address); /* functie de convertire a adresei IP a clientului in sir de caractere */

    // databases
    DatabaseManagerMySQL* MariaDB;

    //clients vector
    std::vector<ClientManager*> clients_list;

    // clients
    void AddClient(int descriptor);
    ClientManager* GetClient(int descriptor);

    // clients requests actions
    bool Register(char* user_credentials_buffer); // returns true if account was succesfully created
    int Login(char* user_credentials_buffer, int fd);   // returns the id of the user in mariadb if login was successfull
    bool Logout(int fd);
};

#endif // TCPSERVER_H
