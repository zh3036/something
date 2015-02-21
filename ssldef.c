
#include "ssldef.h"



SSL_CTX * SslInit(char* key,char* cert)
{
  SSL_CTX *ssl_context;
  SSL_load_error_strings(); 
  //this part i do not know whether i should initiate
  // them in the main function or here is fine
  SSL_library_init();
  if ((ssl_context = SSL_CTX_new(TLSv1_server_method())) == NULL)
  {
    LogWrite(ERROR, "ERROR creating SSL context", 
      "in initialization", NULL);
    return NULL;
  }

  if (SSL_CTX_use_PrivateKey_file(ssl_context, key,
                                SSL_FILETYPE_PEM) == 0)
  {
    SSL_CTX_free(ssl_context);
    LogWrite(ERROR,"wrong", "Error associating private key.\n",NULL);
    return NULL;
  }

    if (SSL_CTX_use_certificate_file(ssl_context, cert,
                                     SSL_FILETYPE_PEM) == 0)
    {
      SSL_CTX_free(ssl_context);
      unix_error("ERROR associating certificate", -1);
      return NULL;
    }
  return ssl_context;
}



