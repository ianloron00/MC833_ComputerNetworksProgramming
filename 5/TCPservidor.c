#include "./udp.h"

#define TRUE 1
#define FALSE 0

// possible connectable clients, and its state
#define MAX_NUM_CONN 10
int CLIENTS[MAX_NUM_CONN][3];

void init_clients_list()
{
  for (int i = 0; i < MAX_NUM_CONN; i++)
  {
    memset(CLIENTS[i], 0, 3 * sizeof(int));
  }
}

void print_clients_list()
{
  // port
  // socket
  // state:  0 - disconnected, 1 - connected, 2 - pair messaging
  printf("Clients List:\nport\t | socket  | state\n");
  for (int i = 0; i < MAX_NUM_CONN; i++)
  {
    printf("%d\t |\t %d | %d\n", CLIENTS[i][0], CLIENTS[i][1], CLIENTS[i][2]);
  }
  printf("\n");
}

void update_clients_list(int sockfd, int state)
{
  int port = get_port_number(sockfd);

  // ADD SOCKET TO IT
  for (int i = 0; i < MAX_NUM_CONN; i++)
  {
    if (port == CLIENTS[i][0] || CLIENTS[i][0] == 0)
    {
      CLIENTS[i][0] = port;
      CLIENTS[i][1] = sockfd;
      CLIENTS[i][2] = state;
      return;
    }
  }
}

void add_client(int connfd)
{
  update_clients_list(connfd, 1);
  print_clients_list();
}

int _get_socket(int port)
{
  for (int i = 0; i < MAX_NUM_CONN; i++)
  {
    if (CLIENTS[i][0] == port)
      return CLIENTS[i][1];
  }
  return -1;
}

int _get_port(int sockfd)
{
  for (int i = 0; i < MAX_NUM_CONN; i++)
  {
    if (CLIENTS[i][1] == sockfd)
      return CLIENTS[i][0];
  }
  return -1;
}

ssize_t listen_client(int sockfd)
{
  ssize_t n;
  char recvline[MAXLINE];
  int peerport;
  memset(&recvline, 0, sizeof(recvline));

  if ((n = Read(sockfd, recvline, MAXLINE)) > 0)
  {
    // start peer UDP communication
    if ((peerport = atoi(recvline)) > 0)
    {
      int cliport = _get_port(sockfd);
      int peerfd = _get_socket(peerport);

      if (peerfd != -1) {
        printf("starting UDP communication between clients %d and %d\n", cliport, peerport);
        char portbuf[6];
        // NEEDS TO CREATE RANDOM PORT
        sprintf(portbuf, "%d", (cliport/10 + 1));
        Writen(sockfd, portbuf, sizeof(portbuf));
        Writen(peerfd, portbuf, sizeof(portbuf));

        update_clients_list(sockfd, 2);
        update_clients_list(peerfd, 2);
        print_clients_list();
      }
      else
        printf("port %d is not in list\n", peerport);
    }
    else {
      //  testing
      recvline[n] = '\0';
      printf("echoing: %s\n", recvline);
      Writen(sockfd, recvline, sizeof(recvline));
    }
  }

  if (n < 0 && errno != EINTR)
    err_quit("str_echo: read error");

  return n;
}

int main(int argc, char *argv[])
{
  int listenfd, addrlen, new_socket;
  int max_sd, i, valread, sd;
  SAI address;
  fd_set readfds;

  if (argc != 2)
  {
    char error[MAXLINE + 1];
    strcpy(error, "uso: ");
    strcat(error, argv[0]);
    strcat(error, "<Port>");
    perror(error);
    exit(1);
  }
  usi port = (usi)atoi(argv[1]);

  init_clients_list();

  // type of socket created
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(port);

  // create a master socket
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  // bind the socket to localhost port 8888
  if (bind(listenfd, (SA *)&address, sizeof(address)) < 0)
    err_quit("bind");
    
  printf("Listener on port %d \n", ntohs(address.sin_port));

  Listen(listenfd, LISTENQ);

  // accept the incoming connection
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while (TRUE)
  {
    FD_ZERO(&readfds);
    FD_SET(listenfd, &readfds);
    max_sd = listenfd;

    // add child sockets to set
    for (i = 0; i < MAX_NUM_CONN; i++)
    {
      sd = CLIENTS[i][1];
      if (sd > 0)
        FD_SET(sd, &readfds);

      max_sd = max(max_sd, sd);
    }

    Select(max_sd + 1, &readfds, NULL, NULL, NULL);

    // receives an incoming connection
    if (FD_ISSET(listenfd, &readfds))
    {
      new_socket = Accept(listenfd, (SA *)&address,(socklen_t *)&addrlen);
      printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
             new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      // add new socket to array of sockets
      add_client(new_socket);
    }

    // else its some IO operation on some other socket
    for (i = 0; i < MAX_NUM_CONN; i++)
    {
      if((sd = CLIENTS[i][1]) == 0) 
        break;

      // listen to client message
      if (FD_ISSET(sd, &readfds))
      {
        valread = listen_client(sd);
        
        // Check if it was for closing
        if (valread == 0)
        {
          Getpeername(sd, (struct sockaddr *)&address,
                      (socklen_t *)&addrlen);
          printf("Host disconnected , ip %s , port %d \n",
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));

          // Close the socket and mark as 0 in list for reuse
          Close(sd);
          CLIENTS[i][1] = 0;
          CLIENTS[i][2] = 0;
          print_clients_list();
        }
      }
    }
  }

  return 0;
}
