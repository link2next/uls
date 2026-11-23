include ../cfg.mk

bin_PROGRAMS = Html5Toks
Html5Toks_SOURCES = Html5Toks.cpp Html5Lex.cpp
CLEANFILES = Html5LexBasis.h

Html5Toks_CXXFLAGS = -Wall -std=c++11
#Html5Toks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
Html5Toks_CPPFLAGS = -I$(uls_incdir)
Html5Toks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(Html5Toks_SOURCES): Html5LexBasis.h

Html5LexBasis.h: html5.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.Html5LexBasis $<
