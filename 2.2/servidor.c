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
// https://www.gta.ufrj.br/ensino/eel878/sockets/inet_ntoaman.html
// to print properly IP and port numbers
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

#define MAXLINE 4096

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
        printf( "Server peer IP address is: %s\n", 
            (char *) inet_ntop(AF_INET, &peeraddr.sin_addr, buffer, sizeof(buffer) ) );
        printf( "Server peer Port number is: %d\n", ntohs(peeraddr.sin_port) );
    }
}

/* 
* Read message from client and write to standard output 
*/
void read_msg( int sockfd ) {
    ssize_t n;
    char recvline[MAXLINE];

    while ( ( n = recv(sockfd, recvline, MAXLINE, 0) ) > 0 ) {
        printf("server received message of size = %ld: %s\n", n, recvline);
        memset(recvline, 0, n);
    }
    
    if(n == 0) {
		puts("Client disconnected");
		fflush(stdout);
	}
    else if (n < 0) {
        perror("read error");
        exit(1);
    }
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    char   buf[MAXDATASIZE];
    time_t ticks;

    char buffer[INET_ADDRSTRLEN];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port        = 0;   
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }
    /*
    * to print chosen IP address and port number. 
    */
    socklen_t len = sizeof(servaddr);
    if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1) {
        perror("getsockname");
        exit(1);
    }
    else {
        printf( "Server IP address is: %s\n", (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ) );
        printf( "Server Port number is: %d\n", ntohs(servaddr.sin_port) );
    }

    for ( ; ; ) {
        
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL ) ) == -1 ) {
            perror("accept");
            exit(1);
        }

        get_peer_port( connfd );

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));

        read_msg(connfd);

        close(connfd);
    }


    return(0);
}
