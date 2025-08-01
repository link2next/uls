#!/bin/bash

HOME_DIR=$PWD
opt_debug=no
arch_os="$(uname -s)"
#arch_os = { 'Darwin', 'Linux', ... }

if [ $# -ge 1 ]; then
	if [ "$1" = "debug" ]; then
		opt_debug=yes
	fi
fi


if [ "$arch_os" = "Darwin" ]; then
	uls_sysprops_fpath=/tmp/uls_sysprops.txt
	if [ -f "$uls_sysprops_fpath" ]; then
		echo "Please run it after 'make install'"
		exit 0
	fi
	uls_sysprops_fpath=/usr/local/etc/uls/uls.sysprops
else
	uls_sysprops_fpath=/tmp/uls_sysprops.txt
fi

if [ ! -f "$uls_sysprops_fpath" ]; then
	echo "$uls_sysprops_fpath: not found!"
	exit 1
fi

ULSJNI_HOME=".."
if [ ! -d "$ULSJNI_HOME" ]; then
	echo "$ULSJNI_HOME: not found!"
	exit 1
fi
cd "$ULSJNI_HOME"
ULSJNI_HOME=$PWD
cd "$HOME_DIR"

TESTS_DIR=$ULSJNI_HOME/tests

ULS_SRCDIR="$ULSJNI_HOME/.."
if [ ! -d "$ULS_SRCDIR" ]; then
	echo "$ULS_SRCDIR: not found!"
	exit 1
fi
cd "$ULS_SRCDIR"
ULS_SRCDIR=$PWD
cd "$ULS_SRCDIR"

if [ ! -d "$ULS_SRCDIR/src/.libs" ]; then
	echo "libuls not yet built!"
	exit 1
fi

ULS_CLASSPATH=.:$ULSJNI_HOME/classes/

if [ -n "$ULS_CLASSPATH" ]; then
	OPT_CP="-cp $ULS_CLASSPATH"
else
	OPT_CP=""
fi

LIB_PATH=$ULSJNI_HOME/libulsjni/.libs:$ULS_SRCDIR/src/.libs
LIB_PATH1=$ULSJNI_HOME/libulsjni:$ULS_SRCDIR/src
export LD_LIBRARY_PATH=$LIB_PATH
#export DYLD_LIBRARY_PATH=$LIB_PATH

cd "$ULSJNI_HOME/src"

test_outfile1="/tmp/$$_ulsjni_out_1_1.txt"
java -Djava.library.path="$LIB_PATH" $OPT_CP uls.tests.UlsDump input1.txt $TESTS_DIR > $test_outfile1
if [ $? != 0 ]; then
	echo "Failed to execute 'java uls.tests.UlsDump'!"
	cat $test_outfile1
	exit 1
fi

test_outfile2="/tmp/$$_ulsjni_out_2_1.txt"
java -Djava.library.path="$LIB_PATH" $OPT_CP uls.tests.UlsTestStream $TESTS_DIR > $test_outfile2
if [ $? != 0 ]; then
	echo "Failed to execute 'java uls.tests.UlsTestStream'!"
	cat $test_outfile2
	exit 1
fi

diff -q $TESTS_DIR/out_1_1.txt $test_outfile1
if [ $? != 0 ]; then
	echo "fail(stdout): diff for '$test_outfile1'"
	exit 1
fi

diff -q $TESTS_DIR/out_2_1.txt $test_outfile2
if [ $? != 0 ]; then
	echo "fail(stdout): diff for '$test_outfile2'"
	exit 1
fi

if [ "$opt_debug" = "no" ]; then
	rm $test_outfile1
	rm $test_outfile2
else
	echo "output $test_outfile1 ..."
	echo "output $test_outfile2 ..."
fi

cd "$HOME_DIR"
exit 0
