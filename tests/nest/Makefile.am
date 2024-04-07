include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = nest
nest_SOURCES = nest.c
nest_CFLAGS = -Wall
nest_CPPFLAGS = -I$(top_srcdir)/src
nest_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(nest_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
