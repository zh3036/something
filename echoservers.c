/* 
 *
 * Zihan Zhou Project 1 checkpoint 2
 * some functions are from the tiny.c sever
 fromm csapp book
 * asd
 */
/* $begin echoserversmain */



// #include "httpPro.h"
// #include "netdef.h"
#include "fdbuf.h"
#include "log.h"
#include "netdef.h"
#include "httpPro.h"
#include "httpdef.h"

// the struct to keep buffer to resume reading



typedef struct { /* a pool of connected descriptors */ 
    int maxfd;        /* largest descriptor in read_set */   
    fd_set read_set;  /* set of all active descriptors */
    fd_set ready_set; /* subset of descriptors ready for reading  */
    int nready;       /* number of ready descriptors from select */   
    int maxi;         /* highest index into client array */
    time_fd clientfd[FD_SETSIZE];    /* set of active descriptors */
} pool; 

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);
void serveHG(int fd,char* method, char* path);

// int full_flag=0;

#define FILENAMELENGTH 303


void serveHG(int fd,char* method, char* path){
  struct stat sbuf;
  LogWrite(LOG, method, path, fd);
  if (stat(path, &sbuf) < 0) {
    LogWrite(SORRY, "404", "FILE NOT FOUND", fd);
  }
  if (!(S_ISREG(sbuf.st_mode)) 
    || !(S_IRUSR & sbuf.st_mode)) {
    LogWrite(SORRY, "404", "FILE NOT FOUND", fd);
  }
  serve_static(fd, path, &sbuf,method);
  // serve_static(fd, path, sbuf.st_size,method);
}

void read_requesthdrs(time_fd *tf,int* conn,int *length) 
{// need to check connection header
  // content length header 
  //
  char buf[MAXLINE];
  char left[MAXLINE];
  char right[MAXLINE];
  do{
    bufreadline(tf, buf, MAXLINE);
    sscanf(buf, "%s %s",left,right);
    if(strncasecmp("connection:", left,11)==0){
      if(strncasecmp("close", right, 5)==0){
        *conn=0;
      }
      ;
    }
    if(strncasecmp("content-length:",left,14)==0){
      *length=atoi(right);
    }
    // LogWrite(LOG, "headers:", buf, tf->fd);
  } while(strcmp(buf, "\r\n"));
  return;
}



