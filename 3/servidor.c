#include "./wrappers.h"
#include "./auxiliary.h"
// rodar todos ao mesmo tempo com o cliente sem sleep, rodar rapidinho 
/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) { 
    char* time_conn = get_time_connection();

    //write_conn_info( "Connection started! " );
   // write_conn_info( time_conn );

   // write_peer_info( connfd, 1 );
    sleep( 38 );

   // write_conn_info( "Connection closed. " );
   // write_conn_info( get_time_connection() );
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t pid;
    char   error[MAXLINE + 1];
    printf("%d",argc);
    if (argc !=3) {
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

    Listen( listenfd, (int)atoi(argv[2]));  

    // write_sock_info( listenfd, 1 );

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
