include ../cfg.mk

bin_PROGRAMS = Css3Toks
Css3Toks_SOURCES = Css3Toks.cpp Css3Lex.cpp

Css3Toks_CXXFLAGS = -Wall -std=c++11
#Css3Toks_CPPFLAGS = -I$(uls_inc_dir) -DULS_USE_WSTR
Css3Toks_CPPFLAGS = -I$(uls_inc_dir)
Css3Toks_LDADD = -L$(uls_lib_dir) -lulscpp -luls

$(Css3Toks_SOURCES): Css3LexBasis.h

Css3LexBasis.h: css3.ulc
	LD_LIBRARY_PATH="$(uls_lib_dir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.Css3LexBasis -pCSS_ $<
