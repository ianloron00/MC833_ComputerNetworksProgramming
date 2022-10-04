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
    file = fopen("/home/ianloron00/grad/833/MC833/2.2/out/output.txt", "a");
    printf("received =>\n%s\n", rec );
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

/*
* FALTA RECEBER INPUT DA PORTA
*/
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
