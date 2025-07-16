
include $(top_srcdir)/uls.config

ACLOCAL_AMFLAGS = -I m4

all:
	@./make_SampleLex.sh ../../tools/ulc2class/ulc2class $(TMP_SYSPROPS) ../../src

check: all
	@./run_tests.sh $(TMP_SYSPROPS)

clean-local:
	rm -f a?.uls
