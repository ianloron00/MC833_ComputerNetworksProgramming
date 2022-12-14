#include "udp.h"

int main(int argc, char **argv)
{
  int sockfd;
  SAI servaddr, cliaddr;
  const char* port = argv[1];

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons((usi) atoi(port));

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");

  init_chat_file();
  chat(sockfd, cliaddr, 1);

  return 0;
}