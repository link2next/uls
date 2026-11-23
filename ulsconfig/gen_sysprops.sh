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

. ./common.sh

HOME_DIR="$PWD"

if [ $# -lt 5 ]; then
	echo "usage: $0 <B|I> <out_file> <uls_version> <etc_dir> <uls_inst_dir> [uls_lib_dir]"
	exit 1
fi

# stage = B(build) or I(install)
stage=$1

outfile=$2
fname=`basename "$outfile"`
out_dir=`dirname "$outfile"`
out_dir=`readpath_abs "$out_dir"`

if [ ! -d "$out_dir" ]; then
	echo "can't find '$out_dir'!"
	exit 1
fi

outfpath="$out_dir/$fname"
if [ -e "$outfpath" ]; then
	rm -f "$outfpath"
fi

uls_version=$3
debug_num=${uls_version}
major_num=${debug_num%%.*}
debug_num=${debug_num#${major_num}.}
minor_num=${debug_num%%.*}
debug_num=${debug_num#${minor_num}.}

uls_etc_dir=`readpath_abs "$4"`
uls_inst_dir=`readpath_abs "$5"`

if [ "$stage" = "I" ]; then
	uls_lib_dir=`readpath_abs "$6"`
else
	uls_lib_dir=
fi

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
	local dname_lib=$1
	local dname_share=$2

	gen_namval_pair ULS_ETC     "$uls_etc_dir"
	gen_namval_pair ULS_HOME    "$uls_inst_dir"
	gen_namval_pair ULS_ULCS    "$uls_inst_dir/$dname_share"
	gen_namval_pair ULS_SHARE   "$uls_inst_dir/$dname_share"
	gen_namval_pair ULS_DLLPATH "$uls_inst_dir/$dname_lib/.libs:$uls_inst_dir/ulscpp/.libs:$uls_inst_dir/ulsjni/libulsjni/.libs"
}

gen_inst_sysprops()
{
	local libdir dname_share
	local dirpath

	libdir=$1
	dname_share=$2

	dirpath=$uls_inst_dir
	if [ "$dirpath" = "/" ]; then
		dirpath=""
	fi

	gen_namval_pair ULS_ETC     "$uls_etc_dir"
	gen_namval_pair ULS_HOME    "$uls_inst_dir"
	gen_namval_pair ULS_ULCS    "$dirpath/$dname_share/ulcs"
	gen_namval_pair ULS_SHARE   "$dirpath/$dname_share"
	gen_namval_pair ULS_DLLPATH "$libdir"
}

# start with utf8-BOM
#echo -n -e \\xEF\\xBB\\xBF > "$outfpath"
# The e-option may fail if its interpreter isn't bash.
cat ./utf8bom.txt > "$outfpath"

arch_os="`uname -s`"
if [ "$arch_os" = "Darwin" ]; then
	arch_os=macOS
fi 
gen_namval_pair OS $arch_os 
gen_namval_pair ULS_MBCS utf-8
gen_namval_pair ULS_VERSION_MAJOR $major_num
gen_namval_pair ULS_VERSION_MINOR $minor_num
gen_namval_pair ULS_VERSION_DEBUG $debug_num

if [ "$stage" = "B" ]; then
	gen_build_sysprops src ulc_exam
else
	gen_inst_sysprops "$uls_lib_dir" share/uls
fi

