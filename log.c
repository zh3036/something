#include "log.h"
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
