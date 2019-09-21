#include "master.h"


static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;





void setNonBlocking(int sock)
{
	int flags = 0;
	
	flags = fcntl(sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flags);
}
//**************************************************************************
void make_ipv4addr(struct sockaddr_in *addr, int port)
{
	memset(addr, 0, sizeof(&addr));
	addr -> sin_family = AF_INET;
	addr -> sin_port   = htons(port);
}

//**********************************************************************
int getNewConnection(int epollfd, int socklsn, char* ipbuff)
{	
	socklen_t 				clientaddrLen;
	int 					sockrcv;	
    struct sockaddr_in      clientaddr;
   
	clientaddrLen = (socklen_t)sizeof(clientaddr);
    
	sockrcv = accept(socklsn, (struct sockaddr *)&clientaddr, &clientaddrLen);
	setNonBlocking (sockrcv);
    memcpy(ipbuff,&(clientaddr.sin_addr.s_addr),IPBUFLEN);
	return sockrcv;
}
//**********************************************************************
int setSend(char* ipbuff)
{
	int sockd = 0;
	struct sockaddr_in addr;
    
    make_ipv4addr(&addr, REMOTEPORT);
    memcpy(&(addr.sin_addr.s_addr),ipbuff,IPBUFLEN);
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	
	connect(sockd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	
	return sockd;
}
//************************************************************************
void* handleNewConnect(void* args)
{
    printf("start handle new connection\n");
    struct arg_t* arg = (struct arg_t*)args;
	int i = 0;
    char ipbuff[IPBUFLEN];
    printf("start accapt connect\n");
    int recvsock = getNewConnection(arg->epollfd, arg->socklsn ,ipbuff);
    printf("clientIP:%s",ipbuff);
    sem_post(&sem_connect);
	for(i = 0; i < MAXCONNEC; i++)
	{
		if((*(arg->listOfClients))[i].recvsock == 0)
		{	
            pthread_mutex_lock(&mutex[i]);
            (*(arg->listOfClients))[i].recvsock = recvsock;
                printf("start connect to client\n");
			(*(arg->listOfClients))[i].sendsock = setSend(ipbuff);
            (*(arg->listOfClients))[i].OffCount = 0;
            pthread_mutex_unlock(&mutex[i]);
            struct epoll_event 	event;
            event . data.fd = recvsock;
  	        event . events  = EPOLLIN;
	        epoll_ctl (arg->epollfd, EPOLL_CTL_ADD, recvsock, &event);
            pthread_mutex_lock(&OLC);
            ONLINECOUNT++;
            pthread_mutex_unlock(&OLC);
            break;
		}
	}
    if(i == MAXCONNEC)
    {
        struct Packet full;
        memset(&full,0,sizeof(full));
        full.flag = MF_FULL;
        send(recvsock,&full,sizeof(full),0);
        close(recvsock);
    }
    printf("finish connect\n");
    free(args);
    return NULL;
}
//************************************************************************************************************
void offLineConnect(int epollfd,int recvSock,struct client_node (*listOfClients)[MAXCONNEC])
{
    struct epoll_event event;
	event . data.fd = recvSock;
	event . events  = EPOLLIN;
    int i =0;
    for(i = 0; i < MAXCONNEC; i++)
	{
		if((*listOfClients)[i].recvsock == recvSock)
		{	
            pthread_mutex_lock(&mutex[i]);
            close((*listOfClients)[i].recvsock);
			close((*listOfClients)[i].sendsock);
            (*listOfClients)[i].recvsock = 0;
            (*listOfClients)[i].sendsock = 0;
            (*listOfClients)[i].OffCount = 0;
            pthread_mutex_unlock(&mutex[i]);
            epoll_ctl(epollfd, EPOLL_CTL_DEL, recvSock, &event);
            pthread_mutex_lock(&OLC);
            ONLINECOUNT++;
            pthread_mutex_unlock(&OLC);
            break;
		}
	}
}
