include ../cfg.mk

bin_PROGRAMS = EngToks
EngToks_SOURCES = EngToks.cpp EngLex.cpp

EngToks_CXXFLAGS = -Wall -std=c++11
#EngToks_CPPFLAGS = -I$(uls_inc_dir) -DULS_USE_WSTR
EngToks_CPPFLAGS = -I$(uls_inc_dir)
EngToks_LDADD = -L$(uls_lib_dir) -lulscpp -luls

$(EngToks_SOURCES): EngLexBasis.h

EngLexBasis.h: eng.ulc
	LD_LIBRARY_PATH="$(uls_lib_dir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.EngLexBasis $<
