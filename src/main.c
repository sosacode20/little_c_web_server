#include "common.h"
#include "meta.h"
#include "robust_io.h"

char WEBSERVER_ROOT[256];

void serve(int fd);

int open_listenfd(int port);

void sigchld_handler(int sig);

int main(int argc, char **argv)
{
    int listenfd, connfd, port;

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
    strcpy(WEBSERVER_ROOT, argv[2]);

    signal(SIGCHLD, sigchld_handler);
    listenfd = open_listenfd(port);

    fprintf(stdout, "Servidor corriendo en el puerto %d\n", port);
    fprintf(stdout, "Raiz del servidor %s\n", WEBSERVER_ROOT);

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
            serve(connfd);
            // write(connfd, str, strlen(str)+1);
            // send_response(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
}

void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    rio_readline(rp, buf, MAXLINE);

    while (strcmp(buf, "\r\n"))
    {
        rio_readline(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}

int is_dir(char *uri)
{

    file_info fi;
    if (get_file_info(uri, &fi) == 0)
    {
        if (strcmp(fi.file_type, "directory") == 0)
        {
            return 1;
        }
    }
    return 0;
}

void serve(int fd)
{
    int is_directory;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE];
    rio_t rio;

    init_rio_buf(&rio, fd);
    rio_readline(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    printf("%s %s %s\n", method, uri, version);

    if (strcasecmp(method, "GET"))
    {
        clienterror(fd, method, "501", "Not Implemented",
                    "This Webserver only implements GET method");
        return;
    }
    read_requesthdrs(&rio);

    strcpy(filename, WEBSERVER_ROOT);
    strcat(filename, uri);
    is_directory = is_dir(filename);

    if (stat(filename, &sbuf) < 0)
    {
        clienterror(fd, filename, "404", "Not found",
                    "Couldn’t find this file");
        return;
    }

    if (!is_directory)
    {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        {
            clienterror(fd, filename, "403", "Forbidden", "Couldn’t read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    else
    {
        serve_dynamic(fd, filename);
    }
}

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