#!/usr/bin/env python3
# -*- coding: UTF-8 -*- vim: et ts=4 sw=4
#
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
# FILE: upgrade_ulc.py
# DATE: July 2024
# AUTHOR: Stanley Hong
# DESCRIPTION: Convert the format of ulc file to upper version
#   This file is part of ULS, Unified Lexical Scheme.
#

import os
import sys
import getopt
import logging
import codecs

# ============= COMMON =============================================================
logging.basicConfig(level=logging.INFO)

def ucf_log(msg):
	logging.error(msg)

def ucf_panic(msg):
	logging.error(msg)
	raise SystemExit("Terminiated!")

HOME_DIR = os.getcwd()
progname = os.path.basename(sys.argv[0])

opt_force = False
opt_outfpath = ''

def usage():
	print("Convert the format of ulc-2.3 file to ulc-2.4")
	print("usage: {} [-v] [--output=<file>] <input.ulc>".format(progname))
	print("       -o, --output  : the filepath of output")
	print("       -f, --force   : force")
	print("       -h, --help    : print help message.")
	print("")

try:
	opts_list, appl_argv = getopt.getopt(sys.argv[1:], "fo:h", [ 'force=', 'output=', 'help' ] )
except getopt.GetoptError as err:
	print(str(err))
	usage()

for opt,optarg in opts_list:
	if opt in ('-o', '--output'):
		opt_outfpath = optarg
	elif opt in opt in ('-f', '--force'):
		opt_force = True
	elif opt in ('-h', '--help'):
		usage()
	else:
		print("Unknown option: {}".format(opt))
		sys.exit(1)

# ============= ====== =============================================================

def check_ulc_hdr(fin):
	firstline = fin.readline()
	if not firstline:
		return (False, '')

	lst1 = list(firstline)
	if len(lst1) < 3:
		ucf_log("...not an ulc file or version not proper")
		return (False, '')

	if lst1[:3] == list(codecs.BOM_UTF8):
		offset1 = 3
	else:
		offset1 = 0

	lst2 = []
	for num in lst1[offset1:]:
		if not (num >= 32 and num <= 126) and num != 9 and num != 10 and num != 13:
			return (False, '')
		ch = chr(num)
		lst2.append(ch)

	line2 = ''.join(lst2).rstrip()
	if not line2.startswith('#@ulc-2.3'):
		ucf_log("...converting support of ulc-file for only v2.3")
		return (False, '')

	return (True, line2[9:])

def upgrade_ulc_file(fin, fout):
	stat = True
	for line in fin:
		line = line.rstrip()
		if not line:
			fout.write('\n')
			continue

		line2 = line.lstrip()
		if not line2.startswith('QUOTE_TYPE:'):
			fout.write("{}\n".format(line))
			continue

		wordlist2 = []
		start_qmark = end_qmark = ''
		bSymmetric = False

		for wrd in line2[11:].split():
			if wrd.startswith('token='):
				wordlist2.append(wrd)
				continue;

			if wrd.startswith('options='):
				wordlist2.append(wrd)
				if 'asymmetric' in wrd[8:].split(','):
					bSymmetric = True;
				continue;

			if not start_qmark:
				start_qmark = end_qmark = wrd
				wordlist2.append(wrd)
				continue;

			if start_qmark and bSymmetric:
				end_qmark = wrd
				wordlist2.append(wrd)
				continue;

			if wrd == 'verbatim0' or wrd == 'verbatim':
				wrd = 'verbatim(eos)'
			elif wrd == 'verbatim1':
				wrd = 'verbatim1'

			wordlist2.append(wrd)

		if not end_qmark:
			ucf_log("...the start quote-mark not found!")
			return False

#		print("   wordlist2(QUOTE_TYPE) = {}".format(wordlist2))
		fout.write("QUOTE_TYPE: {}\n".format(' '.join(wordlist2)))

	return stat

def main(args):
	if len(appl_argv) < 1:
		usage();
		return False

	in_filepath = appl_argv[0]
	if not os.path.isfile(in_filepath):
		ucf_panic("%s: not found!" % in_filepath)

	if not opt_outfpath:
		ucf_panic("%s: Specify output file" % opt_outfpath)
	if os.path.isfile(opt_outfpath) and not opt_force:
		ucf_panic("%s: exists. overwrite?" % opt_outfpath)

	with open(in_filepath, 'rb') as fin:
		bVal, line2 = check_ulc_hdr(fin)
		if not bVal :
			ucf_log("%s: Failed!" % in_filepath)
			return  False

	with open(opt_outfpath, 'wb') as fout:
		fout.write(codecs.BOM_UTF8)

	with open(opt_outfpath, 'at', encoding='utf-8') as fout:
		fout.write("#@ulc-2.4{}\n".format(line2))
		with open(in_filepath, 'rt', encoding='utf-8') as fin:
			fin.readline()
			if not upgrade_ulc_file(fin, fout):
				ucf_log("%s: Failed!" % in_filepath)
				return  False

if __name__ == "__main__":
	main(sys.argv[1:])
