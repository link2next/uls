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
# DATE: June 2014
# AUTHOR: Stanley Hong
# DESCRIPTION: A script to delete ULS package from your system.
#   This file is part of ULS, Unified Lexical Scheme.
#

progpath=$0
progname=$(basename "$progpath")
progdir=$(dirname "$progpath")

source $progdir/uls_common.sh
progdir=$(readlink_m "$progdir")

uninst_info_sh=uls_uninst_info.sh

stage=$1
if [ -z "$stage" ]; then
	if [ ! -x $progdir/$uninst_info_sh ]; then
		echo "$uninst_info_sh: not executable"
		exit 1
	fi

	temp_dir=/tmp/uls_$$_uninst_dir
	mkdir $temp_dir
	cp -f $progdir/$uninst_info_sh $temp_dir/
	cp -f $progdir/uls_common.sh $temp_dir/
	cp -f "$progpath" $temp_dir/$progname

	exec bash $temp_dir/$progname 1 $temp_dir
else
	work_dir=$2

	if [ ! -x $work_dir/$uninst_info_sh ]; then
		echo "$uninst_info_sh: not executable"
		exit 1
	fi

	cd $work_dir
	source $uninst_info_sh
fi

if [ "$need_admin" = "y" ]; then
	if [ "$(id -u)" != "0" ]; then
		echo "$progname: Admin rights required!"
		exit 1
	fi
fi

while : ; do
	echo -n "Do you want to uninstall ULS? (y/n) "
	read yesno
	if [ "$yesno" = 'y' ]; then
		break
	elif [ "$yesno" = 'n' ]; then
		exit 0;
	fi
done

del_file()
{
	while [ -n "$1" ]; do
		if [ -f "$1" ]; then
			rm -f "$1" 
		fi
		if [ -e "$1" ]; then
			echo "$1: not a file"
		fi
		shift
	done
}

del_tree()
{
	while [ -n "$1" ]; do
		if [ -d "$1" ]; then
			rm -rf "$1" 
		fi
		if [ -e "$1" ]; then
			echo "$1: not a dir"
		fi
		shift
	done
}

sysprops_fpath=$etc_dir/uls.sysprops
if [ ! -f "$sysprops_fpath" ]; then
	echo "ULS: not installed."
	exit 0
fi

# strip the utf-8 bom at the first 
tmp_sysprops_fpath=/tmp/uls_$$_sysprops
dd bs=1 skip=3 if="$sysprops_fpath" of=$tmp_sysprops_fpath 2> /dev/null

while read line; do
	if [ -z "$line" ]; then
		continue
	fi
	eval $line
done  < $tmp_sysprops_fpath
rm $tmp_sysprops_fpath

if [ -z "$ULS_HOME" ]; then
	echo "don't know where it's setup!"
    exit 1
fi

if [ "$ULS_HOME" = "/usr" -o "$ULS_HOME" = "/" ]; then
	echo "$progname: the package might has been installed via gdebi."
	echo -n "Do you want to continue removing it? "
	read ans
	if [ "$ans" != "y" ]; then
		echo "Stopped by user!"
    	echo "You can remove the package by entering 'dpkg -P uls'."
		exit 1
	fi
fi

echo "Uninstalling ULS in $ULS_HOME ..."

bin_dir=$ULS_HOME/bin
inc_dir=$ULS_HOME/include
inc_uls_dir=$inc_dir/uls
lib_dir=$ULS_HOME/lib
man_dir=$ULS_HOME/share/man/man1
data_dir=$ULS_HOME/share/uls

del_tree "$inc_uls_dir"
del_file "$inc_dir"/{uls*.h,Uls*.h}

del_file "$lib_dir"/libuls*
del_file "$man_dir"/{ulc2class.1,ulf_gen.1,uls_stream.1}

del_tree "$data_dir"/ulcs
del_file "$data_dir"/uls_examples.tar
del_tree "$data_dir"

del_file "$bin_dir"/{ulc2class,ulf_gen,uls_stream,ulc2yaml}
del_file "$bin_dir"/uls_*.sh
del_file "$bin_dir"/setup_uls_examples
del_file "$bin_dir"/$progname

del_file "$etc_dir"/{uls.langs,uls.id_ranges}
del_file "$sysprops_fpath"
del_tree "$etc_dir"

if [ "$ULS_HOME" != "/usr/local" -a "$ULS_HOME" != "/usr" -a "$ULS_HOME" != "/" ]; then
	del_tree $ULS_HOME
fi

