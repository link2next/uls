
include $(top_srcdir)/uls.config

CLEANFILES = uninst_info.txt
cfgmk_for_examples = ../examples/cfg.mk
all: uninst_info.txt

uninst_info.txt:
	@./gen_uninst_info.sh "$(ax_inst_dir)" "$(ax_lib_dir)" "$(ax_etc_dir)" \
		$(ax_user_name) $(ax_user_home) $@

$(cfgmk_for_examples): ../uls.config
	@./gen_examples_cfgmk.sh $(uls_ver) $(cfgmk_for_examples)

install: uninst_info.txt $(cfgmk_for_examples)
#	@./strip_libs.sh "$(ax_lib_dir)"
	@./make_uls_examples_tar.sh ./uls_examples.tar \
		$(top_srcdir)/examples ./list_uls_examples.txt "$(ax_share_dir)"/uls
	install -m755 uninstall_uls.sh "$(ax_bin_dir)"/uninstall_uls
	install -m755 uninst_info.txt "$(ax_bin_dir)"/uls_uninst_info.sh
	install -m755 setup_uls_examples.sh "$(ax_bin_dir)"/setup_uls_examples
	@./notice.sh "$(ax_inst_dir)" "$(ax_bin_dir)" "$(ax_inc_dir)" "$(ax_lib_dir)"
	@if [ -f $(TMP_SYSPROPS) ]; then \
		rm -f $(TMP_SYSPROPS); \
	fi
