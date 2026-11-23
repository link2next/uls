include $(top_srcdir)/uls.config

noinst_PROGRAMS = dump_toks
dump_toks_SOURCES = dump_toks.c

dump_toks_CFLAGS = -Wall
#dump_toks_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
#dump_toks_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
#enc_mode=wstr
dump_toks_CPPFLAGS = -I$(top_srcdir)/src
dump_toks_LDADD = $(top_builddir)/src/libuls.la
enc_mode=ustr

check: all
	@../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS) $(enc_mode)

$(dump_toks_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	@$(ax_ulc2class_exe) -lc -pTOK_ $<
