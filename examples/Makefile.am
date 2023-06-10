include $(top_srcdir)/uls.config
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = test_1st tokseq yacc \
	DumpToks Css3 Html5 Shell Mkf Natural

check: all
	@bash ./run_examples.sh $(etc_dir)
