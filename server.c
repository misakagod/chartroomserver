#include"master.h"

int initsocket();
int init_epoll(int socklsn, int pipefd);
void epollstuff(int epollfd,int socklsn,int pipe[2]);

static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;


int main(int argc, char **argv)
{
    int socklsn = 0;
    int epollfd = 0;
    int pipe[2] = {0,0};

    init_pipe(pipe);
    socklsn = initsocket();
    epollfd = init_epoll(socklsn,pipe[PIPELSN]);
    epollstuff(epollfd,socklsn,pipe);
}

