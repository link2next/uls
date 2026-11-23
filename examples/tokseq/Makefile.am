include ../cfg.mk

noinst_PROGRAMS = tokseq
tokseq_SOURCES = main.c tokseq_lex.c
CLEANFILES = tokseq_lex.[ch] a?.uls

tokseq_CFLAGS = -Wall 
#tokseq_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
#tokseq_LDADD = -L$(uls_libdir) -lulscompat -luls
tokseq_CPPFLAGS = -I$(uls_incdir)
tokseq_LDADD = -L$(uls_libdir) -luls

$(tokseq_OBJECTS) : tokseq_lex.h

tokseq_lex.h tokseq_lex.c : tokseq.uld
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=c -f tokseq_lex -pTOK_ $<
