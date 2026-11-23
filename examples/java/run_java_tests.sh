#!/bin/sh
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

for cmd in uname diff java; do
	filepath=`which $cmd`
	if [ -z "$filepath" ]; then
		echo "$cmd: not found!"
		exit 1
	fi
done

HOME_DIR=$PWD
work_dir=$HOME_DIR

opt_debug=no
arch_os=`uname -s`
#arch_os = { 'Darwin', 'Linux', ... }
#echo "arch-os is '$arch_os'"

if [ $# -lt 3 ]; then
	echo "usage: $0 <input_dir> <lib_dir> <uls_jar>"
	exit 1
fi

input_dir="$1"
if [ ! -d "$input_dir" ]; then
	echo "$input_dir: not found!"
	exit 1
fi

lib_dir="$2"
if [ ! -d "$lib_dir" ]; then
	echo "$lib_dir: not found!"
	exit 1
fi

filepath_jar="$lib_dir/$3"
if [ ! -f "$filepath_jar" ]; then
	echo "$filepath_jar: not found!"
	exit 1
fi

OPT_CP="-cp ./classes:$filepath_jar"

LIB_PATH="$lib_dir"
export LD_LIBRARY_PATH=$lib_dir
if [ "$arch_os" = "Darwin" ]; then
	export DYLD_LIBRARY_PATH=$lib_dir
fi

echo "Testing ulsjni at $work_dir..."
cd "$work_dir"

echo "  ulsjni(1.1)"
test_outfile1="/tmp/$$_ulsjni_out_1_1.txt"

java -Djava.library.path="$LIB_PATH" $OPT_CP uls.tests.UlsDump . . > $test_outfile1
if [ $? != 0 ]; then
	echo "Failed to execute 'java uls.tests.UlsDump'!"
	echo "java -Djava.library.path=$LIB_PATH $OPT_CP uls.tests.UlsDump . $input_dir"
	exit 1
fi

diff -q $work_dir/out_1_1.txt $test_outfile1
if [ $? != 0 ]; then
	echo "fail(stdout): diff ... '$test_outfile1'"
	exit 1
fi

echo "  ulsjni(2.1)"
test_outfile2="/tmp/$$_ulsjni_out_2_1.txt"

java -Djava.library.path="$LIB_PATH" $OPT_CP uls.tests.UlsTestStream . $input_dir > $test_outfile2
if [ $? != 0 ]; then
	echo "Failed to execute 'java uls.tests.UlsTestStream'!"
	echo "java -Djava.library.path=$LIB_PATH $OPT_CP uls.tests.UlsTestStream . $input_dir"
	exit 1
fi

diff -q $work_dir/out_2_1.txt $test_outfile2
if [ $? != 0 ]; then
	echo "fail(stdout): diff ... '$test_outfile2'"
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
