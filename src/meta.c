#include "meta.h"

int get_file_info(char *file_name, file_info *fi)
{
    stats st;
    get_file_name(file_name, fi->name);
    int res = stat(file_name, &st);
    if (res == -1)
    {
        perror("Ha ocurrido un error al pedir la metadata del archivo");
        return -1;
    }
    char *file_type;
    //Esto es para saber el modo del archivo
    switch (st.st_mode & __S_IFMT)
    {
    case __S_IFSOCK:
        file_type = "socket";
        break;
    case __S_IFREG:
        file_type = "regular";
        break;
    case __S_IFDIR:
        file_type = "directory";
        break;
    default:
        file_type = "other";
        break;
    }
    size_t size = strlen(file_type);
    memcpy(fi->file_type, file_type, size);
    fi->time_modified = (&st.st_mtime);
    //Si se quisiera la fecha ya formateada en string se pudiera usar el siguiente metodo
    // char *m_time = ctime(&st.st_mtime);
    return 0;
}