
include $(top_srcdir)/uls.config

SUBDIRS = src libulsjni tests

ACLOCAL_AMFLAGS = -I m4

clean-local:
	@if [ -d ./classes ]; then \
		rm -rf ./classes; \
	fi
	@if [ -d ./libs ]; then \
		rm -rf ./libs; \
	fi
