include ../cfg.mk

bin_PROGRAMS = ShellToks
ShellToks_SOURCES = ShellToks.cpp ShellLex.cpp
ShellToks_CPPFLAGS = -I$(uls_inc_dir)
ShellToks_LDADD = -L$(uls_lib_dir) -lulscpp -lulscompat -luls

$(ShellToks_SOURCES): ShellLexBasis.h

ShellLexBasis.h: shell.ulc
	$(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.ShellLexBasis $<
