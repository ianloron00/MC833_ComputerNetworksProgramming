// https://notes.shichao.io/unp/ch5/#tcp-echo-client-str_cli-function
// https://notes.shichao.io/unp/ch6/
#include "./auxiliary.h"


/*
 * Generic Function to be executed after fork
 */
void doit(int connfd)
{
  print_peer_info(connfd, 1);
  str_echo(connfd);
}

int main(int argc, char **argv)
{
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  pid_t childpid;
  char error[MAXLINE + 1];

  if (argc != 2)
  {
    strcpy(error, "Definir: ");
    strcat(error, argv[0]);
    strcat(error, " <Port>");
    perror(error);
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  servaddr.sin_port = htons((unsigned short int)atoi(argv[1]));
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) == -1)
  {
    perror("bind");
    exit(1);
  }

  Listen(listenfd, LISTENQ);
  print_server_info(listenfd);

  for (;;)
  {
    connfd = Accept(listenfd, (SA *)NULL, NULL);

    if ((childpid = Fork()) == 0)
    {
      Close(listenfd);
      doit(connfd);
      exit(0);
    }

    Close(connfd);
  }

  return (0);
}