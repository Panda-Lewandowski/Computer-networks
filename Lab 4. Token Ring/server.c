#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SIZE 256
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS] = { 0 };
int messages = 0;
int token = 1;
int currentClient = 0;
int clientCount = 0;

void obrSoed(unsigned int fd)
{
    struct sockaddr_in client_addr;
    int addrSize = sizeof(client_addr);
    int incom = accept(fd, (struct sockaddr*) &client_addr, (socklen_t*) &addrSize);
    if (incom < 0)
    {
        printf("Can't accept!\n");
        exit(-6);
    }

    printf("\nNew connection: fd = %d; ip = %s:%d.\n", incom, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] == 0)
        {
            clients[i] = incom;
            printf("Managed as client #%d.\n", i);
            break;
        }
}

void obrClient(unsigned int fd, unsigned int client_id)
{
    char msg[SIZE];
    memset(msg, 0, SIZE);
    struct sockaddr_in client_addr;
    int addrSize = sizeof(client_addr);
    int recvSize = recv(fd, msg, SIZE, 0);
	
    if (recvSize == 0)
    {
        getpeername(fd, (struct sockaddr*) &client_addr, (socklen_t*) &addrSize);
        printf("User #%d disconnected %s:%d.\n", client_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		if (currentClient == client_id)
			token = 1;
        close(fd);
		clientCount--;
        clients[client_id] = 0;
    }
    else
    {
		if (!strcmp("Ret_token", msg))
		{
			token = 1;
			printf("Client #%d returned token!\n", client_id);
		}
		else 
		{
			msg[recvSize] = '\0';
			printf("Message from #%d client: %s\n", client_id, msg);
			messages++;
		}
    }
}

int main(int argc, char **argv)
{
	struct sockaddr_in serv_addr;
	char buf[SIZE];
	int sock;
	int bytes;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("Can't create socket!\n");
		return -1;
	}

	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);

	if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Can't bind socket!\n");
		return -3;
	}

	if (listen(sock, 3) < 0)
    {
        printf("Can't set to listen!\n");
        return -4;
    }

	printf("Server is listening and waiting for message...\n");
	while (1)
    {
        fd_set readfds; 
        int max_fd; 
        int active_clients_count;
        FD_ZERO(&readfds);  
        FD_SET(sock, &readfds);
        max_fd = sock;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if (fd > 0)
			{
                FD_SET(fd, &readfds);
			}

            max_fd = (fd > max_fd) ? fd : max_fd;
        }

		if (token == 1 && clientCount > 0)
		{
			if (send(clients[currentClient], "token", strlen("token"), 0) < 0)
			{
				printf("Can't send the message!\n");
				return -7;
			}
			printf("Server sent token to %d\n", currentClient);
			token = 0;
		}

        active_clients_count = select(max_fd+1, &readfds, NULL, NULL, NULL);
        if (active_clients_count < 0 && (errno != EINTR))
        {
            printf("Select error!\n");
            return -5;
        }

        if (FD_ISSET(sock, &readfds))
		{
            obrSoed(sock);
			clientCount++;
		}

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if ((fd > 0) && FD_ISSET(fd, &readfds))
                obrClient(fd, i);
        }

		if (token == 1) 
		{
			currentClient++;
			if (currentClient >= clientCount)
			{
				currentClient = 0;
			}
		}
	}
	
    return 0;
}