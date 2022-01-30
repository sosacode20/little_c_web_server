#ifndef _META_H
#define _META_H
/*
    * This code is for handling the metadata info of a file
*/

#include <sys/stat.h>
#include <fcntl.h>
#include "utils.h"

#ifndef NAME_BUFSIZE
#define NAME_BUFSIZE 128
#endif
typedef struct stat stats;

typedef struct file_info
{
    char name[128];
    char file_type[32];
    int size;
    // struct tm *time_modified;
    char time_modified[128];
} file_info;

/*
    *This method return 0 if OK and -1 if some error.
    *The file_name is the route to the file and 'fi' is a pointer
     *to the struct that will be filled
*/
int get_file_info(char *file_name, file_info *fi);

#endif