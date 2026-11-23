#!/bin/sh

my_pid=$$
devnull=/tmp/_ulsdoc_${my_pid}_devnull_1.txt

for cmd in install ls mkdir; do
	filepath=`which $cmd`
	if [ -z "$filepath" ]; then
		echo "$cmd: not found!"
		exit 1
	fi
done

usage()
{
	echo "usage: $0  <share-dir>"
	exit 1
}

if [ $# -lt 1 ]; then
	usage
fi
sharedir=$1

if [ ! -d "$sharedir" ]; then
	echo "$sharedir: not found!"
	exit 1
fi

install_exe=`which install`
if [ ! -x "$install_exe" ]; then
	echo "$install_exe: not found!"
	exit 1
fi

list1=`ls -1 *.1.gz | tr '\n' ' '`
list1=${list1% }
if [ -n "$list1" ]; then
	list2="$list1 "
	while [ -n "$list2" ]; do
		f=${list2%% *}
		list2=${list2#${f} }
#		echo "...install-man '$f'"
		$install_exe -m644 $f "$sharedir"/man/man1/
	done
fi

list1=`eval ls -1 *.pdf 2> $devnull | tr '\n' ' '`
list1=${list1% }
if [ -n "$list1" ]; then
	list2="$list1 "
	while [ -n "$list2" ]; do
		f=${list2%% *}
		list2=${list2#${f} }
#		echo "...install-pdf '$f'"
		$install_exe -m644 $f "$sharedir"/uls/
	done
fi

if [ -e $devnull ]; then
	rm -f $devnull
fi