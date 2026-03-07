include $(top_srcdir)/uls.config

noinst_PROGRAMS = dump_strs
dump_strs_SOURCES = strings.c

dump_strs_CFLAGS = -Wall
dump_strs_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
dump_strs_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
dump_strs_CPPFLAGS = -I$(top_srcdir)/src
dump_strs_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(dump_strs_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
