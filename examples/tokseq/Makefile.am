include ../cfg.mk

ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = tokseq
tokseq_SOURCES = main.c tokseq_lex.c

tokseq_CPPFLAGS = -I$(uls_inc_dir)
tokseq_LDADD = -L$(uls_lib_dir) -luls

$(tokseq_OBJECTS) : tokseq_lex.h

tokseq_lex.h tokseq_lex.c : tokseq.uld
	$(ulc2class_exe) --lang=c -f tokseq_lex -pTOK_ $<
