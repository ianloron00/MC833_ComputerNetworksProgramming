#ifndef WRAPPER
#define WRAPPER

#include "./wrappers.h"
#define SA struct sockaddr

#define MAXLINE 4096
#define MAXOUTPUT 16384

#define LISTENQ 10
#define MAXDATASIZE 100
#define max(x, y) ((x > y) ? (x) : (y))

#endif

void err_quit(const char *msg)
{
  perror(msg);
  exit(1);
}

void save_info(FILE *fd, char *info)
{
  fprintf(fd, "%s\n", info);
  fclose(fd);
}

void str_cli(const char *input_file, const char *output_file,
             int sockfd1, int sockfd2)
{
  printf("socket 1: %d; socket 2: %d\n", sockfd1, sockfd2);
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
    if (FD_ISSET(sockfd1, &rset))
    {
      if ((n = Read(sockfd1, buf, MAXLINE)) == 0)
      {
        if (hasSentInput)
        {
          s1Finished = 1;
          printf("shutdown 1 read\n");
          Shutdown(sockfd1, SHUT_RD);
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
    if (FD_ISSET(sockfd2, &rset))
    {
      if ((n = Read(sockfd2, buf, MAXLINE)) == 0)
      {
        if (hasSentInput)
        {
          s2Finished = 1;
          printf("shutdown 2 read\n");
          Shutdown(sockfd2, SHUT_RD);
          FD_CLR(sockfd2, &rset);
          return;
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
        printf("shutdown write\n");
        Shutdown(sockfd1, SHUT_WR);
        Shutdown(sockfd2, SHUT_WR);
        FD_CLR(fileno(input), &rset);
        continue;
      }
      Writen(sockfd1, buf, n);
      Writen(sockfd2, buf, n);
    }

    if (s1Finished && s2Finished)
      return;
  }
}