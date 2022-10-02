#ifndef WRAPPER
#define WRAPPER

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define MAXOUTPUT 16384

#define LISTENQ 10
#define MAXDATASIZE 100

#endif
/*
 *  ---- CLIENT ----
*/

/*
* To get socket's IP address and port number. 
*/
char* get_client_info( int sockfd ) {

    struct sockaddr_in cliaddr;
    char buffer[INET_ADDRSTRLEN];
    char* ans = malloc( sizeof(char) * 51 );

    socklen_t len = sizeof(cliaddr);
    if (getsockname(sockfd, (struct sockaddr *)&cliaddr, &len) == -1) {
        perror("getsockname");
        exit(1);
    }
    else {
        snprintf( ans, 55, "%s%s%s%d%s\n", 
            "Client => IP address: ",
            (char *) inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer) ), 
            "; Port number: ",
            ntohs(cliaddr.sin_port),
            "\n"
        );
    }

    return ans;
}

/*
* To print socket's IP address and port number. 
*/
void print_client_info( int sockfd ) {
    printf("%s", get_client_info( sockfd ) );
}

/*
* Reads message from server and print it.
*/
char read_msg_once( int sockfd ) {

    ssize_t n;
    char recvline[MAXLINE];

    if ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            exit(1);
        }
        recvline[n] = 0;
    }

    if (n < 0) {
        perror("read error");
        exit(1);
    }

    return (n > 0);
}

/*
* Reads message from server and print it.
*/
void read_msg( int sockfd ) {
    
    ssize_t n = read_msg_once( sockfd );
    
    while ( n > 0 ) {
        n = read_msg_once( sockfd );
    }
}

/* 
* Read message from standard input and write to server 
*/
void write_recv_msg( int sockfd ) {

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




/*
 *  ---- SERVER ----
*/
// void get_server_info( int listenfd, const struct sockaddr *servaddr ) {
//     char buffer[INET_ADDRSTRLEN];

//     socklen_t len = sizeof(servaddr);
//     if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1) {
//         perror("getsockname");
//         exit(1);
//     }
//     else {
//         printf( "Server => IP address: %s, Port number: %d\n", 
//         (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ),
//         ntohs(servaddr.sin_port) );
//     }
// }
/*
* Print connected peer socket's IP address and port number. 
*/
void get_peer_port( int sockfd ) {

    struct sockaddr_in peeraddr;
    char buffer[INET_ADDRSTRLEN];

    socklen_t len = sizeof(peeraddr);
    if (getpeername(sockfd, (struct sockaddr *)&peeraddr, &len) == -1) {
        perror("getpeername");
        exit(1);
    }
    else {
        printf( "(A client) => IP address: %s; Port number: %d\n", 
            (char *) inet_ntop(AF_INET, &peeraddr.sin_addr, buffer, sizeof(buffer) ),
            ntohs(peeraddr.sin_port) );
    }
}

/* 
* Prints time of connection with client and sends its time 
*/
void display_time_connection( int connfd ) {
    char   buf[MAXDATASIZE];
    time_t ticks;

    /* prints the time a connection is established 
    and send to client */
    ticks = time(NULL);
    printf( "Connection established at: %.24s\r\n", ctime(&ticks) );
    snprintf( buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks) );
    Writen(connfd, buf, strlen(buf));
}
