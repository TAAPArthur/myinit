# sinit version
VERSION = 1.1

# paths
PREFIX = /usr/
MANPREFIX = $(PREFIX)/share/man

CC = cc
CFLAGS   = -Wextra -Wall -Os
LDFLAGS  = -s -static

BIN = myinit

all: $(BIN)


$(BIN): $(BIN).o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

install: all
	install -D -t $(DESTDIR)$(PREFIX)/bin  $(BIN)
	(cd $(DESTDIR)$(PREFIX)/bin; ln -sf $(BIN) init )
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
