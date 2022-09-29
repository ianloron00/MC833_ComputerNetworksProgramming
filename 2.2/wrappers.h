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

#define MAXLINE 4096
#define MAXOUTPUT 16384

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read( int fd, char *ptr ) {
    if ( read_cnt <= 0 ) {
        again:
            if ( ( read_cnt = read( fd, read_buf, sizeof( read_buf ) ) ) < 0 ) {
                if ( errno == EINTR )
                    goto again;
                return (-1);
            } else if ( read_cnt == 0 ) 
                return (0);
            read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

ssize_t Readline ( int fd, void *vptr, size_t maxlen ) {
    ssize_t n = 1, rc;
    char c, *ptr;

    ptr = ( char * ) vptr;
    for ( n = 1; n < maxlen; n++ ) {
        if ( ( rc = my_read( fd, &c ) ) == 1 ) {
            *ptr++ = c;
            if ( c == '\n')
                break;
        } else if ( rc == 0 ) {
            *ptr = 0;
            return (n - 1);
        } else
            return (-1);
    }

    *ptr = 0;
    return (n - 1);
}

ssize_t readbuf( void **vptrptr ) {
    if ( read_cnt )
        *vptrptr = read_ptr;
    return (read_cnt);
}

int Socket ( int family, int type, int flags ) {
    int sockfd;
    if ( (sockfd = socket( family, type, flags) ) < 0 ) {
        perror ( "socket" );
        exit(1);
    }
    return sockfd;
}

void Listen ( int fd, int backlog ) {
    char *ptr;

    if ( ( ptr = getenv( "LISTENQ" ) ) != NULL )
        backlog = atoi( ptr );

    if ( listen( fd, backlog ) < 0 ) {
        perror( "Listen error" );
        exit(1);
    }
}

ssize_t Writen( int fd, const void *vptr, size_t n ) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (const char *) vptr;
    nleft = n;
    while( nleft > 0 ) {
        if ( ( nwritten = write(fd, ptr, nleft ) ) <= 0 ) {
            if ( nwritten < 0 && errno == EINTR )
                nwritten = 0;
            else
                return (-1);
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}

int Accept ( int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen ) {
    int connfd;
    if ( ( connfd = accept( sockfd, (struct sockaddr *) cliaddr, addrlen ) ) == -1 ) {
        perror("Accept");
        exit(1);
    }

    return connfd;
}

void Connect( int sockfd, struct sockaddr * servaddr, ssize_t length ) {
    if ( connect( sockfd, (struct sockaddr *) &servaddr, length ) < 0 ) {
        perror("connect error");
        exit(1);
    }
}

void Bind ( int sockfd, struct sockaddr * myaddr, int addrlen ) {
    if ( bind( sockfd, (struct sockaddr *)&myaddr, addrlen ) == -1 ) {
        perror("Bind");
        exit(1);
    }
}

pid_t Fork(void) {
    return fork();
}

void Close( int fd ) {
    close( fd );
}
