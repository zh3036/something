#include "log.h"
int LogWrite( int type, char *s1, char *s2, int num)

{
  // int num=tf->fd;
  extern char* logfilename;
  int fd ;
  int ret=0;  
  char date[MAXBUF];

  bzero(date, MAXBUF);
  get_time_str(date);
  char *i=strstr(date,"\n");
  *i=0;



  char logbuffer[MAXBUF];
  bzero(logbuffer, MAXBUF);
  switch (type) {

    case ERROR: 
      sprintf(logbuffer,"ERROR: %s:%s Errno=%d",s1, s2, num); 
      break;

    case SORRY: 
      sprintf(logbuffer, "HTTP/1.1 %s %s\r\n",s1 ,s2);
      sprintf(logbuffer, "%sDate: %s\r\n",logbuffer,date);
      sprintf(logbuffer, "%sConnection: Keep-Alive\r\n",logbuffer);
      sprintf(logbuffer, "%sContent-length: %d\r\n", logbuffer, 0);
      sprintf(logbuffer, "%sServer: Liso/1.0\r\n\r\n", logbuffer);
      ret=Rio_writen(num, logbuffer, strlen(logbuffer));      
      sprintf(logbuffer,"SORRY: %s:%s:in fd %d",s1,s2,num); 
      break;

    case LOG: 
      sprintf(logbuffer,"INFO: %s:%s:%d",s1, s2,num); 
      break;

  }

  if((fd = open(logfilename, O_WRONLY | O_APPEND,0644)) >= 0){
    Write(fd,logbuffer,strlen(logbuffer)); 
    Write(fd,"\n",1);      
    close(fd);  
  }
  return ret;


}
