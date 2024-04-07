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

using namespace std;
using namespace uls;
using namespace uls::collection;

namespace
{
	const char * PACKAGE_NAME = "ShellToks";
	string config_name = "shell.ulc";
	int  opt_verbose;

	void Usage(void)
	{
		err_log("Where the 'simplest.ulc' is:");
		listUlcSearchPaths();
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

	int shelltoks_options(int opt, char * optarg)
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
			err_log("undefined option -%c", opt);
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
		const char * tstr = shlex->getTokStr().c_str();

		switch (t) {
		case ShellLex::WORD:
			uls_printf("\t[   WORD] %s\n", tstr);
			break;

		case ShellLex::NUM:
			uls_printf("\t[    NUM] %s\n", tstr);
			break;

		case ShellLex::REDIRECT:
			uls_printf("\t[  REDIR] %s\n", tstr);
			break;

		case ShellLex::EQ:
			uls_printf("\t[     ==] %s\n", tstr);
			break;

		case ShellLex::NE:
			uls_printf("\t[     !=] %s\n", tstr);
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
main(int argc, char **argv)
{
	ShellLex *shelllex;
	string input_file;
	int   i0;

	if ((i0=uls_getopts(argc, argv, "vhV", shelltoks_options)) <= 0) {
		return i0;
	}

	shelllex = new ShellLex(config_name);

	if (i0 < argc) {
		input_file = argv[i0];
	} else {
		Usage();
		return 1;
	}

	if (shelllex->source(input_file) < 0) {
		err_log("can't open '%s'", input_file.c_str());
	} else {
		dumpTokens(shelllex);
	}

	delete shelllex;
	return 0;
}

