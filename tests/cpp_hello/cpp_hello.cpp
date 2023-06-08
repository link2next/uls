/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
  <file> cpp_hello.cpp </file>
  <brief>
    testing the ULS-API in C++.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <uls/UlsLex.h>
#include <uls/UlsIStream.h>
#include <uls/UlsOStream.h>
#include <uls/uls_util.h>

#include "Sample2Lex.h"

using namespace std;
using namespace uls::crux;
using namespace AAA::BBB;

class sample_xdef {
public:
	int tok_id;
	int prec;
	int node_id;

	sample_xdef() {
		tok_id = 0;
		prec = 1;
		node_id = -1;
	}

	void set(int t, int p, int n)
	{
		tok_id = t;
		prec = p;
		node_id = n;
	}
};

LPCTSTR progname;
tstring out_dir = _T(".");
int  opt_verbose;
int  test_mode;

sample_xdef *xdefs_list;

static void
usage(void)
{
	err_log(_T("usage: %s [-m<0|1>] <input-file>"), progname);
}

static int
cpphello_options(int opt, LPTSTR optarg)
{
	int   stat = 0;

	switch (opt) {
	case _T('m'):
		test_mode = ult_str2int(optarg);
		break;

	case _T('v'):
		opt_verbose = 1;
		break;

	default:
		err_log(_T("undefined option -%c"), opt);
		stat = -1;
		break;
	}

	return stat;
}

void
test_input_string(Sample2Lex *sam_lex)
{
	sam_lex->pushLine(_T("C++ hello world\n\t\n"));

	while (sam_lex->getToken() != Sample2Lex::EOI) {
		sam_lex->dumpTok();
	}
}

void
test_input_file(Sample2Lex *sam_lex, tstring& filename)
{
	tstring *lxm;

	sam_lex->printf(_T("Welcome to ULS World!\n"));
	sam_lex->printf(_T("Testing ULS in C++ ...\n"));

	sam_lex->pushFile(filename);
	sam_lex->setFileName(filename);

	while (sam_lex->getToken() != Sample2Lex::EOI) {
		sam_lex->getTokStr(&lxm);
		sam_lex->printf(_T("\t[%8t] %10s at %w\n"), lxm->c_str());
	}
}

void init_xdefs(void)
{
	sample_xdef *xdef;

	xdef = &xdefs_list[0];
	xdef->set('*', 27, 1);

	xdef = &xdefs_list[1];
	xdef->set('+', 26, 2);

	xdef = &xdefs_list[2];
	xdef->set('/', 25, 3);

	xdef = &xdefs_list[3];
	xdef->set('&', 24, 4);

	xdef = &xdefs_list[4];
	xdef->set('^', 23, 5);
}

void test_extra_tokdefs(UlsLex *lex)
{
	sample_xdef *xdef;
	int i, t;

	xdefs_list = new sample_xdef[5];
	init_xdefs();

	for (i = 0; i < 5; i++) {
		xdef = xdefs_list + i;
		uls_printf(_T("\txdefs[%d] = %d\n"), i, xdef->tok_id);
		lex->setExtraTokdef(xdef->tok_id, xdef);
	}

	tstring line = _T("C++ *hello/ &world^\n\t\n");
	lex->pushLine(line.c_str());

	while ((t=lex->getToken()) != lex->toknum_EOI) {
		lex->dumpTok(_T("\t"), _T(""));

		xdef = (sample_xdef *) lex->getExtraTokdef();
		if (xdef != NULL)
			lex->printf(_T(" prec=%d node_id=%d"), xdef->prec, xdef->node_id);
		lex->printf(_T("\n"));
	}

	delete [] xdefs_list;
}

