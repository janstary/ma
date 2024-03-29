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
	./ma anbn.ma  < anbn.in  | diff - anbn.out
	./ma sort.ma  < sort.in  | diff - sort.out
	./ma succ.ma  < succ.in  | diff - succ.out
	./ma copy.ma  < copy.in  | diff - copy.out
	./ma pi1k.ma  < pi11.in  | diff - pi11.out
	./ma pi1k.ma  < pi12.in  | diff - pi12.out
	./ma pi1k.ma  < pi13.in  | diff - pi13.out
	./ma pi2k.ma  < pi22.in  | diff - pi22.out
	./ma pi2k.ma  < pi23.in  | diff - pi23.out
	./ma pi2k.ma  < pi24.in  | diff - pi24.out
	./ma pi3k.ma  < pi33.in  | diff - pi33.out
	./ma pi3k.ma  < pi34.in  | diff - pi34.out
	./ma pi3k.ma  < pi35.in  | diff - pi35.out
	./ma pikk.ma  < pi33.in  | diff - pi33.out
	./ma pikk.ma  < pi44.in  | diff - pi44.out
	./ma pikk.ma  < pi55.in  | diff - pi55.out

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
