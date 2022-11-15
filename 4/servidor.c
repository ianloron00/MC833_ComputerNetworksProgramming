// https://notes.shichao.io/unp/ch5/#tcp-echo-client-str_cli-function
// https://notes.shichao.io/unp/ch6/
#include "./auxiliary.h"

void str_echo(int sockfd)
{
  ssize_t n;
  char buf[MAXLINE];

again:
  while ((n = read(sockfd, buf, MAXLINE)) > 0)
    Writen(sockfd, buf, n);

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0)
    err_quit("str_echo: read error");
}

void send_hello(int connfd)
{
  char hello[MAXLINE] = "Hello from server to client in:\n";
  char *time_conn = get_time_connection();
  strcat(hello, (const char *)get_conn_info(connfd));
  strcat(hello, "\n");
  strcat(hello, (const char *)time_conn);
  strcat(hello, "\n");
  // Writen( connfd, hello, strlen(hello) );
  Writen(connfd, hello, strlen(hello));
  printf("-- hello sent:\n%s\n", hello);
}

void echo_cli(int connfd)
{
  char echo[MAXLINE];
  Readline(connfd, &echo, strlen(echo));
  printf("-- received input: --\n%s\n", echo);
  sleep(1);
  // Writen( connfd, echo, strlen(echo) );
  Writen(connfd, echo, strlen(echo));
  printf("-- message echoed. --");
}

/*
 * Generic Function to be executed after fork
 */
void doit(int connfd)
{
  print_peer_info(connfd, 1);
  send_hello(connfd);
  sleep(1);
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
      // str_echo(connfd);
      exit(0);
    }

    Close(connfd);
  }

  return (0);
}