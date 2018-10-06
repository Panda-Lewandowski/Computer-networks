#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "info.h"

void perror_and_exit(char *s, int exit_code)
{
	perror(s);
	exit(exit_code);
}

void create_message(char* buf)
{
	srand (time(NULL));  
	char msg[50*5 + 49];
	int k = 0; 
	for(int i = 0 ; i < 50 ; i++ ) {
		for(int j = 0; j < 5; j++)
		{
			msg[k] = (char) 65+rand()%(90-65);
			k++;
		}
		msg[k] = (char) 32;
		k++;
    }

	strcpy(buf, (const char *) msg);
}


int main()
{
    int sock;
	char message[MSG_LEN];
	char buf[MSG_LEN];
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
		perror_and_exit("socket()", 1);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT); 
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		perror_and_exit("connect()", 2);

	create_message(message);
	printf("The client has this message:\n");
	printf("%s\n\n", message);
	printf("Sending message...\n\n");
    if(send(sock, message, sizeof(message), 0) < 0)
		perror_and_exit("send()", 3);

    if(recv(sock, buf, sizeof(message), 0) < 0)
		perror_and_exit("recv()", 4);

	printf("The server return to client this message:\n");
    printf("%s\n\n", buf);
    close(sock);

    return 0;
}