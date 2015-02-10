
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "unistd.h"
#include "string.h"
#include "netdef.h"
void get_filetype(char *filename, char *filetype) ;
int serve_static(int fd, char *filename, struct stat *sbuf,char* method);
char *get_time_str(char *time_buf);

int LogWrite( int type, char *s1, char *s2, int num);