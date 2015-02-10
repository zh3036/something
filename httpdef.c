#include "httpdef.h"



void get_filetype(char *filename, char *filetype) 
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename,".css"))
    strcpy(filetype, "text/css");
  else if (strstr(filename,".png"))
    strcpy(filetype, "text/png");
  else
    strcpy(filetype, "text/plain");
}  

char *get_time_str(char *time_buf)
{
    time_t    now_sec;
    struct tm    *time_now;
    if(    time(&now_sec) == -1)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);
        // perror("time() in get_time.c");
        return NULL;
    }
    if((time_now = gmtime(&now_sec)) == NULL)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);
        // perror("localtime in get_time.c");
        return NULL;
    }
    char *str_ptr = NULL;
    if((str_ptr = asctime(time_now)) == NULL)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);

        // perror("asctime in get_time.c");
        return NULL;
    }
    strcat(time_buf, str_ptr);
    return time_buf;
}




int serve_static(int fd, char *filename, struct stat *sbuf, char* method) 
{
  int ret=0;
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];
  char date[MAXBUF],*lm;
  struct tm *tm;
  int filesize=sbuf->st_size;
  tm =localtime(&(sbuf->st_mtime));
  get_time_str(date);
  lm=asctime(tm);
  char* i = strstr(lm,"\n");
  *i=0;
  i=strstr(date,"\n");
  *i=0;
  
  /* Send response headers to client */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sDate: %s\r\n",buf,date);
  sprintf(buf, "%sConnection: Keep-Alive\r\n",buf);
  sprintf(buf, "%sServer: Liso/1.0\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sLast-Modified: %s\r\n",buf,lm);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));

  /* Send response body to client */
  if(method[0]=='G'){
    srcfd = Open(filename, O_RDONLY, 0);
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);
  } 
  return ret;
}





int LogWrite( int type, char *s1, char *s2, int num)

{
  extern char* logfilename;
  int fd ;
  int ret=0;

  char logbuffer[MAXBUF*2];

  switch (type) {

    case ERROR: 
      sprintf(logbuffer,"ERROR: %s:%s Errno=%d",s1, s2, num); 
      break;

    case SORRY: 

      sprintf(logbuffer, \
        "HTTP/1.1 %s %s\r\n\r\n",s1 ,s2);
      ret=Write(num,logbuffer,strlen(logbuffer));
      sprintf(logbuffer,"SORRY: %s:%s:in fd %d",s1,s2,num); 
      break;

    case LOG: 
      sprintf(logbuffer,"INFO: %s:%s:%d",s1, s2,num); 
      break;

  }

       /* no checks here, nothing can be done a failure anyway */

  if((fd = open(logfilename, O_WRONLY | O_APPEND,0644)) >= 0){
    Write(fd,logbuffer,strlen(logbuffer)); 
    Write(fd,"\n",1);      
    close(fd);  
  }
  return ret;


}
