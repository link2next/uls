include ../cfg.mk

noinst_PROGRAMS = ulc2xml
ulc2xml_SOURCES = main.c ulc2xml.c ulc_gram.y
CLEANFILES = ulc_gram.[ch] ulc_lex.h

ulc2xml_CFLAGS = -Wall 
ulc2xml_CPPFLAGS = -I$(uls_incdir)
ulc2xml_LDADD = -L$(uls_libdir) -luls
YFLAGS = -d

$(ulc2xml_OBJECTS) : ulc_lex.h ulc_gram.h

%_lex.h : %.ulc
	LD_LIBRARY_PATH="$(uls_libdir)" $(ulc2class_exe) --lang=c -o$@ $<

ulc_gram.c: ulc_gram.y
	$(YACCCOMPILE) $<
	mv y.tab.c ulc_gram.c
	mv y.tab.h ulc_gram.h
