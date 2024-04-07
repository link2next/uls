
ACLOCAL_AMFLAGS = -I m4

dist_man_MANS = ulc2class.1.gz ulf_gen.1.gz uls_stream.1.gz

ulc2class.1: $(top_builddir)/tools/ulc2class/ulc2class
	@$(HELP2MAN) --no-discard-stderr -o $@ -s 1 --name="ulc2class" --help-option="-H" $(top_builddir)/tools/ulc2class/ulc2class

ulf_gen.1: $(top_builddir)/tools/ulf_gen/ulf_gen
	@$(HELP2MAN) --no-discard-stderr -o $@ -s 1 --name="ulf_gen" --help-option="-H" $(top_builddir)/tools/ulf_gen/ulf_gen

uls_stream.1: $(top_builddir)/tools/uls_stream/uls_stream
	@$(HELP2MAN) --no-discard-stderr -o $@ -s 1 --name="uls_stream" --help-option="-H" $(top_builddir)/tools/uls_stream/uls_stream

%.1.gz : %.1
	@gzip -f $<

check:
	@touch $(dist_man_MANS)

install:
	install -m755 -d "$(datadir)/man/man1"
	install -m755 -d "$(datadir)/uls"
	@if ls *.1.gz > /dev/null 2>&1; then install -m644 *.1.gz "$(datadir)"/man/man1; fi
	@if ls *.pdf > /dev/null 2>&1; then install -m644 *.pdf "$(datadir)"/uls/; fi

clean-local:
	@rm -f $(dist_man_MANS)
