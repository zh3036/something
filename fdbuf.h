#ifndef FDBUF
#define FDBUF 010

#include "netdef.h"
#include "assert.h"



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
} time_fd;

int elap_time(time_fd* tf);
void ini_fd(time_fd* tf, int fd);
int bufload(time_fd* tf,size_t n);
int bufread(time_fd* src_tf, char* dst_buf  ,size_t n);
int bufdestroy(time_fd* tf);
int isfinish_bufload(time_fd* tf);
#endif