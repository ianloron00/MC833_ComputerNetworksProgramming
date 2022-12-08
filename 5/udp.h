#include "auxiliary.h"

void chat(int sockfd, SAI cliaddr, short isMaster);
void udp_master(usi master_port);
void udp_slave(usi master_port);

void chat(int sockfd, SAI peeraddr, short isMaster)
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

    memset(&recvline, 0, sizeof(recvline));
    memset(&sendline, 0, sizeof(sendline));

    if (FD_ISSET(sockfd, &rset))
    {
      n = Recvfrom(sockfd, (char *)recvline,
                   MAXLINE, MSG_WAITALL,
                   (SA *)&peeraddr, &clilen);
      recvline[n] = '\0';

      if (strcmp(recvline, "finalizar_chat\n") == 0)
      {
          if (isMaster)
            save_chat_info(get_time_connection());
            
        printf("finishing UDP communication\n");
        break;
        exit(0);
      }

      if (strcmp(recvline, "ACK") != 0)
      {
        char buf[MAXLINE] = "peer: ";
        strcat(buf, recvline);
        printf("%s", buf);

        if (isMaster)
          save_chat_info(buf);
      }
    }

    if (FD_ISSET(fileno(fp), &rset))
    {
      if (Fgets(sendline, MAXLINE, stdin) != NULL)
      {
        Sendto(sockfd, (const char *)sendline,
               strlen(sendline), 0,
               (const SA *)&peeraddr, sizeof(peeraddr));

        if (isMaster)
          save_chat_info(sendline);
        
        if (strcmp(sendline, "finalizar_chat\n") == 0)
        {
          if (isMaster)
            save_chat_info(get_time_connection());

          printf("finishing UDP communication\n");
          break;
          exit(0);
        }

      }
    }
  }
}

void udp_master(usi port)
{
  int sockfd;
  SAI servaddr, cliaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr,
           sizeof(servaddr)) < 0)
    err_quit("bind failed");

  init_chat_file();
  chat(sockfd, cliaddr, 1);
}

void udp_slave(usi port)
{
  int sockfd;
  SAI servaddr;

  // Creating socket file descriptor
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  char ack[5] = "ACK";
  Sendto(sockfd, ack, sizeof(ack), 0,
         (const SA *)&servaddr, sizeof(servaddr));
  chat(sockfd, servaddr, 0);

  close(sockfd);
}