int main(int argc, char **argv)
{
  int listenfd, connfd, port,ports;

  char LogFile[FILENAMELENGTH], LockFile[FILENAMELENGTH];
  char wwwFolder[FILENAMELENGTH], cgiScript[FILENAMELENGTH];
  char privateKey[FILENAMELENGTH],certificate[FILENAMELENGTH];

  
  socklen_t clientlen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientaddr;
  static pool pool; 
  printf("%d\n", FD_SETSIZE);

  signal( SIGPIPE, SIG_IGN );
  port=1234;
  //printf("we have %d args\n",argc);
  port=port+argc;

  if(argc != 9){
  	printf("somehing wrong here..not enough argcs\
  		usage \n");
    exit(0);
  } else {
  	port = atoi(argv[1]);
  	ports = atoi(argv[2]);
    // before copy need chck the length of args
    strncpy(LogFile,argv[3],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    strncpy(LockFile, argv[4],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    strncpy(wwwFolder,argv[5],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    strncpy(cgiScript,argv[6],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    strncpy(privateKey, argv[7],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    strncpy(certificate, argv[8],FILENAMELENGTH)[FILENAMELENGTH-1]=0;
    // if (access(LogFile, W_OK) || access(LockFile, 0) \
    //   ||access(wwwFolder, R_OK)||access(cgiScript, X_OK)\
    //   ||access(privateKey, 0)||access(certificate, 0)){
    //   fprintf(stderr, "some filename is wrong, plz check\n");
    //   exit(0);
    // }
    logfilename = LogFile;
    LogWrite(LOG, "prove", "can write", -1);
    // logfile need writable
    // www folder need be readable 
    // cgiscript need be runnable
  } 
  listenfd = Open_listenfd(port);
  init_pool(listenfd, &pool);
  while (1) {
  	/* Wait for listening/connected descriptor(s) to become ready */
  	pool.ready_set = pool.read_set;

    // if you get a file descriptor with a value as high 
    //as FD_SETSIZE, you cannot put that descriptor into 
    // an fd_set.

  	pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

  	/* If listening descriptor ready, add new client to pool */
    
    if (FD_ISSET(listenfd, &pool.ready_set)) { 
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        if(connfd>=1022) LogWrite(LOG, "abnormal", "large fd", connfd);
        if(connfd<FD_SETSIZE)
          add_client(connfd, &pool);
        else{
          LogWrite(SORRY, "503", "Service Unavailable", connfd);
          close(connfd);
        }
     }
    
    check_clients(&pool); 
  }
	/* Echo a text line from each ready connected descriptor */ 
}

/* $end echoservers main */

/* $begin init_pool */
void init_pool(int listenfd, pool *p) 
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;
    for (i=0; i< FD_SETSIZE; i++)  
      p->clientfd[i].fd=-1; //-1 means it is ready

    /* Initially, listenfd is only member of select read set */
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set); 
}
/* $end init_pool */

/* $begin add_client */
void add_client(int connfd, pool *p) 
{
    int i;
    p->nready--;
      for (i = 0; i < FD_SETSIZE; i++){  /* Find an available slot */
        if (p->clientfd[i].fd < 0) { 
            /* Add connected descriptor to the pool */
            ini_fd(&(p->clientfd[i]), connfd);
            /* Add the descriptor to descriptor set */
            FD_SET(connfd, &p->read_set);
            /* Update max descriptor and pool highwater mark */
            if (connfd > p->maxfd ) 
                p->maxfd = connfd; 
            if (i > p->maxi)
              p->maxi = i;
            break;
        }
      }
   if (i == FD_SETSIZE){ /* Couldn't find an empty slot */
      //this need to further coding
      //modify logwrite to acutlly send the error header
      // need to close the connection
     LogWrite(SORRY, "503", 
      "Service Unavailable", connfd);
     close(connfd);
   }
 }
      
// }
/* $end add_client */

/* $begin check_clients */
void check_clients(pool *p) 
{
  int i, connfd,j;
  time_fd tf;
  char buf[MAXLINE]; 
  // int tem;
  char method[FILENAMELENGTH], path[FILENAMELENGTH];
  char version[FILENAMELENGTH];
  char *v;
  memset(buf, '0', MAXLINE);



  for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    tf=p->clientfd[i];
    connfd = tf.fd;
    /* If the descriptor is ready, echo a text line from it */
    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) { 
      p->nready--;
      
      for(j=0;j<5 && !isfinish_bufload(&tf);j++){
        bufload(&tf, MAXBUF);
      }

      if(isfinish_bufload(&tf) && tf.p_flag !=1){  
        //not in post mode, deal with a new request
        bufreadline(&tf, buf, MAXLINE-1);
        sscanf(buf, "%s %s %s", method, path, version);
        v = strstr(version, "HTTP/1.1");
        if (v == NULL){ //check for the version 
          LogWrite(SORRY,"505","HTTP VERSION NOT SUPPORTED", tf.fd);
        } else if (!(strstr(method,"GET") || //check for method
                           strstr(method,"POST") ||
                           strstr(method, "HEAD"))){
          LogWrite(SORRY, "501", "Not Implemented", tf.fd);
        } else {// now the version is right, method is right
          int con_len=-1,is_conn=1;
          //let us parse the header to get the content length 
          // and whether should we close the connection
          read_requesthdrs(&tf, &is_conn, &con_len); 
          if(!is_conn) {
            Close(bufdestroy(&tf));
            FD_CLR(connfd, &p->read_set);
            p->clientfd[i].fd=-1;
          } else if(method[0]=='P'){// if it is post method
            if(con_len==-1){ // if there are no length content
              LogWrite(SORRY, "411", "Length Required", tf.fd);
            } else {
              tf.p_flag=1;
              tf.pcnt=con_len;
              // here process post
            }                             
          } else{ //method here is either GET or HEAD
            if(strcmp(path,"/")==0) 
              serveHG(tf.fd,method,"index.html");
            else
              serveHG(tf.fd,method,path+1);
            //process HEAD
            // if it is GET then send also the file
          }  
        }  
      }  
      if(elap_time(&tf)>3){
        Close(bufdestroy(&tf));
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i].fd=-1;
      }
      //in post mode, need to read the data
      if(tf.p_flag==1){
        int tem;
        char post_buf[MAXBUF];
        for(int k=0;k<5 && tf.pcnt>0;k++){
          tem=bufread(&tf, post_buf, tf.pcnt);
          tf.pcnt-=tem;
        }
        if(tf.pcnt<=0) {
          tf.p_flag=0;
          LogWrite(SORRY, "200", "OK", tf.fd);
        }
      }
    }
  }

        // start processing

        // 1. read the request line  ***
        //  a. if it not HTTP/1.1   ****
        //      505_HTTP_VERSION_NOT_SUPPORTED ***
        // 2. parse the request line to get method****
        //  a. POST : then just send 200 back ***
        //    1. length required 411 ***
        //    2. receive the rest body but do nothing ***

        // 3. for HEAD and GET , we get the path***
        //  b. HEAD : then generate headers send back 
        //      1.Connection****
        //      2.Date****
        //      6. last modified
        //      3. Server : Liso/1.0 ***
        //      4. content-length ****
        //      5. content-type***
        //  c. GET : use HEAD's headers' and with filecontent   
        //  d. other: not implement 501 
        //  a. cgi --> not implemente
        //  b. find the path, if not exist->404 ****
        //  c. if exist, make the reply headers ***
        //  d. send the headers, send the body****
        //  e. if in the header comming in there are connection closed
        //     then close the connfd ****
         // end processing
        // the code for closing and destory a tf
        // bufdestroy(&tf);
        // FD_CLR(connfd, &p->read_set);
        // p->clientfd[i].fd=-1;



  
}


/* $end check_clients */

