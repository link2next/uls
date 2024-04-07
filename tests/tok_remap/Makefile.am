include $(top_srcdir)/uls.config

noinst_PROGRAMS = tok_remap
tok_remap_SOURCES = tok_remap.c

tok_remap_CFLAGS = -Wall
#tok_remap_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#tok_remap_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
tok_remap_CPPFLAGS = -I$(top_srcdir)/src
tok_remap_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(tok_remap_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
