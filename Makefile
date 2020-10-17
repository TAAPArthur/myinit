# sinit version
VERSION = 1.1

# paths
PREFIX = /usr/
MANPREFIX = $(PREFIX)/share/man

CC = cc
CFLAGS   = -Wextra -Wall -Os
LDFLAGS  = -s -static

OBJ = sinit.o
BIN = sinit

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	cp -f bin/* $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(MANPREFIX)/man8
	sed "s/VERSION/$(VERSION)/g" < $(BIN).8 > $(DESTDIR)$(MANPREFIX)/man8/$(BIN).8

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(MANPREFIX)/man8/$(BIN).8

clean:
	rm -f $(BIN) $(OBJ)

.PHONY:
	all install uninstall clean
