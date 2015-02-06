#include "httpdef.h"


void rio_readinitb(rio_t *rp, int fd) 
{
    rp->rio_fd = fd;  
    rp->rio_cnt = 0;  
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char*)usrbuf;

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR)  /* interrupted by sig handler return */
		nwritten = 0;    /* and call write() again */
	    else
		return -1;       /* errorno set by write() */
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;

    while (rp->rio_cnt <= 0) {  /* refill if buf is empty */
	rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, 
			   sizeof(rp->rio_buf));
	if (rp->rio_cnt < 0) {
	    if (errno != EINTR) /* interrupted by sig handler return */
		return -1;
	}
	else if (rp->rio_cnt == 0)  /* EOF */
	    return 0;
	else 
	    rp->rio_bufptr = rp->rio_buf; /* reset buffer ptr */
    }

    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;          
    if (rp->rio_cnt < (int)n)   
    	cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = (char *)usrbuf;

    for (n = 1; n < (int)maxlen; n++) { 
	if ((rc = rio_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n')
		break;
	} else if (rc == 0) {
	    if (n == 1)
		return 0; /* EOF, no data read */
	    else
		break;    /* EOF, some data was read */
	} else
	    return -1;	  /* error */
    }
    *bufp = 0;
    return n;
}





void client_error(int fd, char *cause, char *errnum, 
    char *shortmsg, char *longmsg) 
{
  char buf[RIO_BUFSIZE], body[BUFSIZ];

  /* Build the HTTP response body */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  rio_writen(fd, buf, strlen(buf));
  rio_writen(fd, body, strlen(body));
}

/* $end clienterror */
int parse_uri(char *uri, char *hostname, char *path, int *port)
{
  char *b,*a;
  strcpy(path,"/");// the path is at least "/"
  b = strstr(uri,"http://");//check http prefix

  if(b!=NULL){ //if exist
    strcpy(hostname,b+7);//remove it and cpy rest to hostname
  } else {
    strcpy(hostname,uri); //just cpy
  }

  a = index(hostname,':'); // check for the port
  if(a!=NULL){ //port exist
    *a='\0'; //make it end so the host done
    a++; // next is port
    b=index(a,'/'); // check for the '/' after
    if(b!=NULL){ // if exist 
      *b='\0';   // make it end
      b=b+1; //rest is path
      strcat(path,b); // append rest
    }
    if (strlen(a) != 0)
      *port= atoi(a); //store the port
    else
      *port = 80;
  } else {
    *port=80; // default port
    b = index(hostname,'/'); //check /
    if(b!=NULL) { //exist
      *b='\0';
      b++;
      strcat(path,b);
    }
  }
  return 0;
}

int make_requestlines(/*int fd,*/ char *buf, char *method, char *uri, char *host, char *path, int *port) {
	//this can be use for checking for methods
    sscanf(buf, "%s %s", method, uri);
    if (strcasecmp(method, "GET")) {
        return -1;
    }
    if (strlen(uri) == 0) {
        return -1;
    }
    if (parse_uri(uri, host, path, port) < 0) {
        return -1;
    }
    return 0;
}