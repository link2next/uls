include $(top_srcdir)/uls.config

noinst_PROGRAMS = strings
strings_SOURCES = strings.c

strings_CFLAGS = -Wall
#strings_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#strings_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
strings_CPPFLAGS = -I$(top_srcdir)/src
strings_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(strings_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
