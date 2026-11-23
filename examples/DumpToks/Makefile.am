include ../cfg.mk

bin_PROGRAMS = DumpToks
DumpToks_SOURCES = DumpToks.cpp
CLEANFILES = DumpToksBasis.h a?.uls

DumpToks_CXXFLAGS = -Wall -std=c++11
DumpToks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
#DumpToks_CPPFLAGS = -I$(uls_incdir)
DumpToks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(DumpToks_SOURCES): DumpToksBasis.h

DumpToksBasis.h: dumptoks.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.DumpToksBasis $<
