#include "auxiliary.h"

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