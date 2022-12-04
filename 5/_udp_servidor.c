#include "udp.h"

#define PORT 8080
// #define MAXLINE 1024

void udp_server(int sockfd, SAI cliaddr)
{
  int n;
  char buffer[MAXLINE];

  socklen_t len = sizeof(cliaddr); // len is value/result

  for (;;)
  {
    n = Recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    Sendto(sockfd, (const char *)buffer, strlen(buffer),
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           len);
  }
}
// Driver code
int main()
{
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");

  udp_server(sockfd, cliaddr);

  return 0;
}