
include $(top_srcdir)/uls.config

all:
	@echo "Nothing to build"

install:
	./proc_ulc_files.sh ./ulc_genlst.txt "$(ax_inc_dir)"/uls $(top_builddir) $(TMP_SYSPROPS)
	@install -m644 *.ulc "$(ax_share_dir)"/uls/ulcs/

