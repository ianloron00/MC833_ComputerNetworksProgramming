#include "udp.h"

#define PORT 8080

// Driver code
int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr; //, cliaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  // memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);
  // servaddr.sin_port = htons(atoi(argv[1]));

  // Filling client information
  // cliaddr.sin_family = AF_INET;
  // cliaddr.sin_addr.s_addr = INADDR_ANY;
  // cliaddr.sin_port = htons(PORT);

  // udp_client(sockfd, servaddr);
  chat(sockfd, servaddr);

  close(sockfd);
  return 0;
}