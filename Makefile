PROGNAME = suid-wrap
DESTDIR := /usr/local/bin

BINGRP = root
BINOWN = root
BINMODE = 0555

CC = gcc
CFLAGS = -Wall -Os

LD = ld
LDFLAGS = -Os
LDLIBS =

INSTALL = install
INSTALL_PROGRAM = ${INSTALL} -s
INSTALL_DATA = ${INSTALL}

SOURCES = suid-wrap.c
OBJECTS = ${SOURCES:%.c=%.o}

##
## RULES
##

.PHONY: all clean install uninstall distclean

all: ${PROGNAME}

${PROGNAME}: ${OBJECTS}
	${CC} ${LDFLAGS} $^ -o $@ ${LDLIBS}

perms:
	chown root.root ${PROGNAME}
	chmod u+s ${PROGNAME}

clean:
	rm -f *~ ${OBJECTS} ${PROGNAME}

distclean: clean

install: all
	${INSTALL} -d ${DESTDIR}
	${INSTALL_PROGRAM} -g ${BINGRP} -o ${BINOWN} -m ${BINMODE} ${PROGNAME} ${DESTDIR}
	chmod u+s ${DESTDIR}/${PROGNAME}

uninstall:
	rm -f ${DESTDIR}/${PROGNAME}

