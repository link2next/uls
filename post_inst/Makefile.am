
include $(top_srcdir)/uls.config

ACLOCAL_AMFLAGS = -I m4

all: uninst_info.txt

uninst_info.txt:
	@./gen_uninst_info.sh "$(prefix)" "$(etc_dir)" $@

install: uninst_info.txt
	install -m755 -d "$(bindir)" "$(datadir)" $(etc_dir)
	@./make_uls_examples_tar.sh ./uls_examples.tar $(top_srcdir)/examples ./list_uls_examples.txt "$(datadir)/uls/"
	install -m755 uninstall_uls.sh $(bindir)/uninstall_uls
	install -m755 uninst_info.txt $(bindir)/uls_uninst_info.sh
	install -m755 setup_uls_examples.sh $(bindir)/setup_uls_examples
	@bash ./notice.sh "$(prefix)" "$(bindir)" "$(includedir)" "$(libdir)"
	@if [ -f $(TMP_SYSPROPS) ]; then \
		rm -f $(TMP_SYSPROPS); \
	fi

check:

clean:
	rm -f uninst_info.txt
