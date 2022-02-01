#ifndef _LIST_H
#define _LIST_H
#include "common.h"

typedef struct list{
    char *buf;
    int length;
    int capacity;
}list;

/*
    This function initialice the internal data of the list called 'lis'
*/
int create_list(list *lis, size_t capacity);

/*
    Add nelements from buf to lis.buf
*/
int add_chars_to_list(list *lis, char *buf, size_t nelem);

/*
    This function release the internal buffer of list
*/
int free_list(list *lis);


#endif