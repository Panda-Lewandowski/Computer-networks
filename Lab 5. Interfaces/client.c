#include <stdio.h>		
#include <string.h>		
#include <stdlib.h>		
#include <signal.h>		
#include <sys/socket.h>	
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>

#define SIZE 4096

int main(int argc, char **argv)
{
	srand(time(NULL));
 
    int sock, bytes;
    char buf[SIZE], cmd[4];
    struct sockaddr_in serv_addr;
    struct hostent* host;
   
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Can't create socket!\n");
        return -1;
    }
 
    host = gethostbyname("localhost");
    if (!host)
    {
        printf("Can't get this host by name!\n");
        return -2;
    }
    serv_addr.sin_addr = *((struct in_addr*) host->h_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
 
    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Can't establish connection with the server!\n");
        return -3;
    }
 
    do {
        memset(buf, 0, SIZE);
        bytes = recv(sock, buf, sizeof(buf), 0);
        if (bytes < 0)
        {
            perror("Can't recieve data!\n");
            return -5;
        }
 
        printf("Server sent us %s\n", buf);
 		int com = buf[0]-48;
        if (com == 1)
        {
			printf("Received command to print interfaces.\n");
			system("ifconfig >> t.txt");
			memset(buf, 0, SIZE);
			int fd = open("t.txt", O_RDONLY);
			read(fd, buf, SIZE);
			
			//Посылка интерфейсов
            if (send(sock, buf, strlen(buf), 0) < 0)
            {
                printf("Can't send the message!\n");
                return -4;
            }
 
            printf("Message was sended.\n");
            //sleep(1);
			close(fd);
			system("rm ./t.txt");
        }
        else if (com == 2)
        {
			printf("Received command to print routes.\n");
			system("route >> t.txt");
			memset(buf, 0, SIZE);
			int fd = open("t.txt", O_RDONLY);
			read(fd, buf, SIZE);
			
			//Посылка таблицы маршрутизации

            if (send(sock, buf, strlen(buf), 0) < 0)
            {
                printf("Can't send the message!\n");
                return -4;
            }
 
            printf("Message was sended.\n");
			close(fd);
			system("rm ./t.txt");
			
        }

    } while (1);
 
    return 0;
}