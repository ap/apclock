include Metadata

BINDIR = /usr/X11R6/bin

PROG = apclock

CFLAGS := $(if $(RPM_OPT_FLAGS), $(RPM_OPT_FLAGS), $(CFLAGS))
CFLAGS += -Wimplicit -Wall -std=gnu99 -pedantic-errors
CFLAGS += -I/usr/X11R6/include
CFLAGS += $(shell pkg-config --cflags cairo)

LDFLAGS := -lm
LDFLAGS += -L/usr/X11R6/lib -lXext -lX11
LDFLAGS += $(shell pkg-config --libs cairo)

SRC = main.c hand.c cairoutil.c dockapp.c

EXTRAS = $(addprefix $(EXENAME), .pod .pod.in .1) \
		 twingauge.svg twingauge.png Makefile Metadata replace # COPYING ChangeLog

.PHONY: all
all: $(EXENAME) $(EXENAME).1

$(EXENAME): $(SRC:.c=.o)
	$(CC) $(LDFLAGS) $^ -o $@

default_bg.png: twingauge.png
	ln -s $< $@

%.png: %.svg
	rsvg-convert -o $@ $^

%.png_c: %.png
	od -vtuC $^ \
	| sed 's/^[0-9]\+//; s/\([0-9]\+\)/\1,/g; /^ *$$/d' \
	| sed '1s/^/#ifndef ___$*_H\n#define ___$*_H\nunsigned char $*_png[] = {\n/; $$s/,*$$/ };\n#endif/;' \
	> $@

%.1: %.pod
	pod2man $< | sed 's/^\.TH .*/.TH $(EXENAME) 1/' > $@

%.pod: %.pod.in Metadata
	./replace Metadata $< > $@ || rm $@

%.spec: %.spec.in Metadata
	./replace Metadata $< > $@ || rm $@

Makefile.depend: $(SRC)
	$(CC) -MM -MG $^ > $@

.PHONY: clean
clean:
	rm -f $(EXENAME) $(SRC:.c=.o)

.PHONY: distclean
distclean: clean
	-rm -f *.spec *.pod *.png Makefile.depend

.PHONY: realclean
realclean: distclean
	-rm -f *.png_c *.png *.1 Makefile.depend

.PHONY: install
install: all
	strip      $(EXENAME)
	install -d              $(DESTDIR)/$(BINDIR)
	install -c $(EXENAME)   $(DESTDIR)/$(BINDIR)
	install -d              $(DESTDIR)/$(MANDIR)/man1
	install -c $(EXENAME).1 $(DESTDIR)/$(MANDIR)/man1

DISTFILES = $(SRC:.c=.h) $(SRC) $(EXTRAS)
DISTNAME = $(EXENAME)-$(VERSION)

.PHONY: dist
dist: $(DISTFILES)
	rm -rf /tmp/$(DISTNAME)
	mkdir /tmp/$(DISTNAME)
	cp -a $(DISTFILES) /tmp/$(DISTNAME)/
	tar cvjf $(DISTNAME).tar.bz2 -C /tmp $(DISTNAME)/
	rm -rf /tmp/$(DISTNAME)

.PHONY: rpm
rpm: dist $(EXENAME).spec
	rpmbuild -ta $(DISTNAME).tar.bz2

include Makefile.depend
