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
}

/*
* Generic Function to be executed after fork
*/
void doit( int connfd ) {
    
    display_time_connection( connfd );
    get_peer_port( connfd );

    sleep( 1 );
    send_commands( connfd );
    save_result_commands( connfd );
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
        printf( "Server => IP address: %s, Port number: %d\n", 
        (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ),
        ntohs(servaddr.sin_port) );
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
