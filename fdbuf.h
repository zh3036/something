#ifndef FDBUF
#define FDBUF 010

// #include "netdef.h"
#include "assert.h"
#include "timefd.h"

int elap_time_load(time_fd* tf);
int elap_time_body(time_fd* tf);
int ini_time_load(time_fd* tf);
int ini_time_body(time_fd* tf);
int elap_time(time_fd* tf);
void ini_fd(time_fd* tf, int fd,char secure);
int Bufload(time_fd* tf,size_t n);
int secure_bufload(time_fd* tf,size_t n);
int bufload(time_fd* tf,size_t n);
int bufread(time_fd* src_tf, char* dst_buf  ,size_t n);
int bufdestroy(time_fd* tf);
int isfinish_bufload(time_fd* tf);
int bufreadline(time_fd* src_tf,char* dst_buf ,size_t maxlen);
#endif