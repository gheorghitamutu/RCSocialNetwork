#include "tcpserver.h"

void wrapper(TCPServer* tcpServer);

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        printf("Unexpected arguments:");
        for(int index = 1; index < argc; index++)
        {
            printf("%s\n", argv[index]);
        }
    }

    TCPServer* tcpServer = new TCPServer;

    std::thread* thread_check = new std::thread(wrapper, tcpServer);
    thread_check->detach();

    tcpServer->Run();

    return 0;
}

void wrapper(TCPServer* tcpServer)
{
    tcpServer->CheckClientsTimeout();
}
