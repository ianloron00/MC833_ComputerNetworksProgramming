#include "./wrappers.h"

/*
* Print socket's IP address and port number. 
*/
void get_port( int sockfd ) {

    struct sockaddr_in cliaddr;
    char buffer[INET_ADDRSTRLEN];

    socklen_t len = sizeof(cliaddr);
    if (getsockname(sockfd, (struct sockaddr *)&cliaddr, &len) == -1) {
        perror("getsockname");
        exit(1);
    }
    else {
        printf( "Client => IP address: %s; Port number: %d\n", 
            (char *) inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer) ), 
            ntohs(cliaddr.sin_port) );
    }
}

/*
* Reads message from server, only once.
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
* Reads message from server.
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

void print_server_info( char** argv ) {

    printf( "(Server) => IP address: %s; Port number: %s\n", 
    (char *) argv[1], 
    (char *) argv[2] );
}

int main(int argc, char **argv) {
    int    sockfd;
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 3) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress> | <Port> ");
        perror(error);
        exit(1);
    }

    sockfd = Socket( AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
    servaddr.sin_port   = htons( (unsigned short int) atoi(argv[2]) );

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
    // Connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) );

    read_msg_once( sockfd );

    get_port( sockfd );

    print_server_info( argv );
    
    for( ; ; )
        write_recv_msg( sockfd );


    exit(0);
}

//popen( <comando> );