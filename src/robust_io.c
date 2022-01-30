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
    if (n == 0)
        return 0;

    while (rp->rio_count == 0)
    { /* refill if buffer is empty */
        ssize_t rc = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rc < 0)
        {                       /* read() error */
            if (errno == EINTR) /* interrupted by a signal */
                continue;       /* no data was read, try again */
            else
                return -1; /* errno set by read(), give up */
        }
        if (rc == 0) /* EOF */
            return 0;
        rp->rio_bufptr = rp->rio_buf; /* read() success, buffer is filled */
        rp->rio_count = rc;           /* 0 < rc <= sizeof(rp->rio_buf) */
    }

    /* Copy min(n, rp->rio_count) bytes from internal buf to user buf */
    size_t cnt = rp->rio_count; /* 0 < rp->rio_count */
    if (n < cnt)                /* 0 < n */
        cnt = n;
    (void)memcpy(usr_buf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_count -= cnt;

    return cnt;
}

ssize_t rio_readline(rio_t *rp, void *usr_buf, size_t n_bytes)
{
    size_t n;
    char *bufp = usr_buf;

    for (n = 1; n < n_bytes; n++)
    {
        ssize_t rc = rio_read(rp, bufp, 1);
        if (rc < 0)
            return -1; /* errno set by read(), give up */
        if (rc == 0)
        {
            if (n == 1)
                return 0; /* EOF, no data read */
            else
                break; /* EOF, some data was read */
        }
        if (*bufp++ == '\n') /* read() success, 0 < rc <= 1 */
            break;
    }
    *bufp = '\0';

    return n;
}
