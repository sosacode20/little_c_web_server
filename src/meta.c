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
    strcpy(fi->file_type, file_type);
    // fi->time_modified = (&st.st_mtime);
    // Si se quisiera la fecha ya formateada en string se pudiera usar el siguiente metodo
    char *m_time = ctime(&st.st_mtime);
    if (m_time[strlen(m_time) - 1] == '\n')
        m_time[strlen(m_time) - 1] = '\0';
    strcpy(fi->time_modified, m_time);
    fi->size = st.st_size;
    return 0;
}