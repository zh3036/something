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
#include "netdef.h"
#include "httpdef.h"
#include "ssldef.h"
#include "daemon.h"

// the struct to keep buffer to resume reading



time_fd tem_tf;

static Pool pool; 

void init_pool(int s_listenfd,int listenfd, Pool *p);
void add_client(int connfd, Pool *p);
void add_secure_client(int connfd, Pool *p,SSL_CTX* ssl_context);
SSL* wrap_ssl_socket(SSL_CTX* ssl_context, int client_sock);
void check_clients(Pool *p);
void serveHG(time_fd *tf,char* method, char* path);
void read_requesthdrs(time_fd *tf,int* conn,int *length) ;
void LogWriteHandle(int type, char *s1, char *s2, time_fd *tf);
// int full_flag=0;

#define FILENAMELENGTH 303
#define LOADTIME 5
#define MAGICLENGTH -237774

int consume;


int main(int argc, char **argv)
{
  int secure_listenfd,listenfd, connfd, port,secure_port;
  SSL_CTX *ssl_context;
  char LogFile[FILENAMELENGTH], LockFile[FILENAMELENGTH];
  char wwwFolder[FILENAMELENGTH], cgiScript[FILENAMELENGTH];
  char privateKey[FILENAMELENGTH],certificate[FILENAMELENGTH];

  
  socklen_t clientlen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientaddr;
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
  	secure_port = atoi(argv[2]);
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
    wwwfolder=wwwFolder;
    cgi=cgiScript;
    LogWrite(LOG, "prove", "canWrite", NULL);
    // logfile need writable
    // www folder need be readable 
    // cgiscript need be runnable
  } 
  // daemonize(LockFile);


  ssl_context = SslInit(privateKey, certificate);
  if(ssl_context==NULL)
  {
    printf("failed to establish ssl_context\n");
    return -1;
  }
  listenfd = Open_listenfd(port);
  secure_listenfd=Open_listenfd(secure_port);
  init_pool(secure_listenfd,listenfd, &pool);
  while (1) {
  	/* Wait for listening/connected descriptor(s) to become ready */
  	pool.ready_set = pool.read_set;

    // if you get a file descriptor with a value as high 
    //as FD_SETSIZE, you cannot put that descriptor into 
    // an fd_set.

    struct timeval timeout;
    timeout.tv_sec=3;

  	pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, &timeout);

  	/* If listening descriptor ready, add new client to pool */
    
    if (FD_ISSET(listenfd, &pool.ready_set)) { 
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      tem_tf.fd=connfd;
      if(connfd>=1022) LogWrite(LOG, "abnormal", "large fd", &tem_tf);
      if(connfd<FD_SETSIZE)
        add_client(connfd, &pool);
      else
      {
        LogWrite(SORRY, "503", "Service Unavailable", &tem_tf);
        close(connfd);
      }
    }
    if (FD_ISSET(secure_listenfd, &pool.ready_set)) { 
      printf("https connection comming\n");
      connfd = Accept(secure_listenfd, (SA *)&clientaddr, &clientlen);
      printf("https connection comming2\n");
      tem_tf.fd=connfd;
      if(connfd>=1022) LogWrite(LOG, "abnormal", "large fd", &tem_tf);
      if(connfd<FD_SETSIZE)
      {
        printf("before the call of add secure\n");
        add_secure_client(connfd, &pool,ssl_context);
      }
      else
      {
        LogWrite(SORRY, "503", "Service Unavailable", &tem_tf);
        close(connfd);
      }
    }

    check_clients(&pool); 
  }
	/* Echo a text line from each ready connected descriptor */ 
}

/* $end echoservers main */

/* $begin init_pool */
void init_pool(int s_listenfd,int listenfd, Pool *p) 
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;
    for (i=0; i< FD_SETSIZE; i++)  
      p->clientfd[i].fd=-1; //-1 means it is ready

    /* Initially, listenfd and slistenfd is only member of select read set */
    p->maxfd = listenfd;
    if(s_listenfd>listenfd)
      p->maxfd=s_listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set); 
    FD_SET(s_listenfd, &p->read_set);
}
/* $end init_pool */

