#ifndef LIBS
#define LIBS

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
// lab. 4
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

#define MAXLINE 4096
#define MAXOUTPUT 16384
#define _XOPEN_SOURCE 700

#define SA struct sockaddr
#define SAI struct sockaddr_in

#endif

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

typedef void Sigfunc(int);

void err_quit(const char *msg)
{
  perror(msg);
  exit(1);
}

// pt. 5

void Getpeername(int sockfd, SA* peeraddr, socklen_t *buflen) {
  if (getpeername(sockfd, (SA*)peeraddr, buflen) == -1)
    err_quit("getpeername");
}

void Sendto(int fd, const void *buf, size_t n, int flags,
            const struct sockaddr *addr, socklen_t addr_len)
{

  if (sendto(fd, buf, n, flags, addr, addr_len) < 0)
  {
    err_quit("Sendto");
  }
}

int Recvfrom(int fd, void *buf, size_t n, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
  int m;
  if ((m = recvfrom(fd, buf, n, flags, addr, addr_len)) < 0)
  {
    err_quit("Recvfrom");
  }

  return m;
}
// pt. 4

void Inet_pton(int inet, const char *ip, void *sin_addr)
{
  if (inet_pton(inet, ip, sin_addr) <= 0)
  {
    perror("inet_pton error (client)");
    exit(1);
  }
}

/**
 * Open a file
 * NEEDS TO BE MODIFIED TO TREAT AS DESCRIPTOR
 */
FILE *Fopen(const char *fd, const char *purpose)
{
  // e.g: "input.txt", "r"
  FILE *file = fopen(fd, purpose);

  if (!file) // equivalent to saying if ( file == NULL )
  {
    printf("oops, file %s can't be read\n", fd);
    exit(1);
  }

  return file;
}

void Shutdown(int sockfd, int howto)
{
  /* Returns: 0 if OK, –1 on error */
  if ((shutdown(sockfd, howto)) == -1)
  {
    char erro[50];
    sprintf(erro, "%s %d ", "Shutdown from socket:", sockfd);
    perror(erro);
    exit(1);
  }
}

int Select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout)
{
  /* Returns: positive count of ready descriptors, 0 on timeout, –1 on error */
  int c = select(nfds, readfds, writefds, exceptfds, timeout);
  if (c == 0)
  {
    perror("Select: timeout");
    exit(1);
  }
  else if (c == -1)
  {
    perror("Select: error");
    exit(1);
  }
  return c;
}

// lab. 3 --------------------

// Sigfunc *signal(int signo, Sigfunc *func)
// {
//   struct sigaction act, oact;

//   act.sa_handler = func;
//   sigemptyset(&act.sa_mask);
//   act.sa_flags = 0;
//   if (signo == SIGALRM)
//   {
// #ifdef SA_INTERRUPT
//     act.sa_flags |= SA_INTERRUPT;
// #endif
//   }
//   else
//   {
// #ifdef SA_RESTART
//     act.sa_flags |= SA_RESTART;
// #endif
//   }
//   if (sigaction(signo, &act, &oact) < 0)
//     return (SIG_ERR);

//   return (oact.sa_handler);
// }

// void Signal(int signo, Sigfunc *func)
// {
//   if (signal(signo, func) < 0)
//   {
//     perror("Signal");
//     exit(1);
//   }
// }

// lab.2 --------------
static ssize_t my_read(int fd, char *ptr)
{
  if (read_cnt <= 0)
  {
  again:
    if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
    {
      if (errno == EINTR)
        goto again;
      return (-1);
    }
    else if (read_cnt == 0)
      return (0);
    read_ptr = read_buf;
  }

  read_cnt--;
  *ptr = *read_ptr++;
  return (1);
}

ssize_t readbuf(void **vptrptr)
{
  if (read_cnt)
    *vptrptr = read_ptr;
  return (read_cnt);
}

ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n = 1, rc;
  char c, *ptr;

  ptr = (char *)vptr;
  for (n = 1; n < maxlen; n++)
  {
    if ((rc = my_read(fd, &c)) == 1)
    {
      *ptr++ = c;
      if (c == '\n')
        break;
    }
    else if (rc == 0)
    {
      *ptr = 0;
      return (n - 1);
    }
    else
      return (-1);
  }

  *ptr = 0;
  return (n - 1);
}

ssize_t Read(int fd, void *vptr, size_t maxlen)
{
  ssize_t n;
  if ((n = read(fd, vptr, maxlen)) < 0)
  {
    perror("Read");
    exit(1);
  }
  return n;
}

ssize_t Readtext(int fd, void *vptr, size_t maxlen)
{
  ssize_t n = 0, m;
  char line[MAXLINE];
  while ((m = read(fd, line, sizeof(line))) > 0)
  {
    n += m;
    strcat(vptr, line);
  }

  return n;
}

int Socket(int family, int type, int flags)
{
  int sockfd;
  if ((sockfd = socket(family, type, flags)) < 0)
  {
    perror("socket");
    exit(1);
  }
  return sockfd;
}

void Listen(int fd, int backlog)
{
  char *ptr;

  if ((ptr = getenv("LISTENQ")) != NULL)
    backlog = atoi(ptr);

  if (listen(fd, backlog) < 0)
  {
    perror("Listen error");
    exit(1);
  }
}

ssize_t Write(int fd, const void *ptr, size_t n)
{
  ssize_t nwrite;
  if ((nwrite = write(fd, ptr, n)) < 0)
  {
    perror("Write");
    exit(1);
  }
  return nwrite;
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = (const char *)vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ((nwritten = write(fd, ptr, nleft)) <= 0)
    {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;
      else
        return (-1);
    }

    nleft -= nwritten;
    ptr += nwritten;
  }

  return (n);
}

int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
  int connfd;
  if ((connfd = accept(sockfd, (struct sockaddr *)cliaddr, addrlen)) == -1)
  {
    perror("Accept");
    exit(1);
  }

  return connfd;
}

void Connect(int sockfd, struct sockaddr *servaddr, ssize_t length)
{
  if (connect(sockfd, (struct sockaddr *)&servaddr, length) < 0)
  {
    perror("connect error");
    exit(1);
  }
}

void Bind(int sockfd, struct sockaddr *myaddr, int addrlen)
{
  if (bind(sockfd, (struct sockaddr *)&myaddr, addrlen) == -1)
  {
    perror("Bind");
    exit(1);
  }
}

pid_t Fork(void)
{
  return fork();
}

void Close(int fd)
{
  close(fd);
}
