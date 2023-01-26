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
  <file> MkfToks.cpp </file>
  <brief>
    Tokenize and dump the 'Makefile' file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#include "MkfLex.h"
#include <uls/UlsUtils.h>
#include <iostream>

using namespace std;
using namespace uls::collection;

namespace
{
	LPCTSTR PACKAGE_NAME = _T("MkfToks");
	tstring config_name = _T("mkf.ulc");
	int  opt_verbose;

	void Usage(void)
	{
		otstringstream oss;

		oss << _T("Dumping the tokens in Makefile.") << _tendl;
		oss << _T("Usage:") << PACKAGE_NAME << _T(" <makefile>") << _tendl;

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

	int mkftoks_options(int opt, LPTSTR optarg)
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
			stat = -1;
			break;
		}

		return stat;
	}

	// <brief>
	// This is a virtual method, inherited from 'UlsLex' class.
	// This prints to the stdout the string representation of the current TokNum.
	// </brief>
	// <return>none</return>
	void dumpToken(MkfLex *mkflex)
	{
		int t = mkflex->getTokNum();

		tstring* lxm;
		mkflex->getTokStr(&lxm);
		LPCTSTR tstr = lxm->c_str();

		switch (t) {
		case MkfLex::TABBLK:
			/*
			* In case that the current is not really TABBLK,
			*    the mkf command block of a makefile rule,
			* subdivide the TokStr of it and get tokens from it using by uls_push_line().
			*/
			mkflex->printf(_T("\t[ TABBLK] %s\n"), tstr);
			break;

		case MkfLex::WORD:
			mkflex->printf(_T("\t[   WORD] %s\n"), tstr);
			break;

		case MkfLex::NUM:
			mkflex->printf(_T("\t[    NUM] %s\n"), tstr);
			break;

		default:
			mkflex->dumpTok();
			break;
		}
	}

	// <brief>
	// This Executes tokenizing and dump the output to stdout.
	// </brief>
	// <return>none</return>
	void dumpTokens(MkfLex *mkflex)
	{
		int t;

		for ( ; ; ) {
			if ((t=mkflex->getTok()) == MkfLex::ERR) {
				_tcerr << _T("Error in tokenizing!") << _tendl;
				break;
			}

			if (t == MkfLex::EOI) {
				break;
			}

			dumpToken(mkflex);
		}
	}
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	MkfLex *mkflex;
	tstring input_file;
	int   i0;

	if ((i0=uls::parseCommandOptions(n_targv, targv, _T("vhV"), mkftoks_options)) <= 0) {
		return i0;
	}

	mkflex = new MkfLex(config_name);

	if (i0 < n_targv) {
		input_file = targv[i0];
	} else {
		Usage();
		return 1;
	}

	if (mkflex->include(input_file) < 0) {
		_tcerr << _T(": Can't open ") << input_file << _tendl;
	} else {
		dumpTokens(mkflex);
	}

	delete mkflex;
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
