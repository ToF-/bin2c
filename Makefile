bin2c:  bin2c.c
	cc bin2c.c -o bin/bin2c

install: bin2c
	cp bin/bin2c /usr/local/bin/.

clean:
	rm bin/bin2c
