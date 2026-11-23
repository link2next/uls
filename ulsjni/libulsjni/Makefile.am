include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libulsjni.la
libulsjni_la_SOURCES = \
	UlsLex.c UlsIStream.c UlsOStream.c

noinst_HEADERS =
include_HEADERS =

libulsjni_la_CPPFLAGS = -include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src -I../include \
	-I$(ax_java_home)/include
if IS_OS_LINUX
libulsjni_la_CPPFLAGS += -I$(ax_java_home)/include/linux
endif
if IS_OS_APPLE
libulsjni_la_CPPFLAGS += -I$(ax_java_home)/include/darwin
endif

libulsjni_la_CFLAGS  = -funsigned-char \
	-Wall -Wno-char-subscripts -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast

libulsjni_la_LIBADD  = -L$(ax_java_home)/lib $(top_builddir)/src/libuls.la
libulsjni_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)

install: install-libLTLIBRARIES
