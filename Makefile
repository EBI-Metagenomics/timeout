.POSIX:

TIMEOUT_VERSION := 0.0.1

CC := gcc
CFLAGS := $(CFLAGS) -std=c11 -O3 -Wall -Wextra

SRC := argless.c fatal.c mssleep.c strto.c timeout.c warn.c
OBJ := $(SRC:.c=.o)

PREFIX ?= $(HOME)


all: timeout

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $<

argless.o: argless.h
fatal.o: fatal.h
mssleep.o: mssleep.h
strto.o: strto.h
timeout.o: argless.h
warn.o: warn.h

timeout: $(OBJ)
	$(CC) -o $@ $(OBJ)

test check: timeout
	rm -f stdin stdout stderr pid
	./timeout sleep 10
	sleep 1
	tee -a >stdin &
	tail -f stdout &
	tail -f stderr &
	sleep 1
	ps -p `cat pid`
	kill `cat pid`
	rm -f stdin stdout stderr pid


dist:
	mkdir -p timeout-$(TIMEOUT_VERSION)
	cp -R LICENSE Makefile README.md argless.c argless.h timeout.c timeout-$(TIMEOUT_VERSION)
	tar -cf - timeout-$(TIMEOUT_VERSION) | gzip > timeout-$(TIMEOUT_VERSION).tar.gz
	rm -rf timeout-$(TIMEOUT_VERSION)

distclean:
	rm -f timeout-*.tar.gz

ACTUAL = $(shell ls *.tar.gz)
DESIRED = $(shell echo "timeout-"`./timeout --version | cut -f 3 -d' '`".tar.gz")


distcheck: dist timeout
	test $(ACTUAL) = $(DESIRED)

clean: distclean
	rm -f timeout $(OBJ) stdin stdout stderr pid

install: timeout
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f timeout $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/timeout

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/timeout

.PHONY: all clean dist distclean test distcheck check install uninstall
