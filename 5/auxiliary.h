#ifndef WRAPPER
#define WRAPPER

#include "./wrappers.h"
#define SA struct sockaddr
#define SAI struct sockaddr_in

#define MAXLINE 4096
#define MAXOUTPUT 16384

#define LISTENQ 10
#define MAXDATASIZE 100
#define max(x, y) ((x > y) ? (x) : (y))

// change it to your path
#define OUTPUT_CONN_PATH "~/connection_info.txt"

#endif

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

void save_info(FILE *fd, char *info)
{
  fprintf(fd, "%s\n", info);
  fclose(fd);
}

void str_cli(const char *input_file, const char *output_file,
             int sockfd1, int sockfd2)
{
  int maxfdp1;
  fd_set rset;
  char buf[MAXLINE];
  int n, hasSentInput = 0, s1Finished = 0, s2Finished = 0;
  FILE *input = Fopen(input_file, "r");

  FD_ZERO(&rset);
  for (;;)
  {
    memset(buf, 0, strlen(buf));
    FD_SET(fileno(input), &rset);
    FD_SET(sockfd1, &rset);
    FD_SET(sockfd2, &rset);
    maxfdp1 = max(max(sockfd1, sockfd2), fileno(input)) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);

    /* socket 1 is readable */
    if (FD_ISSET(sockfd1, &rset) && !s1Finished)
    {
      if ((n = Read(sockfd1, buf, MAXLINE)) == 0)
      {
        if (hasSentInput)
        {
          s1Finished = 1;
          FD_CLR(sockfd1, &rset);
          continue;
        }
        else
          err_quit("str_cli: server 1 terminated prematurely");
      }
      save_info(Fopen(output_file, "a"), buf);
      Write(fileno(stdout), buf, n);
    }

    /* socket 2 is readable */
    if (FD_ISSET(sockfd2, &rset) && !s2Finished)
    {
      if ((n = Read(sockfd2, buf, MAXLINE)) == 0)
      {
        if (hasSentInput)
        {
          s2Finished = 1;
          FD_CLR(sockfd2, &rset);
          continue;
        }
        else
          err_quit("str_cli: server 2 terminated prematurely");
      }
      save_info(Fopen(output_file, "a"), buf);
      Write(fileno(stdout), buf, n);
    }

    /* input is readable */
    if (!hasSentInput && FD_ISSET(fileno(input), &rset))
    {
      if ((n = Read(fileno(input), buf, MAXLINE)) == 0)
      {
        hasSentInput = 1;
        Shutdown(sockfd1, SHUT_WR);
        Shutdown(sockfd2, SHUT_WR);
        FD_CLR(fileno(input), &rset);
        continue;
      }
      Writen(sockfd1, buf, n);
      Writen(sockfd2, buf, n);
    }

    if (s1Finished && s2Finished) {
      Close(sockfd1);
      Close(sockfd2);
      return;
    }
  }
}

char *Fgets(char *str, int n, FILE *stream)
{
  return fgets(str, n, stream);
}

int Fputs(const char *str, FILE *stream)
{
  return fputs(str, stream);
}

// lab. 3
/*
 * Calls waitpid to prevent zombies
 */
void sig_chld(int signo)
{
  pid_t pid;
  int stat;

  while ((pid = waitpid(-1, &stat, WNOHANG) > 0))
    printf("child %d terminated\n", pid);
  return;
}

/*
 * To get socket's IP address and port number.
 */
char *get_conn_info(int sockfd)
{
  struct sockaddr_in cliaddr;
  char buffer[INET_ADDRSTRLEN];
  int len = 100;
  char *ans = malloc(sizeof(char) * len);

  socklen_t clilen = sizeof(cliaddr);
  if (getsockname(sockfd, (struct sockaddr *)&cliaddr, &clilen) == -1)
  {
    perror("getsockname");
    exit(1);
  }

  else
  {
    snprintf(ans, len, "IP address: %s; Port number: %d",
             (char *)inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer)),
             ntohs(cliaddr.sin_port));
  }

  return ans;
}

/*
 * Print connected peer socket's IP address and port number.
 */
char *get_peer_info(int sockfd, int isServer)
{

  char buffer[INET_ADDRSTRLEN];
  struct sockaddr_in peeraddr;
  int len = 100;
  char *ans = malloc(sizeof(char) * len);

  socklen_t buflen = sizeof(peeraddr);
  if (getpeername(sockfd, (struct sockaddr *)&peeraddr, &buflen) == -1)
  {
    perror("getpeername");
    exit(1);
  }
  else
  {
    snprintf(ans, len,
             isServer
                 ? "(A client) => IP address: %s; port number: %d"
                 : "(Server) => IP address: %s; port number: %d",
             (char *)inet_ntop(AF_INET, &peeraddr.sin_addr, buffer, sizeof(buffer)),
             ntohs(peeraddr.sin_port));
  }

  return ans;
}

/*
 * Get socket's IP address and port number expliciting whether it is a server or client.
 */
char *get_sock_info(int sockfd, int isServer)
{
  int len = 100;
  char *ans = malloc(sizeof(char) * len);
  snprintf(ans, len,
           isServer
               ? "Server => %s"
               : "Client => %s",
           get_conn_info(sockfd));

  return ans;
}

/*
 * To print socket's IP address and port number.
 */
void print_client_info(int sockfd)
{
  printf("%s\n", get_sock_info(sockfd, 0));
}

/*
 * print server IP and port number
 */
int print_server_info(int listenfd)
{
  return printf("%s\n", get_sock_info(listenfd, 1));
}

/*
 * Read message from standard input and send to server
 */
void send_recv_msg(int sockfd)
{

  char sendline[MAXLINE];

  printf("Write a message: ");
  fgets(sendline, MAXLINE, stdin);

  if (send(sockfd, sendline, strlen(sendline), 0) < 0)
  {
    perror("send failed.");
  }
  else
  {
    puts("Message sent to the server.\n");
  }
}

void print_peer_info(int sockfd, int isServer)
{
  printf("%s\n", get_peer_info(sockfd, isServer));
}

void save_conn_info(char *info)
{
  FILE *file;
  file = fopen(OUTPUT_CONN_PATH, "a");
  save_info(file, info);
}

void save_sock_info(int sockfd, int isServer)
{
  save_conn_info(get_sock_info(sockfd, isServer));
}

void save_peer_info(int sockfd, int isServer)
{
  save_conn_info(get_peer_info(sockfd, isServer));
}

/*
 * Prints time of connection with client and sends its time
 */
char *get_time_connection()
{
  time_t ticks;
  int len = 40;
  ticks = time(NULL);
  char *strtime = malloc(sizeof(char) * len);

  snprintf(strtime, len,
           "Time: %.24s\r", ctime(&ticks));

  return strtime;
}
