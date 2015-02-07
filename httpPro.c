#include "httpPro.h"
#include "log.h"




char *get_time_str(char *time_buf)
{
    time_t    now_sec;
    struct tm    *time_now;
    if(    time(&now_sec) == -1)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);
        // perror("time() in get_time.c");
        return NULL;
    }
    if((time_now = gmtime(&now_sec)) == NULL)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);
        // perror("localtime in get_time.c");
        return NULL;
    }
    char *str_ptr = NULL;
    if((str_ptr = asctime(time_now)) == NULL)
    {
        LogWrite(SORRY, "500", "Internal Sever Error", -1);

        // perror("asctime in get_time.c");
        return NULL;
    }
    strcat(time_buf, str_ptr);
    return time_buf;
}




int reply_normal_information(unsigned char *send_buf, unsigned char *file_buf, int file_size, char *mime_type)
{
    char *str = "HTTP/1.1 200 OK\r\nServer:Mutu/Linux(0.1)\r\nDate:";
    register int index = strlen(str);
    memcpy(send_buf, str, index);

    char time_buf[TIME_BUFFER_SIZE];
    memset(time_buf, '\0', sizeof(time_buf));
    str = get_time_str(time_buf);
    int len = strlen(time_buf);
    memcpy(send_buf + index, time_buf, len);
    index += len;

    len = strlen(ALLOW);
    memcpy(send_buf + index, ALLOW, len);
    index += len;

    memcpy(send_buf + index, "\r\nContent-Type:", 15);
    index += 15;
    len = strlen(mime_type);
    memcpy(send_buf + index, mime_type, len);
    index += strlen(mime_type);

    memcpy(send_buf + index, "\r\nContent-Length:", 17);
    index += 17;
    char num_len[8];
    memset(num_len, '\0', sizeof(num_len));
    sprintf(num_len, "%d", file_size);
    len = strlen(num_len);
    memcpy(send_buf + index, num_len, len);
    index += len;

    memcpy(send_buf + index, "\r\n\r\n", 4);
    index += 4;
    

    memcpy(send_buf + index, file_buf, file_size);
    index += file_size;
    return index;

}



