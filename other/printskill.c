#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	char* msg ="Hello! This message is made for test. \nGood day to study! \n-------------------------------------- \nProcess Loading ...................... \n-------------------------------------- \n \n \n[Status] \nC Programming ... 100% \nC++ Programming ... 100% \nData Structure ... 100% \nComputer Network ... 100% \nTCP/IP Socket Programming ... 90% \nLinux Basic ... 100% \nWeb Programming (Html, CSS, JS, MySQL, Php) ... 100% \nOperating System ... 30% \n-------------------------------------- \nUpdate Successfully! \nMade by herrtane \n";
	int length = strlen(msg);

	for(int i=0; i<length; i++){
		printf("%c", msg[i]);
		fflush(stdout);
		usleep(40000);
	}
	return 0;
}
