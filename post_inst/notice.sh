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
# FILE: uninstall_uls
# DATE: 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: To notice the usage of the package to users.
#   This file is part of ULS, Unified Lexical Scheme.
#

top_dir="$1"

bin_dir="$2"
if [ -z "$bin_dir" ]; then
	bin_dir=$top_dir/bin
fi

inc_dir="$3"
if [ -z "$inc_dir" ]; then
	inc_dir=$top_dir/include
fi

lib_dir="$4"
if [ -z "$lib_dir" ]; then
	lib_dir=$top_dir/lib
fi

echo '=== NOTICE ==================================================================='
echo "Append the following lines at the end of ~/.bashrc to run the uls utilities."
echo "   export PATH=\$PATH:${bin_dir}"
echo "   export LD_LIBRARY_PATH=${lib_dir}"
echo "To compile source program that calls uls APIs,"
echo "   gcc -c -I${inc_dir} your_source_program.c"
echo "To link the uls library with your object code,"
echo "   gcc -L${lib_dir} your_source_program.o -luls"
echo "For detailed information,"
echo "   refer to the 'Makefile.am' files in 'examples' directory."
echo '=============================================================================='
