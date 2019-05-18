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
# FILE: uls_common.sh
# DATE: Feb 2018
# AUTHOR: Stanley Hong
# DESCRIPTION:
#

extract_existing_dirpath()
{
	local a_dir=$1
	local b_dir=
	local homdir fname

	if [ -z "$a_dir" ]; then
		a_dir=.
		echo ".;"
		return 0
	fi

	a_dir=$(echo $a_dir | tr -s '/')
	if [ "$a_dir" = "/" ]; then
		echo "/;"
		return 0
	fi

	a_dir=${a_dir%%/}

	while : ; do
		if [ -d "$a_dir" ]; then
			break;
		fi

		fname=$(basename $a_dir)

		if [ -n "$b_dir" ]; then
			if [ "$fname" != "." ]; then
				b_dir=$fname/$b_dir
			fi
		else
			b_dir=$fname
		fi

		a_dir=$(dirname $a_dir)
		if [ -z "$a_dir" ]; then
			a_dir = "."
		fi
	done

	echo "$a_dir;$b_dir"
	return 0
}

readlink_m()
{
	local a_dir=$1
	local b_dir b1_dir b2_dir
	local homdir

	b_dir=$(extract_existing_dirpath "$a_dir")
	b1_dir=${b_dir%;*}
	b2_dir=${b_dir#${b1_dir};}

	homdir=$PWD
	if ! cd "$b1_dir"; then
		echo "Cannot chdir to '$b1_dir'"
		exit 1
	fi

	b1_dir=$PWD
	cd "$homdir"

	if [ -n "$b2_dir" ]; then
		if [ "$b1_dir" != "/" ]; then
			echo "$b1_dir/$b2_dir"
		else
			echo "/$b2_dir"
		fi
	else
		echo "$b1_dir"
	fi

	return 0
}

check_dir_admin_privilege()
{
	local a_dir=$1
	local b_dir b1_dir c_dir
	local stat

	b_dir=$(extract_existing_dirpath "$a_dir")
	b1_dir=${b_dir%;*}

	c_dir="$b1_dir/uls_test__touch__dir__$$"
	if mkdir "$c_dir" > /dev/null 2>&1; then
		stat=1
		rmdir "$c_dir"
	else
		stat=0
	fi

	return $stat 
}
