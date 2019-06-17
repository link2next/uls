include $(top_srcdir)/uls.config

bin_PROGRAMS = uls_stream
uls_stream_SOURCES = main.c read_uls.c write_uls.c filelist.c
uls_stream_CPPFLAGS = -I$(top_srcdir)/tools/ulstools -I$(top_srcdir)/src
uls_stream_CFLAGS = -g
uls_stream_LDADD = $(top_builddir)/tools/ulstools/libulstools.a $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

$(uls_stream_OBJECTS) : sample_lex.h

sample_lex.h : sample.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<

check:
	@bash ./run_tests.sh "$(top_builddir)/src" $(TMP_SYSPROPS)
