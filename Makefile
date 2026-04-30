yeezy: main.c yz.c
	cc -Wall -Wextra -o yeezy main.c yz.c -lm

install: yz
	mkdir -p /usr/local/bin
	cp -f yeezy /usr/local/bin
	chmod 755 /usr/local/bin/yeezy

uninstall:
	rm -f /usr/local/bin/yeezy

clean:
	rm -f ./yeezy

.PHONY: yeezy install uninstall clean
