include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = gcc_toks
gcc_toks_SOURCES = gcc_toks.c
gcc_toks_CFLAGS = -Wall
gcc_toks_CPPFLAGS = -I$(top_srcdir)/src
gcc_toks_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath)

$(gcc_toks_OBJECTS) : gcc_lex.h

%_lex.h : %.ulc
	$(ulc2class_exe) -lc -pTOK_ $<
