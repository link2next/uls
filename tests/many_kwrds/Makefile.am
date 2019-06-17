include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = many_kwrds
many_kwrds_SOURCES = many_kwrds.c
many_kwrds_CFLAGS = -Wall
many_kwrds_CPPFLAGS = -I$(top_srcdir)/src
many_kwrds_LDADD = $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(many_kwrds_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
