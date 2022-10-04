#include "./wrappers.h"
#include "./auxiliary.h"

/*
* Send commands to client execute it and return output.
*/ 
void send_commands( int connfd ) {
    char comms[] = "ifconfig;pwd;ls -l;END\n";
    Writen( connfd, comms, strlen(comms) );
    printf( "Message sent: %s\n\n", comms );
}

/*
 * Save results, given by clients, in a file. 
*/
void save_result_commands( int connfd ) {
    char rec[MAXOUTPUT];
    FILE *file;
    ssize_t n = Readtext( connfd, rec, sizeof(rec) );
    printf( "Message of size %ld received\n", n );
    file = fopen("/home/ianloron00/grad/833/MC833/2.2/out/output.txt", "a+");
    fprintf(file, "%s\n", rec );
    fclose( file );
}

// void save_

/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) {
    
    display_time_connection( connfd );
    write_peer_info( connfd, 1 );

    sleep( 1 );
    send_commands( connfd );
    save_result_commands( connfd );
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t pid;

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

    write_sock_info( listenfd, 1 );
    write_peer_info( listenfd, 1 );

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
