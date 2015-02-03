#include "netdef.h"
#include "assert.h"
#include "log.c"
#include "httpdef.c"

typedef struct buf {
  char buffer[MAXBUF];
  char* bufptr;
  int cnt;
  struct buf *next;
  struct buf *prev;
} fd_buf;

typedef struct {
  int fd;
  struct timeval tms;
  unsigned int ini_time;
  fd_buf* fbuf;
} time_fd;

// caculate the time passed since the object is inied
int elap_time(time_fd* tf){
  return tf->tms.tv_sec-tf->ini_time;
}


fd_buf* ini_buf(){
  fd_buf* fb;
  fb=(fd_buf*)malloc(sizeof(fd_buf));
  fb->bufptr=fb->buffer;
  fb->cnt=0;
  fb->next=NULL;
  fb->prev=NULL;
  return fb;
}

// ini the fd num with the struct and set time
void ini_fd(time_fd* tf, int fd){
  tf->fd=fd;
  tf->ini_time=tf->tms.tv_sec;
  tf->fbuf = ini_buf();
}

void add_buf(fd_buf* fb){
;}

int bufread(time_fd* tf){
  ;
  return 0;
}


