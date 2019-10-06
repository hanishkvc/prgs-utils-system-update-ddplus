

ddplus: ddplus.c
	gcc -Wall -o ddplus.generic ddplus.c -static -g
	gcc -Wall -o ddplus ddplus.c -static -D PROCD1
	strip ddplus

clean:
	rm ddplus ddplus.generic || /bin/true

testa0:
	dd if=/dev/urandom of=/tmp/sdb bs=1K count=1 seek=5M
	touch /tmp/sda
	time ./ddplus /tmp ext sam 0 0 4000000000 ext 1024 0xa5
	mv /tmp/sda /tmp/sda.1024
	touch /tmp/sda
	time ./ddplus /tmp ext sam 0 0 4000000000 ext 4096 0xa5
	mv /tmp/sda /tmp/sda.4096
	touch /tmp/sda
	time ./ddplus /tmp ext sam 0 0 4000000000 ext 8192 0xa5
	mv /tmp/sda /tmp/sda.8192

testa1:
	rm /tmp/sda /tmp/sdb /tmp/sdb.orig || /bin/true
	dd if=/dev/urandom of=/tmp/sdb bs=1KB count=1 seek=2MB
	cp /tmp/sdb /tmp/sdb.orig
	touch /tmp/sda
	time ./ddplus /tmp extr sam 0 0 2000001000 extr 8192 423
	rm /tmp/sdb
	touch /tmp/sdb
	time ./ddplus /tmp sam ext 0 0 2000001000 extr 8192 423
	cmp /tmp/sdb /tmp/sdb.orig

testa2:
	rm /tmp/sda /tmp/sdb /tmp/sdb.orig || /bin/true
	dd if=/dev/urandom of=/tmp/sdb bs=1KB count=1MB seek=1MB
	cp /tmp/sdb /tmp/sdb.orig
	touch /tmp/sda
	echo "TESTA2:Encode..."
	time ./ddplus /tmp ext sam 0 0 2000000000 ext 8192 191717
	rm /tmp/sdb
	touch /tmp/sdb
	time ./ddplus /tmp sam ext 0 0 2000000000 sam 8192 191717
	echo "TESTA2:md5sums should not match below"
	md5sum /tmp/sdb /tmp/sdb.orig
	rm /tmp/sdb
	touch /tmp/sdb
	time ./ddplus /tmp sam ext 0 0 2000000000 ext 8192 191717
	echo "TESTA2:md5sums should match below"
	md5sum /tmp/sdb /tmp/sdb.orig

