#ifndef WRAPPER
#define WRAPPER

#include "./wrappers.h"

#endif
/*
 *  ---- CLIENT ----
*/




/*
 *  ---- SERVER ----
*/
// void get_server_info( int listenfd, const struct sockaddr *servaddr ) {
//     char buffer[INET_ADDRSTRLEN];

//     socklen_t len = sizeof(servaddr);
//     if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1) {
//         perror("getsockname");
//         exit(1);
//     }
//     else {
//         printf( "Server => IP address: %s, Port number: %d\n", 
//         (char *) inet_ntop(AF_INET, &servaddr.sin_addr, buffer, sizeof(buffer) ),
//         ntohs(servaddr.sin_port) );
//     }
// }