SUBDIRS = src tests

checklog: check
	cat tests/*.log

gcov: check
	$(MAKE) $(MAKEFLAGS) -C src gcov

gitclean:
	git clean -xdf
