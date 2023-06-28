include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = id_stat
id_stat_SOURCES = id_stat.c
id_stat_CFLAGS = -Wall
#id_stat_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#id_stat_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
id_stat_CPPFLAGS = -I$(top_srcdir)/src
id_stat_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(id_stat_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
