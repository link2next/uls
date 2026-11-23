#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ULS],[1.13],[link2next@gmail.com])

AC_CANONICAL_HOST
is_x_mac=no
is_x_linux=no

case "$host_os" in
  linux*)
    is_x_linux=yes
    AC_MSG_NOTICE([Detected platform Linux])
    ;;
  darwin*)
    is_x_mac=yes
    AC_MSG_NOTICE([Detected platform macOS])
    ;;
  *)
    AC_MSG_WARN([Unsupported OS detected!])
    ;;
esac

AM_CONDITIONAL([IS_OS_LINUX], [test "x$is_x_linux" = "xyes"])
AM_CONDITIONAL([IS_OS_APPLE], [test "x$is_x_mac" = "xyes"])

AC_CONFIG_SRCDIR([ulscpp/UlsLex.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([long double])

AM_INIT_AUTOMAKE([foreign])
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC

ax_shell_path=`which $SHELL`
ax_user_shell=`readlink -f "$ax_shell_path"`
ax_shell_name=`basename "$ax_user_shell"`

is_shell_supported=no
case "$ax_shell_name" in
  dash|bash|zsh|sh)
    is_shell_supported=yes
    ;;
esac

AS_IF([test "x$is_shell_supported" = "xno"], [ 
	AC_MSG_ERROR([Not suppoted shell, $ax_shell_name!])
])

AC_CHECK_PROG([HAVE_JAVA],[java],[yes],[no])
AC_CHECK_PROG([HAVE_JAVAC],[javac],[yes],[no])

AC_CHECK_PROG([HAVE_CMD_WHOAMI],[whoami],[yes],[no])
AS_IF([test "x$HAVE_CMD_WHOAMI" = "xno"], [
	AC_MSG_ERROR([need 'whoami' to build the package])
])

AC_CHECK_PROG([HAVE_CMD_GREP],[grep],[yes],[no])
AS_IF([test "x$HAVE_CMD_GREP" = "xno"], [
	AC_MSG_ERROR([need 'grep' to build the package])
])

AC_CHECK_PROG([HAVE_CMD_FIND],[find],[yes],[no])
AS_IF([test "x$HAVE_CMD_FIND" = "xno"], [
	AC_MSG_ERROR([need 'find' to build the package])
])

AC_CHECK_PROG([HAVE_CMD_READLINK],[readlink],[yes],[no])
AS_IF([test "x$HAVE_CMD_READLINK" = "xno"], [
	AC_MSG_ERROR([need 'readlink' to build the package])
])

AC_CHECK_PROG([HAVE_CMD_TAR],[tar],[yes],[no])
AS_IF([test "x$HAVE_CMD_TAR" = "xno"], [
	AC_MSG_ERROR([need 'tar' to build the package])
])

AS_VAR_IF([HOME], [""], [ HOME=`eval echo ~$USER` ])
ax_user_home=$HOME
AC_SUBST([ax_user_home])

ax_user_name=`whoami`
AC_SUBST([ax_user_name])

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_mutex_init])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h limits.h locale.h errno.h])
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

AC_PATH_PROG(ax_help2man_exe, help2man, false)

AC_ARG_ENABLE([java],
	[AS_HELP_STRING([--enable-java],
		[Compile the uls library for java.])
	],
	[ ax_enable_ulsjni=$enableval ],
	[ ax_enable_ulsjni=no ]
	)
AS_IF([test "x$HAVE_JAVAC" != "xyes"], [ax_enable_ulsjni=no])
AM_CONDITIONAL([WANT_ULSJNI], [test x$ax_enable_ulsjni = xyes])

AC_ARG_WITH([java],
	[AS_HELP_STRING([--with-java=<java-home>],
		[Specify the JAVA_HOME directory.])],
	[ax_java_home=$withval],
	[ax_java_home=$JAVA_HOME]
	)
AS_IF([test "x$ax_java_home" = "x"], [ 
  ax_java_home=/usr/lib/jvm/java-17-openjdk-amd64
])
AC_SUBST([ax_java_home])

AS_IF([test "x$prefix" = "xNONE"], [ 
  prefix=$HOME/.local
])
ax_inst_dir="$prefix"
pfxstr1=`echo $ax_inst_dir | cut -d' ' -f1`
AS_IF([test "x$pfxstr1" != "x$ax_inst_dir"], [
  AC_MSG_ERROR([Improper install directory, '$ax_inst_dir'])
])
AS_IF([test "x$ax_inst_dir" = "x/"], [
  AC_MSG_ERROR([Not permit to install it on the root-directory!])
])
prefix="$ax_inst_dir"
AC_SUBST([ax_inst_dir])

is_under_home=no
case "$ax_inst_dir" in
  "$HOME"*)
    is_under_home=yes
    ;;
esac

AS_IF([test "x$is_under_home" = "xyes"], [ 
    ax_etc_dir=$HOME/.local/etc/uls
  ],
  [
    ax_etc_dir=/usr/local/etc/uls
  ])
AC_SUBST([ax_etc_dir])

case "$libdir" in
  $ax_inst_dir/lib)
    ;;
  $ax_inst_dir/lib/uls)
    ;;
  $ax_inst_dir/lib64)
    ;;
  $ax_inst_dir/lib64/uls)
    ;;
  *)
  	libdir="$ax_inst_dir/lib"
    ;;
esac
ax_lib_dir="$libdir"
AC_SUBST([ax_lib_dir])

bindir="$ax_inst_dir/bin"
ax_bin_dir="$bindir"
AC_SUBST([ax_bin_dir])

includedir="$ax_inst_dir/include"
ax_inc_dir="$includedir"
AC_SUBST([ax_inc_dir])

datadir="$ax_inst_dir/share"
ax_share_dir="$datadir"
AC_SUBST([ax_share_dir])

AC_CONFIG_FILES([Makefile
	ulsconfig/Makefile
	ulc_exam/Makefile
	src/Makefile
	ulscompat/Makefile
	ulscpp/Makefile
	ulsjni/Makefile
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
	tests/lex_input/Makefile
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
	examples/java/Makefile
	post_inst/Makefile
])

AC_OUTPUT
