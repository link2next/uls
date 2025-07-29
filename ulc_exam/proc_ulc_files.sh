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
# FILE: proc_ulc_files
# DATE: July 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: generating the header files from ulc-files.
#   This file is part of ULS, Unified Lexical Scheme.
#

source ../ulsconfig/uls_common.sh

HOME_DIR=$PWD
PACKAGE_DOMAIN=ULS.Collection

ULSLANGS=$1

TARGET_DIR=$2
if [ ! -d "$TARGET_DIR" ]; then
	echo "$TARGET_DIR: not found!"
	exit 1
fi

TOP_BIN_DIR=$3
if [ ! -d "$TOP_BIN_DIR" ]; then
	echo "$TOP_BIN_DIR: not found!"
	exit 1
fi

TMP_SYSPROPS_FILE=$4
if [ ! -f "$TMP_SYSPROPS_FILE" ]; then
	echo "$TMP_SYSPROPS_FILE: not found!"
	exit 1
fi

TARGET_DIR=$(readlink_m "$TARGET_DIR")
TOP_BIN_DIR=$(readlink_m "$TOP_BIN_DIR")
TMP_SYSPROPS_FILE=$(readlink_m "$TMP_SYSPROPS_FILE")

ULC2CLASS=$TOP_BIN_DIR/tools/ulc2class/ulc2class
if [ ! -x "$ULC2CLASS" ]; then
	echo "$ULC2CLASS: not executable!"
	exit 1
fi

LD_LIBPATH=$TOP_BIN_DIR/src
export LD_LIBRARY_PATH="$LD_LIBPATH:$LD_LIBRARY_PATH"

ULSLANGS=$(readlink_m "$ULSLANGS")
if [ ! -f "$ULSLANGS" ]; then
	echo "$ULSLANGS: not found!"
	exit 1
fi

WORK_DIR=$PWD
cd "$WORK_DIR"

ALIGN_COL=23

draw_progress()
{
	local col
	local n_cols
	local line_type

	line_type=$1
	n_cols=$2

	if [ $n_cols -lt 2 ]; then
		return;
	fi
	n_cols=$(($n_cols-2))

	echo -n " "
	col=0
	while [ $col -lt $n_cols ]; do
		echo -n "$line_type"
		col=$(($col+1))
	done

	echo -n " "
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
	echo -n " $in_fname"
	curcol=${#in_fname}
	curcol=$(($curcol+1))
	draw_progress '_' $(($ALIGN_COL-$curcol))

	out_fpath1=
	if [ "$enum_name" != "-" ]; then
		out_fname="${enum_name}_lex.h"
		out_fpath1="${TARGET_DIR}/$out_fname"
		echo -n " $out_fname"
	fi

	out_fpath2=
	if [ "$ClassName" != "-" ]; then
		out_fname="${ClassName}.h"
		out_fpath2="${TARGET_DIR}/$out_fname"
		echo -n " $out_fname"
	fi

	if [ -n "$out_fpath1" ]; then
		$ULC2CLASS -o "$out_fpath1" -y -lc -e ${enum_name}_token $pfx_optstr $in_fname
	fi

	if [ -n "$out_fpath2" ]; then
		$ULC2CLASS -o "$out_fpath2" -y -n ${PACKAGE_DOMAIN}.${ClassName} $pfx_optstr $in_fname
	fi

	echo
done  < "$ULSLANGS"

cd "$HOME_DIR"
