PREFIX = /usr/local

SRC = main.c db.c fuzzy.c hashmap.c
OBJ = ${SRC:.c=.o}

CC = cc
CFLAGS = -std=c99 -Wall -Wextra
LDFLAGS = -lm

all: yeezy

.c.o:
	${CC} -c ${CFLAGS} $<

yeezy: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f yeezy ${OBJ}

install: all
	mkdir -p ${PREFIX}/bin
	cp -f yeezy ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/yeezy

uninstall:
	rm -f ${PREFIX}/bin/yeezy

.PHONY: all yeezy clean install uninstall
