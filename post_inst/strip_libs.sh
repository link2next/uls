#!/bin/sh

my_pid=$$
devnull=/tmp/_ulspost_${my_pid}_devnull_1.txt

usage()
{
	echo "usage: $0  <lib-dir> [path-strip-exe]"
	exit 1
}

if [ $# -lt 1 ]; then
	usage
fi
libdir=$1

if [ $# -ge 2 ]; then
	strip_exe=$2
else
	strip_exe=`which strip`
fi

if [ ! -d "$libdir" ]; then
	echo "$libdir: not found!"
	exit 1
fi

if [ ! -x "$strip_exe" ]; then
	echo "$strip_exe: not found, ...skipping"
	exit 0
fi

line=`$strip_exe --help | grep 'strip-all'`
if [ -z "$line" ]; then
	echo "$strip_exe: Failed to strip the debug-symbols, ...skipping"
	exit 0
fi

cd "$libdir"

list1=`ls -1 libuls*.la 2> $devnull | tr '\n' ' '`
list1=${list1% }
if [ -n "$list1" ]; then
	list2="$list1 "
	while [ -n "$list2" ]; do
		f=${list2%% *}
		list2=${list2#${f} }
		if [ -f "$f" ]; then
#			echo "...deleting '$f'"
			rm -f "$f"
		fi
	done
fi

list1=`ls -1 libuls*.so.* libuls*.a 2> $devnull | tr '\n' ' '`
list1=${list1% }
if [ -n "$list1" ]; then
	list2="$list1 "
	while [ -n "$list2" ]; do
		f=${list2%% *}
		list2=${list2#${f} }
		if [ ! -L "$f" ]; then
			if [ -f "$f" ]; then
#				echo "...stripping '$f'"
				$strip_exe --strip-all "$f"
			fi
		fi
	done
fi

if [ -e $devnull ]; then
	rm -f $devnull
fi
