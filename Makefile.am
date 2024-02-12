
SUBDIRS = ulsconfig ulc_exam \
	src ulscompat ulscpp tools tests
if WANT_ULSJNI 
SUBDIRS += ulsjni
endif
SUBDIRS += doc post_inst

ACLOCAL_AMFLAGS = -I m4

libtool: $(LIBTOOL_DEPS)
	bash ./config.status --recheck

clean-local:
	@make -C examples clean
