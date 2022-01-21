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
# DATE: Jan 2019
# AUTHOR: Stanley Hong
# DESCRIPTION: upgrade the format of (input) file.
#   The format of input file : 2.1
#   The format of output file : 2.2
#
import os
import sys
import getopt
import shutil
import glob
import logging
import tempfile

opt_verbose = False
opt_force = False
opt_no_bak = False
opt_outfile = ''

input_filepath = None

HOME_DIR = os.getcwd()
upgulc_tmpdir = tempfile.mkdtemp()
utf8_bom_file = os.path.join(upgulc_tmpdir, 'utf8_bom.txt');

logging.basicConfig(level=logging.INFO)

def uls_log(msg):
	logging.error(msg)

def uls_panic(msg):
	logging.error(msg)
	raise SystemExit("program abnormally terminiated!")

progname = os.path.basename(sys.argv[0])

def make_common_utf8_file_header():
	utf8_bom = bytes([ 0xEF, 0xBB, 0xBF ])
#	print("utf8_bom_file: {}".format(utf8_bom_file))
	fp1 = open(utf8_bom_file, 'wb')
	fp1.write(utf8_bom)
	fp1.close()

def usage():
	print("usage: {} -o  <output-filepath> [-f|-v] <input-filepath>".format(progname))
	print("       {} [-f|-v] <input-filepath-1> <input-filepath-2> ...".format(progname))
	print("")
	print("       -f, --force  : override the existing 'output-filepath'.")
	print("           --no-bak : remove the original file after converting.")
	print("       -h, --help   : print help message.")
	print("")
	sys.exit(1)

try:
	opts_list, args = getopt.getopt(sys.argv[1:], "fo:hv", [ "force", "no-bak", "output=", "verbose", "help" ])
except getopt.GetoptError as err:
	print(str(err))
	usage()

for opt,optarg in opts_list:
	if opt in ('-v', '--verbose'):
		opt_verbose = True
	elif opt in ('-f', '--force'):
		opt_force = True
	elif opt == '--no-bak':
		opt_no_bak = True
	elif opt in ('-o', '--output'):
		opt_outfile = optarg
	elif opt in ('-h', '--help'):
		usage()
	else:
		print("Unknown option: {}", opt)
		sys.exit(1)

def writeline(fout, line=None):
	if line:
		line2 = line + '\n'
	else:
		line2 = '\n'
	fout.write(line2)

def convert_attr_quote_type(line, fout):
	is_open = False
	is_verbatim = False
	is_tokid_set = False

	tokid = 0
	tokname = ''
	opts_grp = []

	wrds = line.split()
	keyw = wrds.pop(0)
	if keyw != 'QUOTE_TYPE:':
		return False

	start_mark = wrds.pop(0)
	end_mark = wrds.pop(0)
	if start_mark != end_mark:
		is_asymmetric = True
	else:
		is_asymmetric = False

	for wrd in wrds:
		if is_tokid_set:
			tokname = wrd
			break

		if wrd == 'verbatim':
			is_verbatim = True
		elif wrd == 'nothing':
			opts_grp.append(wrd)
		elif wrd == 'open':
			is_open = True
			opts_grp.append(wrd)
		elif wrd == 'multiline':
			opts_grp.append(wrd)
		elif wrd == 'right_exclusive':
			opts_grp.append(wrd)
		else:
			if is_open:
				break

			if wrd[0] == '-' and wrd[1:].isdigit() or wrd.isdigit():
				tokid = int(wrd)
				is_tokid_set = True
			else:
				tokname = wrd
				break

	if not is_tokid_set:
		tokid = ord(start_mark[0])

	if is_asymmetric:
		opts_grp.append('asymmetric')

	if opts_grp:
		opts_grp_str = 'options=' + ','.join(opts_grp)
	else:
		opts_grp_str = ''

	token_grp = []
	if is_tokid_set:
		token_grp.append(str(tokid))
	if tokname:
		token_grp.append(tokname)

	if token_grp:
		token_grp_str = 'token=' + ','.join(token_grp)
	else:
		token_grp_str = ''

	line2 = "QUOTE_TYPE:"
	if opts_grp_str:
		line2 += ' ' + opts_grp_str

	if token_grp_str:
		line2 += ' ' + token_grp_str

	line2 += ' ' + start_mark

	if is_asymmetric:
		line2 += ' ' + end_mark

	if is_verbatim:
		line2 += ' verbatim1'

	writeline(fout, line2)
	return True

