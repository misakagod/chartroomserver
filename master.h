#ifndef INC_H
#define INC_H


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>




#define REMOTEPORT  8215
#define LOCALPORT   9215

#define MAXIDLEN    0X20
#define MAXMSGLEN   0x400
#define IPBUFLEN    4 
#define MAXCONNEC   100
#define MAXEVENTS   200

#define MF_ONLINE   0x1
#define MF_OFFLINE  0x2
#define MF_MSG      0X3
#define MF_FULL     0X4
#define MF_HEART    0x5


#define PIPELSN     0
#define PIPESIG     1

struct Packet
{
    int flag;
    int onlineCount;
    char IDBUFF[MAXIDLEN];   
    char msgBUFF[MAXMSGLEN];
};

struct client_node
{
    int sendsock;
    int recvsock;
    int OffCount;
};

struct arg_t
{
    int epollfd;
    int socklsn;
    int clientfd;
    int pipefd;
    struct Packet* msg;
    struct client_node (*listOfClients)[MAXCONNEC];
};



#endif

