#/bin/sh

progname=$0
shell_path=/bin/sh
user_home="$HOME"
inst_dir=/tmp/uls
lib_dir=/tmp/uls/lib
etc_dir=/tmp/etc/uls

check_commands_used()
{
	local cmd path1
	local stat=0

	for cmd in $* cp mv rm basename dirname; do
		path1=`which $cmd`
		if [ -z "$path1" ]; then
			echo "$cmd: not found!"
			stat=1
		fi
	done

	return $stat
}

if ! check_commands_used find mkdir rmdir; then
	exit 1
fi

usage()
{
	echo "usage: $progname config <shell-path>"
	echo "       $progname install <user-home> <inst-dir> <lib-dir> <etc-dir>"
	exit 1
}

if [ $# -lt 2 ]; then
	usage
fi
progmode=$1

if [ $progmode = 'config' ]; then
	shell_path=$2
elif [ $progmode = 'install' ]; then
	if [ $# -lt 5 ]; then
		usage
	fi
	user_home="$2"
	if [ ! -d "$user_home" ]; then
		echo "$user_home: not found!"
		exit 1
	fi
	inst_dir="$3"
	lib_dir="$4"
	etc_dir="$5"
else
	echo "Unknown program mode '$progmode'"
	usage
fi

chk_required_shell()
{
	local shpath=$1
	local shname stat=0

	shname=`basename "$shpath"`

	if [ $shname = 'dash' ]; then
		echo "The shell is dash."
	elif [ $shname = 'bash' ]; then
		echo "The shell is bash."
	elif [ $shname = 'zsh' ]; then
		echo "The shell is zsh."
	elif [ $shname = 'sh' ]; then
		echo "The shell is bourne."
	else
		echo "$shname: Not supported shell"
		stat=1; # not-ok
	fi

	return $stat
}

is_installed_already()
{
	local userhome=$1
	local etcdir1 etcdir2
	local stat=1 # false

	etcdir1="$userhome"/.local/etc/uls
	if [ -d "$etcdir1" ]; then
		stat=0 # true
	fi

	etcdri2=/usr/local/etc/uls
	if [ -d "$etcdri2" ]; then
		stat=0 # true
	fi

	return $stat
}

create_inst_dirs()
{
	local olddir="$PWD"

	for dirpath in $inst_dir $etc_dir $lib_dir; do
		if [ ! -e "$dirpath" ]; then
			if ! mkdir -m755 -p "$dirpath"; then
				echo "Failed to create $dirpath!"
				return 1
			fi
		fi
	done

	if ! cd "$inst_dir"; then
		echo "Failed to chdir to $inst_dir!"
		return 1
	fi

	for dpath in bin include/uls share/man/man1 share/uls/ulcs; do
		if [ ! -e "$dpath" ]; then
			if ! mkdir -m755 -p "$dpath"; then
				echo "Failed to create $dpath!"
				return 1
			fi
		fi
	done

	cd "$olddir"
	return 0
}

stat=true
if [ "$progmode" = "install" ]; then
	if is_installed_already "$user_home"; then
		echo "ULS: already installed!"
		stat=false
	fi
else
	if ! chk_required_shell "$shell_path"; then
		echo "Supported shell = [dash, bash, zsh, ksh]"
		stat=false
	fi
fi

if [ $stat = false ]; then
	echo "The prerequisite of '$progmode' is not matched!"
	exit 1
fi

if [ "$progmode" = "install" ]; then
	if ! create_inst_dirs; then
		echo "Failed to create dirs in $inst_dir !"
		exit 1
	fi
fi