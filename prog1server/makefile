prog1server.out: server.o server_functions.o 
	gcc -g -Wall -Werror -o prog1server.out server.o server_functions.o 

server.o: server.c packets.h server_functions.h
	gcc -g -Wall -Werror -c server.c

server_functions.o: server_functions.c packets.h 
	gcc -g -Wall -Werror -c server_functions.c

clean:
	rm -fr *.o *.out