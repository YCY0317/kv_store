//#include <errno.h>
#include <unistd.h>
//#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/time.h>
//#include <sys/socket.h>
#include <netinet/in.h>
//#include <pthread.h>

#include "kv_store.h"


int accept_cb(int fd);
int recv_cb(int fd);
int send_cb(int fd);

int epfd = 0;
struct conn_item connlist[1048576] = {0};

struct timeval ytime;
#define TIME_SUB_MS(tv1, tv2) ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

int set_event(int fd, int event, int flag) {
    if(flag) {
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD ,fd, &ev);
    }
    else {
        struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
}

int accept_cb(int fd) {
    struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	
	int clientfd = accept(fd, (struct sockaddr*)&clientaddr, &len);
	if (clientfd < 0) {
		return -1;
	}
    set_event(clientfd, EPOLLIN, 1);

    connlist[clientfd].fd = clientfd;
	memset(connlist[clientfd].rbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].rlen = 0;
	memset(connlist[clientfd].wbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].wlen = 0;
    
    connlist[clientfd].recv_t.recv_callback = recv_cb;
	connlist[clientfd].send_callback = send_cb;

    //test
    if((clientfd % 1000) == 999) {
        struct timeval ytime_cur;
        gettimeofday(&ytime_cur, NULL);
        int time_used = TIME_SUB_MS(ytime_cur, ytime);
        memcpy(&ytime, &ytime_cur, sizeof(struct timeval));
        printf("clientfd : %d, time_used: %d\n", clientfd, time_used);
    }

    return clientfd;
}

int recv_cb(int fd) {
    char *buffer = connlist[fd].rbuffer;
	//int idx = connlist[fd].rlen;

    int count = recv(fd, buffer, BUFFER_LENGTH, 0);
    if(!count) {
        printf("disconnect\n");

		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);		
		close(fd);
		
		return -1;
    }
    connlist[fd].rlen = count;

    //memcpy(connlist[fd].wbuffer, connlist[fd].rbuffer, connlist[fd].rlen);
	//connlist[fd].wlen = connlist[fd].rlen;
	//connlist[fd].rlen -= connlist[fd].rlen;

    kvstore_request(&connlist[fd]);
    connlist[fd].wlen = strlen(connlist[fd].wbuffer);

    set_event(fd, EPOLLOUT, 0);
	return count;
}

int send_cb(int fd) {
    char *buffer = connlist[fd].wbuffer;
	int idx = connlist[fd].wlen;

    int count = send(fd, buffer, idx, 0);
    set_event(fd, EPOLLIN, 0);
    return count;
}

int init_server(unsigned short port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
    if (-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))) {
		perror("bind");
		return -1;
	}

	listen(sockfd, 10);

	return sockfd;
}

int epoll_entry(void) {
    int port_count = 20;
    unsigned short port = 8888;
    int i = 0;

    epfd = epoll_create(1);
    for(i=0; i<port_count; i++) {
        int sockfd = init_server(port + i);
        connlist[sockfd].fd = sockfd;
        connlist[sockfd].recv_t.accept_callback = accept_cb;
        set_event(sockfd, EPOLLIN, 1);
    }

    gettimeofday(&ytime, NULL);

    struct epoll_event events[1024] = {0};

    while(1) {
        int nready = epoll_wait(epfd, events, 1024, -1);
        int i = 0;
        for(i=0; i< nready; i++) {
            int connfd = events[i].data.fd;
            if(events[i].events & EPOLLIN) {
                int count = connlist[connfd].recv_t.recv_callback(connfd);
            }
            else if(events[i].events & EPOLLOUT) {
                int count = connlist[connfd].send_callback(connfd);
            }
        }
    }


    return 0;
}