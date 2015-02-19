#ifndef SSLDEF
#define SSLDEF 534
#include "lib.h"
#include "log.h"


SSL_CTX * SslInit(char* key,char* cert);
void add_secure_client(int connfd, Pool *p);




#endif