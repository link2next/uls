
SUBDIRS = ulsconfig ulc_exam \
	src ulscompat ulscpp tools tests

if WANT_ULSJNI 
SUBDIRS += ulsjni
endif
SUBDIRS += doc post_inst

ACLOCAL_AMFLAGS = -I m4

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

clean-local:
	@make -C examples clean
	@find . \( -name 'a?.uls' -or -name '*.res' -or -name 'res_*.txt' \) -delete
	./ulsconfig/cleanup_uls.sh $(PWD) clean

distclean-local:
	./ulsconfig/cleanup_uls.sh $(PWD) distclean
