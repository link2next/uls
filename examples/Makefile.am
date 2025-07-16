include $(top_srcdir)/uls.config

SUBDIRS = test_1st tokseq yacc \
	DumpToks Css3 Html5 Shell Mkf Natural

check: all
	@bash ./run_examples.sh $(ax_etc_dir)
