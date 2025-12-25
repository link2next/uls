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

command_list_used="mkdir id"
if ! check_commands_used $command_list_used; then
	exit 1
fi

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

dirempty_cmd_part1=
dirempty_cmd_part2=

check_dirempty_cmd()
{
	local dir_content dummy_file
	local optstr1=
	local optstr2=

	dummy_file=/tmp/__dummy_$$_uninst_file
	ls /tmp >> $dummy_file

	dir_content=$(ls -A /tmp)
	if [ -n "$dir_content" ]; then
		optstr1="-A"
	fi

	dir_content=$(ls -1 /tmp)
	if [ -n "$dir_content" ]; then
		optstr1="$optstr1 -1"
	fi

	if [ -n "$optstr1" ]; then
		dirempty_cmd_part1="ls $optstr1"
	fi

	dir_content=$(head -3 $dummy_file)
	if [ -n "$dir_content" ]; then
		dirempty_cmd_part2="| head -3"
	else
		dir_content=$(head $dummy_file)
		dirempty_cmd_part2="| head"
	fi

	rm $dummy_file
}

check_dirempty_cmd
if [ -z "$dirempty_cmd_part1" ]; then
	echo "Failed to create dir-empty check routine!"
	exit 1
fi

is_dir_empty()
{
	local dirpath=$1
	local stat=0
	local dirempty_cmd
	local dir_content

	if [ ! -d "$dirpath" ]; then
		if [ -e "$dirpath" ]; then
			echo "$dirpath: what's it?"
			return 1 # false
		else
			echo "$dirpath: not a directory"
			return 0 # empty
		fi
	fi

	dirempty_cmd="$dirempty_cmd_part1 '"$dirpath"' $dirempty_cmd_part2"
	dir_content=$(eval $dirempty_cmd)
	if [ -n "$dir_content" ]; then
		stat=1 # nonempty
	fi

	return $stat
}

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
	local tmp_sysprops_file=/tmp/_$$_sysprops_noBOM.txt
	local import_vars_sh=/tmp/_$$_vars_from_sysprops.sh

	# Cut the BOM at the start of the file
	dd bs=3 skip=1 if=$sysprops_file of=$tmp_sysprops_file >/dev/null 2>&1

	echo "#!/bin/bash" > $import_vars_sh
	while read line; do
		if [ -z "$line" ]; then
			continue
		fi
		varval=${line#*=}
		varnam=${line%=${varval}}
		echo "$varnam=$varval" >> $import_vars_sh
	done  < $tmp_sysprops_file

	source $import_vars_sh
	rm -f $tmp_sysprops_file $import_vars_sh
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

can_del_etcdir=
if [ $(basename "$etc_dir") == uls ]; then
	can_del_etcdir=yes
else
	can_del_etcdir=no
fi

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

instdir_empty=yes

del_tree "$inc_uls_dir"
cd "$inc_dir"
del_files {uls*.h,Uls*.h}
if ! is_dir_empty "$inc_dir"; then
	echo "$inc_dir: not empty!"
	instdir_empty=no
fi

cd "$lib_dir"
del_files libuls* uls*.jar
if ! is_dir_empty "$lib_dir"; then
	echo "$lib_dir: not empty!"
	instdir_empty=no
fi

cd "$bin_dir"
del_files {ulc2class,ulf_gen,uls_stream}
del_files uls_*.sh
del_files setup_uls_examples
del_files $progname
if ! is_dir_empty "$bin_dir"; then
	echo "$bin_dir: not empty!"
	instdir_empty=no
fi

cd "$man_dir"
del_files {ulc2class.1.gz,ulf_gen.1.gz,uls_stream.1.gz}

cd "$data_dir"
del_tree ulcs
del_files uls_examples.tar
if ! is_dir_empty "$data_dir"; then
	echo "$data_dir: not empty!"
	instdir_empty=no
fi

cd "$etc_dir"
del_files {uls.langs,uls.id_ranges,uls.sysprops}

cd /tmp

if is_dir_empty "$etc_dir"; then
	if [ "$can_del_etcdir" = "yes" ]; then
		del_tree "$etc_dir"
	fi
fi

if [ "$sysdir_installed" = "no" ] && [ "$instdir_empty" = "yes" ]; then
	del_tree "$ULS_INST_DIR"
fi
