# sinit version
VERSION = 1.1

# paths
PREFIX = /usr/
MANPREFIX = $(PREFIX)/share/man

CC = cc
CFLAGS   = -Wextra -Wall -Os

BIN = myinit
UTILS_SRC = $(wildcard util/*.c)
UTILS = $(UTILS_SRC:.c=)

all: $(BIN) $(UTILS)


$(BIN): $(BIN).o
	$(CC) -s -static $(LDFLAGS) -o $@ $^ $(LDLIBS)

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

install: all
	install -D -t $(DESTDIR)$(PREFIX)/bin  $(BIN) $(UTILS) bin/*
	ln -sf $(BIN) $(DESTDIR)$(PREFIX)/bin/init
	mkdir -p $(DESTDIR)$(MANPREFIX)/man8
	sed "s/VERSION/$(VERSION)/g" < $(BIN).8 > $(DESTDIR)$(MANPREFIX)/man8/$(BIN).8

uninstall:
	(cd $(DESTDIR)$(PREFIX)/bin/; rm -f $(UTILS))
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(MANPREFIX)/man8/$(BIN).8

clean:
	rm -f $(BIN) $(UTILS)
	find . -name '*.o' -exec rm {} +

.PHONY:
	all install uninstall clean
