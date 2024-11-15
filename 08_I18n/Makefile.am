RU = ru/LC_MESSAGES

bin_PROGRAMS=guess-number
pkgdata_DATA=$(RU)/$(PACKAGE).mo

all: $(RU)/$(PACKAGE).mo

$(RU)/$(PACKAGE).mo: ru.po
	mkdir -p `dirname $@`
	msgfmt $< -o $@

ru.po: guess-number.c
	xgettext -k_ -c guess-number.c -o guess-number.pot
	msgmerge -U $@ guess-number.pot

gitclean: git clean -fd

clean:
	rm -rf .deps *.cache *.m4 compile *.guess *.in *.log config.h depcomp ru \
	*.status configure $(bin_PROGRAMS) *.o install-sh .*in missing stamp-h1
