
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "unistd.h"
#include "string.h"
#include "netdef.h"
#include "log.h"
void get_filetype(char *filename, char *filetype) ;
int serve_static(time_fd *tf, char *filename, struct stat *sbuf,char* method);
char *get_time_str(char *time_buf);
