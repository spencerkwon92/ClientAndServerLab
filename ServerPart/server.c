#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h> // allow us to lunch the thread.
#include <unistd.h>
#include <semaphore.h>

#define SIZEOFBUFFER 3
#define MAX 1000
#define CONTMAX 200
#define SA struct sockaddr

void *handle_request(void *socketfd)
{

    char message[MAX];
    char buffer[MAX];
    char sub[MAX];

    int bytes_read = 0;
    int size = 0;

    int socket = (int)(long)socketfd;

    bytes_read = read(socket, buffer, CONTMAX);
    buffer[bytes_read] = 0;
    printf("From Client (%d): %s\n", bytes_read, buffer);
    char directory[] = "root/";

    strcat(directory, buffer);
    printf("Opening %s\n", directory);

    FILE *fp = fopen(directory, "r");
    if (fp == NULL)
    {
        strcat(message, buffer);
        printf("Error: could not open file %s", directory);
    }

    while (fgets(buffer, CONTMAX, fp))
    {
        printf("%s", buffer);
        strcat(sub, buffer);
        size += sizeof(fp);
    }

    fclose(fp);

    printf("File is %i bytes.\n", size);
    printf("Sending: %i$\n", size);
    printf("Sneding (%i, %i): %s\n", size, size, sub);

    sprintf(message, "File Size: %i\n %s", size, sub);

    write(socketfd, message, strlen(message));
}

// Driver function
int main(int argc, char **argv)
{
    int server_sock, client_sock, len;
    short port;
    struct sockaddr_in server_addr, client_addr;

    if (argc < 2)
    {
        printf("Usage: %s PORT\n", argv[0]);
        exit(0);
    }

    port = (short)atoi(argv[1]);

    printf("Launching Server on Port: %d\n", port);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("Could not create socket\n");
        exit(0);
    }

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if ((bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    {
        printf("Error binding socket\n");
        perror("Error");
        exit(0);
    }

    printf("Socket Bound\n");

    if ((listen(server_sock, 3)) != 0)
    {
        printf("Listen Failed\n");
        exit(0);
    }

    len = sizeof(client_addr);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);

        if (client_sock < 0)
        {
            printf("Error accepting connection");
            exit(0);
        }

        printf("New Connection\n");

        pthread_t thread;
        if (pthread_create(&thread, NULL, &handle_request, (void *)(long)client_sock) != 0)
        {
            perror("ERROR");
        }
    }
}
