include ../cfg.mk

ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = test_1st
test_1st_SOURCES = main.c
test_1st_CPPFLAGS = -I$(uls_inc_dir)
test_1st_LDADD = -L$(uls_lib_dir) -luls 
