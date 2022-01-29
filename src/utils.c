#include "utils.h"

int get_file_name(char *file_name, char *output_name_buffer)
{
    size_t end_index = strlen(file_name) - 1;
    ssize_t start_index = end_index - 1;
    for (; start_index > 0; start_index--)
    {
        if(file_name[start_index] == '/')
        {
            start_index +=1;
            break;
        }
    }
    if(start_index > end_index)
    {
        /*
            Solo ocurrira esto si la direccion termina en algo como: "home/".
            es decir, si termina el string en el caracter '/'... de ahi que lo que abria que hacer aqui es:
            TODO: Soltar una excepcion de que el formato del nombre de archivo es incorrecto
        */
       return -1;
    }
    memcpy(output_name_buffer, file_name + start_index, end_index - start_index + 1);
    return 0;
}