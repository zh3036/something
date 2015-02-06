#include "fdbuf.h"

// caculate the time passed since the object is inied
int elap_time(time_fd* tf){
  return tf->tms.tv_sec-tf->ini_time;
}

fd_buf* ini_buf(){
  fd_buf* fb;
  fb=(fd_buf*)malloc(sizeof(fd_buf));
  fb->bufptr_end=fb->buffer;
  fb->bufptr_start=fb->buffer;
  fb->next=NULL;
  return fb;
}

// ini the fd num with the struct and set time
void ini_fd(time_fd* tf, int fd){
  tf->fd=fd;
  tf->cnt=0;
  tf->ini_time=tf->tms.tv_sec;
  tf->tail_buf = ini_buf();
  tf->header_buf= tf->tail_buf;
}


void add_buf(time_fd* tf){
  tf->tail_buf->next=ini_buf();
  // tf->fbuf->next->prev=tf->fbuf;
  tf->tail_buf=tf->tail_buf->next;
}

int bufload(time_fd* tf,size_t n){
  // if(!(tf->fbuf)) //if no buf before create the first buf
  //   tf->fbuf = ini_buf();
  if(tf->tail_buf->bufptr_end==
     tf->tail_buf->buffer+MAXBUF)// if buf is full, add a buf
    add_buf(tf);
  //check how many space left
  size_t spare=MAXBUF - (tf->tail_buf->bufptr_end 
                      - tf->tail_buf->buffer);
  //min, decide how many can we read
  size_t toread=spare;
  if(toread>n) toread=n;
  //see how many we actually read
  int cnt=read(tf->fd, tf->tail_buf->bufptr_end, toread);
  switch(cnt){
    case -1: return -1; // error
    case 0: return 0;// EOF
    default: 
      tf->cnt+=cnt;
      tf->tail_buf->bufptr_end+=cnt;
  }
  return cnt;
}

int bufread(time_fd* src_tf, char* dst_buf  ,size_t n){
  //how many left for the current buffer
  size_t toread=src_tf->header_buf->bufptr_end-
              src_tf->header_buf->bufptr_start;
  if(toread==0){//if current buffer is empty,remove it
    fd_buf* bf=src_tf->header_buf;
    src_tf->header_buf=src_tf->header_buf->next;
    free(bf);
    toread=src_tf->header_buf->bufptr_end-
           src_tf->header_buf->bufptr_start;
  } 
  if(n<toread) toread=n;

  if(src_tf->header_buf != NULL){
    memcpy(dst_buf, src_tf->header_buf->bufptr_start,toread);
    src_tf->header_buf->bufptr_start+=toread;
    src_tf->cnt-=toread;
  } else{
    toread=0;
  }
  return toread;
}

int bufdestroy(time_fd* tf){ 
  while(tf->header_buf!=NULL){
    fd_buf* bf=tf->header_buf;
    tf->header_buf=tf->header_buf->next;
    free(bf);
  }
  return close(tf->fd);
}


