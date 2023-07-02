
include $(top_srcdir)/uls.config

SUBDIRS = tests src libulsjni

ACLOCAL_AMFLAGS = -I m4

check: all
	(cd tests; ./run_tests.sh $(TMP_SYSPROPS))

clean-local:
	rm -f tests/a?.uls