def convert_attr_id_chars(line, fout):
	wrds = line.split()

	keyw = wrds.pop(0)
	if keyw != 'ID_FIRST_CHARS:' and keyw != 'ID_CHARS:':
		return False

	if 'a-z' not in wrds:
		wrds.append('a-z')

	if 'A-Z' not in wrds:
		wrds.append('A-Z')

	features_str = ' '.join(wrds)
	writeline(fout, "{} {}".format(keyw, features_str))

#
# convert_ulc_config_fp
#
def convert_ulc_config_fp(fin, fout):
	header_lines = []
	ulc_ver = ''
	ulc_parent = ''

	for line in fin:
		line = line.rstrip()
		if line == '%%':
			break;

		if line.find('FILE_VERSION:') >= 0:
			wrds = line.split()
			ulc_ver = wrds[1]
		elif line.startswith('#@'):
			ulc_parent = line[2:]
		else:
			header_lines.append(line)

	if not ulc_ver:
		uls_log("Lexical attribute 'FILE_VERSION' not found")
		return False

	if ulc_ver != '2.1' and ulc_ver != '2.2':
		uls_log("Unsupported ULC version: %s" % ulc_ver)
		return False

	if ulc_parent:
		writeline(fout, "#@ulc-2.3:{}".format(ulc_parent))
	else:
		writeline(fout, "#@ulc-2.3")
	writeline(fout)

	writeline(fout, "DOMAIN: uls.link2next.io://season-1")

	for line in header_lines:
		if not line or line.startswith('#'):
			is_trivial = True
		else:
			is_trivial = False

		if is_trivial:
			writeline(fout, line)
		elif line.find('QUOTE_TYPE:') >= 0:
			convert_attr_quote_type(line, fout)
		elif line.find('ID_FIRST_CHARS:') >= 0 or line.find('ID_CHARS:') >= 0:
			convert_attr_id_chars(line, fout)
		else:
			writeline(fout, line)

	writeline(fout, "NUMBER_PREFIXES: 0x:16 0X:16 0:8")
	writeline(fout, "")
	writeline(fout, "%%")

	for line in fin:
		line = line.rstrip()
		writeline(fout, line)
	writeline(fout)

	return True

def convert_ulc_config(infile, outfile=''):
	if not os.path.exists(infile):
		uls_log("%s: not found!" % infile)
		return False

	infile_bak = infile + '.bak'
	os.rename(infile, infile_bak)
	if not outfile:
		outfile = infile

	filename = os.path.basename(infile)
	fname = os.path.splitext(filename)[0]
	outfile1 = os.path.join(upgulc_tmpdir, fname + '.s1');

	with open(outfile1, 'wt', encoding='utf-8') as fout, open(infile_bak, 'rt', encoding='utf-8') as fin:
		convert_ulc_config_fp(fin, fout)

	with open(outfile, 'wb') as fout:
		with open(utf8_bom_file, 'rb') as fin:
			bdata = fin.read()
			fout.write(bdata)
		with open(outfile1, 'rb') as fin:
			bdata = fin.read()
			fout.write(bdata)

	if not os.path.exists(infile):
		os.rename(infile_bak, infile)

	if opt_no_bak:
		if os.path.exists(infile_bak):
			os.remove(infile_bak)

	return True;

def convert_ulc_config_list(input_file_list):
	for infile in args:
		print("Upgrading the format of {}...".format(infile))
		convert_ulc_config(infile)

	return True;

def main():
	global input_filepath
	stat = True

	if not opt_outfile:
		stat = convert_ulc_config_list(args)
		return stat

	if len(args) < 1:
		uls_log("specify output filepath!")
		return False

	input_filepath = args[0]

	if opt_verbose:
		print("Input: {}".format(input_filepath))
		print("Output: {}".format(opt_outfile))

	if opt_force == False and os.path.exists(opt_outfile):
		uls_log("%s: already exists!" % opt_outfile)
		stat = False
	else:
		if opt_verbose:
			print("Upgrade ulc-file {} from v2.1 to v2.3...".format(infile))

		if convert_ulc_config(input_filepath, opt_outfile) == False:
			stat = False

	return stat

if __name__ == "__main__":
	make_common_utf8_file_header()
	main()
	os.chdir(HOME_DIR)
	shutil.rmtree(upgulc_tmpdir)
