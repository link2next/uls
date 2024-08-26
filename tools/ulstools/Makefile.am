
include $(top_srcdir)/uls.config

lib_LIBRARIES = libulstools.a

libulstools_a_SOURCES = utils.c
libulstools_a_CPPFLAGS = \
	-include $(CONFIG_HEADER) -I$(top_srcdir) -I$(top_srcdir)/src
libulstools_a_CFLAGS = -funsigned-char -Wall -Wno-char-subscripts

noinst_HEADERS =
include_HEADERS =

ACLOCAL_AMFLAGS = -I m4s
