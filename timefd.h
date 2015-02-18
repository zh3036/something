#ifndef TIMEFD
#define TIMEFD 222
#include "lib.h"
typedef struct buf {
  char buffer[MAXBUF];
  char* bufptr_end;//
  char* bufptr_start;
  struct buf *next;
  
} fd_buf;

typedef struct {
  int fd;
  struct timeval tms;
  unsigned int ini_time;
  fd_buf* tail_buf;
  fd_buf* header_buf;
  size_t cnt;//total num of content in buf
  char p_flag;
  int pcnt;
} time_fd;
#endif