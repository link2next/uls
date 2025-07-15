#!/bin/bash

homedir=$PWD
if [ $# -lt 3 ]; then
	echo "usage: $0 <ulc2class> <sysprops-filepath> <libdir>"
	exit 1
fi

ULC2CLS=$1
sysprops_filepath=$2
if [ ! -f "$sysprops_filepath" ]; then
	echo "$sysprops_filepath: not found!"
	exit 1
fi

libdir=$3
if [ ! -d "$libdir" ]; then
	echo "$libdir: not found!"
	exit 1
fi
cd $libdir
libdir=$PWD
cd $homedir 

if [ ! -x "$ULC2CLS" ]; then
	echo "Can't find the ulc2class"
	exit 1
fi

ULC_JAVA_FNAME1=Sample1Lex
ULC_JAVA_FNAME2=Sample2Lex

export ULS_SYSPROPS=$sysprops_filepath
export LD_LIBRARY_PATH=$libdir

$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME1}.java -ljava -n uls.tests.${ULC_JAVA_FNAME1} sample.ulc
$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME2}.java -ljava -n uls.tests.${ULC_JAVA_FNAME2} sample2.uld
