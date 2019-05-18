include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = logging
logging_SOURCES = logging.c

logging_CFLAGS = -Wall
#logging_CPPFLAGS = -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
logging_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscompat -I$(top_srcdir)/src
logging_LDADD = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la

check: all
	@bash ../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath)

$(logging_OBJECTS) : sample_lex.h

%_lex.h : %.ulc
	$(ulc2class_exe) -lc -pTOK_ $<
