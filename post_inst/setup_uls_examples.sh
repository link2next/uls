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
# FILE: setup_uls_examples.sh
# DATE: 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: To create uls examples directories from uls_examples.tar
#   This file is part of ULS, Unified Lexical Scheme.
#

home_dir="$PWD"
pkg_fname=uls_examples

usage()
{
	echo "usage: $0 <dest-dir>"
	echo " For examples,"
	echo "   $0 ."
	echo "   $0 ~"
	echo "   $0 ~/projs"
	exit 0
}

progpath=$0
progname=$(basename "$progpath")
progdir=$(dirname "$progpath")

source $progdir/uls_common.sh
progdir=$(readlink_m "$progdir")

uninst_info_sh=uls_uninst_info.sh
if [ ! -x $progdir/$uninst_info_sh ]; then
	echo "$uninst_info_sh: not executable"
	exit 1
fi
source $progdir/$uninst_info_sh

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

examples_tar=$ULS_SHARE/$pkg_fname.tar
if [ ! -f "$examples_tar" ]; then
	echo "Can't find the uls-examples in your system."
	echo "$examples_tar: not found!"
	exit 1
fi

cmd_list="ls cp mv rm mkdir tar"

for cmd in $cmd_list; do
	if ! which $cmd > /dev/null; then
		echo "$cmd: not found!"
		exit 1
	fi 
done

gen_mkcfg_of_uls_examples()
{
	local inst_dir
	local out_fpath
	local dpath_out_fpath

	inst_dir="$1"
	out_fpath="$2"

	if [ ! -d "$inst_dir" ]; then
		echo "$inst_dir: not a directory!"
		exit 1
	fi

	dpath_out_fpath=$(dirname "$out_fpath")
	if [ ! -d "$dpath_out_fpath" ]; then
		echo "$dpath_out_fpath: not a directory!"
		exit 1
	fi

	cat > $out_fpath <<TXT
uls_inst_dir = $inst_dir
uls_etc_dir = $ULS_ETC 
uls_bin_dir = \$(uls_inst_dir)/bin
uls_inc_dir = \$(uls_inst_dir)/include
uls_lib_dir = \$(uls_inst_dir)/lib
ulc2class_exe = \$(uls_bin_dir)/ulc2class
TXT
}

if [ $# -lt 1 ]; then
	usage
fi

dest_dir=$(readlink_m "$1")
if [ ! -d "$dest_dir" ]; then
	mkdir -p "$dest_dir"
fi

echo "Creating the uls examples in $dest_dir ..."
tar -C "$dest_dir" -xf $examples_tar
doc_dir=$dest_dir/$pkg_fname/doc
if [ $? != 0 ] || [ ! -d "$doc_dir" ]; then
	echo "Failed to create the directory of uls-examples or"
	echo "  can't find '$doc_dir'!"
	exit 1
fi

if ls -1 "$ULS_SHARE"/*.pdf 1> /dev/null 2>&1; then
	cp -f "$ULS_SHARE"/*.pdf "$doc_dir"/
fi

gen_mkcfg_of_uls_examples "$ULS_HOME" "$dest_dir/$pkg_fname/cfg.mk"
cd "$dest_dir/$pkg_fname"
if [ ! -f "configure" ]; then
	if which autoreconf > /dev/null; then
		autoreconf -i
		rc=$?
	else
		rc=-1;
	fi

	if [ $rc != 0 ]; then
		echo "Can't generate the 'configure' for $pkg_fname"
		exit 1
	fi
fi
./configure

echo "done!"
cd "$home_dir"

echo '=============================================================================='
echo "The examples of ULS is installed in $dest_dir/$pkg_fname"
echo "Refer to 'README' to build and test the examples."
echo '=============================================================================='
echo
