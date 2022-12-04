// https://notes.shichao.io/unp/ch5/#tcp-echo-client-str_cli-function
// https://notes.shichao.io/unp/ch6/
#include "./auxiliary.h"
// possible connectable clients, and its state
#define MAX_NUM_CONN 10
// 0 - disconnected, 1 - connected, 2 - pair messaging
int CLIENTS[MAX_NUM_CONN][2];

void initialize_list_clients() {
  for (int i = 0; i < MAX_NUM_CONN; i++) {
    memset(CLIENTS[i], 0, 2*sizeof(int));
  }
}

void print_list_clients() {
  printf("Clients List:\nport number | state\n");
  for (int i = 0; i < MAX_NUM_CONN; i++) {
    printf("%d\t  |\t %d\n", CLIENTS[i][0], CLIENTS[i][1]);
  }
  printf("\n");
}

void update_list_clients(int sockfd, int state) {
  int port = get_port_number(sockfd);

  for (int i = 0; i < MAX_NUM_CONN; i++) {
    if (port == CLIENTS[i][0] || CLIENTS[i][0] == 0) {
      CLIENTS[i][0] = port;
      CLIENTS[i][1] = state;
      return;
    }
  }
}

void listen_to_clients(int connfd) {
  update_list_clients(connfd, 1);
  print_list_clients();
}


/*
 * Generic Function to be executed after fork
 */
void doit(int connfd)
{
  // int *num_conn = malloc(sizeof(int));
  // *num_conn = 0;
  print_peer_info(connfd, 1);
  listen_to_clients(connfd);
  str_echo(connfd);
  printf("after str_echo:\n");
  print_list_clients();
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
    err_quit("bind");

  Listen(listenfd, LISTENQ);
  print_server_info(listenfd);
  initialize_list_clients();

  for (;;)
  {
    connfd = Accept(listenfd, (SA *)NULL, NULL);

    if ((childpid = Fork()) == 0)
    {
      Close(listenfd);
      doit(connfd);
      exit(0);
    }

    printf("before closing connection:\n");
    print_list_clients();
    Close(connfd);
  }

  return (0);
}