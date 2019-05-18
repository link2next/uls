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
# FILE: check_arch.sh
# DATE: June 2015
# AUTHOR: Stanley Hong
# DESCRIPTION: To get the arch-type and arch-bits.
#   This file is part of ULS, Unified Lexical Scheme.
#

source ../ulsconfig/uls_common.sh

mach_bits=0

if [ "$mach_bits" = "0" ] &&  which arch > /dev/null; then
	mach_str=$(arch)
	if [ "$mach_str" = "i686" ]; then
		mach_bits=32
	elif [ "$mach_str" = "x86_64" ]; then
		mach_bits=64
	fi
fi

if [ "$mach_bits" = "0" ] && which uname > /dev/null; then
	mach_str=$(uname -m)
	if [ "$mach_str" = "i686" ]; then
		mach_bits=32
	elif [ "$mach_str" = "x86_64" ]; then
		mach_bits=64
	fi
fi

if [ "$mach_bits" = "0" ] && which dpkg > /dev/null; then
	mach_str=$(dpkg --print-architecture)
	if [ "$mach_str" = "i386" ]; then
		mach_bits=32
	elif [ "$mach_str" = "amd64" ]; then
		mach_bits=64
	fi
fi

if [ "$mach_bits" = "0" ] && which getconf > /dev/null; then
	mach_bits=$(getconf LONG_BIT)
fi

sbin_init_fpath=/sbin/init
if [ "$mach_bits" = "0" ] && [ -f $sbin_init_fpath ]; then
	if [ -L $sbin_init_fpath ]; then
		sbin_init_fpath=$(readlink_m $sbin_init_fpath)
	fi

	mach_str=$(file $sbin_init_fpath)
	if echo "$mach_str" | grep -q 32-bit; then
		mach_bits=32
	elif echo "$mach_str" | grep -q 64-bit; then
		mach_bits=64
	fi
fi

echo $mach_bits
