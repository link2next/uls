include ../cfg.mk

bin_PROGRAMS = DumpToks
DumpToks_SOURCES = DumpToks.cpp
DumpToks_CPPFLAGS = -I$(uls_inc_dir)
DumpToks_LDADD = -L$(uls_lib_dir) -lulscpp -luls

$(DumpToks_SOURCES): DumpToksBasis.h

DumpToksBasis.h: dumptoks.ulc
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.DumpToksBasis $<
