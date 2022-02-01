#ifndef _UTILS_H
#define _UTILS_H
#include "common.h"

/*
Extract the file name from the route in 'file_name'
*/
int get_file_name(char *file_name, char *output_name_buffer);

/*
Write a client error message to the socket
*/
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

/*
Serve static file
*/
void serve_static(int fd, char *filename, int filesize);

/*
Serve directory
*/
void serve_dynamic(int fd, char *filename);

char *read_file(char *filename);

void clear_spaces(char *string, char *newstring);

#endif