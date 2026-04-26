yz: main.c yz.c
	cc -Wall -Wextra -o yz main.c yz.c

install: yz
	mkdir -p /usr/local/bin
	cp -f yz /usr/local/bin
	chmod 755 /usr/local/bin/yz

uninstall:
	rm -f /usr/local/bin/yz

.PHONY: yz install uninstall
