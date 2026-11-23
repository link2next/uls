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
#
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
# DATE: Oct. 2026
# AUTHOR: Stanley Hong
# DESCRIPTION: A script to delete ULS package from your system.
#   This file is part of ULS, Unified Lexical Scheme.
#

shell_exe=$SHELL
progpath=$0
homedir="$PWD"
my_pid=$$

work_dir=/tmp
work_dpath=none
user_home=none
devnull=/tmp/_uls_${my_pid}_devnull_1.txt

get_abs_dirpath()
{
	local dpath=$1
	local olddir="$PWD"
	local abs_dirpath="/dirr$my_pid/not/found"
	local stat=1 # false

	if [ -d "$dpath" ]; then
		if cd "$dpath"; then
			abs_dirpath="$PWD"
			cd "$olddir"
			stat=0
		fi
	fi

	echo $abs_dirpath
	return $stat
}

progname=`basename "$progpath"`
progdir=`dirname "$progpath"`
if [ ! -d "$progdir" ]; then
	echo "$progdir: not found!"
	exit 1
fi
progdir=`get_abs_dirpath "$progdir"`

coverages_uls="bin lib/uls lib64/uls include/uls share/man/man1 share/uls"
uninst_info_sh='uls_uninst_info.sh'
stage=$1

check_commands_used()
{
	local cmd path1
	local stat=0

	if ! which which > $devnull 2>&1; then
		echo "Can't find the program 'which'!"
		exit 1
	fi

	for cmd in $* cp mv rm basename dirname; do
		path1=`which $cmd`
		if [ -z "$path1" ]; then
			echo "$cmd: not found!"
			stat=1
		fi
	done

	rm $devnull
	return $stat
}

chk_dir_rw_ok()
{
	local tgt_dir=$1
	local a_dir a_file
	local olddir="$PWD"
	local stat=0

	if ! cd "$tgt_dir"; then
		cd "$olddir"
		return 1
	fi

	a_dir=_${my_pid}_touch__dir
	mkdir $a_dir > $devnull 2>&1
	if [ $? != 0 ]; then
		stat=1
	else
		rmdir $a_dir

		a_file=_${my_pid}_touch__file
		touch $a_file > $devnull 2>&1
		if [ $? != 0 ]; then
			stat=1
		else
			rm -f $a_file;
		fi
	fi

	cd "$olddir"
	return $stat
}

get_work_dpath()
{
	local n tmp_dirpath stat=0
	local username=`whoami`
	local tmp_dname tempdir=

	n=1
	while [ $n -lt 1000 ]; do
		tmp_dname="_uninst_uls_${username}_${n}"
		tmp_dirpath="/tmp/$tmp_dname"

		if [ ! -e $tmp_dirpath ]; then
			if mkdir $tmp_dirpath; then
				tempdir=$tmp_dirpath

				break
			fi
		elif [ -d $tmp_dirpath ]; then
			if chk_dir_rw_ok $tmp_dirpath; then
				tempdir=$tmp_dirpath
				break
			fi
		fi
		n=$(($n+1))
	done

	if [ -d $tempdir ]; then
		work_dpath=$tmp_dname/$my_pid
	else
		stat=1
	fi

	return $stat
}

if [ -z "$stage" ]; then
	if ! check_commands_used find mkdir rmdir touch head; then
		exit 1
	fi

	if ! get_work_dpath; then
		echo "Can't create uls-temp-dir!"
		exit 1
	fi

	work_dir=/tmp/$work_dpath
	if [ -d $work_dir ]; then
		echo "$work_dir: occupied by another!"
		exit 1
	fi
	mkdir $work_dir

	work_dir2=`get_abs_dirpath $work_dir`
	if [ $work_dir2 != $work_dir ]; then
		echo "$work_dir: Failed to create working dir!"
		exit 1
	fi

	if [ ! -x "$progdir"/$uninst_info_sh ]; then
		echo "$uninst_info_sh: not executable"
		exit 1
	fi

	cp -f "$progdir"/$uninst_info_sh $work_dir/
	cp -f "$progpath" $work_dir/$progname
#	chmod u+x $work_dir/$progname

	exec "$shell_exe" $work_dir/$progname 1 $work_dpath
else
	work_dpath=$2
	work_dir=/tmp/$work_dpath

	if [ ! -f $work_dir/$uninst_info_sh ]; then
		echo "$uninst_info_sh: not found!"
		exit 1
	fi

	cd $work_dir
	. ./$uninst_info_sh
fi

# Getting properties from uninst_info.sh
user_home="$uls_user_home"
etc_dir="$uls_etc_dir"
inst_dir="$uls_inst_dir"
lib_dir="$uls_lib_dir"
devnull=$work_dir/_uls_${my_pid}_devnull_2.txt

for dir1 in "$etc_dir" "$inst_dir" "$lib_dir"; do
	if [ ! -d "$dir1" ]; then
		echo "$dir1: not a directory!"
		exit 1
	fi
done

if [ "$inst_dir" = "/" ]; then
	echo "Can't uninstall uls on the root-directory!"
	exit 1
