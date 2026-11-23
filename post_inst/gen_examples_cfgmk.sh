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
# FILE: gen_examples_cfgmk.sh
# DATE: Nov. 2026
# AUTHOR: Stanley Hong
# DESCRIPTION: Create config file to be used in Makefiles
#   This file is part of ULS, Unified Lexical Scheme.
#

home_dir="$PWD"

progpath=$0
progname=`basename "$progpath"`

usage()
{
	echo "usage: $0 <version> <outfile>"
	exit 1
}

gen_examples_cfgmk()
{
	local ulsver out_fpath

	ulsver=$1
	out_fpath=$2

	cat > $out_fpath <<TXT
uls_instdir := \$(ax_inst_dir)
uls_etcdir := \$(ax_etc_dir)
uls_bindir := \$(ax_inst_dir)/bin
uls_libdir := \$(ax_lib_dir)
uls_incdir := \$(ax_inst_dir)/include

uls_ver := $ulsver
ulc2class_exe := \$(uls_bindir)/ulc2class
uls_jar := uls-\$(uls_ver).jar
TXT
}

if [ $# -lt 2 ]; then
	usage
fi

verstr=$1
outfile=$2

gen_examples_cfgmk $verstr "$outfile"

cd "$home_dir"
