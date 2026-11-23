include ../cfg.mk

bin_PROGRAMS = EngToks
EngToks_SOURCES = EngToks.cpp EngLex.cpp
CLEANFILES = EngLexBasis.h

EngToks_CXXFLAGS = -Wall -std=c++11
#EngToks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
EngToks_CPPFLAGS = -I$(uls_incdir)
EngToks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(EngToks_SOURCES): EngLexBasis.h

EngLexBasis.h: eng.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.EngLexBasis $<
