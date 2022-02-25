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
# DESCRIPTION: A testsuite for ulc2class
#   This file is part of ULS, Unified Lexical Scheme.
#

test_home="$PWD"
progname=`basename $0`
#opt_verbose=$ULS_VERBOSE
opt_verbose=no

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$1:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$1"
fi

if [ $# -ge 2 ]; then
	export ULS_SYSPROPS=$2
fi

MAX_N_CASES=16
test_prog=

MODE_LIST="c cpp java"
temp_dir="/tmp"

test_1case()
{
	local ts=$1
	local tc=$2
	local infile=$3
	local outfile_res=$4
	local errfile_res=$5
	local tmpfile1
	local tmpfile2
	local cmdline

	tmpfile1="$temp_dir/$outfile_res"
	tmpfile2="$temp_dir/lex2_err.txt"

	cmdline="$test_prog --lang=$ts -o $tmpfile1"

	if [ $infile != "-" ]; then
		cmdline="$cmdline $infile"
	fi

	if [ "$opt_verbose" = "yes" ]; then
		echo "   $cmdline"
	fi

#	echo $cmdline
	$cmdline 2> $tmpfile2
	if [ $? != 0 ]; then
		echo "fail to exec $test_prog"
		return 1
	fi

	if [ $outfile_res != "-"  ]; then
		diff -q $tmpfile1 $outfile_res;
		if [ $? != 0 ]; then
			echo "fail(stdout): diff for '$tmpfile1'";
			return 1
		fi
	fi

	if [ $errfile_res != "-" ]; then
		diff -q $tmpfile2 $errfile_res;
		if [ $? != 0 ]; then
			echo "fail(stderr): diff for '$tmpfile2'";
			return 1
		fi
	fi

	rm $tmpfile1 $tmpfile2
}

test_progname=ulc2class
test_prog=./"$test_progname"
if [ ! -e "$test_prog" ]; then
	echo "$progname: $test_prog not exists, exiting ..."
	exit 1
fi

i=1
while [ $i -le $MAX_N_CASES ]; do
	in_file="sample${i}.ulc";
	if [ ! -f "$in_file" ]; then
		i=$(($i+1))
		continue;
	fi
	echo "checking $in_file ..."

	for m in $MODE_LIST; do
		n_files=0

		out_file="out_${m}_${i}.txt";

		if [ -f "$out_file" ]; then
			n_files=$(($n_files+1))
		else
			out_file="-"
		fi

		err_file="err_${m}_${i}.txt";
		if [ -f "$err_file" ]; then
			n_files=$(($n_files+1))
		else
			err_file="-"
		fi

		if [ $n_files -gt 0 ]; then
			echo "  $test_progname($in_file, $m)"
			test_1case $m $i $in_file $out_file $err_file
			if [ $? != 0 ]; then
				exit 1
			fi
		fi
	done
	i=$(($i+1))
done

echo "done."

