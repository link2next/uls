include $(top_srcdir)/uls.config

lib_LTLIBRARIES = libuls.la
libuls_la_SOURCES = \
	uls_prim.c uls_misc.c uls_util.c uls_version.c uls_auw.c uls_fileio.c \
	utf8_enc.c utf_file.c uls_log.c csz_stream.c fdfilter.c \
	uls_ieee754.c uls_lf_percent_f.c uls_lf_sprintf.c uls_print.c \
	uls_tokdef.c twoplus.c onechar.c idkeyw.c uls_freq.c uls_conf.c uld_conf.c uls_emit.c \
	uls_context.c uls_langs.c uls_core.c uls_lex.c unget.c uls_stream.c uls_istream.c uls_ostream.c \
	uls_num.c litesc.c litstr.c uls_input.c uls_sysprops.c \
	uls_dump.c uls_init.c

noinst_HEADERS = 
include_HEADERS = uls.h 

uls/uls_const_linux.h: array_names00.txt
	./gen_array_names00.sh $< > $@

uls_prim.c: uls/uls_const_linux.h

libuls_la_CPPFLAGS = -D_ULS_IMPLDLL -DULS_SRC_DIR=\"$(PWD)\" \
	-DULS_ETC_DIR=\"$(ax_etc_dir)\" \
	-include $(CONFIG_HEADER) -I$(top_srcdir)
libuls_la_CFLAGS  = -funsigned-char \
	-Wall -Wno-char-subscripts -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
libuls_la_LDFLAGS = -version-info $(so_current):$(so_revision):$(so_age)
libuls_la_LIBADD  = -lpthread 

ACLOCAL_AMFLAGS = -I m4

install: install-libLTLIBRARIES install-includeHEADERS
	install -m755 -d "$(includedir)"/uls
	cp -f uls/*.h "$(includedir)"/uls/

