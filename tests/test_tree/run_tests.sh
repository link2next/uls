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
# FILE: run_tests
# DATE: July 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: A testsuite for uls_stream.
#   This file is part of ULS, Unified Lexical Scheme.
#

opt_verbose=$ULS_VERBOSE
temp_dir=/tmp

uls_stream_dir=$1
MAKE_STREAM=$uls_stream_dir/uls_stream
ULC_FILE=$uls_stream_dir/sample.ulc
OUTPUT_FILE_DFL=a.uls

TEST_FILE_1=a.list
TARGET_DIR_1=.
OUTPUT_FILE_1=./res_a.txt

TEST_FILE_2=b.list
TARGET_DIR_2=$uls_stream_dir
OUTPUT_FILE_2=./uls_b2.txt

TEST_DIR=$temp_dir/uls_test
OUTPUT_FILE_DFL1="$TEST_DIR/uls_res1.txt"
OUTPUT_FILE_DFL2="$TEST_DIR/uls_res2.txt"

if [ -n "$LD_LIBRARY_PATH" ]; then
        export LD_LIBRARY_PATH="$1:$LD_LIBRARY_PATH"
else
        export LD_LIBRARY_PATH="$1"
fi

if [ $# -ge 2 ]; then
        export ULS_SYSPROPS=$2
fi

if [ ! -x $MAKE_STREAM ]; then
	echo "$MAKE_STREAM : not found";
	exit 1;
fi

if [ ! -e $TEST_DIR ]; then
	mkdir -p $TEST_DIR
fi

cmdline1="$MAKE_STREAM -L $ULC_FILE -C $TEST_FILE_1 -b $TARGET_DIR_1"
if [ "$opt_verbose" = "yes" ]; then
	echo "$cmdline1"
fi
$cmdline1

cmdline2="$MAKE_STREAM -L $ULC_FILE $OUTPUT_FILE_DFL"
if [ "$opt_verbose" = "yes" ]; then
	echo "$cmdline2"
fi
$cmdline2 > $OUTPUT_FILE_DFL1

cmdline3="diff --brief $OUTPUT_FILE_1 $OUTPUT_FILE_DFL1"
if [ "$opt_verbose" = "yes" ]; then
	echo "$cmdline3"
fi

$cmdline3
if [ $? != 0 ]; then
	echo "FAIL: $TEST_FILE_1"
	exit 1
fi

if [ -f $OUTPUT_FILE_DFL ]; then
	rm -rf $OUTPUT_FILE_DFL
fi

if [ -d $TEST_DIR ]; then
	rm -rf $TEST_DIR
fi

