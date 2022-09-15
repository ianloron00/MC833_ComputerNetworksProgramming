#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
    int    sockfd, n;
    char   recvline[MAXLINE + 1];
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;

    char buffer[INET_ADDRSTRLEN];

    if (argc != 2) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <Port>");
        perror(error);
        exit(1);
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons( (unsigned short int) atoi(argv[1]) );
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            exit(1);
        }
    }

    if (n < 0) {
        perror("read error");
        exit(1);
    }

    /*
    * to print chosen IP address and port number. 
    */
    socklen_t len = sizeof(servaddr);
    if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1) {
        perror("getsockname");
        exit(1);
    }
    else {
        printf( "Client IP address is: %s\n", (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ) );
        printf( "Client Port number is: %d\n", ntohs(servaddr.sin_port) );
    }

    exit(0);
}
