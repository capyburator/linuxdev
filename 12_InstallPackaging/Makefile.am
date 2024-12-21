SUBDIRS = tests

RU = ru/LC_MESSAGES

bin_PROGRAMS=guess-number
pkgdata_DATA=$(RU)/$(PACKAGE).mo

all: $(RU)/$(PACKAGE).mo doxygen-doc

$(RU)/$(PACKAGE).mo: ru.po
	mkdir -p `dirname $@`
	msgfmt $< -o $@

ru.po: guess-number.c
	xgettext -k_ -c guess-number.c -o guess-number.pot
	msgmerge -U $@ guess-number.pot

guess-number.c: roman.h

@DX_RULES@
doxygen-doc/man/man1/guess-number.c.1:	doxygen-doc

man1_MANS = doxygen-doc/man/man1/guess-number.c.1

install-data-local:
	mkdir -p $(DESTDIR)/$(docdir)
	cp -a doxygen-doc/html $(DESTDIR)/$(docdir)

http:	doxygen-doc
	python3 -m http.server --directory $</html 5000

checklog: check
	cat tests/*.log

gcov: check
	$(MAKE) $(MAKEFLAGS) -C gcov

gitclean:
	git clean -xfd
