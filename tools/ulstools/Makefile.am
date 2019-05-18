
include $(top_srcdir)/uls.config

lib_LIBRARIES = libulstools.a
libulstools_a_SOURCES = utils.c

noinst_HEADERS = 
include_HEADERS =

libulstools_a_CPPFLAGS = \
	-include $(CONFIG_HEADER) -I$(top_srcdir) -I$(top_srcdir)/src
libulstools_a_CFLAGS = -funsigned-char -Wall -Wno-char-subscripts 

ACLOCAL_AMFLAGS = -I m4s
