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

#include <uls/uls_util.h>

using namespace std;
using namespace uls::crux;

namespace
{
	const char * PACKAGE_NAME = "DumpToks";
	int  opt_verbose;
	string config_name;
	string input_file;

	void Usage(void)
	{
		err_log("Usage: %s [-L <ulc-file>] <file>", PACKAGE_NAME);
		err_log("   Dumping the tokens under <ulc-file> in file specified in argv[].", PACKAGE_NAME);
		err_log("%s home page: <%s>\n", PACKAGE_NAME, ULS_URL);
	}

	void Version(void)
	{
		err_log(ULS_GREETING);
		err_log("Copyright (C) %d-%d All rights reserved.",
			ULS_COPYRIGHT_YEAR_START, ULS_COPYRIGHT_YEAR_CURRENT);
		err_log("Unless required by applicable law or agreed to in writing, software");
		err_log("distributed under the License is distributed on an \"AS IS\" BASIS,");
		err_log("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.");
		err_log("");
	}

	int options(int opt, char * optarg)
	{
		int   stat = 0;

		switch (opt) {
		case 'L':
			config_name = optarg;
			break;
		case 'v':
			opt_verbose = 1;
			break;
		case 'h':
			Usage();
			stat = 1;
			break;
		case 'V':
			Version();
			stat = 1;
			break;
		default:
			err_log("undefined option -%c", opt);
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
			uls_printf("\txdefs[%d] = %d\n", i, xdef->tok_id);
			lex->setExtraTokdef(xdef->tok_id, xdef);
		}

		string line = string("C++ *hello/ &world^\n\t\n");
		lex->pushLine(line.c_str());

		while ((t=lex->getToken()) != lex->toknum_EOI) {
			lex->dumpTok("\t", "");

			xdef = (sample_xdef *) lex->getExtraTokdef();
			if (xdef != NULL)
				lex->printf(" prec=%d node_id=%d", xdef->prec, xdef->node_id);
			lex->printf("\n");
		}

		delete []xdefs_list;
	}

	bool test_streaming(UlsLex *lex, string out_dir)
	{
		string uls_file;
		string *lxm;
		int t;

		uls_file = out_dir;
		uls_file += ULS_FILEPATH_DELIM;
		uls_file += "a2.uls";

		UlsOStream *ofile = new UlsOStream(uls_file, lex, "<<tag>>");
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
			lex->printf("\t[%7t]  %s\n", lxm->c_str());
		}

		delete ifile;

		return true;
	}

	bool test_tokseq(UlsLex *lex, string out_dir)
	{
		UlsTmplList  tmpl_list;
		string uls_file;
		string *lxm;
		int t, stat=0;

		uls_file = out_dir;
		uls_file += ULS_FILEPATH_DELIM;
		uls_file += "a3.uls";

		tmpl_list.setValue("TVAR1", "unsigned long long");
		tmpl_list.setValue("TVAR2", "long double");

		// Write a output-stream
		UlsOStream *ofile = new UlsOStream(uls_file, lex, "<<tag>>");
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
			lex->printf("\t[%7t] %s\n", lxm->c_str());
		}

		delete ifile;

		return true;
	}
}

int
main(int argc, char **argv)
{
	string out_dir;
	int   i0;

	if ((i0=uls_getopts(argc, argv, "L:vhV", options)) <= 0) {
		return i0;
	}

	if (config_name.compare("") == 0)
		config_name = "dumptoks.ulc";

	out_dir = ".";

	if (i0 < argc) {
		input_file = argv[i0];
	} else {
		input_file = "dumptoks_sam.txt";
	}

	if (i0 + 1 < argc) {
		out_dir = argv[i0 + 1];
		if (uls_dirent_exist(out_dir.c_str()) != ST_MODE_DIR) {
			err_panic("%s: not a directory'", out_dir.c_str());
		}
	}

	UlsLex *sample1_lex = new UlsLex("simple");
	UlsLex *sample2_lex = new UlsLex(config_name);

	if (sample1_lex == NULL || sample2_lex == NULL) {
		uls::listUlcSearchPaths();
		err_panic("%s: can't init uls-object", __func__);
	}

	test_extra_tokdefs(sample2_lex);

	uls_printf(" ----------- ----------- ----------- ----------- ----------- \n");
	test_streaming(sample1_lex, out_dir);

	uls_printf(" ----------- ----------- ----------- ----------- ----------- \n");
	test_tokseq(sample2_lex, out_dir);

	delete sample1_lex;
	delete sample2_lex;
	return 0;
}
