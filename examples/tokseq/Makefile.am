include ../cfg.mk

noinst_PROGRAMS = tokseq
tokseq_SOURCES = main.c tokseq_lex.c
#tokseq_CPPFLAGS = -I$(uls_inc_dir) -DULS_USE_WSTR
#tokseq_LDADD = -L$(uls_lib_dir) -lulscompat -luls
tokseq_CPPFLAGS = -I$(uls_inc_dir) 
tokseq_LDADD = -L$(uls_lib_dir) -luls

$(tokseq_OBJECTS) : tokseq_lex.h

tokseq_lex.h tokseq_lex.c : tokseq.uld
	$(ulc2class_exe) --lang=c -f tokseq_lex -pTOK_ $<

clean-local:
	-rm -f a?.uls
