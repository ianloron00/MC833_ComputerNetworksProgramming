#include "auxiliary.h"

void udp_main(short isMaster);
void chat(int sockfd, SAI cliaddr);
void init_udp_master(int *sockfd, SAI cliaddr, usi port);
void init_udp_slave(int *sockfd, SAI cliaddr, usi port);

void udp_send(int sockfd, SAI cliaddr, char *sendline, socklen_t clilen);
short udp_recv(int sockfd, SAI cliaddr, char *sendline);

void udp_main(short isMaster)
{
  int *sockfd = malloc(sizeof(int));
  SAI cliaddr;
  usi port = 8081;
  printf("in main\n");

  if (isMaster)
  {
    init_udp_master(sockfd, cliaddr, port);
  }
  else
  {
    init_udp_slave(sockfd, cliaddr, port);
  }
  // servaddr.sin_family = AF_INET; // IPv4
  // servaddr.sin_addr.s_addr = INADDR_ANY;
  // servaddr.sin_port = htons(port);

  printf("communication initialized\n");
  print_peer_info(*sockfd, 0);
  // sprintf("IP: %s\nPort: %d\n",
  //         servaddr.sin_addr.s_addr,
  //         servaddr.sin_port);

  chat(*sockfd, cliaddr);
  Close(*sockfd);
}

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

    if (FD_ISSET(sockfd, &rset))
    {
      // n = udp_recv(sockfd, peeraddr, recvline);
      n = Recvfrom(sockfd, (char *)recvline,
               MAXLINE, MSG_WAITALL,
               (SA *)&peeraddr, &clilen);
      recvline[n] = '\0';

      if (strcmp(recvline, "finalizar_chat") == 0)
        exit(0);

      Fputs(recvline, stdout);
    }

    if (FD_ISSET(fileno(fp), &rset))
    {
      if (Fgets(sendline, MAXLINE, stdin) != NULL)
      {
        printf("sending %s\n", sendline);
        Sendto(sockfd, (const char *)sendline,
               strlen(sendline), 0,
               (const SA *)&peeraddr, sizeof(peeraddr));

        // udp_send(sockfd, peeraddr, sendline, sizeof(peeraddr));
        if (strcmp(sendline, "finalizar_chat") == 0)
          exit(0);
      }
    }
  }
}

void udp_send(int sockfd, SAI cliaddr, char *sendline, socklen_t clilen)
{
  // socklen_t clilen = sizeof(cliaddr);
  Sendto(sockfd, (const char *)sendline,
         strlen(sendline), MSG_CONFIRM,
         (const SA *)&cliaddr, clilen);
}

short udp_recv(int sockfd, SAI cliaddr, char *recvline)
{
  int n;
  socklen_t clilen = sizeof(cliaddr);

  n = Recvfrom(sockfd, (char *)recvline,
               MAXLINE, MSG_WAITALL,
               (SA *)&cliaddr, &clilen);
  return n;
}

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

void init_udp_master(int *sockfd, SAI cliaddr, usi port)
{
  SAI servaddr;

  // Creating socket file descriptor
  *sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  // Bind the socket with the server address
  if (bind(*sockfd, (const SA *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");
}

void init_udp_slave(int *sockfd, SAI servaddr, usi port)
{
  // Creating socket file descriptor
  *sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);
}

// DEPRECATED
// ##################################################### //
void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  int n;
  socklen_t len;
  char mesg[MAXLINE];

  for (;;)
  {
    len = clilen;
    // n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    n = Recvfrom(sockfd, mesg, MAXLINE, MSG_WAITALL, pcliaddr, &len);
    printf("%s\n", mesg);
    // Sendto(sockfd, mesg, n, 0, pcliaddr, len);
    Sendto(sockfd, mesg, n, MSG_CONFIRM, pcliaddr, len);
  }
}

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  int n;
  char sendline[MAXLINE], recvline[MAXLINE + 1];

  while (Fgets(sendline, MAXLINE, fp) != NULL)
  {
    printf("sent %s\n", sendline);
    // Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
    Sendto(sockfd, sendline, strlen(sendline),
           MSG_CONFIRM, pservaddr, servlen);

    // n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
    n = Recvfrom(sockfd, recvline, MAXLINE,
                 MSG_WAITALL, (SA *)pservaddr, &servlen);

    recvline[n] = 0;
    Fputs(recvline, stdout);
  }
}