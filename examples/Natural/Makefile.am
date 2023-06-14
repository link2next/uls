include ../cfg.mk

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = EngToks
EngToks_SOURCES = EngToks.cpp EngLex.cpp

EngToks_CPPFLAGS = -I$(uls_inc_dir)
EngToks_LDADD = -L$(uls_lib_dir) -lulscpp -lulscompat -luls

$(EngToks_SOURCES): EngLexBasis.h

EngLexBasis.h: eng.ulc
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.EngLexBasis $<
