#include"master.h"


static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;




void CheckHeart(struct client_node (*listOfClients)[MAXCONNEC],int pipefd)
{
    int i = 0;
    for (i = 0;i < MAXCONNEC;++i)
    {
          if((*listOfClients)[i].OffCount >= 2)
          {
            pthread_mutex_lock(&pipe_mutex[PIPESIG]);
            write(pipefd,(char*)&((*listOfClients)[i].recvsock),sizeof(int));
            pthread_mutex_unlock(&pipe_mutex[PIPESIG]);
          }
    }

}

//*************************************************************************************************
void StopCount(struct client_node (*listOfClients)[MAXCONNEC])
{
    int i = 0;
    for (i = 0;i < MAXCONNEC;++i)
    {
       if(((*listOfClients)[i].sendsock != 0 ) && ((*listOfClients)[i].recvsock != 0))
        {
            pthread_mutex_lock(&mutex[i]);
            (*listOfClients)[i].OffCount += 1;
            pthread_mutex_unlock(&mutex[i]);  
        }
    }
}

//************************************************************************

void HeartBeat(struct client_node (*listOfClients)[MAXCONNEC],int beatfd)
{
    int i = 0;
    for (i = 0;i < MAXCONNEC;++i)
    {
        if((*listOfClients)[i].recvsock == beatfd)
        {
            pthread_mutex_lock(&mutex[i]);
            (*listOfClients)[i].OffCount = 0;
            pthread_mutex_unlock(&mutex[i]); 
            break;
        }
    }
}
//******************************************************************************

void* Heart(void* arg)
{
    struct arg_t* args = (struct arg_t*)arg;
    while(1)
    {
        StopCount(args->listOfClients);
        CheckHeart(args->listOfClients, args->pipefd);
        sleep(30);
    }
    return NULL;
}

