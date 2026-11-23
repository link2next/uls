include $(top_srcdir)/uls.config

noinst_PROGRAMS = cpp_hello
cpp_hello_SOURCES = cpp_hello.cpp Sample2Lex.cpp

cpp_hello_CXXFLAGS = -Wall -std=c++11

#cpp_hello_CPPFLAGS = -DULS_USE_WSTR -I$(top_srcdir)/ulscpp -I$(top_srcdir)/src
cpp_hello_CPPFLAGS = -I$(top_srcdir)/ulscpp -I$(top_srcdir)/src
cpp_hello_LDADD = $(top_builddir)/ulscpp/libulscpp.la $(top_builddir)/src/libuls.la

CONFIG_CLEAN_FILES = out*.uls
CLEANFILES = a?.uls

check: all
	@../run_tests.sh $(noinst_PROGRAMS) $(tests_libpath) $(TMP_SYSPROPS)

$(cpp_hello_OBJECTS) : Sample2Lex.cpp Sample2Lex.h

Sample2Lex.cpp Sample2Lex.h : sample2.uld
	@$(ax_ulc2class_exe) -lcpp -n AAA.BBB.Sample2Lex -f Sample2Lex $<

