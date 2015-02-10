






CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -DDRIVER -std=gnu99

OBJS =netdef.o echoservers.o fdbuf.o log.o httpdef.o

all: liso

liso: $(OBJS)
	$(CC) $(CFLAGS) -o lisod $(OBJS)


echoservers.o: echoservers.c 
fdbuf.o: fdbuf.h fdbuf.c
netdef.o: netdef.h netdef.c 
httpdef.o: httpdef.h httpdef.c
log.o: log.h log.c

clean:
	@rm -f *.o lisod


submit:
	@make clean; cd ..; tar cvf echoserver_checkpoint-2.tar 15-441-project-1 

run:
	@./lisod 1234 12345 a b c d e f

lldb:
	@lldb lisod 1234 12345 a b c d e f
