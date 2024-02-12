#!/bin/bash

if [ $# -lt 1 ]; then
	ULC2CLS=$(which ulc2class)
else
	ULC2CLS=$1
fi

if [ ! -x "$ULC2CLS" ]; then
	echo "Can't find the ulc2class"
	exit 1
fi

ULC_JAVA_FNAME1=Sample1Lex
ULC_JAVA_FNAME2=Sample2Lex

$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME1}.java -ljava -n uls.tests.${ULC_JAVA_FNAME1} sample.ulc
$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME2}.java -ljava -n uls.tests.${ULC_JAVA_FNAME2} sample2.uld
