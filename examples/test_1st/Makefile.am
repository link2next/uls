include ../cfg.mk

noinst_PROGRAMS = test_1st
test_1st_SOURCES = main.c

test_1st_CFLAGS = -Wall
#test_1st_CPPFLAGS = -I$(uls_incdir) -DULS_USE_WSTR
#test_1st_LDADD = -L$(uls_libdir) -lulscompat -luls
test_1st_CPPFLAGS = -I$(uls_incdir)
test_1st_LDADD = -L$(uls_libdir) -luls
