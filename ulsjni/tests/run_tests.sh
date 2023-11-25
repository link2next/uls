#!/bin/bash

HOME_DIR=$PWD

if [ $# -lt 1 ]; then
	echo "usage: $0 <sysprops-path>"
	exit 1
fi

uls_sysprops_fpath=$1
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

LIB_PATH=$ULSJNI_HOME/libulsjni/.libs:../../src/.libs
export LD_LIBRARY_PATH=$LIB_PATH
export DYLD_LIBRARY_PATH=$LIB_PATH

cd "$ULSJNI_HOME/src"

test_outfile1="/tmp/$$_ulsjni_out_1_1.txt"
#echo "output $test_outfile1 ..."
java -Djava.library.path=$LIB_PATH $OPT_CP uls.tests.UlsDump input1.txt $TESTS_DIR > $test_outfile1
diff -q $TESTS_DIR/out_1_1.txt $test_outfile1
if [ $? != 0 ]; then
	echo "fail(stdout): diff for '$test_outfile1'";
	exit 1
fi

test_outfile2="/tmp/$$_ulsjni_out_2_1.txt"
#echo "output $test_outfile2 ..."
java -Djava.library.path=$LIB_PATH $OPT_CP uls.tests.UlsTestStream $TESTS_DIR > $test_outfile2
diff -q $TESTS_DIR/out_2_1.txt $test_outfile2
if [ $? != 0 ]; then
	echo "fail(stdout): diff for '$test_outfile2'";
	exit 1
fi

cd "$HOME_DIR"
exit 0
