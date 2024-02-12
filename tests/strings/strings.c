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
  <file> strings.c </file>
  <brief>
    Extract the literal strings from input-file.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/


#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"

#include <stdlib.h>
#include <string.h>

#include "sample_lex.h"

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;

LPCTSTR config_name;
LPTSTR input_file;

uls_lex_t *sample_lex;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps all the literal strings in inputfiles"));
	err_log(_T("  according to the token defintions in %s"), config_name);
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("\t%s [-c <config-file>]  <inputfile> ..."), progname);
	err_log(_T(""));
	err_log(_T("  For example,"));
	err_log(_T("      %s input1.txt"), progname);
	err_log(_T("  A default config-file used, %s if you don't specifiy the config-file."), config_name);
}

static int
options(int opt, LPTSTR optarg)
{
	int stat = 0;

	switch (opt) {
	case _T('c'):
		config_name = optarg;
		break;

	case _T('m'):
		test_mode = ult_str2int(optarg);
		break;

	case _T('v'):
		opt_verbose = 1;
		break;

	case _T('h'):
		usage();
		stat = 1;
		break;

	default:
		err_log(_T("undefined option -%c"), opt);
		usage();
		stat = -1;
		break;
	}

	return stat;
}

void
test_uls_0(LPTSTR fpath)
{
	int tok;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return;
	}

	if (uls_push_fp(sample_lex, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return;
	}

	for ( ; ; ) {
		tok = uls_get_tok(sample_lex);
		if (tok == TOK_EOI) break;

		if (tok == TOK_SQUOTE) {
			uls_printf(_T("\t'%s'\n"), uls_lexeme(sample_lex));
		} else if (tok == '"') {
			uls_printf(_T("\t\"%s\"\n"), uls_lexeme(sample_lex));
		} else if (tok == TOK_ID) {
			uls_printf(_T("\t%s\n"), uls_lexeme(sample_lex));
		}
	}

	uls_fp_close(fp);
}

void
test_uls_1(LPCTSTR fpath)
{
	int tok;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return;
	}

	if (uls_push_fp(sample_lex, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return;
	}

	for ( ; ; ) {
		tok = uls_get_tok(sample_lex);
		if (tok == TOK_EOI) break;

		if (tok == TOK_CS_QUOTE) {
			uls_printf(_T("\t'%s'\n"), uls_lexeme(sample_lex));
		} else if (tok == TOK_ID) {
			uls_printf(_T("\t%s\n"), uls_lexeme(sample_lex));
		}
	}

	uls_fp_close(fp);
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i0;

	progname = uls_split_filepath(targv[0], NULL);
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sample_lex=uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	input_file = targv[i0];

	switch (test_mode) {
	case 0:
		test_uls_0(input_file);
		break;
	case 1:
		test_uls_1(input_file);
		break;
	default:
		break;
	}

	uls_destroy(sample_lex);

	return 0;
}

int main(int argc, char *argv[])
{
	LPTSTR *targv;
	int stat;

	ULS_GET_WARGS_LIST(argc, argv, targv);
	stat = _tmain(argc, targv);
	ULS_PUT_WARGS_LIST(argc, targv);

	return stat;
}
