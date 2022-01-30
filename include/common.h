#ifndef _COMMON_H
#define _COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <wait.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <dirent.h>

#ifndef MAXLINE
#define MAXLINE 4096
#endif

#ifndef MAXBUF
#define MAXBUF 64536
#endif

#endif