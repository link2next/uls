include $(top_srcdir)/uls.config

bin_PROGRAMS = ulc2yaml
ulc2yaml_SOURCES = ulc2yaml.c
ulc2yaml_CPPFLAGS = -I$(top_srcdir)/tools/ulstools -I$(top_srcdir)/src
ulc2yaml_LDADD = $(top_builddir)/tools/ulstools/libulstools.a $(top_builddir)/src/libuls.la

ACLOCAL_AMFLAGS = -I m4

check:
	@echo "checking nothing..."
