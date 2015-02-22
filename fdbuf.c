#include "fdbuf.h"

// caculate the time passed since the object is inied
int elap_time_load(time_fd* tf){
  if(tf->have_ini_time){
    // printf("elap found inied time\n");
    gettimeofday(&(tf->tms_load), NULL);
    // printf("%d passed\n", (int)(tf->tms_load.tv_sec-tf->ini_time_load));
    return tf->tms_load.tv_sec-tf->ini_time_load;
  }

  return 0;
}

int elap_time_body(time_fd* tf){
  if(tf->have_ini_body){
    gettimeofday(&(tf->tms_body), NULL);
    return tf->tms_body.tv_sec-tf->ini_time_body;
  }
  return 0;
}

int ini_time_load(time_fd* tf)
{
  if(!tf->have_ini_time)
  {
    printf("should happen once\n");
    gettimeofday(&(tf->tms_load), NULL);
    tf->ini_time_load=tf->tms_load.tv_sec;
    printf("time inied %u\n", tf->ini_time_load);
    tf->have_ini_time=1;    
    return 0;
  } 
  return 1;  
}

int ini_time_body(time_fd* tf)
{
  if(!tf->have_ini_body)
  {
    gettimeofday(&(tf->tms_body), NULL);
    tf->ini_time_body=tf->tms_body.tv_sec;
    tf->ini_time_body=1;
    return 0;
  }
  return 1;  
}

fd_buf* ini_buf(){
  fd_buf* fb;
  fb=(fd_buf*)calloc(sizeof(fd_buf),1);
  fb->bufptr_end=fb->buffer;
  fb->bufptr_start=fb->buffer;
  fb->next=NULL;
  return fb;
}

// ini the fd num with the struct and set time
void ini_fd(time_fd* tf, int fd,char secure){
  tf->secure=secure;
  tf->fd=fd;
  tf->cnt=0;
  tf->p_flag=0;
  tf->pcnt=-1;
  tf->tail_buf = ini_buf();
  tf->header_buf= tf->tail_buf;
  tf->have_ini_time=0;
  tf->ini_time_body=0;
}


void add_buf(time_fd* tf){
  tf->tail_buf->next=ini_buf();
  // tf->fbuf->next->prev=tf->fbuf;
  tf->tail_buf=tf->tail_buf->next;
}
int Bufload(time_fd *tf, size_t n)
{
  if(tf->secure==NORMAL)
    return bufload(tf,n);
  if(tf->secure==SECURE)
    return secure_bufload(tf, n);
  return -1;
}

int secure_bufload(time_fd *tf,size_t n)
{
  int bufret,pendbyte;
  bufret=bufload(tf, n); //read first
  if(bufret<0)       // if error then return
    return bufret;      
  while(( pendbyte = SSL_pending(tf->client_context))>0)
    {//check for pending bytes, if there are any load again
      int bufret2;
      bufret2=bufload(tf, pendbyte);
      if(bufret2<0)
      {
        bufret=-1;
        break;        
      }
      else
        bufret+=bufret2;
    }
  return bufret;
}

int bufload(time_fd* tf,size_t n){
  // if(!(tf->fbuf)) //if no buf before create the first buf
  //   tf->fbuf = ini_buf();
  int cnt;
  if(tf->tail_buf->bufptr_end==
     tf->tail_buf->buffer+MAXBUF)// if buf is full, add a buf
    add_buf(tf);
  //check how many space lefts
  size_t spare=MAXBUF - (tf->tail_buf->bufptr_end 
                      - tf->tail_buf->buffer);
  //min, decide how many can we read
  size_t toread=spare;
  if(toread>n) toread=n;
  //see how many we actually read
  if(tf->secure==SECURE)
  {
    cnt=SSL_read(tf->client_context,tf->tail_buf->bufptr_end,toread);

    printf("cnt is %d\n", cnt);
    printf("read contets: %s\n", tf->tail_buf->buffer);
  }
  else 
  {

    cnt=read(tf->fd, tf->tail_buf->bufptr_end, toread);
  }
  printf("recv data %d bytes :\n %s\n", cnt,tf->tail_buf->bufptr_end);
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
  if(src_tf->header_buf->bufptr_start==src_tf->tail_buf->bufptr_end)
    return 0;

  size_t toread = src_tf->header_buf->bufptr_end - 
                    src_tf->header_buf->bufptr_start;
  if(toread==0 )
  {
    if(src_tf->header_buf->bufptr_start != src_tf->header_buf->buffer)//if current buffer is empty,remove it
    { 
      fd_buf* bf=src_tf->header_buf;
      src_tf->header_buf=src_tf->header_buf->next;
      free(bf); 
      toread= src_tf->header_buf->bufptr_end-
              src_tf->header_buf->bufptr_start;
    }
    else
    {
      return 0;
    }
  } 
  if(n<toread) toread=n;

  if(src_tf->header_buf != NULL){
    memcpy(dst_buf, src_tf->header_buf->bufptr_start,toread);
    src_tf->header_buf->bufptr_start+=toread;
    src_tf->cnt-=toread;
    if(src_tf->p_flag ==1)
      printf("read buffer:\n%s\n",dst_buf);
  } else{
    toread=0;
  }
  return toread;
}

int bufreadline(time_fd* src_tf,char* dst_buf ,size_t maxlen){

  int n, rc;
  char c, *bufp = (char *)dst_buf;

  for (n = 1; n < (int)maxlen; n++) { 
    if ((rc = bufread(src_tf, &c, 1)) == 1) {
      *bufp++ = c;
      if (c == '\n')
        break;
      } else if (rc == 0) {
        if (n == 1)
          return 0; /* EOF, no data read */
        else
          break;    /* EOF, some data was read */
      } else
        return -1;    /* error */
  }
  *bufp = 0;
  printf("read a line\n:%s\n",dst_buf);
  return n;
}

int isfinish_bufload(time_fd* tf){
  if (tf->tail_buf && strstr(tf->tail_buf->bufptr_start,"\r\n\r\n"))
    return 1;
  return 0;
}

int bufdestroy(time_fd* tf){ 
  int fd=tf->fd;
  while(tf->header_buf!=NULL){
    fd_buf* bf=tf->header_buf;
    tf->header_buf=tf->header_buf->next;
    free(bf);
  } 
  if(tf->secure==SECURE && tf->client_context!=NULL)
  {
    SSL_shutdown(tf->client_context);
    SSL_free(tf->client_context);
  }
  return fd;
}

