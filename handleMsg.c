#include"master.h"


static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;






void HeartBeat(struct client_node (*listOfClients)[MAXCONNEC],int beatfd);


void getMsg(int fd,struct Packet* msg)
{
    read(fd ,msg ,sizeof(struct Packet));
}

//******************************************************************
void sendMsgtoClients(struct client_node (*listOfClients)[MAXCONNEC],struct Packet* msg,int recvsock)
{
    int i = 0;
    for(i = 0;i < MAXCONNEC;++i)
    {
        if(((*listOfClients)[i].sendsock != 0 ) && ((*listOfClients)[i].sendsock != recvsock))
        {
            pthread_mutex_lock(&mutex[i]);
            send((*listOfClients)[i].sendsock, msg, sizeof(struct Packet), 0);
            pthread_mutex_unlock(&mutex[i]);  
        }
    }
}


//******************************************************************************************
void* handleMsg(void* arg)
{
    struct arg_t* args = (struct arg_t*)arg;
    struct Packet msg;
    getMsg(args->clientfd, &msg);
    sem_post(&sem_connect);
    if(msg.flag == MF_MSG||msg.flag ==MF_ONLINE)
    {
        msg.onlineCount = ONLINECOUNT;
        sendMsgtoClients(args->listOfClients, args->msg, args->clientfd);
    }

    else if(msg.flag == MF_OFFLINE)
    {
        int offlinefd = args->clientfd;
        sendMsgtoClients(args->listOfClients, args->msg, args->clientfd);
        pthread_mutex_lock(&pipe_mutex[PIPESIG]);
        write(args->pipefd,(char*)&offlinefd,sizeof(int));
        pthread_mutex_unlock(&pipe_mutex[PIPESIG]);
    }
    else if(msg.flag == MF_HEART)
    {
        HeartBeat(args->listOfClients, args->clientfd);
    }
    free(args);
    return NULL;
}
