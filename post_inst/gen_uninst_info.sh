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
# FILE: gen_uninst_info.sh
# DATE: Jan 2018
# AUTHOR: Stanley Hong
# DESCRIPTION:
#

. ../ulsconfig/common.sh

HOME_DIR="$PWD"

if [ $# -lt 3 ]; then
	echo "usage: $0 <inst_dir> <lib_dir> <etc_dir> <user_name> <user_home> <out_file>"
	exit 1
fi

inst_dir=`readpath_abs "$1"`
lib_dir=`readpath_abs "$2"`
etc_dir=`readpath_abs "$3"`

user_name="$4"
user_home="$5"

outfile="$6"
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

touch "$outfpath"
#chmod u+x "$outfpath"

gen_namval_pair()
{
	local nam
	local val

	nam=$1
	val=$2

	if [ -z "$val" ]; then
		echo "$nam=''" >> "$outfpath"
	else
		if echo "$val" | grep  -q ' '; then
			echo "$nam='$val'" >> "$outfpath"
		else
			echo "$nam=$val" >> "$outfpath"
		fi
	fi
}

echo "#!/bin/sh" >> "$outfpath"
echo >> "$outfpath"

gen_namval_pair uls_user_name $user_name
gen_namval_pair uls_user_home $user_home
gen_namval_pair uls_inst_dir $inst_dir
gen_namval_pair uls_lib_dir $lib_dir
gen_namval_pair uls_etc_dir $etc_dir

