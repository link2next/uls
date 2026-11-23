include ../cfg.mk

bin_PROGRAMS = MkfToks
MkfToks_SOURCES = MkfToks.cpp MkfLex.cpp
CLEANFILES = MkfLexBasis.h

MkfToks_CXXFLAGS = -Wall -std=c++11 
MkfToks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
#MkfToks_CPPFLAGS = -I$(uls_incdir)
MkfToks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(MkfToks_SOURCES): MkfLexBasis.h

MkfLexBasis.h: mkf.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.MkfLexBasis $<
