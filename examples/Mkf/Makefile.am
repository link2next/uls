include ../cfg.mk

bin_PROGRAMS = MkfToks
MkfToks_SOURCES = MkfToks.cpp MkfLex.cpp
#MkfToks_CPPFLAGS = -I$(uls_inc_dir) -DULS_USE_WSTR
MkfToks_CPPFLAGS = -I$(uls_inc_dir)
MkfToks_LDADD = -L$(uls_lib_dir) -lulscpp -luls

$(MkfToks_SOURCES): MkfLexBasis.h

MkfLexBasis.h: mkf.ulc
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.MkfLexBasis $<
