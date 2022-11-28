#!/bin/bash

ULC2CLS=$(which ulc2class)
ULC_JAVA_FNAME1=Sample1Lex
ULC_JAVA_FNAME2=Sample2Lex

if [ ! -x "$ULC2CLS" ]; then
	echo "Can't find the ulc2class"
	exit 1;
fi

$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME1}.java -ljava -n uls.tests.${ULC_JAVA_FNAME1} sample.ulc
$ULC2CLS -o ../src/uls/tests/${ULC_JAVA_FNAME2}.java -ljava -n uls.tests.${ULC_JAVA_FNAME2} sample2.uld
