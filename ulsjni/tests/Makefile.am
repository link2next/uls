
include $(top_srcdir)/uls.config

ACLOCAL_AMFLAGS = -I m4

all:

check: all
	@./run_tests.sh $(TMP_SYSPROPS)

clean-local:
	rm -f a?.uls
