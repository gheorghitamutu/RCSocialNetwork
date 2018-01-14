#include "tcpserver.h"

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
    tcpServer->Run();

    return 0;
}
