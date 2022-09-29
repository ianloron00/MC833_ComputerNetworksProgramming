#include "./wrappers.h"

/*
* Print socket's IP address and port number. 
*/
void get_client_info( int sockfd ) {

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

/*
* Print IP and port number of connected server.
*/
void print_server_info( char** argv ) {

    printf( "(Server) => IP address: %s; Port number: %s\n", 
    (char *) argv[1], 
    (char *) argv[2] );
}

void exec_server_commands( int sockfd, struct sockaddr* servaddr ) {
    char input[MAXLINE], *ptr, *comm, outcomm[MAXOUTPUT], output[MAXOUTPUT];
    FILE *file;
    char b[2] = ";";
    Readline( sockfd, input, sizeof( input ) );

    printf( "Input: %s\n", input );

    /* break entry data into commands */
    comm = strtok_r( input, b, &ptr);
    printf( "Commands outputs:\n" );

    while ( comm != NULL && strcmp(comm, "END") ) {
        
        printf("Entered while\n");
        file = popen( comm, "r" );
        if(!file){
            fprintf(stderr, "Could not open pipe for input <%s>.\n", comm );
            return;
        }
        
        /* saves its output */
        // ocsize = fscanf( file, "%s", outcomm );
        while ( fgets( outcomm, MAXLINE, file ) ) {
            strcat( output, outcomm );
            // printf("output =>\n %s\n", outcomm );
            memset( outcomm, 0, MAXLINE );
        }
        // printf( "%s =>\n %s\n--------------\n", comm, outcomm );
        // strcat( output, outcomm );
        // memset( outcomm, 0, MAXLINE );
        printf("output =>\n %s\n", output );

        pclose( file );
        printf("closed\n");
        comm = strtok_r( NULL, b, &ptr );
        printf("next command: %s\n", comm );
    }

    printf( "Outside the while\n" );
    /* send data to server */
    Writen( sockfd, output, sizeof(output) );
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

    get_client_info( sockfd );

    print_server_info( argv );
    
    for( ; ; ) {
        exec_server_commands( sockfd, (struct sockaddr *) &servaddr );
    }


    exit(0);
}

//popen( <comando> );