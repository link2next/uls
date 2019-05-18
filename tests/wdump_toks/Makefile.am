include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = wdump_toks
wdump_toks_SOURCES = wdump_toks.c

wdump_toks_CFLAGS = -Wall
#wdump_toks_CPPFLAGS = -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
wdump_toks_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
wdump_toks_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath)

$(wdump_toks_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	$(ulc2class_exe) -lc -pTOK_ $<
