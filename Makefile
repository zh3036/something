
default: echo_server

echo_server:
	@gcc echoservers.c -o liso -Wall -Werror


clean:
	@rm liso 
