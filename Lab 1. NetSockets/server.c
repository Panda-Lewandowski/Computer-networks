#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "info.h"

void perror_and_exit(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in server_addr, client_addr;
	int sock, slen = sizeof(client_addr);
	char buf[MSG_LEN];

	printf("Server started\n");

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		perror_and_exit("socket");

	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, &server_addr, sizeof(server_addr)) == -1)
		perror_and_exit("bind");

	while (1)
	{
		if (recvfrom(sock, buf, MSG_LEN, 0, &client_addr, &slen) == -1)
			perror_and_exit("recvfrom()");
		printf("Received packet from %s:%d\nData: %s\n\n",
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
	}

	close(sock);
	return 0;
}