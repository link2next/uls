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
  <file> DumpToks.cpp </file>
  <brief>
    Demonstrate the C++ API of Uls
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "DumpToksBasis.h"
#include <uls/UlsUtils.h>
#include <uls/UlsLex.h>
#include <uls/UlsIStream.h>
#include <uls/UlsOStream.h>

#include <iostream>
#include <string>
#include <sstream>

#include <uls/uls_util.h>

using namespace std;
using namespace uls::crux;

namespace
{
	LPCTSTR PACKAGE_NAME = _T("DumpToks");
	int  opt_verbose;
	tstring config_name;
	tstring input_file;

	void Usage(void)
	{
		otstringstream oss;

		oss << _T("Usage: ") << PACKAGE_NAME << _T(" [-L <ulc-file>] <file>") << _tendl;
		oss << _T("   Dumping the tokens under <ulc-file> in file specified in argv[].") << _tendl;
		oss << _T("Home page: ") << ULS_URL << _tendl;

		_tcerr << oss.str() << _tendl;
	}

	void Version(void)
	{
		otstringstream oss;

		oss << ULS_GREETING << _tendl;
		oss << _T("Copyright (C) ") << ULS_COPYRIGHT_YEAR_START << _T("-") <<  ULS_COPYRIGHT_YEAR_CURRENT << _T(" All rights reserved.") << _tendl;
		oss << _T("Unless required by applicable law or agreed to in writing, software") << _tendl;
		oss << _T("distributed under the License is distributed on an \"AS IS\" BASIS,") << _tendl;
		oss << _T("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.") << _tendl;

		_tcerr << oss.str() << _tendl;
	}

	int options(int opt, LPTSTR optarg)
	{
		int   stat = 0;

		switch (opt) {
		case _T('L'):
			config_name = optarg;
			break;
		case _T('v'):
			opt_verbose = 1;
			break;
		case _T('h'):
			Usage();
			stat = 1;
			break;
		case _T('V'):
			Version();
			stat = 1;
			break;
		default:
			_tcerr << _T("undefined option -") << (TCHAR) opt << _tendl;
			stat = -1;
			break;
		}

		return stat;
	}

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

	static sample_xdef *xdefs_list;

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
			lex->printf(_T("\txdefs[%d] = %d\n"), i, xdef->tok_id);
			lex->setExtraTokdef(xdef->tok_id, xdef);
		}

		LPCTSTR line = _T("C++ *hello/ &world^\n\t\n");
		lex->pushLine(line);

		while ((t=lex->getToken()) != lex->toknum_EOI) {
			lex->dumpTok(_T("\t"), _T(""));

			xdef = (sample_xdef *) lex->getExtraTokdef();
			if (xdef != NULL)
				lex->printf(_T(" prec=%d node_id=%d"), xdef->prec, xdef->node_id);
			lex->printf(_T("\n"));
		}

		delete []xdefs_list;
	}

	bool test_streaming(UlsLex *lex, tstring out_dir)
	{
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

		return true;
	}

	bool test_tokseq(UlsLex *lex, tstring out_dir)
	{
		UlsTmplList  tmpl_list;
		tstring uls_file;
		tstring *lxm;
		int t, stat = 0;

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
				return false;
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
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	tstring out_dir;
	int   i0;

	if ((i0=uls::parseCommandOptions(n_targv, targv, _T("L:vhV"), options)) <= 0) {
		return i0;
	}

	if (config_name.compare(_T("")) == 0)
		config_name = _T("dumptoks.ulc");

	out_dir = _T(".");

	if (i0 < n_targv) {
		input_file = targv[i0];
	} else {
		input_file = _T("dumptoks_sam.txt");
	}

	if (i0 + 1 < n_targv) {
		out_dir = targv[i0 + 1];
		if (uls::direntExist(out_dir.c_str()) != ST_MODE_DIR) {
			_tcerr << out_dir << _T(": not a directory") << _tendl;
			return -1;
		}
	}

	UlsLex *sample1_lex = new UlsLex(_T("simple"));
	UlsLex *sample2_lex = new UlsLex(config_name);

	if (sample1_lex == NULL || sample2_lex == NULL) {
		uls::listUlcSearchPaths();
		_tcerr << _T("Can't init uls-object") << _tendl;
		return -1;
	}

	test_extra_tokdefs(sample2_lex);

	_tcout << _T(" ----------- ----------- ----------- ----------- -----------") << _tendl;
	test_streaming(sample1_lex, out_dir);

	_tcout << _T(" ----------- ----------- ----------- ----------- -----------") << _tendl;
	test_tokseq(sample2_lex, out_dir);

	delete sample1_lex;
	delete sample2_lex;
	return 0;
}

#ifndef __WINDOWS__
int
main(int argc, char *argv[])
{
	LPTSTR *targv;
	int stat;

	ULSCPP_GET_WARGS_LIST(argc, argv, targv);
	stat = _tmain(argc, targv);
	ULSCPP_PUT_WARGS_LIST(argc, targv);

	return stat;
}
#endif
