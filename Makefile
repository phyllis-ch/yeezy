PREFIX = /usr/local

SRC = main.c cmd.c db.c fuzzy.c
OBJ = ${SRC:.c=.o}

# CFLAGS = -g -std=c99 -Wall -Wextra -pedantic -O0
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2 ${DFLAGS}
LDFLAGS = -lm

CC = cc

all: yeezy

$(OBJ): config.h

.c.o:
	${CC} -c ${CFLAGS} $<

config.h:
	cp config.def.h $@

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
