#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX 2000
#define SA struct sockaddr

int main(int argc, char **argv)
{
    int sockfd, bytes_read;
    short port;
    struct sockaddr_in servaddr;
    char buffer[MAX];

    if (argc < 2)
    {
        printf("Usage: %s PORT\n", argv[0]);
        exit(0);
    }

    port = (short)atoi(argv[1]);

    printf("Enter file to retrieve: ");
    scanf("%s", &buffer);

    printf("Connecting on Localhost at port %d to get file %s.\n", port, buffer);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed.\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        printf("Invalid address, not supported\n");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("Error");
        printf("Connection failed\n");
        exit(0);
    }

    // sprintf(buffer, "I am sending this message to the server.");

    write(sockfd, buffer, strlen(buffer));
    bzero(&buffer, sizeof(buffer));
    bytes_read = read(sockfd, buffer, sizeof(buffer));
    buffer[bytes_read] = 0;

    printf("%s\n", buffer);
}