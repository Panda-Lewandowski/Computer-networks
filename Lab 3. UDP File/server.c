#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "info.h"

void perror_and_exit(char *s, int sock)
{
    close(sock);
	perror(s);
	exit(1);
}

int write_to_binary_file(const char* filename, const unsigned char* bytes)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
        return -1;

    fwrite(bytes, 1, MAX_FILE_BYTE_SIZE, f);

    fclose(f);
    return 0;
}

int send_size_message(int s, struct sockaddr_in client_addr)
{
    printf("Sending message back!\n");
    if (sendto(s, MAX_FILE_BYTE_SIZE_STR, sizeof(MAX_FILE_BYTE_SIZE_STR), 0, 
                                (const struct sockaddr *) &client_addr, 
                                sizeof(client_addr)) < 0)
        return -1;

    return 0;
}

int main(void)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		perror_and_exit("socket", sock);

    struct sockaddr_in server_addr, client_addr;
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
		perror_and_exit("bind", sock);

    unsigned char msg[MAX_FILE_BYTE_SIZE];
    int clilen = sizeof(client_addr);
    while (1)
    {
        printf("Server is listenning at %d port...\n", SOCK_SERVER_PORT);
        int recv_size;
        if ((recv_size = recvfrom(sock, msg,  MAX_FILE_BYTE_SIZE, 0,
                                        (struct sockaddr*) &client_addr, 
                                        (socklen_t *) &clilen)) == -1)
			perror_and_exit("recvfrom()", sock);

        msg[recv_size] = 0;
        printf("--\nThe message is: \"%s\"\n--\n", msg);

        if (strcmp((const char*) msg, CLIENT_MESSAGE_GIVE_FILE_SIZE) == 0)
        {
            if (send_size_message(sock, client_addr) != 0)
                perror_and_exit("sendto()", sock);
            printf("Server message is sent!\n");
        }
        else
        {
            int ret = write_to_binary_file(OUTPUT_FILENAME, msg);
            if (ret != 0)
                perror_and_exit("fwrite()", sock);

            printf("Server wrote to file \"%s\"\n", OUTPUT_FILENAME);
        }
    }

    printf("Shutting down...\n");
    close(sock); 
    return 0;
}