

ddplus: ddplus.c
	gcc -Wall -o ddplus ddplus.c -static -g
	gcc -Wall -o ddplus ddplus.c -static
	strip ddplus

clean:
	rm ddplus

testprep:
	dd if=/dev/urandom of=/tmp/sdb bs=1K count=1 seek=5M
	touch /tmp/sda

test:
	time ./ddplus /tmp ext sam 0 0 4000000000

test1:
	rm /tmp/sda /tmp/sdb /tmp/sdb.orig || /bin/true
	dd if=/dev/urandom of=/tmp/sdb bs=1KB count=1 seek=2MB
	dd if=/dev/urandom of=/tmp/sdb bs=1KB count=1MB seek=1MB
	cp /tmp/sdb /tmp/sdb.orig
	touch /tmp/sda
	time ./ddplus /tmp ext sam 0 0 2000001000
	rm /tmp/sdb
	touch /tmp/sdb
	time ./ddplus /tmp sam ext 0 0 2000001000
	cmp /tmp/sdb /tmp/sdb.orig

