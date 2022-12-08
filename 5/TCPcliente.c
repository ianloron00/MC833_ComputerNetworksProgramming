#include "./udp.h"

void doit(FILE *fp, int sockfd)
{
  int maxfdp1, stdineof, n;
  fd_set rset;
  char recvline[MAXLINE], sendline[MAXLINE];
  short isMaster = 0;
  int cliport;

  stdineof = 0;
  FD_ZERO(&rset);
  for (;;)
  {
    memset(&recvline, 0, strlen(recvline));
    memset(&sendline, 0, strlen(sendline));

    FD_SET(sockfd, &rset);
    FD_SET(fileno(fp), &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);

    // reads from server, prints message and start UDP communication
    if (FD_ISSET(sockfd, &rset))
    {
      // if server is done.
      if ((n = Read(sockfd, recvline, MAXLINE)) == 0)
      {
        if (stdineof == 1)
          return;

        else
          err_quit("str_cli: server terminated prematurely");
      }
      printf("received: %s", recvline);
      
      // initialize UDP communication
      if ((cliport = atoi(recvline)) > 0)
      {
        printf("\ninitializing UDP connection at port %d\n", cliport);
        isMaster ? udp_master(cliport) : udp_slave(cliport);
        
        // update client status at TCP connection.
        char ack[5] = "ACK";
        Writen(sockfd, ack, 5);
        printf("back to TCP connection!\n");
      }
    }

    // reads input and send to server
    if (FD_ISSET(fileno(fp), &rset))
    {
      if ((n = Read(fileno(fp), sendline, MAXLINE)) == 0)
      {
        Shutdown(sockfd, SHUT_WR);
        FD_CLR(fileno(fp), &rset);
        continue;
      }
      
      printf("sending: %s", sendline);
      Writen(sockfd, sendline, n);
      if ((cliport = atoi(sendline)) > 0)
      {
        isMaster = 1;
      }
    }
  }
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
  doit(stdin, sockfd);

  exit(0);
}
