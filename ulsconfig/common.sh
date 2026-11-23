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
# FILE: uls_common.sh
# DATE: Oct. 2026
# AUTHOR: Stanley Hong
# DESCRIPTION:
#

split_existing_dirpath()
{
	local a_dir=$1 b_dir=
	local filename

	while : ; do
		if [ "$a_dir" = "." ] || [ "$a_dir" = "/" ]; then
			break;
		fi

		filename=`basename "$a_dir"`
		if [ -z "$filename" ] || [ "$filename" = "." ]; then
			a_dir=`dirname "$a_dir"`
			continue
		fi

		if [ -d "$a_dir" ]; then
				break;
		fi

		if [ -n "$b_dir" ]; then
			b_dir="$filename/$b_dir"
		else
			b_dir="$filename"
		fi

		a_dir=`dirname "$a_dir"`
	done

	echo "$a_dir;$b_dir"
}

normalize_relative_path()
{
	local a_dir=$1 b_dir=
	local a2_dir filename
	local n=0

	while : ; do
		filename=`basename "$a_dir"`
		if [ -n "$filename" ] && [ "$filename" != "." ]; then
			if [ "$filename" = ".." ]; then
				n=$((n+1))
			elif [ $n -gt 0 ]; then
				n=$((n-1))
			else
				if [ -n "$b_dir" ]; then
					b_dir="$filename/$b_dir"
				else
					b_dir="$filename"
				fi
			fi
		fi

		a_dir=`dirname "$a_dir"`
		if [ "$a_dir" = "." ]; then
			break
		fi
	done

	if [ $n -gt 0 ]; then
		a_dir=
		while [ $n -gt 0 ]; do
			if [ -n "$a_dir" ]; then
				a_dir="$a_dir/.."
			else
				a_dir=".."
			fi
			n=$((n-1))
		done
		a2_dir="$a_dir/$b_dir"

	elif [ -z "$b_dir" ]; then
		a2_dir=
	else
		a2_dir="$b_dir"
	fi
	echo "$a2_dir"
}

normalizePath()
{
	local a_dir=$1
	local b_dir is_abs

	if [ -z "$a_dir" ]; then
		echo "."
		return 0
	fi

	while : ; do
		b_dir=${a_dir%/}
		if [ -z "$b_dir" ]; then
			echo "/"
			return 0
		fi
		if [ "$a_dir" = "$b_dir" ]; then
			break
		fi
		a_dir="$b_dir"
	done

	is_abs=
	while : ; do
		b_dir=${a_dir#/}
		if [ "$a_dir" = "$b_dir" ]; then
			break
		fi
		is_abs=yes
		a_dir="$b_dir"
	done

	b_dir=`normalize_relative_path "$a_dir"`

	if [ "$is_abs" = "yes" ]; then
		b_dir="/$b_dir"
	else
		a_dir=${b_dir#../}
		if [ "$a_dir" = "$b_dir" ]; then
			# If it's not like ../../a/b, prepend './'
			if [ -n "$a_dir" ]; then
				b_dir="./$a_dir"
			else
				b_dir="."
			fi
		fi
	fi

	echo "$b_dir"
	return 0
}

is_abs_path()
{
	local a_dir="$1"
	local b2_dir stat

	b2_dir=${a_dir#/}
	if [ "$a_dir" != "$b2_dir" ]; then
		stat=0 # true
	else
		stat=1 # false
	fi

	return $stat
}

readpath_abs()
{
	local a_dir=$1
	local b_dir b1_dir b2_dir
	local dname

	if [ -z "$a_dir" ] || [ "$a_dir" = "." ]; then
		echo "."
		return 0
	fi

	b_dir=`dirname "$a_dir"`
	dname=`basename "$a_dir"`

	b_dir=`split_existing_dirpath "$b_dir"`
	b1_dir=${b_dir%;*}
	b2_dir=${b_dir#*;}

	if ! is_abs_path "$b1_dir"; then
		if [ "$b1_dir" = "." ]; then
			b1_dir="$PWD"
		else
			b1_dir=${b1_dir#./}
			b1_dir="$PWD/$b1_dir"
		fi
	fi

	if [ -n "$b2_dir" ]; then
		if [ "$b1_dir" = "/" ]; then
			b_dir="/$b2_dir"
		else
			b_dir="$b1_dir/$b2_dir"
		fi
	else
		b_dir="$b1_dir"
	fi

	b_dir=`normalizePath "$b_dir/$dname"`
	echo "$b_dir"
	return 0
}
