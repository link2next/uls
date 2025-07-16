#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ULS], [1.8], [link2next@gmail.com])
AC_CONFIG_SRCDIR([ulscpp/UlsLex.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([long double])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_YACC

AC_CHECK_PROG([HAVE_JAVA],[java],[yes],[no])
AC_CHECK_PROG([HAVE_JAVAC],[javac],[yes],[no])
AC_CHECK_PROG([HAVE_JAVAH],[javah],[yes],[no])

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_mutex_init])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h locale.h malloc.h])
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_LONG_DOUBLE

AC_CONFIG_MACRO_DIR([m4])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit dup2 getcwd memmove memset mkdir modf setlocale])
AC_CHECK_FUNCS([strchr strcasecmp strdup strerror strstr strrchr strtol strtoul])
AC_CHECK_FUNCS([readlink realpath])

AC_PATH_PROG(HELP2MAN, help2man, false)

AC_ARG_ENABLE([java],
	[AS_HELP_STRING([--disable-java],
		[Don't compile the uls library for java.])
	],
	[ ax_enable_ulsjni=$enableval ],
	[ ax_enable_ulsjni=yes ]
	)
AM_CONDITIONAL([WANT_ULSJNI], [test x$ax_enable_ulsjni = xyes])

AC_ARG_WITH([java],
	[AS_HELP_STRING([--with-java=<java-home>],
		[Specify the JAVA_HOME directory.])],
	[ax_java_home=$withval],
	[ax_java_home=$JAVA_HOME]
	)

if test "x$ax_java_home" = "x"; then
	ax_java_home="/usr/lib/jvm/java-8-openjdk-amd64"
fi

AC_ARG_WITH([jdk-incdir],
	[AS_HELP_STRING([--with-jdk-incdir=<dirpath>],
		[Specify the include directory of jdk.])
	],
	[ ax_jdk_cppflags="-I$withval" ],
	[ ax_jdk_cppflags="-I$ax_java_home/include -I$ax_java_home/include/linux -I$ax_java_home/include/darwin" ]
	)
AC_SUBST([ax_jdk_cppflags])

AC_ARG_WITH([jdk-libdir],
	[AS_HELP_STRING([--with-jdk-libdir=<dirpath>],
		[Specify the library directory of jdk.])
	],
	[ ax_jdk_ldflags="-L$withval" ],
	[ ax_jdk_ldflags="-L$ax_java_home/lib" ]
	)
AC_SUBST([ax_jdk_ldflags])

AC_ARG_WITH([etc-dir],
  [AS_HELP_STRING([--with-etc-dir=<dirpath>],
    [use this option to specify the directory where uls.sysprops resides.])],
  [
    ax_etc_dir=$withval
  ],
  [
    ax_etc_dir=/usr/local/etc/uls
  ])

AC_SUBST([ax_etc_dir])

AC_CONFIG_FILES([Makefile
	ulsconfig/Makefile
	ulc_exam/Makefile
	src/Makefile
	ulscompat/Makefile
	ulscpp/Makefile
	ulsjni/Makefile
	ulsjni/tests/Makefile
	ulsjni/libulsjni/Makefile
	ulsjni/src/Makefile
	tools/Makefile
	tools/ulstools/Makefile
	tools/ulc2class/Makefile
	tools/ulf_gen/Makefile
	tools/uls_stream/Makefile
	doc/Makefile
	tests/Makefile
	tests/cpp_hello/Makefile
	tests/dump_toks/Makefile
	tests/id_stat/Makefile
	tests/getch/Makefile
	tests/line/Makefile
	tests/log/Makefile
	tests/many_kwrds/Makefile
	tests/nest/Makefile
	tests/strings/Makefile
	tests/tok_remap/Makefile
	tests/two_lex/Makefile
	tests/gcc_toks/Makefile
	tests/file_enc/Makefile
	examples/Makefile
	examples/test_1st/Makefile
	examples/tokseq/Makefile
	examples/yacc/Makefile
	examples/DumpToks/Makefile
	examples/Css3/Makefile
	examples/Html5/Makefile
	examples/Shell/Makefile
	examples/Mkf/Makefile
	examples/Natural/Makefile
	examples/ulc2yaml/Makefile
	post_inst/Makefile
])

AC_OUTPUT
