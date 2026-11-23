include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libulscpp.la
libulscpp_la_SOURCES = \
	UlsLex.cpp UlsStream.cpp UlsIStream.cpp UlsOStream.cpp \
	UlsAuw.cpp UlsUtils.cpp

noinst_HEADERS = 
include_HEADERS = 

libulscpp_la_CXXFLAGS = -funsigned-char -std=c++14 -Wall -Wno-char-subscripts
libulscpp_la_CPPFLAGS = -D_ULSCPP_IMPLDLL -include $(CONFIG_HEADER) -I$(top_srcdir)/src
libulscpp_la_LIBADD  = $(top_builddir)/src/libuls.la
libulscpp_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)

install: install-libLTLIBRARIES install-includeHEADERS
	cp -f uls/*.h "$(ax_inc_dir)"/uls/
