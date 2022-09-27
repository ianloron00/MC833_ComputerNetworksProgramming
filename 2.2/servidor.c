#include "./wrappers.h"

#define LISTENQ 10
#define MAXDATASIZE 100


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

    n = Readline ( sockfd, recvline, MAXLINE );
    if (n) {
        printf("server received message of size = %ld: %s\n", n, recvline);
        memset(recvline, 0, n);
    }

    else if(n == 0) {
		puts("Client disconnected");
		fflush(stdout);
	}
}

/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) {
    char   buf[MAXDATASIZE];
    time_t ticks;


    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));
    Writen(connfd, buf, strlen(buf));

    get_peer_port( connfd );

    read_msg(connfd);

    sleep( 1 );
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t pid;

    char buffer[INET_ADDRSTRLEN];

    listenfd = Socket( AF_INET, SOCK_STREAM, 0 );

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port        = 0;   
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    // Bind( listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr) );

    Listen( listenfd, LISTENQ );  

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
        
        connfd = Accept( listenfd, (struct sockaddr *) NULL, NULL );

        if ( (pid = Fork() ) == 0 ) {
            Close( listenfd );
            doit( connfd );
            Close( connfd );
            exit( 0 );
        }

        Close(connfd);
    }


    return(0);
}
