#include "./auxiliary.h"

void doit(int sockfd1, int sockfd2, SA *servaddr, 
          const char *input_file, const char *output_file)
{
  str_cli(input_file, output_file, sockfd1, sockfd2);
}

int main(int argc, char **argv)
{
  int sockfd1, sockfd2;
  char error[MAXLINE + 1];
  struct sockaddr_in servaddr;
  
  if (argc != 8)
  {
    strcpy(error, "uso: ");
    strcat(error, argv[0]);
    strcat(error, " <IPaddress> <Port1> <Port2> [<] <input.txt> [>] <output.txt>");
    perror(error);
    exit(1);
  }
  const char* ip = argv[1], *port1 = argv[2], *port2 = argv[3];
  const char* input_file = argv[5], *output_file = argv[7];

  sockfd1 = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  Inet_pton(AF_INET, ip, &servaddr.sin_addr);
  servaddr.sin_port = htons((unsigned short int)atoi(port1));
  if (connect(sockfd1, (SA *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("connect error");
    exit(1);
  }

  sockfd2 = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  Inet_pton(AF_INET, ip, &servaddr.sin_addr);
  servaddr.sin_port = htons((unsigned short int)atoi(port2));
  if (connect(sockfd2, (SA *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("connect error");
    exit(1);
  }

  print_client_info(sockfd1);
  print_peer_info(sockfd1, 0);
  print_client_info(sockfd2);
  print_peer_info(sockfd2, 0);
  doit(sockfd1, sockfd2, (SA *)&servaddr, input_file, output_file);

  exit(0);
}