/* $begin add_client */
void add_client(int connfd, Pool *p) 
{
    int i;
    p->nready--;
      for (i = 0; i < FD_SETSIZE; i++){  /* Find an available slot */
        if (p->clientfd[i].fd < 0) { 
            /* Add connected descriptor to the pool */
            ini_fd(&(p->clientfd[i]), connfd,NORMAL);
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
      tem_tf.fd=connfd;
     LogWrite(SORRY, "503", 
      "Service Unavailable", &tem_tf);
     close(connfd);
   }
 }

void add_secure_client(int connfd, Pool *p,SSL_CTX* ssl_context)
{
  printf("start to add secure client\n");
  int i;
  p->nready--;
  for (i = 0; i < FD_SETSIZE; i++){  /* Find an available slot */
    if (p->clientfd[i].fd < 0) { 
        /* Add connected descriptor to the pool */
        ini_fd(&(p->clientfd[i]), connfd,SECURE);
        p->clientfd[i].client_context = wrap_ssl_socket(ssl_context, connfd);
        if(p->clientfd[i].client_context ==NULL)
        {//check if the ssl connection is properly set up or not
          Close(bufdestroy(&(p->clientfd[i])));
          FD_CLR(connfd, &p->read_set);
          p->clientfd[i].fd=-1;
          break;
        }
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
  if (i == FD_SETSIZE)
  { /* Couldn't find an empty slot */
    //this need to further coding
    //modify logwrite to acutlly send the error header
    // need to close the connection
    tem_tf.fd=connfd;
    LogWrite(SORRY, "503", 
    "Service Unavailable", &tem_tf);
    close(connfd);
  }
}
      
// }
/* $end add_client */

/* $begin check_clients */
void check_clients(Pool *p) 
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
    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) 
    { 
      p->nready--;
      for(j=0;j<LOADTIME && !isfinish_bufload(&tf);j++)
      {
        if(Bufload(&tf, MAXBUF)<0)
        {
          Close(bufdestroy(&tf));
          FD_CLR(connfd, &p->read_set);
          p->clientfd[i].fd=-1;     
          break; 
        }

        // time out needed here
        //check for incompete request
      }

      if(isfinish_bufload(&tf) && tf.p_flag !=1){  
        printf("prepare look at the buffer:\n");
        printf("%s\n",tf.header_buf->buffer);
        //not in post mode, deal with a new request
        bufreadline(&tf, buf, MAXLINE-1);
        sscanf(buf, "%s %s %s", method, path, version);
        v = strstr(version, "HTTP/1.1");
        if (v == NULL){ //check for the version 
          LogWriteHandle(SORRY,"505","HTTP VERSION NOT SUPPORTED", &tf);
          read_requesthdrs(&tf, &consume, &consume);          
        } else if (!(strstr(method,"GET") || //check for method
                           strstr(method,"POST") ||
                           strstr(method, "HEAD"))){
          LogWriteHandle(SORRY, "501", "Not Implemented", &tf);
          read_requesthdrs(&tf, &consume, &consume);
        } else {// now the version is right, method is right
          int con_len=MAGICLENGTH,is_conn=1;
          //let us parse the header to get the content length 
          // and whether should we close the connection
          read_requesthdrs(&tf, &is_conn, &con_len); 
          if(!is_conn) {
            Close(bufdestroy(&tf));
            FD_CLR(connfd, &p->read_set);
            p->clientfd[i].fd=-1;
          } 
          else if(method[0]=='P')
          {// if it is post method
            if(con_len<=-1 && con_len != MAGICLENGTH)
            { 
            // if there are no length content
              LogWriteHandle(SORRY, "411", "Length Required", &tf);
            } 
            else
            {
              tf.p_flag=1;
              tf.pcnt=con_len;
              // here process post
            }                             
          } 
          else
          { //method here is either GET or HEAD
            if(con_len != MAGICLENGTH)
            {
              if(con_len<=-1)
              { 
                printf("bad length on get or head\n");
              // if there are no length content
                LogWriteHandle(SORRY, "400", "BAD REQUEST", &tf);
              } 
              else
              {
                tf.p_flag=1;
                tf.pcnt=con_len;
                // here process post
              } 
            } 
            else if(strstr(path, "cgi"))
            {
              LogWriteHandle(SORRY, "501", "Not Implemented", &tf);
            }
            else
            {
              char PATH[MAXBUF];
              bzero(PATH,MAXBUF-1);
              sprintf(PATH,"%s/",wwwfolder);
              if (path[0]=='/')
              {
                strcat(PATH, path+1);
              }
              else 
              {
                strcat(PATH, path);
              }
              if (PATH[strlen(PATH)-1]=='/')
              {
                strcat(PATH, "index.html");
              }
              serveHG(&tf,method,PATH);
            }
          }  
        }  
      }  
      // if(elap_time(&tf)>3){
      //   Close(bufdestroy(&tf));
      //   FD_CLR(connfd, &p->read_set);
      //   p->clientfd[i].fd=-1;
      // }
      //in post mode, need to read the data
      if(tf.p_flag==1){
        int tem;
        char post_buf[MAXBUF];
        for(int k=0;k<5 && tf.pcnt>0;k++){
          tem=bufread(&tf, post_buf, tf.pcnt);
          tf.pcnt-=tem;
        }
        //time out for body can be added here
        if(tf.pcnt<=0) {
          tf.p_flag=0;
          LogWriteHandle(SORRY, "200", "OK", &tf);
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

void LogWriteHandle(int type, char *s1, char *s2, time_fd* tf){
  int ret;
  ret = LogWrite(type, s1, s2, tf);
  if(ret==-1)
  {
    Close(bufdestroy(tf));
    FD_CLR(tf->fd, &pool.read_set);
    tf->fd=-1;
  }
}


void serveHG2(time_fd *tf,char* method, char* path){
  int ret;
  struct stat sbuf;
  stat(path, &sbuf);
  LogWrite(LOG, method, path, tf);
  if(access(path, R_OK)==-1) {
    LogWriteHandle(SORRY, "404", "FILE NOT FOUND", tf);
    return;
  }
  ret=serve_static(tf, path, &sbuf,method);
  if(ret==-1)
  {
    Close(bufdestroy(tf));
    FD_CLR(tf->fd, &pool.read_set);
    tf->fd=-1;
  }
}

void serveHG(time_fd *tf,char* method, char* path){
  struct stat sbuf;
  int ret;
  LogWrite(LOG, method, path, tf);
  if (stat(path, &sbuf) < 0) {
    LogWriteHandle(SORRY, "404", "FILE NOT FOUND", tf);
    return;
  }
  if (!(S_ISREG(sbuf.st_mode)) 
    || !(S_IRUSR & sbuf.st_mode)) {
    LogWriteHandle(SORRY, "404", "FILE NOT FOUND", tf);
    return;
  }
  ret=serve_static(tf, path, &sbuf,method);
  if(ret==-1)
  {
    Close(bufdestroy(tf));
    FD_CLR(tf->fd, &pool.read_set);
    tf->fd=-1;
  }
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
    }
    if(strncasecmp("content-length:",left,14)==0){
      for (unsigned long ic = 0; ic < strlen(right); ++ic)
      { 
        if(!isdigit(right[ic]))
        {
          *length=-1;
          return;
        }
      }
      *length=atoi(right);
    }
    // LogWriteHandle(LOG, "headers:", buf, tf->fd);
  } while(strcmp(buf, "\r\n"));
  return;
}

SSL* wrap_ssl_socket(SSL_CTX* ssl_context, int client_sock)
{
  SSL* client_context;
  tem_tf.fd=client_sock;
  if ((client_context = SSL_new(ssl_context)) == NULL)
  {
    // close(sock);
    SSL_CTX_free(ssl_context);
    LogWrite(ERROR, "creating client SSL context", "ssl_new", &tem_tf);
    return NULL;
  }

  if (SSL_set_fd(client_context, client_sock) == 0)
  {
    // close(sock);
    SSL_free(client_context);
    SSL_CTX_free(ssl_context);
    LogWrite(ERROR, "creating client SSL context", "ssl_set_fd", &tem_tf);
    return NULL;
  }  

  if (SSL_accept(client_context) <= 0)
  {
    // close(sock);
    SSL_free(client_context);
    SSL_CTX_free(ssl_context);
    LogWrite(ERROR, "Error accepting (handshake) \
                    client SSL context", "ssl_accpet", &tem_tf);
    return NULL;
  }
  return client_context;
}


/* $end check_clients */

