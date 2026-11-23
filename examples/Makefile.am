include $(top_srcdir)/uls.config

SUBDIRS = test_1st tokseq yacc \
	DumpToks Css3 Html5 Shell Mkf Natural \
	ulc2yaml
if WANT_ULSJNI
SUBDIRS += java
endif 

check: all
	@./run_examples.sh "$(ax_etc_dir)" "$(ax_lib_dir)"
