/* 
 *
 * Zihan Zhou Project 1 checkpoint 1
 * echoserver based on select
 * 
 * I read several times the select server in CSAPP
 * and then write it without the book
 * and then during debugging find that my bug are those 
 * parts that are different from the code in the book.......
 * some wrapper function are borrowed from csapp.h
 *
 *
 *
 * echoservers.c - A concurrent echo server based on select
 */
/* $begin echoserversmain */

#include "netdef.h"


typedef struct { /* a pool of connected descriptors */ 
    int maxfd;        /* largest descriptor in read_set */   
    fd_set read_set;  /* set of all active descriptors */
    fd_set ready_set; /* subset of descriptors ready for reading  */
    int nready;       /* number of ready descriptors from select */   
    int maxi;         /* highest index into client array */
    int clientfd[FD_SETSIZE];    /* set of active descriptors */
} pool; 

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);

int full_flag=0;

#define FILENAMELENGTH 100

int main(int argc, char **argv)
{
    int listenfd, connfd, port,ports,i;

    char LogFile[FILENAMELENGTH], LockFile[FILENAMELENGTH];
    char wwwFolder[FILENAMELENGTH], cgiScript[FILENAMELENGTH];
    char privateKey[FILENAMELENGTH],certificate[FILENAMELENGTH];

    
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    static pool pool; 


    port=1234;
    //printf("we have %d args\n",argc);
    port=port+argc;

    if(argc != 9){
    	printf("somehing wrong here..not enough argcs\
    		usage \n");
    } else 
    {
    	port = atoi(argv[1]);
    	ports = atoi(argv[2]);
      strcpy(LogFile,argv[3]);
      strcpy(LockFile, argv[4]);
      strcpy(wwwFolder,argv[5]);
      strcpy(cgiScript,argv[6]);
      strcpy(privateKey, argv[7]);
      strcpy(certificate, argv[8]);
      
    } 
    



    /*if (argc == 2) {*/
      /*port = atoi(argv[1]);*/
    /*}else if(argc==3){*/
      /*port = atoi(argv[1]);*/
    /*}else{*/
      /*fprintf(stderr, "usage: %s <port>\n", argv[0]);*/
      /*//exit(0);*/
    /*}*/


    listenfd = Open_listenfd(port);
    init_pool(listenfd, &pool);
    while (1) {
	/* Wait for listening/connected descriptor(s) to become ready */
	pool.ready_set = pool.read_set;
	pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

	/* If listening descriptor ready, add new client to pool */
  if (full_flag==0){
    if (FD_ISSET(listenfd, &pool.ready_set)) { 
        //connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
          //add_client(connfd, &pool);
    pool.nready--;
      for (i = 0; i < FD_SETSIZE; i++){  /* Find an available slot */
        if (pool.clientfd[i] < 0) { 
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            /* Add connected descriptor to the pool */
            pool.clientfd[i] = connfd;
            /* Add the descriptor to descriptor set */
            FD_SET(connfd, &pool.read_set);
            /* Update max descriptor and pool highwater mark */
            if (connfd > pool.maxfd) 
              pool.maxfd = connfd; 
            if (i > pool.maxi)
              pool.maxi = i;
            break;
        }
      }
   if (i == FD_SETSIZE){ /* Couldn't find an empty slot */
     full_flag=1;
   }
    }
    check_clients(&pool); 
  }
	/* Echo a text line from each ready connected descriptor */ 
    }
}
/* $end echoservers main */

/* $begin init_pool */
void init_pool(int listenfd, pool *p) 
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;
    for (i=0; i< FD_SETSIZE; i++)  
      p->clientfd[i] = -1;//-1 means it is ready

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
        if (p->clientfd[i] < 0) { 
            /* Add connected descriptor to the pool */
            p->clientfd[i] = connfd;
            /* Add the descriptor to descriptor set */
            FD_SET(connfd, &p->read_set);
            /* Update max descriptor and pool highwater mark */
            if (connfd > p->maxfd) 
              p->maxfd = connfd; 
            if (i > p->maxi)
              p->maxi = i;
            break;
        }
      }
   if (i == FD_SETSIZE){ /* Couldn't find an empty slot */
     full_flag=1;
   }
      
}
/* $end add_client */

/* $begin check_clients */
void check_clients(pool *p) 
{
    int i, connfd, n;
    char buf[MAXLINE]; 
    int tem;

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
      connfd = p->clientfd[i];
      /* If the descriptor is ready, echo a text line from it */
      if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) { 
          p->nready--;
          tem=0;
          if ((n = recv(connfd, buf, MAXLINE,0)) >1) {
            //printf("Server received %d  bytes on fd %d\n", 
            //   n,  connfd);
            while(n>0){
              tem=send(connfd, buf+tem, n-tem,0); 
              n=n-tem;
            }
          }
          /* EOF detected, remove descriptor from pool */
          else { 
            Close(connfd); 
            FD_CLR(connfd, &p->read_set); 
            p->clientfd[i] = -1;
          }
      }
   }
}
/* $end check_clients */

