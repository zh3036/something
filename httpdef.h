
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "unistd.h"
#include "string.h"
#include "netdef.h"
#include "log.h"
#include "httpPro.h"

void get_filetype(char *filename, char *filetype) ;
void serve_static(int fd, char *filename, struct stat *sbuf,char* method);
