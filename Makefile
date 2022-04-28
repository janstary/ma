PREFIX	= $(HOME)
BINDIR	= $(PREFIX)/bin
MANDIR	= $(PREFIX)/man/man1
CFLAGS	= -Wall -pedantic -D_OPENBSD_SOURCE

all: ma
tm: ma.c ma.h
	$(CC) $(CFLAGS) -o tm tm.c

test: ma
	./ma rev.ma   < rev.in   | diff - rev.out
	./ma add.ma   < add.in   | diff - add.out
	./ma sub.ma   < sub.in   | diff - sub.out
	./ma mul.ma   < mul.in   | diff - mul.out
	./ma gcd.ma   < gcd.in   | diff - gcd.out
	./ma succ.ma  < succ.in  | diff - succ.out
	./ma copy.ma  < copy.in  | diff - copy.out

lint: ma.1
	mandoc -Tlint -Wstyle ma.1

install: test ma.1
	install -d -m 755 $(BINDIR) && install -m 755 ma   $(BINDIR)
	install -d -m 755 $(MANDIR) && install -m 644 ma.1 $(MANDIR)

uninstall:
	rm -f $(BINDIR)/ma
	rm -f $(MANDIR)/ma.1

clean:
	rm -f ma *.o *.core *~
