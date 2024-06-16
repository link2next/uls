include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libulsjni.la
libulsjni_la_SOURCES = \
	UlsLex.c UlsIStream.c UlsOStream.c

noinst_HEADERS =
include_HEADERS =

libulsjni_la_CPPFLAGS = \
	-include $(CONFIG_HEADER) -I$(top_srcdir)/src \
	-I../include $(ax_jdk_cppflags)
libulsjni_la_CFLAGS  = -funsigned-char \
	-Wall -Wno-char-subscripts -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast

libulsjni_la_LIBADD  = $(ax_jdk_ldflags) $(top_builddir)/src/libuls.la
libulsjni_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)

ACLOCAL_AMFLAGS = -I m4

install: install-libLTLIBRARIES

