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
#include <sys/time.h>
#include <stdio.h>
#include "fdbuf.h"
  
void test_time();  
int main(void)
{
        // test_time();
        FILE* f=fopen("test.log", "r");
        int fd=fileno(f);
        time_fd tst;
        ini_fd(&tst,fd);
        bufload(&tst, 150);
        char buf[200];
        printf("%s\n", tst.tail_buf->buffer);
        // bufread(&tst,buf,100);
        // printf("%s\n", buf);
}

int test_string(){
        char *s;
        char *e;
        char b[200]="12345678901234567890";
        char dst[200];
        int i;
        s=b+1;
        e=b+10;
        printf("%s\n", s);      
        printf("%s\n", e);
        i=e-s;
        printf("%d\n", i);
        memcpy(dst, s, i);
        printf("%s\n", dst);

        return 0;
}

void test_time(){
        int i;
        struct timeval tv;
        int u=tv.tv_usec;
        long s=tv.tv_sec;

        for(i = 0; i < 100; i++){
                for(int j=0;j<10000;j++){;}
                gettimeofday(&tv, NULL);
                printf("%d\t%ld\n", tv.tv_usec-u, tv.tv_sec-s);
                u=tv.tv_usec;
                        s=tv.tv_sec;

               
        }
}




// caculate the time passed since the object is inied
int elap_time(time_fd* tf){
  return tf->tms.tv_sec-tf->ini_time;
}

fd_buf* ini_buf(){
  fd_buf* fb;
  fb=(fd_buf*)malloc(sizeof(fd_buf));
  fb->bufptr_end=fb->buffer;
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
  int spare=MAXBUF - (tf->tail_buf->bufptr_end 
                      - tf->tail_buf->buffer);
  //min, decide how many can we read
  int toread=spare;
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
  int toread=src_tf->header_buf->bufptr_end-
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


