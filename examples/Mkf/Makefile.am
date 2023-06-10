include ../cfg.mk

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = MkfToks
MkfToks_SOURCES = MkfToks.cpp MkfLex.cpp

#MkfToks_CPPFLAGS = -I$(uls_inc_dir)
MkfToks_CPPFLAGS = -DULS_USE_WSTR -I$(uls_inc_dir)
MkfToks_LDADD = -L$(uls_lib_dir) -lulscpp -lulscompat -luls 

$(MkfToks_SOURCES): MkfLexBasis.h

MkfLexBasis.h: mkf.ulc 
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.MkfLexBasis $< 
