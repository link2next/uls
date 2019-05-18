include $(top_srcdir)/uls.config

bin_PROGRAMS = ulc2class
ulc2class_SOURCES = ulc2class.c
ulc2class_CPPFLAGS = -I$(top_srcdir)/tools/ulstools -I$(top_srcdir)/src
ulc2class_LDADD = $(top_builddir)/tools/ulstools/libulstools.a $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check:
	@bash ./run_tests.sh "$(top_builddir)/src"
