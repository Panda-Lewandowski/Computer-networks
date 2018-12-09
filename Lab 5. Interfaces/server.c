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

#define SIZE 4096
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS] = { 0 };
int messages = 0;
int tek_token = 0;
int kol_client = 0;

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
			kol_client++;
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
        close(fd);
        clients[client_id] = 0;
		kol_client--;
    }
    else
    {
        msg[recvSize] = '\0';
        printf("Message from #%d client: \n%s\n", client_id, msg);
		messages++;
    }
	tek_token++;
	if(tek_token >= kol_client)
	{
		tek_token = 0;
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
	printf("To send command input command number and client number\n");
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
		FD_SET(0, &readfds);
        active_clients_count = select(max_fd+2, &readfds, NULL, NULL, NULL)-1;
        if (active_clients_count < 0 && (errno != EINTR))
        {
            printf("Select error!\n");
            return -5;
        }
        if (FD_ISSET(sock, &readfds))
		{
            obrSoed(sock);
		}
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if ((fd > 0) && FD_ISSET(fd, &readfds))
                obrClient(fd, i);
        }
		if (FD_ISSET(0, &readfds))
		{
			read(0, buf, SIZE);
			int com = buf[0] - 48;
			int client_number = buf[2] - 48;
			if(client_number > kol_client || client_number <= 0)
			{
				printf("There is no client with that number :/\n");
			}
			else
			{
				if(com <= 2 && com > 0)
				{
					printf("Tipa sended 1 com\n");
					if (send(clients[client_number-1], buf, 1, 0) < 0)
            		{
                		printf("Can't send the message!\n");
                		return -7;
            		}
            		printf("Server sent command %d to client №%d\n", com, client_number);
				}
				else
				{
					printf("WUT?\n");
				}
			}
            //printf("Printed something, huh?, %d - %d \n", client_number, kol_client);
		}
	}
    return 0;
}