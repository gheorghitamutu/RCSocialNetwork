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

/* portul folosit */

#define PORT 2728

extern int errno;		/* eroarea returnata de unele apeluri */

class TCPServer
{
public:
    TCPServer();
    char * conv_addr (struct sockaddr_in address);
    int sayHello(int fd);
    void Run();
};

#endif // TCPSERVER_H
