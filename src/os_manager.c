#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>

void class(int sockfd, char *argv[])
{
    char buffer[1024];
    int n;
    for (;;)
    {
        bzero(buffer, 1024);
        printf("Client: ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n')
            ;
        write(sockfd, buffer, sizeof(buffer));
        bzero(buffer, 1024);
        read(sockfd, buffer, sizeof(buffer));
        printf("Server: %s", buffer);
        if ((strncmp(buffer, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

void chat(int sockfd, char *argv[])
{
    char buffer[1024];
    int n;
    for (;;)
    {
        bzero(buffer, 1024);
        printf("Client: ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n')
            ;
        write(sockfd, buffer, sizeof(buffer));
        bzero(buffer, 1024);
        read(sockfd, buffer, sizeof(buffer));
        printf("Server: %s", buffer);
        if ((strncmp(buffer, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4)
    {
        fprintf(stderr, "usage %s hostname port class/chat\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }

    if (strcmp(argv[3], "class") == 0)
    {
        class(sockfd, argv);
    }
    else if (strcmp(argv[3], "chat") == 0)
    {
        chat(sockfd, argv);
    }
    else
    {
        fprintf(stderr, "Invalid mode. Use 'class' or 'chat'.\n");
    }

    close(sockfd);
    return 0;
}

if ('condition')
{
    /**
     * @param {type} name description
     * @return {type} description
     * @addtogroup {groupName} data
     * @brief {briefDescription}
     */
}
else
{
    for (size_t i = 0; i < count; i++)
    {
        while (static_assert)
        {
            cout << "This is a static assert example." << endl;
            std::cout<< "Static assert passed." << std::endl;
            return 0;
            break;
        }
        
    }
    
}

