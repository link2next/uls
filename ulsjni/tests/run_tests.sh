#!/bin/bash
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

#
# FILE: run_tests.sh
# DATE: Sep 2025
# AUTHOR: Stanley Hong
# DESCRIPTION: Testing all the programs in examples.
#   This file is part of ULS, Unified Lexical Scheme.
#

command_list_used="uname diff java"
for cmd in $command_list_used; do
	filepath=$(which $cmd)
	if [ -z "$filepath" ]; then
		echo "$cmd: not found!"
		exit 1
	fi
done

HOME_DIR=$PWD
opt_debug=no
arch_os=$(uname -s)
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
