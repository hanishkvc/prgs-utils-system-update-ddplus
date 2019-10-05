

ddplus: ddplus.c
	gcc -Wall -o ddplus ddplus.c -static
	gcc -Wall -o ddplus ddplus.c -static -g

clean:
	rm ddplus

testprep:
	dd if=/dev/urandom of=/tmp/sdb bs=1K count=1 seek=5M
	touch /tmp/sda

test:
	time ./ddplus /tmp ext sam 0 0 4000000000

