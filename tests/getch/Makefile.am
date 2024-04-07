include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = getch
getch_SOURCES = getch.c
getch_CFLAGS = -Wall
getch_CPPFLAGS = -I$(top_srcdir)/src
getch_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(getch_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -ptok $<
