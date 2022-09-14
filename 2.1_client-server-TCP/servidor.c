#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
// https://www.gta.ufrj.br/ensino/eel878/sockets/inet_ntoaman.html
// to print properly IP and port numbers
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

#define MAXLINE 4096

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    char   buf[MAXDATASIZE];
    time_t ticks;

    char buffer[INET_ADDRSTRLEN];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    // to get local ip: https://www.binarytides.com/get-local-ip-c-linux/
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = 0;   
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }
    /*
    * to print chosen IP address and port number. 
    */
    // inet_ntop(AF_INET, &servaddr.sin_addr.s_addr, buffer, sizeof( buffer ));
    // printf("IP address: %s\n", inet_ntop(servaddr.sin_addr.s_addr) );
    
    // socklen_t len = sizeof(servaddr);
    // if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1) {
    //     perror("getsockname");
    //     exit(1);
    // }
    // else { 
    //     printf("Port number: %d\n", ntohs(servaddr.sin_port));
    // }
    printf("IP address is: %s\n", (char *) inet_ntoa(servaddr.sin_addr));
    printf("port is: %d\n", (int) ntohs(servaddr.sin_port));

    for ( ; ; ) {
      if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
        perror("accept");
        exit(1);
        }

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));
        write(connfd, buf, strlen(buf));


        close(connfd);
    }
    return(0);
}
