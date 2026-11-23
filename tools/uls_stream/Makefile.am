include $(top_srcdir)/uls.config

bin_PROGRAMS = uls_stream
uls_stream_SOURCES = main.c read_uls.c write_uls.c filelist.c
uls_stream_CPPFLAGS = -I$(top_srcdir)/tools/ulstools -I$(top_srcdir)/src
uls_stream_CFLAGS = -Wall
uls_stream_LDADD = $(top_builddir)/tools/ulstools/libulstools.a $(top_builddir)/src/libuls.la
CLEANFILES = sample1_lex.[ch]

all: tmpl_ex.uls

$(uls_stream_OBJECTS) : sample1_lex.h

sample1_lex.h : sample1.ulc
	$(ax_ulc2class_exe) -lc -pTOK_ $<

tmpl_ex.uls : tmpl_ex.txt
	@ULS_SYSPROPS=$(TMP_SYSPROPS) ./uls_stream -L ./sample1.ulc -t txt -o $@ -TT $<  

check:
	@./run_tests.sh $(top_builddir)/src $(TMP_SYSPROPS)
