#include "./auxiliary.h"

void doit(int sockfd1, SA *servaddr, const char *input_file, const char *output_file)
{
  str_cli(input_file, output_file, sockfd1);
}

int main(int argc, char **argv)
{
  int sockfd1;
  char error[MAXLINE + 1];
  struct sockaddr_in servaddr;

  if (argc != 7)
  {
    strcpy(error, "uso: ");
    strcat(error, argv[0]);
    strcat(error, " <IPaddress> <Port1> [<] <input.txt> [>] <output.txt>");
    perror(error);
    exit(1);
  }

  sockfd1 = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;

  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  servaddr.sin_port = htons((unsigned short int)atoi(argv[2]));

  if (connect(sockfd1, (SA *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("connect error");
    exit(1);
  }

  print_client_info(sockfd1);
  print_peer_info(sockfd1, 0);
  doit(sockfd1, (SA *)&servaddr, argv[4], argv[6]);

  exit(0);
}
