include $(top_srcdir)/uls.config

noinst_PROGRAMS = logging
logging_SOURCES = logging.c

logging_CFLAGS = -Wall
logging_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
logging_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
#logging_CPPFLAGS = -I$(top_srcdir)/src
#logging_LDADD = $(top_builddir)/src/libuls.la

check: all
	@../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(logging_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	@$(ax_ulc2class_exe) -lc -pTOK_ $<
