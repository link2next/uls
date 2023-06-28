include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = line_by_line
line_by_line_SOURCES = line.c
line_by_line_CFLAGS = -Wall
#line_by_line_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#line_by_line_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
line_by_line_CPPFLAGS = -I$(top_srcdir)/src
line_by_line_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(line_by_line_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -ptok $<
