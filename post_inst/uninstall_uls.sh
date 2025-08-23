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
homedir=$PWD

progname=$(basename "$progpath")
progdir=$(dirname "$progpath")
if [ -z "$progdir" ]; then
	progdir=$PWD
else
	if [ ! -d "$progdir" ]; then
		echo "$progdir: not found!"
		exit 1
	fi
	cd "$progdir"
	progdir=$PWD
	cd "$homedir"
fi

source $progdir/uls_common.sh

uninst_info_sh=uls_uninst_info.sh

stage=$1
if [ -z "$stage" ]; then
	if [ ! -x "$progdir"/$uninst_info_sh ]; then
		echo "$uninst_info_sh: not executable"
		exit 1
	fi

	temp_dir=/tmp/uls_$$_uninst_dir
	if [ -d $temp_dir ]; then
		echo "Failed to create working dir for uninstall uls... Try again!"
		exit 1
	fi
	mkdir $temp_dir

	cp -f "$progdir"/$uninst_info_sh $temp_dir/
	cp -f "$progdir"/uls_common.sh $temp_dir/
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

del_files()
{
	local sym_list=
	local reg_list=

	while [ -n "$1" ]; do
		file=$1
		if [ -L "$file" ]; then
			sym_list="$sym_list $file"
		elif [ -f "$file" ]; then
			reg_list="$reg_list $file"
		fi
		shift
	done

	for filepath in $sym_list; do
		if [ -e "$filepath" ]; then
			echo "deleting (symlink) $filepath ..."
			rm -f "$filepath"
		fi
	done

	for filepath in $reg_list; do
		if [ -e "$filepath" ]; then
			echo "deleting (regular) $filepath ..."
			rm -f "$filepath"
		fi
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

import_vars_from_sysprops()
{
	local sysprops_file=$1
	local tmp_sysprops_file=/tmp/uls_$$_sysprops

	# Cut the BOM at the start of the file
	dd bs=1 skip=3 if="$sysprops_file" of=$tmp_sysprops_file 2> /dev/null

	while read line; do
		if [ -z "$line" ]; then
			continue
		fi
		eval $line
	done  < $tmp_sysprops_file
	rm $tmp_sysprops_file
}

# etc_dir is set from uninst_info.sh
sysprops_fpath=$etc_dir/uls.sysprops
if [ ! -f "$sysprops_fpath" ]; then
	echo "ULS: not installed."
	exit 0
fi

import_vars_from_sysprops "$sysprops_fpath"

if [ ! -d "$ULS_HOME" ]; then
	echo "Don't know where it's installed!"
	exit 1
fi
ULS_INST_DIR=$ULS_HOME

sysdir_installed=no
if [ "$ULS_INST_DIR" = "/usr/local" -o "$ULS_INST_DIR" = "/usr" -o "$ULS_INST_DIR" = "/" ]; then
	sysdir_installed=yes
fi

if [ "$ULS_INST_DIR" = "$HOME/.local" -o "$ULS_INST_DIR" = "$HOME/local" ]; then
	sysdir_installed=yes
fi

if [ "$ULS_INST_DIR" = "/usr" -o "$ULS_INST_DIR" = "/" ]; then
	echo "$progname: the package might has been installed via gdebi."
	echo -n "Do you want to continue removing it? "
	read ans
	if [ "$ans" != "y" ]; then
		echo "Stopped by user!"
		echo "You can remove the package by entering 'dpkg -P uls'."
		exit 1
	fi
fi

echo "Uninstalling ULS in $ULS_INST_DIR ..."

bin_dir=$ULS_INST_DIR/bin
inc_dir=$ULS_INST_DIR/include
inc_uls_dir=$inc_dir/uls
lib_dir=$ULS_INST_DIR/lib
man_dir=$ULS_INST_DIR/share/man/man1
data_dir=$ULS_INST_DIR/share/uls

del_tree "$inc_uls_dir"
del_files "$inc_dir"/{uls*.h,Uls*.h}

del_files "$lib_dir"/libuls*
del_files "$man_dir"/{ulc2class.1.gz,ulf_gen.1.gz,uls_stream.1.gz}

del_tree "$data_dir"/ulcs
del_files "$data_dir"/uls_examples.tar
del_tree "$data_dir"

del_files "$bin_dir"/{ulc2class,ulf_gen,uls_stream}
del_files "$bin_dir"/uls_*.sh
del_files "$bin_dir"/setup_uls_examples
del_files "$bin_dir"/$progname

del_files "$etc_dir"/{uls.langs,uls.id_ranges}
del_files "$sysprops_fpath"
del_tree "$etc_dir"

if [ "$sysdir_installed" = "no" ]; then
	del_tree $ULS_INST_DIR
fi

