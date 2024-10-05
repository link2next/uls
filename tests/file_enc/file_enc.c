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
    Stanley Hong <link2next@gmail.com>, 2024.
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
test_escchar_map(uls_lex_ptr_t uls, LPCTSTR fpath)
{
	int tok;
	FILE *fp;
	LPCTSTR tokstr, qstr_type;
	unsigned char ch;
	int i, len;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return;
	}

	for ( ; (tok = uls_get_tok(uls)) != TOK_EOI; ) {
		tokstr = uls_lexeme(uls);

		if (tok == TOK_SQUOTE) {
			qstr_type = _T("SQ");
		} else if (tok == TOK_DQUOTE) {
			qstr_type = _T("DQ");
		} else {
			qstr_type = NULL;
		}

		if (qstr_type != NULL) {
			len = uls_lexeme_len(uls);
			uls_printf(_T("#%-2d: %3s(%2d)"), uls_get_lineno(uls), qstr_type, len);
			for (i = 0; i < len; i++ ) {
				ch = tokstr[i];
				if (ch == '\\') {
					uls_printf(_T("  -"));
				} else {
					uls_printf(_T(" %02X"), ch);
				}

			}
			uls_printf(_T("\n"));
		}
	}

	uls_fp_close(fp);
}

void
test_dump_1(uls_lex_ptr_t uls, LPTSTR fpath)
{
	LPCTSTR tokstr;
	FILE   *fp;
	int t;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T(" file open error"));
		return;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return;
	}

	uls_set_tag(uls, fpath, 1);

	for ( ; ; ) {
		t = uls_get_tok(uls);
		tokstr = uls_tokstr(uls);

		if (t == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), tokstr);
			break;
		}

		if (t == TOK_EOI) {
			break;
		}

		uls_printf(_T("#%d(%6d) : %s\n"), uls_get_lineno(uls), t, tokstr);
	}

	uls_fp_close(fp);
}

void
test_uls_strings_2(uls_lex_ptr_t uls, LPTSTR fpath)
{
	int tok;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return;
	}

	for ( ; ; ) {
		tok = uls_get_tok(uls);
		if (tok == TOK_EOI) break;

		if (tok == TOK_SQUOTE) {
			uls_printf(_T("\t'%s'\n"), uls_lexeme(uls));
		} else if (tok == '"') {
			uls_printf(_T("\t\"%s\"\n"), uls_lexeme(uls));
		} else if (tok == TOK_ID) {
			uls_printf(_T("\t%s\n"), uls_lexeme(uls));
		}
	}

	uls_fp_close(fp);
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	uls_lex_ptr_t sample_lex;
	int i0;

	progname = uls_split_filepath(targv[0], NULL);
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	input_file = targv[i0];

	switch (test_mode) {
	case 0:
		test_escchar_map(sample_lex, input_file);
		break;
	case 1:
		test_dump_1(sample_lex, input_file);
		break;
	case 2:
		test_uls_strings_2(sample_lex, input_file);
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
