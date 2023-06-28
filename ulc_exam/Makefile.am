
include $(top_srcdir)/uls.config

all:
	@echo "ax_etc_dir = '$(ax_etc_dir)'"

install:
	install -m755 -d "$(includedir)/uls" "$(datadir)/uls/ulcs"
	bash ./proc_ulc_files.sh ulc_genlst.txt "$(includedir)/uls" $(top_builddir) $(TMP_SYSPROPS)
	install -m644 *.ulc "$(datadir)"/uls/ulcs/

clean:
	rm -f *.h
