#ifndef TIMEFD
#define TIMEFD 222
#include "lib.h"

#define SECURE 1
#define NORMAL 0


typedef struct buf {
  char buffer[MAXBUF];
  char* bufptr_end;//
  char* bufptr_start;
  struct buf *next;
  
} fd_buf;

typedef struct {
  int fd;
  char secure;
  SSL *client_context;
  struct timeval tms;
  unsigned int ini_time;
  fd_buf* tail_buf;
  fd_buf* header_buf; 
  size_t cnt;//total num of content in buf
  char p_flag;//indicate whether this fd is in recieving content mode
  int pcnt;// for the purpose to record the content
} time_fd;
#endif