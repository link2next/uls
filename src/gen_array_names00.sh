#!/bin/bash
set -e

if [ $# -lt 1 ]; then
	echo "usage: $0 <filepath>"
	exit 1
fi

in_filepath=$1
if [ ! -f "$in_filepath" ]; then
	echo "$in_filepath: not found"
	exit 1
fi


gen_array_name00()
{
	local typnam="$1" typnam_big
	local arrsiz_nam="$2" arrsiz_val
	local m_nam  m_val 
	local line line2

	typnam_big=$(echo $typnam | tr 'a-z' 'A-Z')

	line=$(grep define uls/uls_const.h | grep $arrsiz_nam)
	line2=${line#*define}
	arrsiz_val=$(echo $line2 | awk '{print $2}')

	m_nam=${typnam_big}_TYPE00_${arrsiz_nam}
	m_val=${typnam}_s00array${arrsiz_val}

	echo "#define $m_nam $m_val"
}

echo "#ifndef __ULS_CONST_LINUX_H__"
echo "#define __ULS_CONST_LINUX_H__"
echo

while read word1 word2; do
	gen_array_name00 $word1 $word2
done < "$in_filepath"

echo
echo "#endif"
