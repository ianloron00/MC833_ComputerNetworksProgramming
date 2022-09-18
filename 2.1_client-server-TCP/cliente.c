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

/*
* to print chosen IP address and port number. 
*/
void get_port(int sockfd, struct sockaddr_in servaddr) {

    char buffer[INET_ADDRSTRLEN];
    // int    n;
    // char   recvline[MAXLINE + 1];

    // while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
    //     recvline[n] = 0;
    //     if (fputs(recvline, stdout) == EOF) {
    //         perror("fputs error");
    //         exit(1);
    //     }
    // }

    // if (n < 0) {
    //     perror("read error");
    //     exit(1);
    // }

    socklen_t len = sizeof(servaddr);
    if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1) {
        perror("getsockname");
        exit(1);
    }
    else {
        printf( "Client IP address is: %s\n", (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ) );
        printf( "Client Port number is: %d\n", ntohs(servaddr.sin_port) );
    }
}

/* 
* read message from standard input and write to server 
*/
void str_cli( int sockfd ) {

    char sendline[MAXLINE];

    printf( "Write a message: " );
    fgets( sendline, MAXLINE, stdin );
        
    if ( send ( sockfd, sendline, strlen(sendline), 0 ) < 0 ) {
        perror("send failed.");
    }
    else {
        puts("Message sent to the server.\n");
    }
}

int main(int argc, char **argv) {
    int    sockfd;
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;

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

    get_port( sockfd, servaddr );
    
    // for( ; ; )
    str_cli( sockfd );

    exit(0);
}
