
include $(top_srcdir)/uls.config

sysprops_outfile = uls.sysprops

idranges_infile = uls_id_ranges.txt
idranges_outfile = uls.id_ranges

langs_infile = uls_langs.txt
langs_outfile = uls.langs

all:
	@if [ ! -f $(TMP_SYSPROPS) ]; then \
		echo "Generating sysprops at the build stage..."; \
		./gen_sysprops.sh  "$(top_srcdir)" "$(top_srcdir)"/ulsconfig \
			$(uls_ver_major).$(uls_ver_minor).$(uls_ver_debug) B $(TMP_SYSPROPS); \
	fi

install: all
	install -m755 -d "$(includedir)/uls" "$(datadir)/uls/ulcs" "$(ax_etc_dir)" "$(bindir)"
	install -m644 $(langs_infile) "$(ax_etc_dir)"/$(langs_outfile)
	install -m644 $(idranges_infile) "$(ax_etc_dir)"/$(idranges_outfile)
	install -m755 uls_common.sh $(bindir)/
	./gen_sysprops.sh "$(prefix)" "$(ax_etc_dir)" \
		$(uls_ver_major).$(uls_ver_minor).$(uls_ver_debug) I "$(ax_etc_dir)"/$(sysprops_outfile)
