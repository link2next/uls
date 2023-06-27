
SUBDIRS = ulsconfig ulc_exam \
	src ulscompat ulscpp tools tests

SUBDIRS += doc post_inst

ACLOCAL_AMFLAGS = -I m4

libtool: $(LIBTOOL_DEPS)
	bash ./config.status --recheck

