#include "master.h"


void* handleNewConnect(void* args);
void  offLineConnect(int epollfd,int recvSock,struct client_node (*listOfClients)[MAXCONNEC]);
void* handleMsg(void* arg);
void* Heart(void* arg);

static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;


void cleanListOfClients(struct client_node *listOfClients)
{	
    int i =0;
	for(i = 0; i < MAXCONNEC; i++)
	{		
		listOfClients[i].sendsock = 0;
		listOfClients[i].recvsock = 0;
        listOfClients[i].OffCount = 0;
	}
}
//**************************************************************************
void epollstuff(int epollfd,int socklsn,int pipe[2])
{
    struct epoll_event events[MAXEVENTS];
    struct arg_t Heartargs;
    struct client_node listOfClients[MAXCONNEC];
    cleanListOfClients(listOfClients);
    pthread_t heartThd;
    int  numEvts;  
    int i;
    
    Heartargs.listOfClients = &listOfClients;
    Heartargs.pipefd = pipe[PIPESIG];
    pthread_attr_t a; //线程属性
    pthread_attr_init(&a);  //初始化线程属性
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    pthread_create(&heartThd,&a,Heart,&Heartargs);
    while(1)
    {
        numEvts = epoll_wait(epollfd, events, MAXEVENTS, -1);
        for(i = 0; i < numEvts; i++)
        {
            if(events[i].data.fd==socklsn)
            {
                struct arg_t* args = malloc(sizeof(struct arg_t));
                pthread_t connectThd;
                args->epollfd = epollfd;
                args->listOfClients = &listOfClients;
                args->socklsn = socklsn;
                pthread_attr_t a; //线程属性
                pthread_attr_init(&a);  //初始化线程属性
                pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
                pthread_create(&connectThd,&a,handleNewConnect,args);
                sem_wait(&sem_connect);
            }
            else if(events[i].data.fd==pipe[PIPELSN])
            {        
                int offlinefd;
                pthread_mutex_lock(&pipe_mutex[PIPELSN]);
                read(pipe[PIPELSN] ,&offlinefd ,sizeof(int));
                pthread_mutex_unlock(&pipe_mutex[PIPELSN]);
                offLineConnect(epollfd, offlinefd,&listOfClients);
            }
            else
            {
                struct arg_t* args = malloc(sizeof(struct arg_t));
                pthread_t MSGThd;
                args->epollfd = epollfd;
                args->listOfClients = &listOfClients;
                args->clientfd = events[i].data.fd;
                args->pipefd = pipe[PIPESIG];
                pthread_attr_t a; //线程属性
                pthread_attr_init(&a);  //初始化线程属性
                pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);      //设置线程属性
                pthread_create(&MSGThd,&a,handleMsg,args);
                sem_wait(&sem_msg);
            }            
        }
    }
}

