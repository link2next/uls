include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = tok_remap
tok_remap_SOURCES = tok_remap.c
tok_remap_CFLAGS = -Wall
tok_remap_CPPFLAGS = -I$(top_srcdir)/src
tok_remap_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath)

$(tok_remap_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	$(ulc2class_exe) -lc -pTOK_ $<
