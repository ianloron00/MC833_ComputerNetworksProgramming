#include "./auxiliary.h"

void send_hello( int connfd ) {
  char hello[MAXLINE] = "Hello from server to client in:\n";
  char *time_conn = get_time_connection();
  strcat(hello, (const char *)get_conn_info(connfd));
  strcat( hello, "\n" );
  strcat(hello, (const char*) time_conn );
  Writen( connfd, hello, strlen(hello) );
}

void echo_cli( int connfd ) {
  char echo[MAXLINE];
  Readtext( connfd, echo, strlen(echo) );
  sleep(1);
  Writen( connfd, echo, strlen(echo) );
}

/*
 * Generic Function to be executed after fork
 */
void doit(int connfd)
{
  print_peer_info( connfd, 1 );
  send_hello( connfd );
  echo_cli( connfd );
}

int main(int argc, char **argv)
{
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  pid_t pid;
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

  for (;;)
  {
    sleep(2);
    connfd = Accept(listenfd, (SA *)NULL, NULL);

    if ((pid = Fork()) == 0)
    {
      Close(listenfd);
      doit(connfd);
      Close(connfd);
      exit(0);
    }

    Close(connfd);
  }

  return (0);
}