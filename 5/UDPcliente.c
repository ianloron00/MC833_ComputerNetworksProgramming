#include "udp.h"

int main(int argc, char **argv)
{
  int sockfd;
  SAI servaddr;
  const char* port = argv[2];

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons((usi) atoi(port));

  char ack[5] = "ACK";
  Sendto(sockfd, ack, sizeof(ack), 0, 
        (const SA *)&servaddr, sizeof(servaddr));
  chat(sockfd, servaddr, 0);

  close(sockfd);
  return 0;
}