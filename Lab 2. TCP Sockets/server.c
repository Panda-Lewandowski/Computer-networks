#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "info.h"

void perror_and_exit(char *s, int exit_code)
{
	perror(s);
	exit(exit_code);
}

int main()
{
    int sock, listener;
    struct sockaddr_in addr, client_addr;
    char buf[MSG_LEN];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
		perror_and_exit("socket()", 1);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		perror_and_exit("bind()", 2);

    listen(listener, 1);
	printf("Server is listening...\n");
    
    while(1)
    {
		socklen_t cli_addr_size = sizeof(client_addr);
        sock = accept(listener, (struct sockaddr*) &client_addr, &cli_addr_size);
        if(sock < 0)
			perror_and_exit("accept()", 3);

		printf("Received packet from %s:%d\n\n", 
						inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while(1)
        {
            bytes_read = recv(sock, buf, MSG_LEN, 0);
            if(bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0);
        }
    
        close(sock);
    }
    
    return 0;
}