#include "utils.h"
#include "meta.h"
#include "robust_io.h"

#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"
#define BLUE "\x1b[34m" BOLD
#define GREEN "\x1b[32m" BOLD

int get_file_name(char *file_name, char *output_name_buffer)
{
    size_t end_index = strlen(file_name) - 1;
    ssize_t start_index = end_index - 1;
    for (; start_index > 0; start_index--)
    {
        if (file_name[start_index] == '/')
        {

            start_index += 1;
            break;
        }
    }
    if (start_index > end_index)
    {
        /*
            Solo ocurrira esto si la direccion termina en algo como: "home/".
            es decir, si termina el string en el caracter '/'... de ahi que lo que abria que hacer aqui es:
            TODO: Soltar una excepcion de que el formato del nombre de archivo es incorrecto
        */
        return -1;
    }

    strcpy(output_name_buffer, file_name + start_index);
    return 0;
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor="
                  "ffffff"
                  ">\r\n",
            body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
}

void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, buf[MAXBUF];

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: mime/type\r\n\r\n", buf);
    rio_writen(fd, buf, strlen(buf));

    srcfd = open(filename, O_RDONLY, 0);
    srcp = mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    close(srcfd);
    rio_writen(fd, srcp, filesize);
    munmap(srcp, filesize);
}

void serve_dynamic(int fd, char *filename)
{
    char buf[MAXLINE], body[MAXBUF];

    char *template = read_file("./template.html");
    sprintf(body, "%s", template);
    free(template);
    sprintf(body, "%s<script>start(\"%s\")</script>\r\n", body, filename);
    sprintf(body, "%s<script>onHasParentDirectory();</script>\r\n", body);

    struct dirent *de; // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(filename);

    if (dr == NULL) // opendir returns NULL if couldn't open directory
    {
        clienterror(fd, filename, "404", "Not Found", "No se pudo abrir el directorio");
        return;
    }

    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        if (de->d_name[strlen(de->d_name) - 1] == '.')
        {
            continue;
        }
        file_info fi;
        char file[MAXLINE];
        strcpy(file, filename);
        strcat(file, de->d_name);

        if (get_file_info(file, &fi) == 0)
        {
            int is_dir = 0;
            if (strcmp(fi.file_type, "directory") == 0)
            {
                is_dir = 1;
            }
            sprintf(body, "%s<script>addRow(\"%s\", \"%s\", %d, %d, \"%d B\" , new Date(\"%s\").getTime(), \"%s\");</script>\r\n", body, fi.name, fi.name,
                    is_dir, fi.size, fi.size, fi.time_modified, fi.time_modified);
        }
        else
        {
            printf("Error: %s\n", de->d_name);
        }
    }
    closedir(dr);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", "200", "OK");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
}

char *read_file(char *filename)
{
    FILE *fp;
    char *buffer;
    long length;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "File %s not found\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory error\n");
        exit(2);
    }
    fread(buffer, 1, length, fp);
    fclose(fp);
    return buffer;
}

void clear_spaces(char *string, char *newstring)
{
    int size = strlen(string);
    char *oldstring = malloc(size * sizeof(char));
    char *startp_oldstring = oldstring;
    strcpy(oldstring, string);
    char *search = "%20";
    char *ret;
    while (ret = strstr(oldstring, search)) //Si encontro una coincidencia
    {
        int index = ret - oldstring;
        oldstring[index] = 0;
        strcat(newstring, oldstring);
        strcat(newstring, " ");
        oldstring += index + 3;
    }
    int traveled = oldstring - startp_oldstring + 1;
    if (traveled < size)
    {
        strcat(newstring, oldstring);
    }
    free(startp_oldstring);
}