include ../cfg.mk

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = Css3Toks
Css3Toks_SOURCES = Css3Toks.cpp Css3Lex.cpp

Css3Toks_CPPFLAGS = -I$(uls_inc_dir)
Css3Toks_LDADD = -L$(uls_lib_dir) -lulscpp -lulscompat -luls

$(Css3Toks_SOURCES): Css3LexBasis.h

Css3LexBasis.h: css3.ulc
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.Css3LexBasis -pCSS_ $<
