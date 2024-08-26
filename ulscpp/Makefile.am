include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libulscpp.la
libulscpp_la_SOURCES = \
	UlsLex.cpp UlsStream.cpp UlsIStream.cpp UlsOStream.cpp \
	UlsUtils.cpp

noinst_HEADERS = 
include_HEADERS = 

libulscpp_la_CXXFLAGS = -funsigned-char -std=c++11 -Wall -Wno-char-subscripts
libulscpp_la_CPPFLAGS = -D_ULSCPP_IMPLDLL -include $(CONFIG_HEADER) \
	-I$(top_srcdir)/ulscompat -I$(top_srcdir)/src

libulscpp_la_LIBADD  = $(top_builddir)/ulscompat/libulscompat.la $(top_builddir)/src/libuls.la
libulscpp_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)

ACLOCAL_AMFLAGS = -I m4

install: install-libLTLIBRARIES install-includeHEADERS
	install -m755 -d "$(includedir)"/uls
	cp -f uls/*.h "$(includedir)"/uls/
