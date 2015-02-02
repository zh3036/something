#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */


/* Misc constants */
#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */


int open_listenfd(int port) ;



void unix_error(char *msg) /* unix-style error */;

void Close(int fd) ;

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) ;

int Open_listenfd(int port) ;

int Select(int  n, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout) ;

void app_error(char *msg) /* application error */;
