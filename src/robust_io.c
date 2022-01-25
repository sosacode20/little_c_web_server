#include "robust_io.h"

ssize_t rio_readn(int fd, void *buf, size_t n_bytes)
{
    char *bufp = buf;
    size_t n_left = n_bytes;
    size_t nread = 0;

    while (n_left > 0)
    {
        if ((nread = read(fd, bufp, n_left)) < 0)
        {
            if (errno == EINTR) //Interrupted by signal handler return
                nread = 0;
            else
                return -1; //Error causado por el metodo read
        }
        if (nread == 0) // EOF
            break;
        n_left -= nread;
        bufp += nread;
    }
    return n_bytes - n_left;
}

ssize_t rio_writen(int fd, void *buf, size_t n_bytes)
{
    int n_left = n_bytes;
    int n_write = 0;
    char *bufp = buf;
    while (n_left > 0)
    {
        if ((n_write = write(fd, bufp, n_left)) < 0)
        {
            if (errno == EINTR) //Interrupted by signal handler return
                n_write = 0;
            else
                return -1; //Some error in writing
        }
        if (n_write == 0) // EOF
            break;
        n_left -= n_write;
        bufp += n_write;
    }
    return n_bytes - n_left;
}

void init_rio_buf(rio_t *rio_ptr, int fd)
{
    if (!rio_ptr | fd < 0)
        return;
    rio_ptr->rio_bufptr = rio_ptr->rio_buf;
    rio_ptr->rio_fd = fd;
    rio_ptr->rio_count = 0;
}

ssize_t rio_read(rio_t *rp, void *usr_buf, size_t n)
{
    size_t bytes_left = n;
    char *usr_buf_ptr = usr_buf;
    while (bytes_left > 0)
    {
        int rio_cnt = 0;
        if ((rio_cnt = rp->rio_count) <= 0)
        {
            rio_cnt = rp->rio_count = read(rp->rio_fd, rp->rio_buf, RIO_BUFSIZE); //Rellenar el buffer
            rp->rio_bufptr = rp->rio_buf;
            if (rio_cnt < 0) //Un error
            {
                perror("Ocurrio un error en la lectura interna de RIO");
                exit(rio_cnt);
            }
            if (rio_cnt == 0) //EOF
                return n - bytes_left;
        }
        //TODO: Hacer un metodo propio de memcpy para no tener que cargar con la libreria 'string.h' entera
        size_t min_len = rio_cnt < bytes_left ? rio_cnt : bytes_left;
        memcpy(usr_buf_ptr, rp->rio_bufptr, min_len);
        bytes_left -= min_len;
        usr_buf_ptr += min_len;
        rp->rio_bufptr += min_len;
        rp->rio_count -= min_len;
    }
    return n - bytes_left;
}

ssize_t rio_readline(rio_t *rp, void *usr_buf, size_t n_bytes)
{
    char *bufptr = usr_buf;
    char c;
    int n;
    ssize_t b_readead;
    for (n = 1; n < n_bytes; n++) //Gonna read n_bytes - 1 because we need to add the \0 character at the end
    {
        if ((b_readead = rio_read(rp, &c, 1)) < 0) //Error
        {
            perror("Some error has happened while reading the line");
            return b_readead;
        }
        else if (b_readead == 0) //EOF
        {
            if (n == 1) //Nothing was read
                return 0;
            //Something was read
            break;
        }
        else if (c == '\n')
            break;
        *bufptr++ = c; //Si el caracter se leyo satisfactoriamente entonces copialo
    }
    *bufptr = '\0';
    return n - 1;
}
