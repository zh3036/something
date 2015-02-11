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
char *get_time_str(char *time_buf);
int main(int argc, char const *argv[])
{
  char xxx[100];
  char x[12]="-231jsd";
  int i;
  sprintf(xxx, "1234\n");
  strcat(xxx, x+1);
  printf("%s\n", xxx);
  i=atoi("-12j");
  printf("%d\n", i);
  for (int i = 0; i < strlen(x); ++i)
   {
     if (!isdigit(x[i]))
     {
       printf("invalid char: %c\n", x[i]);
     }
   } 
  // sprintf(xxx,"%s3e\n",xxx); 
  // printf("%s\n", xxx);
  return 0;
}

int mainx()
{
  char logbuffer[5];
  sprintf(logbuffer, "\r\n\r\n");
  int i = strlen(logbuffer);
  printf("%d\n", i);
  char log2buffer[100];
  sprintf(log2buffer,"image/pnf/");
  // log2buffer=;
  if(log2buffer[strlen(log2buffer)-1]=='/')
    printf("yes!\n");
  printf("%c\n", log2buffer[strlen(log2buffer)-1]);
  return 1;
}
int time_test(){
  char tb[1000];
  get_time_str(tb);
  printf("%s\n",tb);
  return 1;

}
int main2(int argc, char const *argv[])
{
  /* code */
  char tb[1000];
  get_time_str(tb);
  printf("%s\n",tb);
  return 1;
  return 0;
}
int main3(int argc, char const *argv[])
{
  struct stat sbuf;
  struct tm *tm;
  char filename[100]="test/test2.c";
  if (stat(filename, &sbuf) < 0) {
    printf("filne not exist\n");
  }
  if (!(S_ISREG(sbuf.st_mode)) 
    || !(S_IRUSR & sbuf.st_mode)) {
    printf("no read permission\n");
  }
  // int a = sbuf.st_mtime;
  tm =localtime(&sbuf.st_mtime);
  char * lm=asctime(tm);
  char* i = strstr(lm,"\n");
  *i=0;
  printf("%s\n", lm);
  printf("test\n");
  return 0;
}


char *get_time_str(char *time_buf)
{
    time_t    now_sec;
    struct tm    *time_now;
    if(    time(&now_sec) == -1)
    {
        perror("time() in get_time.c");
        return NULL;
    }
    if((time_now = gmtime(&now_sec)) == NULL)
    {
        perror("localtime in get_time.c");
        return NULL;
    }
    char *str_ptr = NULL;
    if((str_ptr = asctime(time_now)) == NULL)
    {
        perror("asctime in get_time.c");
        return NULL;
    }
    strcat(time_buf, str_ptr);
    return time_buf;
}




int test_sscanf()
{
  char buf[100]="GET / HTTP/1.1\r\n";
  char buf2[100]="Connection: keep-alive\r\n";
  char buf3[100]="Content-Length: 1446\r\n";
  char method[50],uri[50],version[50];
  char left[100],right[100];
  char left2[100],right2[100];
  sscanf(buf, "%s %s %s", method, uri, version);
  printf("%s\n", method);
  printf("%s\n", uri);
  printf("%s\n", version);
  sscanf(buf2, "%s %s", left,right);
  sscanf(buf3, "%s %s", left2, right2);
  printf("%s x %s\n%s x %s\n",left,right,left2,right2 );
  if(strncasecmp(left, "connection:",11 )==0) printf("connect is right\n");

  if (strcasecmp(method, "GET")) {
      return -1;
  }
  return 0;
}


int test_strstr(){
  char a[100]="abcdf\r\n\r";
  char *b = strchr(a, 'b');
  printf("%s\n", b);
  char* c = strstr(a, "\r\n\r\n");
  printf("%s\n", c);

  return 0;
}


int file_test(void)
{
        // test_time();
        FILE* f=fopen("test.log", "r");
        int fd=fileno(f);
        time_fd tst;
        ini_fd(&tst,fd);
        bufload(&tst, 2);
        char buf[200];
        printf("%s\n", tst.tail_buf->buffer);
        printf("---------------\n");
        bufread(&tst,buf,2);
        printf("%s\n", buf);
        for (int i = 0; i < 10; ++i)
        {
                bufload(&tst, 2);
                bufread(&tst,buf,3);
                printf("%s\n", buf);
        }
        return 1;

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


