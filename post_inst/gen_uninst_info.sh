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
# FILE: gen_uninst_info.sh
# DATE: Jan 2018
# AUTHOR: Stanley Hong
# DESCRIPTION:
#

source ../ulsconfig/uls_common.sh

HOME_DIR="$PWD"

if [ $# -lt 3 ]; then
	echo "usage: $0 <uls_inst_dir> <etc_dir> <out_file>"
	exit 1
fi

uls_inst_dir=$(readlink_m "$1")
uls_etc_dir=$(readlink_m "$2")
uls_need_admin=n

outfile=$3
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

touch "$outfpath"
#chmod u+x "$outfpath"

check_admin_privilege()
{
	local a_dir=$1
	local b_dir
	local stat=0

	while : ; do
 		if [ "$a_dir" = "/" ]; then
			uls_need_admin=y
			return 1
		fi

		if [ -d "$a_dir" ]; then
			break;
		fi

		b_dir=$(dirname $a_dir)
		a_dir=$b_dir
	done

	b_dir=$a_dir/test__touch__dir

	mkdir "$b_dir" > /dev/null 2>&1
	if [ $? != 0 ]; then
		uls_need_admin=y
		stat=1
	else
		rmdir "$b_dir"
	fi

	return $stat 
}

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

if check_dir_admin_privilege "$uls_inst_dir"; then
	uls_need_admin=y
fi

if check_dir_admin_privilege "$uls_etc_dir"; then
	uls_need_admin=y
fi

echo "#!/bin/bash" >> "$outfpath"
echo >> "$outfpath"
gen_namval_pair etc_dir $uls_etc_dir
gen_namval_pair need_admin $uls_need_admin

