#ifndef WRAPPER
#define WRAPPER

#include "./wrappers.h"
#define SA struct sockaddr

#define MAXLINE 4096
#define MAXOUTPUT 16384

#define LISTENQ 10
#define MAXDATASIZE 100

// change it to your path
#define OUTPUT_CONN_PATH "~/connection_info.txt"

#endif

void save_info(const char *file_path, char *info)
{
  FILE *file;
  file = fopen(file_path, "a");
  fprintf(file, "%s\n", info);
  fclose(file);
}

int max(int a, int b)
{
  return a > b ? a : b;
}

void str_cli(FILE *fp, int sockfd)
{
  int maxfdp1, stdineof;
  fd_set rset;
  char buf[MAXLINE];
  int n;

  stdineof = 0;
  FD_ZERO(&rset);
  for (;;)
  {
    if (stdineof == 0)
      FD_SET(fileno(fp), &rset);
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);

    /* socket is readable */
    if (FD_ISSET(sockfd, &rset))
    {
      // if ( ( n = Read(sockfd, buf, MAXLINE)) == 0 ) {
      if ((n = Readline(sockfd, buf, MAXLINE)) == 0)
      {
        if (stdineof == 1)
          return;
        else
        {
          perror("str_cli: server terminated prematurely");
          exit(1);
        }
      }
      Write(fileno(stdout), buf, n);
    }
    /* input is readable */
    if (FD_ISSET(fileno(fp), &rset))
    {
      // if ( (n = Read(fileno(fp), buf, MAXLINE)) == 0) {
      if ((n = Readline(fileno(fp), buf, MAXLINE)) == 0)
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

  fprintf(file, "%s\n", info);

  fclose(file);
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