fi

check_need_admin()
{
	local instdir=$1 
	local etcdir=$2 

	if [ ! -d "$instdir" ]; then
		echo "$instdir: Not a directory!"
		return 1
	fi

	if ! chk_dir_rw_ok "$instdir"; then
		return 1
	fi

	if [ ! -d "$etcdir" ]; then
		echo "$instdir | $etcdir: Not a directory!"
		return 1
	fi

	if ! chk_dir_rw_ok "$etcdir"; then
		return 1
	fi

	return 0
}

list_files_patterns()
{
	local basedir cmdline
	local list1 list2 list3
	local name_fpattern filename

	basedir=$1
	shift
	if [ ! -d "$basedir" ]; then
		return 1
	fi

	list2=
	while [ -n "$1" ]; do
		filename=$1
		if [ "$filename" != "*" ]; then
			name_fpattern="-name '$filename'"
		else
			name_fpattern=
		fi

		cmdline="find $basedir \( -type f -or -type l \) $name_fpattern -maxdepth 1 -print | tr '\n' ':'"
		list1=`eval $cmdline 2> $devnull`

		list1=${list1%:}
		if [ -n "$list1" ]; then
			if [ -n "$list2" ]; then
				list2="$list2:$list1:"
			else
				list2="$list1:"
			fi
		fi
		shift
	done

	list3=
	while [ -n "$list2" ]; do
		filename=${list2%%:*}
		list2=${list2#${filename}:}

		filename=${filename#${basedir}/}
		if [ -n "$list3" ]; then
			list3="$list3:$filename"
		else
			list3="$filename"
		fi
	done

	echo $list3
	return 0
}

is_dir_nearly_empty()
{
	local dirpath=$1
	local n_dent_exist=$2
	local n n_limit retval
	local cmdline filename list1 list2

	if [ ! -d "$dirpath" ]; then
#		echo "$dirpath: not found!"
		return 0 # true
	fi

	n_limit=$(($n_dent_exist+2))
	# +2 == 1(dirpath) + 1
	cmdline="find '$dirpath' -maxdepth 1 -print | head -$n_limit | tr '\n' ':'"
	list1=`eval $cmdline 2> $devnull`
	list1=${list1%:}

	if [ -z "$list1" ]; then
		if [ $n_dent_exist -gt 0 ]; then
			retval=1 # false
		else
			retval=0 # true
		fi
		return $retval
	fi

	list2="${list1}:"

	retval=0 # true
	n=0
	while [ -n "$list2" ]; do
		filename=${list2%%:*}
		list2=${list2#${filename}:}

		if [ "$filename" != "$dirpath" ]; then
			n=$(($n+1))
			if [ $n -gt $n_dent_exist ]; then
				retval=1 # false
				break
			fi
		fi
	done

	return $retval
}

is_dir_empty()
{
	local dpath=$1
	local retval

	is_dir_nearly_empty "$dpath" 0
	retval=$?
	return $retval
}

if ! check_need_admin "$inst_dir" "$etc_dir"; then
	echo "$progname: admin privilege needed!"
	exit 1
fi

trash_basedir=$work_dir/trash
mkdir $trash_basedir
if [ ! -d $trash_basedir ]; then
	echo "Failed to create trash-dir $trash_basedir !"
	exit 1
fi

trash_id=0
trash_seqno=0
trash_space=/tmp
new_trash_space()
{
	trash_id=$(($trash_id+1))
	trash_space=$trash_basedir/$trash_id
	mkdir $trash_space
	trash_seqno=0
}

del_files()
{
	local sym_list=
	local reg_list=
	local fpattern filename
	local list2

	while [ -n "$1" ]; do
		list2=`list_files_patterns . "$1"`

		if [ -n "$list2" ]; then
			list2="${list2}:"
			while [ -n "$list2" ]; do
				filename=${list2%%:*}
				list2=${list2#${filename}:}
				if [ -L "./$filename" ]; then
					if [ -n "$sym_list" ]; then
						sym_list="$sym_list $filename"
					else
						sym_list="$filename"
					fi
				elif [ -f "./$filename" ]; then
					if [ -n "$reg_list" ]; then
						reg_list="$reg_list $filename"
					else
						reg_list="$filename"
					fi
				fi
			done
		fi
		shift
	done

	if [ -n "$sym_list" ]; then
		list2="$sym_list "
		while [ -n "$list2" ]; do
			filename=${list2%% *}
			list2=${list2#${filename} }

			if [ -e "./$filename" ]; then
				echo "deleting (symlink) $filename ..."
				rm -f "./$filename"
			fi
		done
	fi

	if [ -n "$reg_list" ]; then
		list2="$reg_list "
		while [ -n "$list2" ]; do
			filename=${list2%% *}
			list2=${list2#${filename} }

			if [ -e "./$filename" ]; then
				trash_seqno=$(($trash_seqno+1))
				echo "deleting (regular) $filename ..."
				mv -f "./$filename" $trash_space/${trash_seqno}_$filename
			fi
		done
	fi
}

del_tree()
{
	local dname

	while [ -n "$1" ]; do
		if [ -d "$1" ]; then
			dname=`basename $1`
			trash_seqno=$(($trash_seqno+1))
			echo "deleting (dir) $1 ..."
			mv -f "$1" $trash_space/${trash_seqno}_$dname
		fi
		if [ -e "$1" ]; then
			echo "Failed to delete dir $1"
		fi
		shift
	done
}

sysprops_fpath="$etc_dir"/uls.sysprops
if [ ! -f "$sysprops_fpath" ]; then
	echo "ULS: not installed."
	exit 1
fi

while : ; do
	printf "Do you want to uninstall ULS? (y/n) "
	read yesno
	if [ "$yesno" = 'y' ]; then
		break
	elif [ "$yesno" = 'n' ]; then
		exit 0;
	fi
done

sysdir_installed=no
if [ "$inst_dir" = "/usr/local" -o "$inst_dir" = "/usr" -o "$inst_dir" = "/" ]; then
	sysdir_installed=yes
fi

if [ "$inst_dir" = "$user_home" -o "$inst_dir" = "$user_home/.local" -o "$inst_dir" = "$user_home/local" ]; then
	sysdir_installed=yes
fi

if [ "$inst_dir" = "/usr/local" ]; then
	echo "$progname: the package might has been installed by package installer."
	printf "Do you want to continue removing it? "
	read ans
	if [ "$ans" != "y" ]; then
		echo "Stopped by user!"
		echo "You can remove the package by entering 'dpkg -P uls'."
		exit 1
	fi
fi

echo "Uninstalling ULS in '$inst_dir' ..."

bin_dir="$inst_dir"/bin
inc_dir="$inst_dir"/include
share_dir="$inst_dir"/share

del_specific_dirs()
{
	local dpath dname
	local list2

	new_trash_space
	cd "$inst_dir"

	list2="$coverages_uls $etc_dir "
	while [ -n "$list2" ]; do
		dpath=${list2%% *}
		list2=${list2#${dpath} }

		dname=`basename $dpath`
		if [ $dname = 'uls' ]; then
			del_tree $dpath
		fi
	done
}

del_instdir_files()
{
	if [ -d "$bin_dir" ]; then
		new_trash_space
		cd "$bin_dir"
		del_files ulc2class ulf_gen uls_stream
		del_files 'uls_*.sh' setup_uls_examples "$progname"
	fi

	if [ -d "$lib_dir" ]; then
		new_trash_space
		cd "$lib_dir"
		del_files 'libuls*' 'uls*.jar'
	fi

	if [ -d "$inc_dir" ]; then
		new_trash_space
		cd "$inc_dir"
		del_files uls.h
	fi

	if [ -d "$share_dir" ]; then
		new_trash_space
		if [ -d "$share_dir"/man/man1 ]; then
			cd "$share_dir"/man/man1
			del_files ulc2class.1.gz ulf_gen.1.gz uls_stream.1.gz
		fi
		cd "$share_dir"
	fi
}

is_instdir_skeleton()
{
	local dname
	local n dirpath

	for dirpath in "$bin_dir" "$lib_dir" "$inc_dir" "$share_dir"/man/man1; do
		if ! is_dir_empty "$dirpath"; then
			return 1
		fi
	done

	# 1 = size({ 'man1' })
	if ! is_dir_nearly_empty "$share_dir"/man 1; then
#		echo "$share_dir/man: nonempty"
		return 1
	fi

	# 1 = size({ 'man' })
	if ! is_dir_nearly_empty "$share_dir" 1; then
#		echo "$share_dir: nonempty"
		return 1
	fi

	cd "$inst_dir"

	if [ -d lib ] && [ -d lib64 ]; then
		echo "Both lib & lib64 exist!"
		echo "It won't delete '$inst_dir'"
		return 1
	fi

	# 5 = size({ 'include' 'lib' 'lib64' bin' 'share' })
	n=0
	for dname in bin lib lib64 include share; do
		if [ -d "$dname" ]; then
			n=$(($n+1))
		fi
	done

	if ! is_dir_nearly_empty "$inst_dir" $n; then
#		echo "$inst_dir: nonempty!"
		return 1 # false
	fi

	return 0 # true
}

del_specific_dirs
del_instdir_files

if is_instdir_skeleton; then
	instdir_empty=yes
else
	instdir_empty=no
fi

cd $work_dir
new_trash_space
if [ "$sysdir_installed" = "no" ] && [ "$instdir_empty" = "yes" ]; then
	del_tree "$inst_dir"
fi

if [ -d "$etc_dir" ]; then
	cd "$etc_dir"
	del_files uls.langs uls.id_ranges uls.sysprops
fi

cd $trash_basedir
m=1
while : ; do
	if [ ! -d $m ]; then
		break
	fi
	rm -rf $m
	m=$(($m+1))
done

#cd /tmp
#rm -rf $work_dpath

if [ -d "$homedir" ]; then
	cd "$homedir"
else
	cd "$HOME"
fi

if [ -e $devnull ]; then
	rm $devnull
fi

echo
echo "done!"
echo
