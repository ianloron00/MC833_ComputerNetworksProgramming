#include "udp.h"

#define PORT 8080
// #define MAXLINE 1024
void udp_client(int sockfd, SAI servaddr)
{
  int n;
  socklen_t len;
  char recvline[MAXLINE];
  char *sendline = malloc(MAXLINE * sizeof(char));

  while (Fgets(sendline, MAXLINE, stdin) != NULL)
  {
    Sendto(sockfd, (const char *)sendline, strlen(sendline),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    printf("message sent.\n");

    n = Recvfrom(sockfd, (char *)recvline, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 &len);
    recvline[n] = '\0';
    printf("Server: %s\n", recvline);
  }
}
// Driver code
int main()
{
  int sockfd;
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  udp_client(sockfd, servaddr);

  close(sockfd);
  return 0;
}