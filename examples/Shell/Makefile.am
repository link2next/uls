include ../cfg.mk

bin_PROGRAMS = ShellToks
ShellToks_SOURCES = ShellToks.cpp ShellLex.cpp
CLEANFILES = ShellLexBasis.h

ShellToks_CXXFLAGS = -Wall -std=c++11
ShellToks_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
#ShellToks_CPPFLAGS = -I$(uls_incdir)
ShellToks_LDADD = -L$(uls_libdir) -lulscpp -luls

$(ShellToks_SOURCES): ShellLexBasis.h

ShellLexBasis.h: shell.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=cpp -o$@ -n uls.collection.ShellLexBasis $<
