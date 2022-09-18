#ifndef FARLLEN
#define FARLLEN

#define AC_BLACK "\x1b[30m"
#define AC_RED "\x1b[31m"
#define AC_GREEN "\x1b[32m"
#define AC_YELLOW "\x1b[33m"
#define AC_BLUE "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN "\x1b[36m"
#define AC_WHITE "\x1b[37m"
#define AC_NORMAL "\x1b[m"
#define VERSION "1.0"
#define LHOST "127.0.0.1"
#define BUFFER 4096

void rwbuffer(int src, int dst);
int createpfconn(char* fwhost, int fwport);
void fwconn(int clisocket, char* fwhost, int fwport);
int openport(int bindport);
void connaccept(int svsocket, char* fwhost, int fwport);

#endif