#include "udp.h"

#define PORT 8080

// Driver code
int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  // memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);
  // servaddr.sin_port = htons(atoi(argv[1]));

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");

  // Filling client information
  // cliaddr.sin_family = AF_INET; // IPv4
  // cliaddr.sin_addr.s_addr = INADDR_ANY;

  // udp_server(sockfd, cliaddr);
  chat(sockfd, cliaddr);

  return 0;
}