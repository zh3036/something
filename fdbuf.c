#include "netdef.h"
#include "assert.h"
#include "log.c"
#include "httpdef.c"

typedef struct buf {
  char buffer[MAXBUF];
  char* bufptr;//
  size_t cnt;//total num of content in buf
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
  tf->fbuf = NULL;
}

void add_buf(time_fd* tf){

;}

int bufread(time_fd* tf,size_t n){
  if(!(tf->fbuf)) //create the first buf
    tf->fbuf = ini_buf();
  if(tf->fbuf->cnt==MAXBUF)
    add_buf(tf);
  //check how many space left
  int empty=MAXBUF - (tf->fbuf->cnt % MAXBUF);
  //min, decide how many can we read
  int toread=empty;
  if(toread>n) toread=n;
  //see how many we actually read
  int cnt=read(tf->fd, tf->fbuf->bufptr, toread);
  switch(cnt){
    case -1: return -1; // error
    case 0: return 0;// EOF
    default: 
      tf->fbuf->cnt+=cnt;
      tf->fbuf->bufptr+=cnt;
  }
  return 0;
}



