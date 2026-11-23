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
# FILE: proc_ulc_files
# DATE: July 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: generating the header files from ulc-files.
#   This file is part of ULS, Unified Lexical Scheme.
#

. ../ulsconfig/common.sh

HOME_DIR=$PWD
PACKAGE_DOMAIN=ULS.Collection

ULSLANGS=$1
if [ ! -f "$ULSLANGS" ]; then
	echo "$ULSLANGS: not found!"
	exit 1
fi

TARGET_DIR=$2
if [ ! -d "$TARGET_DIR" ]; then
	echo "$TARGET_DIR: not found!"
	exit 1
fi
TARGET_DIR=`readpath_abs "$TARGET_DIR"`

TOP_BIN_DIR=$3
if [ ! -d "$TOP_BIN_DIR" ]; then
	echo "$TOP_BIN_DIR: not found!"
	exit 1
fi
TOP_BIN_DIR=`readpath_abs "$TOP_BIN_DIR"`

TMP_SYSPROPS_FILE=$4
if [ ! -f "$TMP_SYSPROPS_FILE" ]; then
	echo "$TMP_SYSPROPS_FILE: not found!"
	exit 1
fi
TMP_SYSPROPS_FILE=`readpath_abs "$TMP_SYSPROPS_FILE"`

ULC2CLASS=$TOP_BIN_DIR/tools/ulc2class/ulc2class
if [ ! -x "$ULC2CLASS" ]; then
	echo "$ULC2CLASS: not executable!"
	exit 1
fi

LD_LIBPATH=$TOP_BIN_DIR/src

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$LD_LIBPATH:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$LD_LIBPATH:"
fi

WORK_DIR=$PWD

ALIGN_COL=23

draw_progress()
{
	local line_type=$1 dash_line=
	local n_cols=$2 col

	if [ $n_cols -lt 2 ]; then
		return;
	fi
	n_cols=$(($n_cols-2))

	col=0
	while [ $col -lt $n_cols ]; do
		dash_line=${dash_line}${line_type}
		col=$(($col+1))
	done

	echo $dash_line
}

export ULC_PATH=$WORK_DIR
export ULS_SYSPROPS=$TMP_SYSPROPS_FILE

while read name enum_name ClassName tok_pfx; do
	if [ -z "$name" ]; then
		continue
	fi

	first_ch=`echo "$name" | cut -b1`
	if [ "$first_ch" = "#" ]; then
		continue
	fi

	if [ "$tok_pfx" != "-" ]; then
		pfx_optstr="-p$tok_pfx"
	else
		pfx_optstr=
	fi

	in_fname="${name}.ulc"
	line=" $in_fname"
	curcol=${#in_fname}
	curcol=$(($curcol+1))
	dashes=`draw_progress '_' $(($ALIGN_COL-$curcol))`
	line="$line $dashes "

	out_fpath1=
	if [ "$enum_name" != "-" ]; then
		out_fname="${enum_name}_lex.h"
		out_fpath1="${TARGET_DIR}/$out_fname"
		line="$line $out_fname"
	fi

	out_fpath2=
	if [ "$ClassName" != "-" ]; then
		out_fname="${ClassName}.h"
		out_fpath2="${TARGET_DIR}/$out_fname"
		line="$line $out_fname"
	fi

	if [ -n "$out_fpath1" ]; then
		$ULC2CLASS -o "$out_fpath1" -y -lc -e ${enum_name}_token $pfx_optstr $in_fname
	fi

	if [ -n "$out_fpath2" ]; then
		$ULC2CLASS -o "$out_fpath2" -y -n ${PACKAGE_DOMAIN}.${ClassName} $pfx_optstr $in_fname
	fi

	echo "$line"
done  < "$ULSLANGS"
