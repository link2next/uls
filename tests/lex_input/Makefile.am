include $(top_srcdir)/uls.config

noinst_PROGRAMS = lex_input
lex_input_SOURCES = main.c sample_g_lex.c 

lex_input_CFLAGS = -Wall
#lex_input_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#lex_input_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
lex_input_CPPFLAGS = -I$(top_srcdir)/src
lex_input_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(lex_input_OBJECTS) : sample_g_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
