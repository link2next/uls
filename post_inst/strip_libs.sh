#!/bin/bash

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
	strip_exe=$(which strip)
fi

if [ ! -d "$libdir" ]; then
	echo "$libdir: not found!"
	exit 1
fi

if [ ! -x "$strip_exe" ]; then
	echo "$strip_exe: not found, ...skipping"
	exit 0
fi

line=$($strip_exe --help | grep 'strip-debug')
if [ -z "$line" ]; then
	echo "$strip_exe: Failed to strip the debug-symbols, ...skipping"
	exit 0
fi

cd "$libdir"
for f in $(ls -1 *.so.* *.a); do
	if [ ! -L "$f" ]; then
		if [ -f "$f" ]; then
			$strip_exe --strip-debug $f
		fi
	fi
done
