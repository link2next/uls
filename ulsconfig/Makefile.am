
include $(top_srcdir)/uls.config

sysprops_outfile = uls.sysprops
idranges_infile = uls_id_ranges.txt
idranges_outfile = uls.id_ranges

langs_infile = uls_langs.txt
langs_outfile = uls.langs

CLEANFILES = $(TMP_SYSPROPS)

all:
#	./chk_prereq.sh config "$(ax_shell_exe)"
	@if [ ! -f $(TMP_SYSPROPS) ]; then \
		echo "Generating sysprops at the build stage..."; \
		./gen_sysprops.sh B $(TMP_SYSPROPS) \
			$(uls_ver_major).$(uls_ver_minor).$(uls_ver_debug) \
			$(top_srcdir)/ulsconfig  $(top_srcdir); \
	fi

install: all
	@./chk_prereq.sh install "$(ax_user_home)" "$(ax_inst_dir)" "$(ax_lib_dir)" "$(ax_etc_dir)"
	install -m644 $(langs_infile) "$(ax_etc_dir)"/$(langs_outfile)
	install -m644 $(idranges_infile) "$(ax_etc_dir)"/$(idranges_outfile)
	@./gen_sysprops.sh I "$(ax_etc_dir)"/$(sysprops_outfile) \
		$(uls_ver_major).$(uls_ver_minor).$(uls_ver_debug) \
		"$(ax_etc_dir)" "$(ax_inst_dir)" "$(ax_lib_dir)" 
