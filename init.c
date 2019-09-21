#include"master.h"

static pthread_mutex_t mutex[MAXCONNEC];
static pthread_mutex_t pipe_mutex[2];
static pthread_mutex_t OLC;
static int ONLINECOUNT = 0;
static sem_t sem_connect;
static sem_t sem_msg;

void make_ipv4addr(struct sockaddr_in *addr, int port);
void setNonBlocking(int sock);

//****************************************************************
void init_pipe(int (*pipefd)[2])
{
    if(pipe(pipefd) == -1)
    {
        printf("pipe err");
        exit(0);
    }
}
//******************************************************************
int initsocket()
{
    
	int  optval = 1;
	int socklsn = 0;
	struct sockaddr_in serveraddr;	

	make_ipv4addr(&serveraddr, LOCALPORT);

    serveraddr.sin_addr.s_addr = INADDR_ANY;

	socklsn = socket(AF_INET, SOCK_STREAM, 0);

	setsockopt( socklsn, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	

	bind(socklsn, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    
    //setNonBlocking (socklsn);
    
	listen(socklsn, MAXCONNEC);

	return socklsn;
}


int init_epoll(int socklsn, int pipefd)
{
    int epollfd;
    struct epoll_event event;
    
    epollfd  = epoll_create1(0);
    
    event . data.fd = ( socklsn );
	event . events  = EPOLLIN | EPOLLPRI;
    
    epoll_ctl(epollfd, EPOLL_CTL_ADD, socklsn, &event);

    event . data.fd = ( pipefd );
	event . events  = EPOLLIN | EPOLLPRI;
    
    epoll_ctl(epollfd, EPOLL_CTL_ADD, pipefd, &event);

    return epollfd;
}
//**********************************************************************
void init_sem()
{
    sem_init(&sem_connect, 0, 0);
    sem_init(&sem_msg, 0, 0);
}