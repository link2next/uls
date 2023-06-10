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
# FILE: run_examples.sh
# DATE: September 2018
# AUTHOR: Stanley Hong
# DESCRIPTION: Testing all the programs in examples.
#   This file is part of ULS, Unified Lexical Scheme.
#

if [ $# -lt 1 ]; then
	etc_dir=/usr/local/etc/uls
else
	etc_dir=$1
fi

if [ ! -d "$etc_dir" ]; then
	echo "$etc_dir: not directory!"
	exit 1
fi

HOME_DIR=$PWD
BASE_DIR=$PWD

EXES="test_1st/test_1st tokseq/tokseq yacc/ulc2xml DumpToks/DumpToks \
	Css3/Css3Toks Html5/Html5Toks Shell/ShellToks Mkf/MkfToks Natural/EngToks"

sysprops_fpath=$etc_dir/uls.sysprops
if [ ! -f "$sysprops_fpath" ]; then
	echo "$sysprops_fpath: not found!"
	exit 1
fi

# strip the utf-8 bom at the first 
tmp_sysprops_fpath=/tmp/uls_$$_sysprops
dd bs=1 skip=3 if="$sysprops_fpath" of=$tmp_sysprops_fpath 2> /dev/null

get_uls_homedir()
{
	local fpath=$1

	while read line; do
		if [ -z "$line" ]; then
			continue
		fi
		eval $line
	done  < "$fpath"
}

get_uls_homedir $tmp_sysprops_fpath
rm -f $tmp_sysprops_fpath

LD_LIBRARY_PATH=/usr/lib/uls:/usr/local/lib:/usr/local/lib64
LD_LIBRARY_PATH=$ULS_DLLPATH:$LD_LIBRARY_PATH
if [ $# -ge 1 ]; then
	LD_LIBRARY_PATH=$1:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

test_prog=
temp_dir=/tmp
tmpfile1="$temp_dir/lex1_res.txt"

test_1case()
{
	local outfile_res=$1
	local infile=$2

	cmdline="./$test_prog"

	if [ -f "$infile" ]; then
		$cmdline $infile > $tmpfile1
	else
		$cmdline > $tmpfile1
	fi

	if [ $? != 0 ]; then
		echo "fail to exec $test_prog";
		return 1
	fi

	diff -q $tmpfile1 $outfile_res;
	# assert: $outfile_res exists
	if [ -f $outfile_res1 ]; then
		diff -q $tmpfile1 $outfile_res;
		if [ $? != 0 ]; then
			echo "fail(stdout): diff for '$test_prog'";
		fi
	fi

	rm $tmpfile1
	return 0
}

for f in $EXES; do
	cd $BASE_DIR

	test_prog=`basename $f`
	dir=${f%/${test_prog}}
	cd $dir

	if [ ! -e $test_prog ]; then
		echo "$test_prog not exists, continuing, ..."
		continue;
	fi

	in_pfx=`echo ${dir} | tr '[A-Z]' '[a-z]'`

	in_file="${in_pfx}_sam.txt";
	out_file="${in_pfx}_out.txt";

	echo "checking $dir ..."

	if [ -f $out_file ]; then
		echo "   $test_prog($in_file, $out_file)..."
		test_1case $out_file $in_file 
	fi

	echo
done

cd $HOME_DIR
echo "done."

