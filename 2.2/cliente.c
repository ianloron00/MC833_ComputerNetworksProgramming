#include "./wrappers.h"
#include "./auxiliary.h"


/*
* Print IP and port number of connected server.
*/
// void print_server_info( char** argv ) {

//     printf( "(Server) => IP address: %s; Port number: %s\n", 
//     (char *) argv[1], 
//     (char *) argv[2] );
// }

/*
* ex. 5 - print inverted and uppercase input
*/
void print_inverted_input( char* argv, ssize_t n ) {
    printf( "Inverted upper input: " );
    for ( int i = n-1; i >= 0; i-- ) {
        printf( "%c", toupper( argv[i] ) );
    }
    printf("\n\n");
}

void exec_server_commands( int sockfd, struct sockaddr* servaddr ) {
    char input[MAXLINE], *ptr, *comm, outcomm[MAXOUTPUT], output[MAXOUTPUT];
    FILE *file;
    char b[2] = ";";

    Readline( sockfd, input, sizeof( input ) );
    printf( "Input: %s\n", input );
    print_inverted_input( input, sizeof(input) );

    /* initializes message with client info */
    strcat( output, get_sock_info( sockfd, 0 ) );

    /* break entry data into commands */
    comm = strtok_r( input, b, &ptr);

    /* reads and compiles  each instruction
    and saves its output in a buffer */
    while ( comm != NULL && strcmp(comm, "END\n") ) {
        
        file = popen( comm, "r" );
        if(!file){
            fprintf(stderr, "Could not open pipe for input <%s>.\n", comm );
            return;
        }
        
        /* saves its output */
        while ( fgets( outcomm, MAXLINE, file ) ) {
            strcat( output, outcomm );
            memset( outcomm, 0, MAXLINE );
        }

        pclose( file );
        comm = strtok_r( NULL, b, &ptr );
    }
    strcat( output, "\n" );
    // printf( "Outside the while.\nFinal message =>\n %s\n", output );
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

    print_client_info( sockfd );

    print_peer_info( sockfd, 0 );
    
    for( ; ; ) {
        exec_server_commands( sockfd, (struct sockaddr *) &servaddr );
    }


    exit(0);
}

//popen( <comando> );