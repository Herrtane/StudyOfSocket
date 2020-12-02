#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 30

void error_handling(char* message);

int main(int argc, char* argv[]){
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	socklen_t addr_size;
	int str_len, i;
	char buf[BUF_SIZE];

	struct epoll_event* ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if (argc!=2){
		printf("Usuage: %s <port>/n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);	// create socket
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));	// addr initialization
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events=EPOLLIN;
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1){

		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
							// ep_events : a buffer storing fds which 
							// 		awake events
		if(event_cnt==-1){
			puts("epoll_wait() error");
			break;
		}

		for(i=0; i<event_cnt; i++){		// we don't have to search all fds. Only 
							// fds in ep_events!

			if(ep_events[i].data.fd==serv_sock){	// let's connect!
				addr_size=sizeof(clnt_addr);
				clnt_sock=accept(serv_sock, 
					(struct sockaddr*) &clnt_addr, &addr_size);
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else{					// let's read message!
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
				if(str_len==0){			// close request!
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					printf("closed client: %d \n", ep_events[i].data.fd);
				}
				else
					write(ep_events[i].data.fd, buf, str_len); // echo
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

