
SUBDIRS = man

ACLOCAL_AMFLAGS = -I m4

install:
	install -m755 -d "$(datadir)"
	@if ls *.pdf > /dev/null 2>&1; then install -m644 *.pdf "$(datadir)"/uls/; fi
