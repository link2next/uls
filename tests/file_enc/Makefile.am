include $(top_srcdir)/uls.config

noinst_PROGRAMS = file_enc
file_enc_SOURCES = file_enc.c

file_enc_CFLAGS = -Wall
#file_enc_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#file_enc_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
file_enc_CPPFLAGS = -I$(top_srcdir)/src
file_enc_LDADD = $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(file_enc_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	ULS_SYSPROPS=$(TMP_SYSPROPS) $(ulc2class_exe) -lc -pTOK_ $<
