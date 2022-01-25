#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>

int open_listenfd(int port)
{
    int listenfd, option_value = 1;
    struct sockaddr_in serveraddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        fprintf(stderr, "unable to create socket of type %d in domain %d\n", SOCK_STREAM, AF_INET);
        exit(1);
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&option_value, sizeof(int)) == -1)
    {
        fprintf(stderr, "Unable to set socket file descriptor option %d at protocol level %d to option value %d", SO_REUSEADDR, SOL_SOCKET, option_value);
        exit(1);
    }

    memset((char *)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (listen(listenfd, 1024) == -1)
    {
        fprintf(stderr, "Unable to start listening on port %d\n", port);
        exit(1);
    }

    return listenfd;
}

void sigchld_handler(int sig)
{
    while (waitpid(-1, 0, WNOHANG) > 0)
    {
    }
}

void send_response(int connfd)
{
    write(connfd, "HTTP/1.1 200 OK\n", 16);
    write(connfd, "Content-length: 70\n", 19);
    write(connfd, "Content-Type: text/html\n\n", 25);
    write(connfd, "<html><body><h1>Bienvenido a nuestro servidor hecho en C</h1></body></html>", 70);
}

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    char webserver_root[256];
    typedef unsigned int socklen_t;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *host_p;
    char *haddrp;
    char str[100];

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s  port webserver-root\n", argv[0]);
        exit(0);
    }

    port = atoi(argv[1]);
    strcpy(webserver_root, argv[2]);

    signal(SIGCHLD, sigchld_handler);
    listenfd = open_listenfd(port);

    fprintf(stdout, "Servidor corriendo en el puerto %d\n", port);
    fprintf(stdout, "Raiz del servidor %s\n", webserver_root);

    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        host_p = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("Connected to %s (%s)\n", host_p->h_name, haddrp);
        if (fork() == 0)
        {
            close(listenfd);
            bzero(str, 100);
            read(connfd, str, 100);
            printf("Echoing back - %s", str);
            // write(connfd, str, strlen(str)+1);
            send_response(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
}