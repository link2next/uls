
dist_man_MANS = ulc2class.1 ulf_gen.1 uls_stream.1

ulc2class.1: $(top_builddir)/tools/ulc2class/ulc2class
	-$(HELP2MAN) -o $@ -s 1 --name="ulc2class" --help-option="-H" $(top_builddir)/tools/ulc2class/ulc2class

ulf_gen.1: $(top_builddir)/tools/ulf_gen/ulf_gen
	-$(HELP2MAN) -o $@ -s 1 --name="ulf_gen" --help-option="-H" $(top_builddir)/tools/ulf_gen/ulf_gen

uls_stream.1: $(top_builddir)/tools/uls_stream/uls_stream
	-$(HELP2MAN) -o $@ -s 1 --name="uls_stream" --help-option="-H" $(top_builddir)/tools/uls_stream/uls_stream

check:
	touch $(dist_man_MANS)

clean-local:
	-rm -f $(dist_man_MANS)
