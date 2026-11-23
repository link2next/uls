
dist_man_MANS = ulc2class.1.gz ulf_gen.1.gz uls_stream.1.gz
CLEANFILES = $(dist_man_MANS)

ulc2class.1: $(top_builddir)/tools/ulc2class/ulc2class
	@$(ax_help2man_exe) --no-discard-stderr -o $@ -s 1 --name="ulc2class" --help-option="-H" $(top_builddir)/tools/ulc2class/ulc2class

ulf_gen.1: $(top_builddir)/tools/ulf_gen/ulf_gen
	@$(ax_help2man_exe) --no-discard-stderr -o $@ -s 1 --name="ulf_gen" --help-option="-H" $(top_builddir)/tools/ulf_gen/ulf_gen

uls_stream.1: $(top_builddir)/tools/uls_stream/uls_stream
	@$(ax_help2man_exe) --no-discard-stderr -o $@ -s 1 --name="uls_stream" --help-option="-H" $(top_builddir)/tools/uls_stream/uls_stream

%.1.gz : %.1
	@gzip -f $<

check:
	@touch $(dist_man_MANS)

install:
	@./inst_docs.sh "$(ax_share_dir)"