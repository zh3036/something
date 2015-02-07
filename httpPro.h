/*
* file: http_session.h
*/
#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <netinet/in.h>
#include "netdef.h"

#define  TIME_BUFFER_SIZE 40

#define RECV_BUFFER_SIZE    1024        /* 1KB of receive buffer */
#define    SEND_BUFFER_SIZE    1050000        /* 1.xMB of send buffer */
#define    URI_SIZE            256            /* length of uri request from client browse */

#define TIME_OUT_SEC        10            /* select timeout of secend */
#define TIME_OUT_USEC        0            /* select timeout of usecend */

#define    FILE_OK                200
#define    FILE_FORBIDEN        403            /* there are no access permission*/
#define FILE_NOT_FOUND        404            /* file not found on server */
#define    UNALLOW_METHOD        405            /* un allow http request method*/
#define FILE_TOO_LARGE        413            /* file is too large */
#define    URI_TOO_LONG        414            /* */
#define    UNSUPPORT_MIME_TYPE    415
#define    UNSUPPORT_HTTP_VERSION    505
#define    FILE_MAX_SIZE        1048576        /* 1MB the max siee of file read from hard disk */


#define ALLOW                "Allow:GET"    /* the server allow GET request method*/
#define    SERVER                "Server:Mutu(0.1 Alpha)/Linux"


char *get_time_str(char *time_buf);
#endif