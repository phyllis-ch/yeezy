yz: main.c yz.c
	cc -Wall -Wextra -o yz main.c yz.c -lm

install: yz
	mkdir -p /usr/local/bin
	cp -f yz /usr/local/bin
	chmod 755 /usr/local/bin/yz

uninstall:
	rm -f /usr/local/bin/yz

clean:
	rm -f ./yz

.PHONY: yz install uninstall clean
