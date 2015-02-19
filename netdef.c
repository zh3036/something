#include "netdef.h"


int open_listenfd(int port) 
{
  int listenfd, optval=1;
  struct sockaddr_in serveraddr;

/* Create a socket descriptor */
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1;

/* Eliminates "Address already in use" error from bind. */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
    (const void *)&optval , sizeof(int)) < 0)
    return -1;

/* Listenfd will be an endpoint for all requests to port
on any IP address for this host */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET; 
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serveraddr.sin_port = htons((unsigned short)port); 
  if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
    return -1;

/* Make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0)
    return -1;
  return listenfd;
}
void unix_error(char *msg, int fd) /* unix-style error */
{
// fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  LogWrite(LOG, msg, strerror(errno), NULL);
//exit(0);
}



int Close(int fd) 
{
  int rc;

  if ((rc = close(fd)) < 0)
    unix_error("Close error",fd);

  return rc;
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) 
{
  int rc;

  if ((rc = accept(s, addr, addrlen)) < 0)
    unix_error("Accept error",s);
  return rc;
}

int Open_listenfd(int port) 
{
  int rc;

  if ((rc = open_listenfd(port)) < 0)
    unix_error("Open_listenfd error",-1);
  return rc;
}

int Select(int  n, fd_set *readfds, fd_set *writefds,
  fd_set *exceptfds, struct timeval *timeout) 
{
  int rc;

  if ((rc = select(n, readfds,writefds, exceptfds, timeout)) < 0)
    unix_error("Select error", n);
  return rc;
}
ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
{
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = (char*)usrbuf;

  while (nleft > 0) {
    if ((nwritten =Write(fd, bufp, nleft)) <= 0) 
    {
      if (errno == EINTR)  /* interrupted by sig handler return */
        nwritten = 0;    /* and callWrite() again */
      else
        return -1;       /* errorno set byWrite() */
    }
  nleft -= nwritten;
  bufp += nwritten;
}
return n;
}

int Rio_writen(int fd, void *usrbuf, size_t n) 
{
  if ((size_t)rio_writen(fd, usrbuf, n) != n){
    LogWrite(ERROR, "500", "internet server", NULL);
    return -1;
  }
  return 0;

}

int Open(const char *pathname, int flags, mode_t mode) 
{
  int rc;
  char *a=(char*)pathname;
  if ((rc = open(pathname, flags, mode))  < 0)
    LogWrite(ERROR, a, "open error", NULL);
  return rc;
}

void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) 
{
  void *ptr;

  if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
    LogWrite(ERROR, "mmap error", (char*)addr, NULL);
  return(ptr);
}

void Munmap(void *start, size_t length) 
{
  if (munmap(start, length) < 0)
    unix_error("munmap error",-1);
}
// void app_error(char *msg) /* application error */
// {
//     fprintf(stderr, "%s\n", msg);
//     //exit(0);
// }

ssize_t Write(int fd, const void *buf, size_t count) 
{
  ssize_t rc;
  if ((rc =write(fd, buf, count)) ==-1)
    LogWrite(ERROR, strerror(errno), (char*)buf,NULL);
  return rc;
}
