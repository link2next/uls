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
  <file> Css3Toks.cpp </file>
  <brief>
    Tokenize and dump the css3 file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/
#include "Css3Lex.h"
#include <uls/UlsUtils.h>

using namespace std;
using namespace uls;
using namespace uls::collection;

namespace
{
	string config_name = "css3.ulc";
	const char * PACKAGE_NAME = "Css3Toks";
	int  opt_verbose;

	void Usage(void)
	{
		err_log("Usage: %s <css3-file>", PACKAGE_NAME);
		err_log("   Dumping the tokens in css3 file.");
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

	int css3toks_options(int opt, char * optarg)
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
	void dumpToken(Css3Lex *css3lex)
	{
		int t = css3lex->getTokNum();
		const char * tstr = css3lex->getTokStr().c_str();

		if (t == Css3Lex::CSS_ID) {
			uls_printf("\t[     ID] %s\n", tstr);
		}
		else if (t == Css3Lex::CSS_NUM) {
			uls_printf("\t[    NUM] %s\n", tstr);
		}
		else if (t == Css3Lex::CSS_PATH) {
			uls_printf("\t[   PATH] '%s'\n", tstr);
		}
		else {
			css3lex->dumpTok();
		}
	}

	// <brief>
	// This Executes tokenizing and dump the output to stdout.
	// </brief>
	// <return>none</return>
	void dumpTokens(Css3Lex *css3lex)
	{
		for ( ; ; ) {
			if (css3lex->getTok() == Css3Lex::CSS_EOI) break;
			dumpToken(css3lex);
		}
	}
}

int
main(int argc, char **argv)
{
	Css3Lex *css3lex;
	string input_file;
	int   i0;

	if ((i0=uls_getopts(argc, argv, "vhV", css3toks_options)) <= 0) {
		return i0;
	}

	css3lex = new Css3Lex(config_name);

	if (i0 < argc) {
		input_file = argv[i0];
	} else {
		Usage();
		return 1;
	}

	css3lex->setFile(input_file);
	dumpTokens(css3lex);

	delete css3lex;

	return 0;
}
