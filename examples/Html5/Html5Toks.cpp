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
  <file> Html5Toks.cpp </file>
  <brief>
    Tokenize and dump the html5 file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#include "Html5Lex.h"
#include <uls/UlsUtils.h>

using namespace std;
using namespace uls;
using namespace uls::collection;

namespace
{
	tstring config_name = _T("html5.ulc");
	LPCTSTR PACKAGE_NAME = _T("Html5Toks");
	int  opt_verbose;

	void Usage(void)
	{
		err_log(_T("usage(%s): dumping the tokens defined as in 'sample.ulc'"), PACKAGE_NAME);
		err_log(_T("\t%s <filepath>"), PACKAGE_NAME);
	}

	void Version(void)
	{
		err_log(ULS_GREETING);
		err_log(_T("Copyright (C) %d-%d All rights reserved."),
			ULS_COPYRIGHT_YEAR_START, ULS_COPYRIGHT_YEAR_CURRENT);
		err_log(_T("Unless required by applicable law or agreed to in writing, software"));
		err_log(_T("distributed under the License is distributed on an \"AS IS\" BASIS,"));
		err_log(_T("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied."));
		err_log(_T(""));
	}

	int html5toks_options(int opt, LPTSTR optarg)
	{
		int   stat = 0;

		switch (opt) {
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
			err_log(_T("undefined option -%c"), opt);
			stat = -1;
			break;
		}

		return stat;
	}

	// <brief>
	// This is a virtual method, inherited from 'UlsLex' class.
	// This prints to the stdout the tstring representation of the current token.
	// </brief>
	// <return>none</return>
	void dumpToken(Html5Lex *html5lex)
	{
		int t = html5lex->getTokNum();
		LPCTSTR tstr = html5lex->getTokStr().c_str();

		switch (t) {
		case Html5Lex::ID:
			uls_printf(_T("\t[     ID] %s\n"), tstr);
			break;
		case Html5Lex::NUM:
			uls_printf(_T("\t[    NUM] %s\n"), tstr);
			break;
		case Html5Lex::TEXT:
			uls_printf(_T("\t[   TEXT] $%s$\n"), tstr);
			break;
		case Html5Lex::TAGBEGIN:
			uls_printf(_T("\t[    TAG] %s\n"), tstr);
			break;
		case Html5Lex::TAGEND:
			uls_printf(_T("\t[   /TAG] %s\n"), tstr);
			break;
		default:
			html5lex->dumpTok();
			break;
		}
	}

	// <brief>
	// This Executes tokenizing and dump the output to stdout.
	// </brief>
	// <return>none</return>
	void dumpTokens(Html5Lex *html5lex)
	{
		for ( ; ; ) {
			if (html5lex->getTok() == Html5Lex::EOI) break;
			dumpToken(html5lex);
		}
	}
}

int
_tmain(int argc, LPTARGV argv)
{
	LPTSTR *targv;
	Html5Lex *html5lex;
	tstring input_file;
	int   i0;

	ULS_GET_WARGS_LIST(argc, argv, targv);

	if ((i0=uls_getopts(argc, targv, _T("vhV"), html5toks_options)) <= 0) {
		return i0;
	}

	html5lex = new Html5Lex(config_name);

	if (i0 < argc) {
		input_file = targv[i0];
	} else {
		Usage();
		return 1;
	}

	if (html5lex->setFile(input_file) < 0) {
		err_log(_T("%s: file open error"), input_file.c_str());
	} else {
		dumpTokens(html5lex);
	}

	delete html5lex;
	ULS_PUT_WARGS_LIST(argc, targv);

	return 0;
}


