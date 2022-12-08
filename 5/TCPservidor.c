// https://notes.shichao.io/unp/ch5/#tcp-echo-client-str_cli-function
// https://notes.shichao.io/unp/ch6/
#include "./udp.h"
// possible connectable clients, and its state
#define MAX_NUM_CONN 10
// 0 - disconnected, 1 - connected, 2 - pair messaging
int CLIENTS[MAX_NUM_CONN][2];
#define MASTER_PORT 8085

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

  // ADD SOCKET TO IT
  for (int i = 0; i < MAX_NUM_CONN; i++) {
    if (port == CLIENTS[i][0] || CLIENTS[i][0] == 0) {
      CLIENTS[i][0] = port;
      CLIENTS[i][1] = state;
      return;
    }
  }
}

void add_client(int connfd) {
  update_list_clients(connfd, 1);
  print_list_clients();
}

void listen_from_clients(int sockfd) {
  ssize_t n;
  char recvline[MAXLINE];
  int peerport;
  
again:
  // receive desired peer to connect and send respective port to both. 
  while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
    printf("client: %s\n", recvline);
    if ((peerport = atoi(recvline)) > 0) {
      int cliport = get_port_number(sockfd);
      printf("starting UDP communication between clients %d and %d\n", cliport, peerport);
      char port[6];
      // convert integer to string
      sprintf(port, "%d", MASTER_PORT);
      Writen(sockfd, port, sizeof(port));
      // NEEDS ARRAY TO ACCESS PEER PORT
      // Writen(peerfd, port, sizeof(port));
    }
  }

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0)
    err_quit("str_echo: read error");
}

/*
 * Generic Function to be executed after fork
 */
void doit(int connfd)
{
  print_peer_info(connfd, 1);
  add_client(connfd);
  listen_from_clients(connfd);
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

    // probably it is just not necessary!
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