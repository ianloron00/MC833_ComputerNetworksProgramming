// #include "./wrappers.h"
#include "./auxiliary.h"

void doit( int sockfd, struct sockaddr *servaddr ) {
  print_client_info( sockfd );
  print_peer_info( sockfd, 0 );
  sleep(1);
  //sleep(1000000);
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
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error (client)");
        exit(1);
    }
    servaddr.sin_port   = htons( (unsigned short int) atoi(argv[2]) );

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    doit( sockfd, (struct sockaddr *) &servaddr );
  
    Close( sockfd );

    exit(0);
}
