
include $(top_srcdir)/uls.config

uls_instdir := $(ax_inst_dir)
uls_etcdir := $(ax_etc_dir)
uls_bindir := $(ax_inst_dir)/bin
uls_libdir := $(ax_lib_dir)
uls_incdir := $(ax_inst_dir)/include

ulc2cls := LD_LIBRARY_PATH=$(uls_libdir) $(uls_bindir)/ulc2class
uls_jar := uls-$(uls_ver).jar
