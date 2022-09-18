#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include "farllen.h"

int createpfconn(char* fwhost, int fwport) {
    int fwsocket;
    struct hostent *forward;
    struct sockaddr_in fwaddr;

    forward = gethostbyname(fwhost);
    bzero((char *) &fwaddr, sizeof(fwaddr));
    fwaddr.sin_family = AF_INET;
    bcopy((char *)forward->h_addr, (char *) &fwaddr.sin_addr.s_addr, forward->h_length);
    fwaddr.sin_port = htons(fwport);
    fwsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fwsocket < 0) {
        printf("%s[-] %sCould not create socket\n", AC_RED, AC_NORMAL);
    }
    if (connect(fwsocket, (struct sockaddr *) &fwaddr, sizeof(fwaddr)) < 0) {
        printf("%s[-] %sCould not connect\n", AC_RED, AC_NORMAL);
    }
    return fwsocket;
}

void rwbuffer(int source, int destiny) {
    char buffer[BUFFER];
    int readbuffer, i, j;

    readbuffer = read(source, buffer, BUFFER);
    while (readbuffer > 0) {
        i = 0;
        while (i < readbuffer) {
            j = write(destiny, buffer + i, readbuffer - i);
            if (j < 0) {
                printf("%s[-] %sError trying to write bytes\n", AC_RED, AC_NORMAL);
            }
            i += j;
        }
        readbuffer = read(source, buffer, BUFFER);
    }
    if (readbuffer < 0) {
        printf("%s[-] %sError trying to read bytes\n", AC_RED, AC_NORMAL);
    }
    shutdown(source, SHUT_RD);
    shutdown(destiny, SHUT_WR);
    close(source);
    close(destiny);
    exit(0);
}

void fwconn(int clisocket, char* fwhost, int fwport) {
    int fwsocket;
    pid_t pid;

    fwsocket = createpfconn(fwhost, fwport);
    pid = fork();
    if (pid < 0) {
        printf("%s[-] %sPID not exists\n", AC_RED, AC_NORMAL);
        exit(1);
    }
    if (pid == 0) {
        rwbuffer(fwsocket, clisocket);
    } else {
        rwbuffer(clisocket, fwsocket);
    }
}

void connaccept(int svsocket, char* fwhost, int fwport) {
    int clisocket;
    pid_t pid;

    clisocket = accept(svsocket, NULL, NULL);
    if (clisocket < 0) {
        printf("%s[-] %sError accepting connection\n", AC_RED, AC_NORMAL);
    }
    pid = fork();
    if (pid < 0) {
        printf("%s[-] %sCould not fork the PID\n", AC_RED, AC_NORMAL);
    }
    if (pid == 0) {
        fwconn(clisocket, fwhost, fwport);
        exit(1);
    }
    close(clisocket);
}

int openport(int bindport) {
    int sockfd, conn, i;
    struct sockaddr_in server;
    int addrlen = sizeof(server);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%s[-] %sCould not create socket\n", AC_RED, AC_NORMAL);
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(LHOST);
    server.sin_family = AF_INET;
    server.sin_port = htons(bindport);
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("%s[-] %sCould not host port\n", AC_RED, AC_NORMAL);
        return 1;
    }
    if (listen(sockfd, 3) < 0) {
        printf("%s[-] %sCould not listen\n", AC_RED, AC_NORMAL);
    }
    conn = accept(sockfd, (struct sockaddr *)&server, (socklen_t*)&addrlen);
    if (conn < 0) {
        printf("%s[-] %sError accepting connection\n", AC_RED, AC_NORMAL);
    }
    return sockfd;
}

void banner() {
    printf(R"EOF(%s
  ______         _ _            
 |  ____|       | | |           
 | |__ __ _ _ __| | | ___ _ __  
 |  __/ _` | '__| | |/ _ \ '_ \ 
 | | | (_| | |  | | |  __/ | | |
 |_|  \__,_|_|  |_|_|\___|_| |_|
                                
   [%sTool developed by MrEmpy%s]
         [%sversion %s%s] 

)EOF", AC_CYAN, AC_NORMAL, AC_CYAN, AC_NORMAL, VERSION, AC_CYAN);
}

void help() {
    printf(R"EOF(%susage: farllen FWHOST FWPORT BINDPORT
    options:
      FWHOST,            forwarded host
      FWPORT,            forwarded port
      BINDPORT           bind port
)EOF", AC_NORMAL);
}

int main(int argc, char** argv) {
    if (argc == 3) {
        help();
        return 1;
    }
    if (argc < 3) {
        help();
        return 1;
    }
    char* fwhost = argv[1];
    int* fwport = atoi(argv[2]);
    int* bindport = atoi(argv[3]);
    int opport;

    banner();
    printf("%s[+] %sPort forwarding started\n", AC_GREEN, AC_NORMAL);
    opport = openport(bindport);
    while (1) {
        connaccept(opport, fwhost, fwport);
    }
}
