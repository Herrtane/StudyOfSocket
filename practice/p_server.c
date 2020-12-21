#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char* msg);

int main(int argc, char* argv[]){

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;

	char msg[BUF_SIZE]; 
	int str_len;

	if(argc!=2){
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);		// create socket
	if(serv_sock == -1)
		error_handling("socket() error!");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));		// initialize serv_addr

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error!");		// bind
								// sockaddr_in : for IPv4
								// sockaddr : for all

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error!");		// wait for connection req

	clnt_addr_size = sizeof(clnt_addr);
	while(1){

		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error!");
		else
			printf("Connected client \n");

		while((str_len = read(clnt_sock, msg, BUF_SIZE)) != 0)
			write(clnt_sock, msg, str_len);

		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char* msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
