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
  <file> EngToks.cpp </file>
  <brief>
    Tokenize and dump English
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Nov. 2017
  </author>
*/
#include "EngLex.h"
#include <uls/UlsUtils.h>
#include <iostream>

using namespace std;
using namespace uls::collection;

using tstring = uls::tstring;
using otstringstream = uls::otstringstream;

namespace
{
	LPCTSTR PACKAGE_NAME = _T("EngToks");
	tstring config_name = _T("eng.ulc");
	int  opt_verbose;

	void Usage(void)
	{
		otstringstream oss;

		oss << _T("Dumping the tokens in plain English text.") << _tendl;
		oss << _T("Usage:") << PACKAGE_NAME << _T(" <english text>") << _tendl;

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

	int engtoks_options(int opt, LPTSTR optarg)
	{
		int   stat = 0;

		switch (opt) {
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

	// <brief>
	// This is a virtual method, inherited from 'UlsLex' class.
	// This prints to the stdout the string representation of the current token.
	// </brief>
	// <return>none</return>
	void dump_token(EngLex *englex)
	{
		int t = englex->getTokNum();

		tstring lxm;
		englex->getTokStr(lxm);
		LPCTSTR tstr = lxm.c_str();

		switch (t) {
		case EngLex::WORD:
			englex->printf(_T("\t[   WORD] %s\n"), tstr);
			break;

		case EngLex::NUM:
			englex->printf(_T("\t[    NUM] %s\n"), tstr);
			break;

		default:
			englex->dumpTok();
			break;
		}
	}

	// <brief>
	// This Executes tokenizing and dump the output to stdout.
	// </brief>
	// <return>none</return>
	void dump_tokens(EngLex *englex)
	{
		for ( ; ; ) {
			if (englex->next() == EngLex::EOI) {
				break;
			}

			dump_token(englex);
		}
	}
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	EngLex *englex;
	tstring input_file;
	int   i0;

	if ((i0=uls::parseCommandOptions(n_targv, targv, _T("vhV"), engtoks_options)) <= 0) {
		return i0;
	}

	englex = new EngLex(config_name);

	if (i0 < n_targv) {
		input_file = targv[i0];
	} else {
		Usage();
		return 1;
	}

	if (englex->set_input_file(input_file) < 0) {
		_tcerr << _T(": Can't open ") << input_file << _tendl;
	} else {
		dump_tokens(englex);
	}

	delete englex;
	return 0;
}

#ifndef __ULS_WINDOWS__
int
main(int argc, char *argv[])
{
	uls::ArgListT targlst;
	LPTSTR *targv;
	int stat;

	ULSCPP_GET_WARGS_LIST(targlst, argc, argv, targv);
	stat = _tmain(argc, targv);
	ULSCPP_PUT_WARGS_LIST(targlst);

	return stat;
}
#endif
