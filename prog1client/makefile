prog1client.out: client.o client_functions.o 
	gcc -g -Wall -Werror -o prog1client.out client.o client_functions.o 

client.o: client.c packets.h client_functions.h
	gcc -g -Wall -Werror -c client.c

client_functions.o: client_functions.c packets.h 
	gcc -g -Wall -Werror -c client_functions.c

clean:
	rm -fr *.o *.out