bool test_simple_streaming(tstring& input_file, tstring& out_dir)
{
	UlsLex *lex = new UlsLex(_T("c++"));
	tstring uls_file;
	tstring *lxm;
	int t;

	uls_file = out_dir;
	uls_file += ULS_FILEPATH_DELIM;
	uls_file += _T("a2.uls");

	UlsOStream *ofile = new UlsOStream(uls_file, lex, _T("<<tag>>"));
	UlsIStream *ifile = new UlsIStream(input_file);

	ofile->startStream(*ifile);

	delete ifile;
	delete ofile;

	// Read the written input-stream previously.
	ifile = new UlsIStream(uls_file, NULL);

	lex->pushInput(*ifile);
	lex->getTokStr(&lxm);

	while (1) {
		t = lex->getTok();
		if (t == lex->toknum_EOI) break;
		lex->printf(_T("\t[%7t]  %s\n"), lxm->c_str());
	}

	delete ifile;
	delete lex;

	return true;
}

bool test_tokseq(UlsLex *lex, tstring& input_file, tstring& out_dir)
{
	UlsTmplList  tmpl_list;
	tstring uls_file;
	tstring *lxm;
	int t, stat=0;

	uls_file = out_dir;
	uls_file += ULS_FILEPATH_DELIM;
	uls_file += _T("a3.uls");

	tmpl_list.setValue(_T("TVAR1"), _T("unsigned long long"));
	tmpl_list.setValue(_T("TVAR2"), _T("long double"));

	// Write a output-stream
	UlsOStream *ofile = new UlsOStream(uls_file, lex, _T("<<tag>>"));
	lex->pushFile(input_file);
	lex->getTokStr(&lxm);

	while (1) {
		t = lex->getTok();

		if (t == lex->toknum_ERR) {
			stat = -1; break;
		}

		if (t == lex->toknum_EOF || t == lex->toknum_EOI) {
			break;
		}

		if (t == lex->toknum_ID && tmpl_list.exist(*lxm) == true) {
			t = lex->toknum_TMPL;
		}

		ofile->printTok(t, *lxm);
	}

	delete ofile;
	if (stat < 0) return false;

	// Read the written input-stream previously.
	UlsIStream *ifile = new UlsIStream(uls_file, &tmpl_list);

	lex->pushInput(*ifile);
	lex->getTokStr(&lxm);

	while (1) {
		t = lex->getTok();
		if (t == lex->toknum_EOI) break;
		lex->printf(_T("\t[%7t] %s\n"), lxm->c_str());
	}

	delete ifile;

	return true;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	Sample2Lex *sample1_lex;
	UlsLex *sample2_lex;
	tstring *input_fpath = NULL;
	int i0, stat = 0;

	progname = uls_split_filepath(targv[0], NULL);
	test_mode = 0;

	if ((i0=uls_getopts(n_targv, targv, _T("m:v"), cpphello_options)) <= 0) {
		usage();
		return -1;
	}

	if (i0 < n_targv) {
		input_fpath = new tstring(targv[i0]);
	}

	sample1_lex = new Sample2Lex();
	sample2_lex = new UlsLex(_T("dumptoks.ulc"));

	switch (test_mode) {
	case 0:
		test_input_string(sample1_lex);
		break;

	case 1:
		if (input_fpath != NULL) {
			test_input_file(sample1_lex, *input_fpath);
		}
		break;

	case 2:
		test_extra_tokdefs(sample2_lex);
		break;

	case 3:
		if (input_fpath != NULL) {
			test_simple_streaming(*input_fpath, out_dir);
		}
		break;

	case 4:
		if (input_fpath != NULL) {
			test_tokseq(sample2_lex, *input_fpath, out_dir);
		}
		break;

	default:
		break;
	}

	delete sample1_lex;
	delete sample2_lex;

	return stat;
}

#ifndef __WINDOWS__
int
main(int argc, char *argv[])
{
	LPTSTR *targv;
	int stat;

	ULS_GET_WARGS_LIST(argc, argv, targv);
	stat = _tmain(argc, targv);
	ULS_PUT_WARGS_LIST(argc, targv);

	return stat;
}
#endif
