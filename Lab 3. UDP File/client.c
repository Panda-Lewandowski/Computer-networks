#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "info.h"

void perror_and_exit(char *s, int sock)
{
    close(sock);
	perror(s);
	exit(1);
}

int read_limited_binary_file(const char* filename, int limit, unsigned char* buf)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;


    char byte;
    int i = 0;
    while(fread(&byte, 1, 1, f))
    {
        if (i >= limit)
        {
            free(buf);
            buf = NULL;
            i = -1;
            break;
        }
        buf[i] = byte;
        i++;
    }

    fclose(f);
    return i;
}

int main(void)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		perror_and_exit("socket", sock);

    struct sockaddr_in server_addr;
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_SERVER_PORT);
    if (inet_aton(SOCK_ADDR, (struct in_addr*) &server_addr.sin_addr) == 0)
        perror_and_exit("inet_aton()", sock);

    struct sockaddr_in client_addr;
    memset((char *)&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(SOCK_CLIENT_PORT);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (const struct sockaddr*) &client_addr, sizeof(client_addr)) < 0)
        perror_and_exit("bind()", sock);

    if (sendto(sock, CLIENT_MESSAGE_GIVE_FILE_SIZE, 
                                        strlen(CLIENT_MESSAGE_GIVE_FILE_SIZE), 0, 
                (const struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        perror_and_exit("sendto()", sock);
        
    printf("Client message is sent!\n");
 
    printf("Recieving message\n");
    char msg[strlen(MAX_FILE_BYTE_SIZE_STR)];
    int rec_size = recv(sock, msg, strlen(MAX_FILE_BYTE_SIZE_STR), 0);
    if (rec_size < 0)
        perror_and_exit("recv()", sock);

    msg[rec_size] = 0;
    printf("--\nThe message is: \"%s\"\n--\n", msg);

    char *p;
    long int max_size = strtol(msg, &p, 10);
    unsigned char* ch = calloc(max_size, 1);
    int ch_len = read_limited_binary_file(INPUT_FILENAME, max_size, ch);
    if (!ch_len)
        perror_and_exit("fread()", sock);

    if (ch_len == -1) 
        perror_and_exit("File is too big", sock);
    printf("Client readed file\n");

    int sending_sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in sending_client_addr;
    memset((char *)&sending_client_addr, 0, sizeof(sending_client_addr));
    sending_client_addr.sin_family = AF_INET;
    sending_client_addr.sin_port = htons(SENDING_SOCK_CLIENT_PORT);
    sending_client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sending_sock, (const struct sockaddr*) &sending_client_addr, sizeof(sending_client_addr)) < 0)
        perror_and_exit("bind()", sock);

    printf("Client sending message!\n");
    if (sendto(sending_sock, ch, ch_len, 0, (const struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        close(sock);
        close(sending_sock);
        free(ch);
        perror("sendto()");
        exit(1);
    }

    printf("Client message is sent!\n");

    printf("Shutting down...\n");
    free(ch);
    close(sock);
    close(sending_sock);
    return 0;
}