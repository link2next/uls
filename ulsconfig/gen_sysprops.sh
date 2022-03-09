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
# FILE: gen_sysprops.sh
# DATE: July 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: To generate the uls system property file.
#   This file is part of ULS, Unified Lexical Scheme.
#

source ../ulsconfig/uls_common.sh

HOME_DIR="$PWD"

if [ $# -lt 5 ]; then
	echo "usage: $0 <uls_inst_dir> <etc_dir> <uls_version> <B|I> <out_file>"
	exit 1
fi

uls_inst_dir=$(readlink_m "$1")
uls_etc_dir=$(readlink_m "$2")

uls_version=$3

debug_num=${uls_version}
major_num=${debug_num%%.*}
debug_num=${debug_num#${major_num}.}

minor_num=${debug_num%%.*}
debug_num=${debug_num#${minor_num}.}

# stage = B or I
stage=$4

outfile=$5
fname=$(basename "$outfile")
out_dir=$(dirname "$outfile")
out_dir=$(readlink_m "$out_dir")

if [ ! -d "$out_dir" ]; then
	echo "can't find '$out_dir'!"
	exit 1
fi

outfpath="$out_dir/$fname"
if [ -e "$outfpath" ]; then
	rm -f "$outfpath"
fi

ARCH_INFO_SH=./check_arch.sh

gen_namval_pair()
{
	local nam
	local val

	nam=$1
	val=$2

	if [ -z "$val" ]; then
		echo "$nam='$val'" >> "$outfpath"
	else
		if echo "$val" | grep  -q ' '; then
			echo "$nam='$val'" >> "$outfpath"
		else
			echo "$nam=$val" >> "$outfpath"
		fi
	fi
}

gen_build_sysprops()
{
	local dname_lib
	local dname_share

	dname_lib=$1
	dname_share=$2

	gen_namval_pair ULS_ETC     "$uls_etc_dir"
	gen_namval_pair ULS_HOME    "$uls_inst_dir"
	gen_namval_pair ULS_ULCS    "$uls_inst_dir/$dname_share"
	gen_namval_pair ULS_SHARE   "$uls_inst_dir/$dname_share"
	gen_namval_pair ULS_DLLPATH "$uls_inst_dir/$dname_lib"
}

gen_inst_sysprops()
{
	local dname_lib=$1
	local dname_share=$2
	local dirpath

	dname_lib=$1
	dname_share=$2

	dirpath=$uls_inst_dir
	if [ "$dirpath" = "/" ]; then
		dirpath=""
	fi

	gen_namval_pair ULS_ETC     "$uls_etc_dir"
	gen_namval_pair ULS_HOME    "$uls_inst_dir"
	gen_namval_pair ULS_ULCS    "$dirpath/$dname_share/ulcs"
	gen_namval_pair ULS_SHARE   "$dirpath/$dname_share"
	gen_namval_pair ULS_DLLPATH "$dirpath/$dname_lib"
}

# start with utf8-BOM
echo -n -e \\xEF\\xBB\\xBF > "$outfpath"

arch_os="$(uname -s)"
if [ "$arch_os" = "Darwin" ]; then
	arch_os=macOS
fi 
gen_namval_pair OS $arch_os 

arch_bits=$($ARCH_INFO_SH)
gen_namval_pair ULS_ARCH_BITS $arch_bits

gen_namval_pair ULS_MBCS utf8

gen_namval_pair ULS_VERSION_MAJOR $major_num
gen_namval_pair ULS_VERSION_MINOR $minor_num
gen_namval_pair ULS_VERSION_DEBUG $debug_num

if [ "$stage" = "B" ]; then
	gen_build_sysprops src ulc_exam
else
	gen_inst_sysprops lib share/uls
fi
