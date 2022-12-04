#include "./auxiliary.h"

void __str_cli(FILE *fp, int sockfd)
{
  int maxfdp1, stdineof, n;
  fd_set rset;
  char buf[MAXLINE];

  stdineof = 0;
  FD_ZERO(&rset);
  for (;;)
  {
    if (stdineof == 0)
      FD_SET(fileno(fp), &rset);
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);

    if (FD_ISSET(sockfd, &rset))
    {
      if ((n = Read(sockfd, buf, MAXLINE)) == 0)
      {
        if (stdineof == 1)
          return;

        else
          err_quit("str_cli: server terminated prematurely");
      }
      Write(fileno(stdout), buf, n);
    }
    if (FD_ISSET(fileno(fp), &rset))
    {
      if ((n = Read(fileno(fp), buf, MAXLINE)) == 0)
      {
        stdineof = 1;
        Shutdown(sockfd, SHUT_WR);
        FD_CLR(fileno(fp), &rset);
        continue;
      }
      Writen(sockfd, buf, n);
    }
  }
}

void doit(int sockfd, SA *servaddr)
{
  __str_cli(stdin, sockfd);
}

int main(int argc, char **argv)
{
  int sockfd;
  SAI servaddr;

  if (argc != 3)
  {
    char error[MAXLINE + 1];
    strcpy(error, "uso: ");
    strcat(error, argv[0]);
    strcat(error, " <IPaddress> <Port>");
    perror(error);
    exit(1);
  }
  const char *ip = argv[1], *port = argv[2];

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  Inet_pton(AF_INET, ip, &servaddr.sin_addr);
  servaddr.sin_port = htons((unsigned short int)atoi(port));
  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
    err_quit("connect error");

  print_client_info(sockfd);
  print_peer_info(sockfd, 0);
  // __str_cli(stdin, sockfd[0]);
  doit(sockfd, (SA *)&servaddr);

  exit(0);
}
