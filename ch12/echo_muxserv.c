#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(int argc, char* argv[]){
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	struct timeval timeout;
	fd_set reads, cpy_reads;
	int fd_max, fd_num, i;


	socklen_t addr_size;
	int str_len, state;
	char buf[BUF_SIZE];

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

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads); 			// set fd of serv_sock to 1
	fd_max=serv_sock;

	while(1){

		cpy_reads=reads;
		timeout.tv_sec=5;
		timeout.tv_usec=5000;

		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;
		if(fd_num==0)				// if timeout
			continue;
		for(i=0; i<fd_max+1; i++){
			if(FD_ISSET(i, &cpy_reads)){
				if(i==serv_sock){	// let's connect!
					addr_size=sizeof(clnt_addr);
					clnt_sock=accept(serv_sock, 
						(struct sockaddr*) &clnt_addr, &addr_size);
					FD_SET(clnt_sock, &reads);
					if(fd_max<clnt_sock)
						fd_max=clnt_sock;
					printf("connected client: %d \n", clnt_sock);
				}
				else{			// let's read message!
					str_len=read(i, buf, BUF_SIZE);
					if(str_len==0){
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
						write(i, buf, str_len); // echo
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

