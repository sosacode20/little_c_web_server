#ifndef _ROBUST_IO_H
#define _ROBUST_IO_H

#include "common.h"

/*
    Unbuffered functions starts here
*/
#pragma region Unbuffered IO functions

/*
    This function reads n_bytes from the file descriptor fd and stores
    it in the user buffer 'buf' and return -1 if an error ocurred, 0 if
    EOF and other positive amount its interpreted as the amount of
    bytes that has been read.
    ***This is Unbuffered IO**
*/
ssize_t rio_readn(int fd, void *buf, size_t n_bytes);

/*
    This function writes up to 'n_bytes' from user buffer 'buf' to the file descriptor fd.
    It return -1 if an error ocurred and other positive number represents the amount of
    bytes actually writen to the file descriptor
*/
ssize_t rio_writen(int fd, void *buf, size_t n_bytes);

#pragma endregion

#pragma region Buffered IO functions

// #define RIO_BUFSIZE 8192
#define RIO_BUFSIZE 128

/*RIO struct buffer*/
typedef struct rio_t{
    int rio_fd; /*File descriptor of this internal buffer*/
    int rio_count; /*Unreaded bytes in internal buffer*/
    char *rio_bufptr; /*Pointer to the next unread byte*/
    char rio_buf[RIO_BUFSIZE]; /*Internal buffer*/
} rio_t;

/*
    *Initialize a new RIO buffer.
    *This is the first thing you need to call if you are working with rio buffering functions
*/
void init_rio_buf(rio_t * rio_ptr, int fd);

/*
    *This function reads up to n_bytes from the file descriptor `rp` points to.
    *And stores the result in usr_buf
*/
ssize_t rio_read(rio_t *rp, void *usr_buf, size_t n_bytes);

/*
    *This function read the next line that is in the file rp.rio_fd points to.
    *It will read up to n_bytes of the line that is returned and will stores it in usr_buf
*/
ssize_t rio_readline(rio_t *rp, void *usr_buf, size_t n_bytes);

#pragma endregion

#endif