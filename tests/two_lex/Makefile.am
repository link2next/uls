include $(top_srcdir)/uls.config

noinst_PROGRAMS = two_lex
two_lex_SOURCES = two_lex.c

two_lex_CFLAGS = -Wall
#two_lex_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#two_lex_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
two_lex_CPPFLAGS = -I$(top_srcdir)/src
two_lex_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(two_lex_OBJECTS) : sam1_lex.h sam2_lex.h

sam1_lex.h : sam1.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pT1_ $<

sam2_lex.h : sam2.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pT2_ $<
