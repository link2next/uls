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
  <file> ShellToks.cpp </file>
  <brief>
    Tokenize and dump the shell-script file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#include "ShellLex.h"
#include <uls/UlsUtils.h>
#include <iostream>

using namespace std;
using namespace uls::collection;

namespace
{
	LPCTSTR PACKAGE_NAME = _T("ShellToks");
	tstring config_name = _T("shell.ulc");
	int  opt_verbose;

	void Usage(void)
	{
		otstringstream oss;

		oss << _T("Dumping the tokens in bash shell script.") << _tendl;
		oss << _T("Usage:") << PACKAGE_NAME << _T(" <shell-script>") << _tendl;

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

	int shelltoks_options(int opt, LPTSTR optarg)
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
	void dumpToken(ShellLex *shlex)
	{
		int t = shlex->getTokNum();

		tstring* lxm;
		shlex->getTokStr(&lxm);
		LPCTSTR tstr = lxm->c_str();

		switch (t) {
		case ShellLex::WORD:
			shlex->printf(_T("\t[   WORD] %s\n"), tstr);
			break;

		case ShellLex::NUM:
			shlex->printf(_T("\t[    NUM] %s\n"), tstr);
			break;

		case ShellLex::REDIRECT:
			shlex->printf(_T("\t[  REDIR] %s\n"), tstr);
			break;

		case ShellLex::EQ:
			shlex->printf(_T("\t[     ==] %s\n"), tstr);
			break;

		case ShellLex::NE:
			shlex->printf(_T("\t[     !=] %s\n"), tstr);
			break;

		default:
			shlex->dumpTok();
			break;
		}
	}

	// <brief>
	// This Executes tokenizing and dump the output to stdout.
	// </brief>
	// <return>none</return>
	void dumpTokens(ShellLex *shlex)
	{
		for ( ; ; ) {
			if (shlex->getToken() == ShellLex::EOI) {
				break;
			}

			dumpToken(shlex);
		}
	}
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	ShellLex *shelllex;
	tstring input_file;
	int   i0;

	if ((i0=uls::parseCommandOptions(n_targv, targv, _T("vhV"), shelltoks_options)) <= 0) {
		return i0;
	}

	shelllex = new ShellLex(config_name);

	if (i0 < n_targv) {
		input_file = targv[i0];
	} else {
		Usage();
		return 1;
	}

	if (shelllex->source(input_file) < 0) {
		_tcerr << _T(": Can't open ") << input_file << _tendl;
	} else {
		dumpTokens(shelllex);
	}

	delete shelllex;
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
