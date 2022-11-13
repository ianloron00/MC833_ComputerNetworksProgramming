#include "./auxiliary.h"

void send_input(int sockfd, const char *input_file, SA *servaddr)
{
  // char msg[MAXLINE];
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  FILE *input = Fopen(input_file, "r");

  printf("-- Sending message line-by-line...\n");
  while ((read = getline(&line, &len, input)) != -1)
  {
    printf("%s", line);
    // Write(sockfd, line, MAXLINE);
    Write(sockfd, line, MAXLINE);
  }
  if (ferror(input))
  {
    /* handle error */
    perror("sent_input");
    exit(1);
  }
  printf("\n\n");
  free(line);
  fclose(input);
}

void save_server_msg(int sockfd, const char *output_file)
{
  printf("-- saving data...\n");
  char msg[MAXOUTPUT];
  Readline(sockfd, msg, strlen(msg));
  // Read(sockfd, msg, strlen(msg));
  // Readtext(sockfd, msg, strlen(msg));
  // read(sockfd, &msg, strlen(msg));
  save_info(output_file, msg);
  printf("-- received from server:\n%s\n ", msg);
}

void doit(int sockfd, SA *servaddr, const char *input_file, const char *output_file)
{
  // receive hello
  save_server_msg(sockfd, output_file);
  send_input(sockfd, input_file, servaddr);
  // receive echo
  save_server_msg(sockfd, output_file);
}

int main(int argc, char **argv)
{
  int sockfd;
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

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
  {
    perror("inet_pton error (client)");
    exit(1);
  }
  servaddr.sin_port = htons((unsigned short int)atoi(argv[2]));

  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("connect error");
    exit(1);
  }

  print_client_info(sockfd);
  print_peer_info(sockfd, 0);
  doit(sockfd, (SA *)&servaddr, argv[4], argv[6]);

  Close(sockfd);

  exit(0);
}