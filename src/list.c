#include "list.h"

int create_list(list *lis, size_t capacity)
{
    if(!lis) return -1;
    capacity = capacity < 512 ? 512 : capacity;
    lis->buf = malloc(capacity * sizeof(char));
    if(!lis->buf)
        return -1;
    lis->capacity = capacity;
    lis->length = 0;
    return 0;
}

int add_chars_to_list(list *lis, char *buf, size_t nelem)
{
    if(!lis || !lis->buf || !buf) return -1;
    long needed_space = nelem - (lis->capacity - (lis->length));
    if(needed_space > 0)
    {
        size_t new_size = lis->capacity + needed_space * 2;
        lis->buf = realloc(lis->buf, new_size);
        lis->capacity = new_size;
        if(!lis->buf)
        {
            fprintf(stderr, "Insuficient RAM space for allocation in the method 'add_char_to_list'\n");
            return -1;
        }
    }
    char *bufptr = lis->buf + lis->length;
    int i = 0;
    for(; i < nelem && buf[i] != '\0'; i++)
        bufptr[i] = buf[i];
    lis->length += i;
    // bufptr[i] = '\0';
    return 0;
}

int free_list(list *lis)
{
    if(!lis) return -1;
    if(!lis->buf)
        return -1;
    free(lis->buf);
    return 0;
}