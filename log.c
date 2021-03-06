#include "netdef.h"

void LogWrite( int type, char *s1, char *s2, int num)

{
  extern char* logfilename;
  int fd ;

  char logbuffer[MAXBUF*2];

  switch (type) {

    case ERROR: 
      sprintf(logbuffer,"ERROR: %s:%s Errno=%d",s1, s2, errno); 
      break;

    case SORRY: 
      sprintf(logbuffer, \
        "<HTML><BODY><H1>nweb Web Server Sorry: \
        %s %s</H1></BODY></HTML>\r\n", s1, s2);
      write(num,logbuffer,strlen(logbuffer));
      sprintf(logbuffer,"SORRY: %s:%s",s1, s2); 
      break;

    case LOG: 
      sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,num); 
      break;

  }

       /* no checks here, nothing can be done a failure anyway */

  if((fd = open(logfilename, O_WRONLY | O_APPEND,0644)) >= 0){
    write(fd,logbuffer,strlen(logbuffer)); 
    write(fd,"\n",1);      
    close(fd);
  }


}
