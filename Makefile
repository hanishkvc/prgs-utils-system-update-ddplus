

ddplus: ddplus.c
	gcc -Wall -o ddplus ddplus.c -static
	gcc -Wall -o ddplus ddplus.c -static -g

clean:
	rm ddplus

