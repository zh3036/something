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


void serve_static(int fd, char *filename, struct stat *sbuf, char* method) 
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];
  char date[MAXBUF],*lm;
  struct tm *tm;
  int filesize=sbuf->st_size;
  tm =localtime(&(sbuf->st_mtime));
  get_time_str(date);
  lm=asctime(tm);
  char* i = strstr(lm,"\n")
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
}
