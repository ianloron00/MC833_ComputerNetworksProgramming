#include "./wrappers.h"
#include "./auxiliary.h"
// rodar todos ao mesmo tempo com o cliente sem sleep, rodar rapidinho 
/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) { 
    sleep( 1 );
}

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t pid;
    char   error[MAXLINE + 1];

    if (argc != 3) {
        strcpy(error,"Definir: ");
        strcat(error,argv[0]);
        strcat(error," <Port> <BACKLOG>");
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

    Listen( listenfd, (int) atoi(argv[2]) );  

    // Signal( SIGCHLD, sig_chld );
 
    for ( ; ; ) {
        sleep( 2 );
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
