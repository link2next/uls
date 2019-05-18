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
# FILE: make_uls_examples_tar.sh
# DATE: June 2015
# AUTHOR: Stanley Hong
# DESCRIPTION: To create the uls_examples.tar which is used by 'setup_uls_examples.sh'
#   This file is part of ULS, Unified Lexical Scheme.
#

source ../ulsconfig/uls_common.sh

usage()
{
	echo "usage: $0  <tmpl-tar> <src-dir> <file-list.txt> <out-dir>"
	exit 1
}

if [ $# -lt 4 ]; then
	usage
fi

tmpl_tar=$(readlink_m "$1")
src_dir=$(readlink_m "$2")
list_file=$(readlink_m "$3")
out_dir=$(readlink_m "$4")

if [ ! -f "$tmpl_tar" -o ! -f "$list_file" ]; then
	usage
fi

if [ ! -d "$src_dir" -o ! -d "$out_dir" ]; then
	usage
fi

pkg_fname=uls_examples
temp_dir=/tmp
work_dir=$temp_dir/$pkg_fname

export_uls_examples()
{
	local flist fpath line dpath

	flist=$1

	while read fpath; do 
		[ -n "$fpath" ] || continue
		line=${fpath##'#'*}
		if [ -z "$line" ]; then
			continue
		fi
	
		dpath=$(dirname "$fpath")
	
		if [ ! -d "$dpath" ]; then
			mkdir -p "$dpath"
			if [ $? != 0 ]; then
				echo "fail to make '$dpath'"
				exit 1
			fi
		fi

		if [ ! -f "$src_dir/$fpath" ]; then
			echo "$src_dir/$fpath: not found"
			exit 1
		fi

		cp -f "$src_dir/$fpath" "$dpath"/
	
	done < "$flist"
}

tar -C "$temp_dir" -xf "$tmpl_tar"
cd "$temp_dir"
if [ ! -d "$pkg_fname" ]; then
	echo "$pkg_fname: not found!"
	exit 1
fi

cd "$work_dir"
export_uls_examples "$list_file"

cd "$temp_dir"
tar -cf "$out_dir/$pkg_fname.tar" "$pkg_fname"
rm -rf "$pkg_fname"

cd "$src_dir"

