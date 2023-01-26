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
  <file> nested.c </file>
  <brief>
    Read from input file and just dump tokens.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

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
	err_log(_T("  Tokenizes the input that includes another file."));
	err_log(_T("  Tests if uls_unget_tok() is properly operating."));
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("  %s [-c <config-file>] <inputfile> ..."), progname);
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
test_uls(LPTSTR fpath)
{
	int fd;
	int tok;
	int i, n_pushes, tabs=0;
	TCHAR buff[16];

	if ((fd = uls_fd_open(fpath, 0)) < 0) {
		err_log(_T(" file open error"));
		return;
	}

	if (uls_set_fd(sample_lex, fd, 0) < 0) {
		err_log(_T("can't set the istream!"));
		return;
	}

	uls_set_tag(sample_lex, fpath, 1);
	uls_want_eof(sample_lex);

	for (n_pushes=0; ; ) {
		if ((tok=uls_get_tok(sample_lex)) == TOK_EOI) break;

		if (n_pushes > 0) {
			uls_unget_tok(sample_lex);
			--n_pushes;
			continue;
		}

		for (i=0; i<tabs; i++) uls_printf(_T("\t"));

		if (tok == TOK_EOF) {
			uls_printf(_T(" EOF: '%s'\n"), uls_lexeme(sample_lex));
			--tabs;
			continue;
		}

		uls_printf(_T("%3d"), uls_get_lineno(sample_lex));
		uls_dump_tok(sample_lex, _T("\t"), _T("\n"));

		if (tok == TOK_ID) {
			if (ult_str_equal( uls_lexeme(sample_lex), _T("WWW"))) {
				uls_push_file(sample_lex, _T("./input1_0.txt"), ULS_WANT_EOFTOK);
				uls_push_file(sample_lex, _T("./input1_1.txt"), ULS_WANT_EOFTOK);
				tabs += 2;

			} else if (ult_str_equal(uls_lexeme(sample_lex), _T("VVV"))) {
				for (i=2; i>=0; i--) {
					uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("VVV%d"), i);
					uls_unget_ch(sample_lex, _T(' '));
					uls_unget_str(sample_lex, buff);
				}

			} else if (ult_str_equal(uls_lexeme(sample_lex), _T("UUU"))) {
				n_pushes = 3;
			}
		}
	}

	close(fd);
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i, i0;

	progname = uls_split_filepath(targv[0], NULL);
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	for (i=i0; i<n_targv; i++) {
		input_file = targv[i];

		if ((sample_lex = uls_create(config_name)) == NULL) {
			err_log(_T("can't init uls-object"));
			return -1;
		}

		test_uls(input_file);
		uls_destroy(sample_lex);
	}

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
