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
# DESCRIPTION: A testsuite for programs in 'tests'.
#   This file is part of ULS, Unified Lexical Scheme.
#

progname=`basename $0`
home_dir="$PWD"
test_dname=$(basename "$home_dir")

MAX_N_CASES=16
MAX_N_MODES=8

temp_dir="/tmp"
test_prog=

enc_mode=ustr

if [ $# -lt 2 ]; then
	echo "usage: $0 <test_prog> <LD_LIBPATH> [sysprops-path]"
	exit 1
fi

test_prog=$1
if [ ! -e ./$test_prog ]; then
	echo "$0: $test_prog not exists!"
	exit 1
fi

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$2:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$2"
fi

if [ $# -ge 3 ]; then
        export ULS_SYSPROPS=$3
fi

if [ $# -ge 4 ]; then
	enc_mode=$4
fi

echo "Checking $test_dname enc_mode=$enc_mode..."

test_1case()
{
	local ts=$1
	local tc=$2
	local infile=$3
	local outfile_res=$4
	local errfile_res=$5
	local tmpfile1="./lex1_res.txt"
	local tmpfile2="./lex2_res.txt"
	local cmdline

	cmdline="./$test_prog -m${ts}"

	if [ $infile != "-" ]; then
		cmdline="$cmdline $infile"
	fi

	$cmdline > $tmpfile1 2> $tmpfile2
	if [ $? != 0 ]; then
		echo "fail to exec $test_prog"
		return 1
	fi

	if [ $outfile_res != "-"  ]; then
		diff -q $tmpfile1 $outfile_res;
		if [ $? != 0 ]; then
			echo "fail(stdout): diff for '$test_prog'";
			return 1
		fi
	fi

	if [ $errfile_res != "-" ]; then
		diff -q $tmpfile2 $errfile_res;
		if [ $? != 0 ]; then
			echo "fail(stderr): diff for '$test_prog'";
			return 1
		fi
	fi

	rm $tmpfile1 $tmpfile2

	return 0
}

m=0
while [ $m -lt $MAX_N_MODES ]; do
	i=0
	while [ $i -le $MAX_N_CASES ]; do
		n_files=0
		in_file="in_${m}_${i}.txt";
		if [ ! -f "$in_file" ]; then
			in_file="-"
		fi

		if [ $enc_mode = 'wstr' ]; then
			out_file="outw_${m}_${i}.txt";
			if [ ! -f "$out_file" ]; then
				out_file="out_${m}_${i}.txt";
			fi
		else
			out_file="out_${m}_${i}.txt";
		fi

		if [ -f "$out_file" ]; then
			n_files=$(($n_files+1))
#			echo "output[$m][$i] = $out_file"
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
			echo "   $test_prog($in_file)..."
			test_1case $m $i $in_file $out_file $err_file
			if [ $? != 0 ]; then
				exit 1
			fi
		fi

		i=$(($i+1))
	done
	m=$(($m+1))
done


