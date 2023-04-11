PREFIX	= $(HOME)
BINDIR	= $(PREFIX)/bin
MANDIR	= $(PREFIX)/man/man1
CFLAGS	= -Wall -pedantic

all: ma
ma: ma.c ma.h
	$(CC) $(CFLAGS) -o ma ma.c

test: ma
	./ma rev.ma   < rev.in   | diff - rev.out
	./ma add.ma   < add.in   | diff - add.out
	./ma sub.ma   < sub.in   | diff - sub.out
	./ma mul.ma   < mul.in   | diff - mul.out
	./ma gcd.ma   < gcd.in   | diff - gcd.out
	./ma succ.ma  < succ.in  | diff - succ.out
	./ma copy.ma  < copy.in  | diff - copy.out
	./ma pi1k.ma  < pi12.in  | diff - pi12.out
	./ma pi1k.ma  < pi13.in  | diff - pi13.out
	./ma pi23.ma  < pi23.in  | diff - pi23.out
	./ma pikk.ma  < pi33.in  | diff - pi33.out
	./ma pikk.ma  < pi44.in  | diff - pi44.out

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
