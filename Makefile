






CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -g -DDRIVER -std=gnu99

OBJS = netdef.o echoservers.o httppro.o httpdef.o

all: liso

liso: $(OBJS)
	$(CC) $(CFLAGS) -o lisod $(OBJS)


echoservers.o: echoservers.c netdef.h
netdef.o: netdef.h netdef.c log.c
httpdef.o: httpdef.h httpdef.c
httppro.o: httppro.c httppro.h


clean:
	@rm -f *.o lisod


submit:
	@make clean; cd ..; tar cvf echoserver_checkpoint-1.tar 15-441-project-1 
