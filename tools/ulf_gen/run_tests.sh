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
# DESCRIPTION: A testsuite for ulf_gen
#   This file is part of ULS, Unified Lexical Scheme.
#

MAKE_ULF=./ulf_gen
ULC_FILE=./sample.ulc
temp_dir=/tmp
#opt_verbose=$ULS_VERBOSE
opt_verbose=no

test_tree_dir=$1
TEST_FILE_1=$test_tree_dir/a.list
TARGET_DIR_1=$test_tree_dir
OUTPUT_FILE_1=./sample.ulf

TEST_DIR=$temp_dir/uls_test
OUTPUT_FILE_DFL="$TEST_DIR/uls_res3.txt"

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$2:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$2"
fi

if [ $# -ge 3 ]; then
	export ULS_SYSPROPS=$3
fi

if [ ! -x $MAKE_ULF ]; then
	echo "$MAKE_ULF : not found";
	exit 1;
fi

if [ ! -e $TEST_DIR ]; then
	mkdir -p $TEST_DIR
fi

cmdline="$MAKE_ULF -o  $OUTPUT_FILE_DFL -L $ULC_FILE -l $TEST_FILE_1 $TARGET_DIR_1"
echo $cmdline

if [ "$opt_verbose" = "yes" ]; then
	echo "$cmdline"
fi

$cmdline
#diff --brief $OUTPUT_FILE_DFL $OUTPUT_FILE_1
if [ $? != 0 ]; then
	echo "FAIL: $TEST_FILE_1"
	exit 1
fi

if [ -d $TEST_DIR ]; then
	rm -rf $TEST_DIR
fi
