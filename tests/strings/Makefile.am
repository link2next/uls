include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = strings
strings_SOURCES = strings.c
strings_CFLAGS = -Wall
strings_CPPFLAGS = -I$(top_srcdir)/src
strings_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath)

$(strings_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	$(ulc2class_exe) -lc -pTOK_ $<
