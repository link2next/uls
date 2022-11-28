include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libulscompat.la
libulscompat_la_SOURCES = ulscompat.c \
	uls_fileio_wstr.c uls_util_wstr.c uls_lf_swprintf.c uls_wprint.c uls_wlog.c \
	uls_emit_wstr.c uls_conf_wstr.c uls_wlex.c uls_tokdef_wstr.c uls_istream_wstr.c uls_ostream_wstr.c

noinst_HEADERS = 
include_HEADERS =

libulscompat_la_CPPFLAGS = -D_ULS_IMPLDLL -DULS_ETC_DIR=\"$(ax_etc_dir)\" \
	-include $(CONFIG_HEADER) -I$(top_srcdir) -I$(top_srcdir)/src
libulscompat_la_CFLAGS  = -funsigned-char -Wall -Wno-char-subscripts -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
libulscompat_la_LIBADD  = $(top_builddir)/src/libuls.la
libulscompat_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)

ACLOCAL_AMFLAGS = -I m4

install: install-libLTLIBRARIES install-includeHEADERS
	install -m755 -d $(includedir)/uls
	cp -f uls/*.h $(includedir)/uls/
