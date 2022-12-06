#include "auxiliary.h"

void chat(int sockfd, SAI cliaddr);
void init_udp_master(int sockfd, SAI cliaddr, usi port);
void init_udp_slave(int sockfd, SAI cliaddr, usi port);

void chat(int sockfd, SAI peeraddr)
{
  int maxfdp, n;
  fd_set rset;
  char sendline[MAXLINE], recvline[MAXLINE];
  FILE *fp = stdin;
  socklen_t clilen = sizeof(peeraddr);

  FD_ZERO(&rset);
  for (;;)
  {
    FD_SET(fileno(fp), &rset);
    FD_SET(sockfd, &rset);
    
    maxfdp = max(fileno(fp), sockfd) + 1;
    Select(maxfdp, &rset, NULL, NULL, NULL);

    memset(&recvline, 0, sizeof(recvline));
    memset(&sendline, 0, sizeof(sendline));

    if (FD_ISSET(sockfd, &rset))
    {
      n = Recvfrom(sockfd, (char *)recvline,
               MAXLINE, MSG_WAITALL,
               (SA *)&peeraddr, &clilen);
      recvline[n] = '\0';

      if (strcmp(recvline, "finalizar_chat") == 0)
        exit(0);

      if (strcmp(recvline, "ACK") != 0)
        printf("peer: %s", recvline);
    }

    if (FD_ISSET(fileno(fp), &rset))
    {
      if (Fgets(sendline, MAXLINE, stdin) != NULL)
      {
        Sendto(sockfd, (const char *)sendline,
               strlen(sendline), 0,
               (const SA *)&peeraddr, sizeof(peeraddr));

        if (strcmp(sendline, "finalizar_chat") == 0)
          exit(0);
      }
    }
  }
}

void init_udp_master(int sockfd, SAI cliaddr, usi port)
{
  SAI servaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  // Bind the socket with the server address
  if (bind(sockfd, (const SA *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");

  chat(sockfd, cliaddr);

}

void init_udp_slave(int sockfd, SAI servaddr, usi port)
{
  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  char ack[5] = "ACK";
  Sendto(sockfd, ack, sizeof(ack), 0, 
        (const SA *)&servaddr, sizeof(servaddr));
  chat(sockfd, servaddr);

  close(sockfd);
}

// from book & Geeks4Geeks //
// #########################################
void udp_server(int sockfd, SAI cliaddr)
{
  int n;
  char buffer[MAXLINE];

  socklen_t len = sizeof(cliaddr); // len is value/result

  for (;;)
  {
    n = Recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (SA *)&cliaddr,
                 &len);
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    Sendto(sockfd, (const char *)buffer, strlen(buffer),
           MSG_CONFIRM, (const SA *)&cliaddr,
           len);
  }
}

void udp_client(int sockfd, SAI servaddr)
{
  int n;
  socklen_t len;
  char recvline[MAXLINE];
  char *sendline = malloc(MAXLINE * sizeof(char));

  while (Fgets(sendline, MAXLINE, stdin) != NULL)
  {
    Sendto(sockfd, (const char *)sendline, strlen(sendline),
           MSG_CONFIRM, (const SA *)&servaddr,
           sizeof(servaddr));
    printf("message sent.\n");

    n = Recvfrom(sockfd, (char *)recvline, MAXLINE,
                 MSG_WAITALL, (SA *)&servaddr,
                 &len);
    recvline[n] = '\0';
    printf("Server: %s\n", recvline);
  }
}