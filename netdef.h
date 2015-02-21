
#ifndef NETDEF_H
#define NETDEF_H
#include "lib.h"
#include "log.h"
// #include "netdef.h"
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */


typedef struct { /* a pool of connected descriptors */ 
  int maxfd;        /* largest descriptor in read_set */   
  fd_set read_set;  /* set of all active descriptors */
  fd_set ready_set; /* subset of descriptors ready for reading  */
  int nready;       /* number of ready descriptors from select */   
  int maxi;         /* highest index into client array */
  time_fd clientfd[FD_SETSIZE];    /* set of active descriptors */
} Pool; 

/*
200_OK -- send proper object data back 
404_NOT_FOUND -- when objects do not exist in the file system
411_LENGTH_REQUIRED -- for POSTs
500_INTERNAL_SERVER_ERROR -- syscall failure right now/other failures
501_NOT_IMPLEMENTED -- for any request you do not handle
503_SERVICE_UNAVAILABLE -- if you can not accept anymore connections
505_HTTP_VERSION_NOT_SUPPORTED -- if you get anything other than 1.1
*/


char* logfilename;
char* wwwfolder;
char* cgi;

int open_listenfd(int port) ;



void unix_error(char *msg,int fd) /* unix-style error */;

int Close(int fd) ;

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) ;

int Open_listenfd(int port) ;

int Select(int  n, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout) ;

void app_error(char *msg) /* application error */;
int rio_writen(int fd, void *usrbuf, size_t n) ;
int Rio_writen(int fd, void *usrbuf, size_t n);
int rio_write_secure(SSL* client_context, void *usrbuf,size_t n);
int Open(const char *pathname, int flags, mode_t mode);
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length) ;


char *get_time_str(char *time_buf);
ssize_t Write(int fd, const void *buf, size_t count) ;
#endif
