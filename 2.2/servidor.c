#include "./wrappers.h"
#include "./auxiliary.h"

/*
* Send commands to client execute it and return output.
*/ 
void send_commands( int connfd ) {
    char comms[] = "ifconfig;pwd;ls -l;END\n";
    Writen( connfd, comms, strlen(comms) );
    printf( "Message sent to %s\n\t\t: %s\n", 
        (const char *) get_peer_info( connfd, 1 ), 
        comms 
    );
}

/*
 * Save results, given by clients, in a file. 
*/
void save_result_commands( int connfd ) {
    char rec[MAXOUTPUT];
    FILE *file;
    
    Readtext( connfd, rec, sizeof(rec) );
    file = fopen("/home/ianloron00/grad/833/MC833/2.2/out/output.txt", "a");
    fprintf(file, "%s\n", rec );
    fclose( file );
}

/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) { 
    char hello[55] = "Hello from server!\n";
    char* time_conn = get_time_connection();

    write_info( "Connection started! " );
    write_info( time_conn );

    strcat( hello, (const char *) time_conn );
    strcat( hello, "\n" );
    Writen( connfd, hello, 55 );
    // Writen( connfd, (const char *) time_conn, 55 );
    write_peer_info( connfd, 1 );
    sleep( 1 );
    send_commands( connfd );
    save_result_commands( connfd );

    write_info( "Connection closed. " );
    write_info( get_time_connection() );
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t pid;
    char   error[MAXLINE + 1];

    if (argc != 2) {
        strcpy(error,"Definir: ");
        strcat(error,argv[0]);
        strcat(error," <Port> ");
        perror(error);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port        = htons( (unsigned short int) atoi(argv[1]) );  
    listenfd = Socket( AF_INET, SOCK_STREAM, 0 );
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    // Bind( listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr) );

    Listen( listenfd, LISTENQ );  

    write_sock_info( listenfd, 1 );

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
