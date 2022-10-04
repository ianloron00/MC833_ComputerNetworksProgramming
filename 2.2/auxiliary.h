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
* To get socket's IP address and port number. 
*/
char* get_info( int sockfd ) {

    struct sockaddr_in cliaddr;
    char buffer[INET_ADDRSTRLEN];
    int len = 100;
    char* ans = malloc( sizeof(char) * len );

    socklen_t clilen = sizeof(cliaddr);
    if (getsockname(sockfd, (struct sockaddr *)&cliaddr, &clilen) == -1) {
        perror("getsockname");
        exit(1);
    }

    else {
        snprintf( ans, len, "IP address: %s; Port number: %d", 
            (char *) inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer) ), 
            ntohs(cliaddr.sin_port)
        );
    }

    return ans;
}

/*
* Print socket's IP address and port number. 
*/
char* get_sock_info( int sockfd, int isServer ) {
    int len = 100;
    char* ans = malloc( sizeof(char)  * len );
    snprintf( ans, len,
        isServer 
        ? "Server => %s"
        : "Client => %s",
        get_info( sockfd )
    );

    return ans;
}

/*
* Print connected peer socket's IP address and port number. 
*/
char* get_peer_info( int sockfd, int isServer ) {
    
    int len = 100;
    char* ans = malloc( sizeof(char)  * len );

    snprintf( ans, len,
        isServer 
        ? "(A client) => %s"
        : "(Server) => %s",
        get_info( sockfd )
    );

    return ans;
}

/*
* To print socket's IP address and port number. 
*/
void print_client_info( int sockfd ) {
    printf("%s\n", get_sock_info( sockfd, 0 ) );
}

/* 
* print server IP and port number 
*/
int print_server_info( int listenfd ) {
    return printf( "%s\n", get_sock_info( listenfd, 1 ) );
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


void print_peer_info( int sockfd, int isServer ) {
    printf( "%s\n", get_peer_info( sockfd, isServer ) ); 
}

void write_info( int sockfd, char* info ) {
    FILE *file;
    file = fopen( "/home/ianloron00/grad/833/MC833/2.2/out/server_info.txt", "a+" );
    
    fprintf( file, "%s\n", info );

    fclose( file );
}

void write_sock_info( int sockfd, int isServer ) {
    write_info( sockfd, get_sock_info( sockfd, isServer ) );
}

void write_peer_info( int sockfd, int isServer ) {
    write_info( sockfd, get_peer_info( sockfd, isServer ) );
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



/*
* -- Client --
*/
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