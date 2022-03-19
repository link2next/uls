include ../cfg.mk

YFLAGS = -d

noinst_PROGRAMS = ulc2xml
ulc2xml_SOURCES = main.c ulc2xml.c ulc_gram.y
ulc2xml_CPPFLAGS = -I$(uls_inc_dir)
ulc2xml_LDADD = -L$(uls_lib_dir) -luls

$(ulc2xml_OBJECTS) : ulc_lex.h ulc_gram.h

%_lex.h : %.ulc
	$(ulc2class_exe) --lang=c -o$@ $<

ulc_gram.c: ulc_gram.y
	$(YACCCOMPILE) $<
	mv y.tab.c ulc_gram.c
	mv y.tab.h ulc_gram.h
