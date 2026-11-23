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
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

#
# FILE: cleanup_uls.sh
# DATE: Sep. 2026
# AUTHOR: Stanley Hong
# DESCRIPTION: A script to clean-up ULS package from your system.
#   This file is part of ULS, Unified Lexical Scheme.
#

check_commands_used()
{
	local cmd progpath line1
	local stat=0

	for cmd in $* basename dirname; do
		progpath=`which $cmd`
		if [ -z "$progpath" ]; then
			echo "$cmd: not found!"
			stat=1
		fi
	done

	return $stat
}

if ! check_commands_used mkdir find; then
	exit 1
fi

if [ $# -lt 2 ]; then
	echo "usage: $0 <target-dir> [clean|distcleran|clobber]"
	exit 1
fi

homedir=$PWD
workdir=$1
progmode=$2

if [ ! -d "$workdir" ]; then
	echo "$workdir: not found!"
	exit 1
fi

add_files()
{
	mode=$1
	list_filepath=$2

	if [ $mode = 'clean' ]; then
		find . \( -name 'Sample*Lex.cpp' -or -name 'Sample*Lex.h' \) -type f >> $list_filepath
		find . \( -name 'Sample*Lex.java' -or -name 'sample_lex.[ch]' \) -type f >> $list_filepath
		find . \( -name '.deps' -or -name '.libs' \) -type d -print >> $list_filepath
		find . \( -name '*.class' -or -name 'uls*.jar' \) -type f -print >> $list_filepath
		echo 'configure~' >> $list_filepath
	elif [ $mode = 'distclean' ]; then
		find . -name '.deps' -type d -print >> $list_filepath
		find . -name 'Makefile' -type f -print >> $list_filepath
		echo 'tools/uls_stream/tmpl_ex.uls' >> $list_filepath
	elif [ $mode = 'clobber' ]; then
		echo 'autom4te.cache' >> $list_filepath
		echo 'm4' >> $list_filepath
		find . -name 'Makefile.in' -type f -print >> $list_filepath
		find . -name 'config.*' -type f -print >> $list_filepath
		echo 'configure' >> $list_filepath
		echo 'aclocal.m4' >> $list_filepath
		echo 'depcomp' >> $list_filepath
		echo 'install-sh' >> $list_filepath
		echo 'ltmain.sh' >> $list_filepath
		echo 'ylwrap' >> $list_filepath
		echo 'missing' >> $list_filepath
		echo 'compile' >> $list_filepath
		echo 'libtool' >> $list_filepath
		echo 'stamp-h1' >> $list_filepath
	fi
}

trash_uls=/tmp/__trash_uls_$$__
if [ -d $trash_uls ]; then
	echo "$trash_uls: already exists!"
	exit 1
fi

mkdir $trash_uls
if [ ! -d $trash_uls ]; then
	echo "Can't mkdir temp-dir in /tmp ..."
	exit 1
fi

listof_delitems="$trash_uls/list_cacheitems.txt"
echo "Cleanning up $workdir ..."
cd "$workdir"

if [ $progmode = 'clean' ]; then
	add_files 'clean' $listof_delitems
elif [ $progmode = 'distclean' ]; then
	add_files 'clean' $listof_delitems
	add_files 'distclean' $listof_delitems
elif [ $progmode = 'clobber' ]; then
	add_files 'clean' $listof_delitems
	add_files 'distclean' $listof_delitems
	add_files 'clobber' $listof_delitems
else
	echo "$progmode: unknown program mode"
	exit 1
fi

lno=1
while read fpath; do
	if [ -z "$fpath" ]; then
		continue
	fi

	filename=`basename "$fpath"`
	filepath2="$trash_uls/${lno}_${filename}"

#	echo "$filepath2"
	if [ -e "$fpath" ]; then
		mv $fpath $filepath2
	fi
	lno=$(($lno+1))
done < $listof_delitems

if [ ! -d ./m4 ]; then
	mkdir m4
fi

cd /tmp
rm -rf $trash_uls
