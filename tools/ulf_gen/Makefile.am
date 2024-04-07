include $(top_srcdir)/uls.config

bin_PROGRAMS = ulf_gen
ulf_gen_SOURCES = ulf_gen.c
ulf_gen_CPPFLAGS = -I$(top_srcdir)/tools/ulstools -I$(top_srcdir)/src
ulf_gen_CFLAGS = -Wall
ulf_gen_LDADD = $(top_builddir)/tools/ulstools/libulstools.a $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check:
	@bash ./run_tests.sh "$(top_srcdir)/tests/test_tree" "$(top_builddir)/src" $(TMP_SYSPROPS)
