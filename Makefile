






CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -DDRIVER -std=gnu99  -Wno-deprecated
LDFLAGS=  -L/usr/local/opt/openssl/lib

OBJS =daemon.o netdef.o ssldef.o echoservers.o fdbuf.o log.o httpdef.o

all: liso

liso: $(OBJS)
	$(CC) $(CFLAGS) -o lisod $(OBJS) -lssl



ssldef.o: ssldef.h ssldef.c
echoservers.o: echoservers.c
fdbuf.o: fdbuf.h fdbuf.c
netdef.o: netdef.h netdef.c 
httpdef.o: httpdef.h httpdef.c
log.o: log.h log.c
daemon.o: daemon.c daemon.h

clean:
	@rm -f *.o lisod


submit:
	@make clean; cd ..; tar cvf echoserver_final.tar 15-441-project-1 

run:
	@./lisod 1234 12345 log lock www cgiS liso.key liso.crt

lldb:
	@lldb lisod 1234 12345 log lock www cgiS liso.key liso.crt

gittag:
	@git tag -f -a final -m "xxx"