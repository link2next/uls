include ../cfg.mk

bin_PROGRAMS = Css3Toks
Css3Toks_SOURCES = Css3Toks.cpp Css3Lex.cpp
CLEANFILES = Css3LexBasis.h

Css3Toks_CXXFLAGS = -Wall -std=c++11
#Css3Toks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
Css3Toks_CPPFLAGS = -I$(uls_incdir)
Css3Toks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(Css3Toks_SOURCES): Css3LexBasis.h

Css3LexBasis.h: css3.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.Css3LexBasis -pCSS_ $<
