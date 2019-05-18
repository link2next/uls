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
# DATE: June 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: A testsuite for the executable uls_stream
#   This file is part of ULS, Unified Lexical Scheme.
#

#opt_verbose=$ULS_VERBOSE
opt_verbose=no
opt_noexec=no

ULC_FILE="sample.ulc"
test_progname=uls_stream
PROG="./$test_progname -s -L $ULC_FILE"

TARGET_FILES="bin-le bin-be txt"
BIN_FORMATS="bin-le bin-be txt"
DFL_OUTPUT_FILE="a.uls"

temp_dir="/tmp"
TEST_DIR=$temp_dir/uls_test

if [ $# -ge 1 ]; then
	if [ -n "$1" ]; then
		export LD_LIBRARY_PATH="$1:$LD_LIBRARY_PATH"
	fi
fi

if [ ! -e $TEST_DIR ]; then
	mkdir -p $TEST_DIR
fi

if [ ! -d $TEST_DIR ]; then
	echo "'$TEST_DIR' is not a directory";
	exit 1;
fi

for fmt in $BIN_FORMATS; do
	cmdline="$PROG -o $TEST_DIR/${fmt}.txt ${fmt}.uls"
	if [ "$opt_verbose" = "yes" ]; then
		echo "$cmdline"
	fi
	$cmdline
done

for testfname in $TARGET_FILES; do
	testfile="$testfname.uls"
	echo "checking $testfile, ...";

	for fmt in $BIN_FORMATS; do
		echo "   ${fmt}-formatting, ...";

		cmdline="$PROG -t $fmt $testfile"
		if [ "$opt_verbose" = "yes" ]; then
			echo "     $cmdline"
		fi

		if [ "$opt_noexec" = "no" ]; then
			$cmdline
			if [ $? != 0 ]; then
				echo "FAIL: $cmdline";
				exit 1
			fi
		fi

		cmdline="$PROG -o $TEST_DIR/$uls_${fmt}_2.txt $DFL_OUTPUT_FILE"
		if [ "$opt_verbose" = "yes" ]; then
			echo "     $cmdline"
		fi
		if [ "$opt_noexec" = "no" ]; then
			$cmdline
			if [ $? != 0 ]; then
				echo "FAIL: $cmdline";
				exit 1
			fi
		fi

		if [ "$opt_noexec" = "no" ]; then
			cmdline="diff $TEST_DIR/${fmt}.txt $TEST_DIR/${fmt}_2.txt"
			if [ "$opt_verbose" = "yes" ]; then
				echo "     $cmdline"
			fi
			$cmdline
			if [ $? != 0 ]; then
				echo "FAIL: $fmt formatting of $testfile";
				exit 1
			fi
		fi
	done
done

if [ "$opt_noexec" = "yes" ]; then
	exit 0
fi

if [ -d $TEST_DIR ]; then
	rm -rf $TEST_DIR
fi

TMPL_ULSFILE=tmpl_ex.uls
echo "checking tmpl_ex.txt ..."
cmdline="$PROG -b -o $TMPL_ULSFILE -TT tmpl_ex.txt"
if [ "$opt_verbose" = "yes" ]; then
	echo "  $cmdline"
fi
$cmdline

# tmpl_ex(int)
ANSWER_FILE_TXT1=tmpl_ex_int.txt
DUMPFILE_TXT1="/tmp/uls_b.txt"
echo "checking tmpl_ex(int) ..."
if [ "$opt_verbose" = "yes" ]; then
	echo $PROG T=\'int\' $TMPL_ULSFILE
fi
$PROG T='int' $TMPL_ULSFILE > $DUMPFILE_TXT1

cmdline="diff $ANSWER_FILE_TXT1 $DUMPFILE_TXT1"
if [ "$opt_verbose" = "yes" ]; then
	echo "  $cmdline"
fi
$cmdline
if [ $? != 0 ]; then
	echo "FAIL: $testfile";
fi
rm -f $DUMPFILE_TXT1

# tmpl_ex(unsigned long)
ANSWER_FILE_TXT2=tmpl_ex_ulong.txt
DUMPFILE_TXT2="/tmp/uls_b.txt"
echo "checking tmpl_ex(ulong) ..."
if [ "$opt_verbose" = "yes" ]; then
	echo $PROG T=\'unsigned long\' $TMPL_ULSFILE
fi
$PROG T='unsigned long' $TMPL_ULSFILE > $DUMPFILE_TXT2

cmdline="diff $ANSWER_FILE_TXT2 $DUMPFILE_TXT2"
if [ "$opt_verbose" = "yes" ]; then
	echo "  $cmdline"
fi
$cmdline
if [ $? != 0 ]; then
	echo "FAIL: $testfile";
fi


rm -f $DUMPFILE_TXT2

rm -f $TMPL_ULSFILE
if [ -f $DFL_OUTPUT_FILE ]; then
	rm -f $DFL_OUTPUT_FILE
fi
