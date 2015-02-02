
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "unistd.h"
#include "string.h"

#define RIO_BUFSIZE 8192

typedef struct rio_str{
    int rio_fd;                /* descriptor for this internal buf */
    int rio_cnt;               /* unread bytes in internal buf */
    char *rio_bufptr;          /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
    struct rio_str *nextbuf;
} rio_t;

ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
;
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
;

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
;