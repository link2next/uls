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
  <file> getch.c </file>
  <brief>
    testing char-stream from uls input source.
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
	err_log(_T("usage: %s [-m <test-mode>] <input-file>"), progname);
	err_log(_T("\t%s input1.txt"), progname);
	err_log(_T("\t%s -m1 input1.txt"), progname);
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
test_get_ch(void)
{
	int tok_id, is_quote;
	uls_nextch_detail_t detail;
	uls_uch_t wch;

	uls_set_file(sample_lex, input_file, 0);
	// uls_set_line(sample_lex, "hello world", -1, 0);

	while (1) {
		is_quote = 0;
		if ((wch = uls_get_uch(sample_lex, &detail)) == ULS_UCH_NONE) {
			if (detail.tok == tokEOI || detail.tok == tokEOF) {
				break;
			}
			if (detail.qmt != NULL) {
				is_quote = 1;
			}
		}

		if (is_quote) {
			tok_id = uls_get_tok(sample_lex);
			uls_printf(_T(" str = '%s' (%d)\n"), uls_lexeme(sample_lex), tok_id);
		} else {
			uls_printf(_T(" ch = '%c'\n"), (char) wch);
		}
	}
	uls_dismiss_input(sample_lex);
}

void
test_want_eof(void)
{
	int tok_id;

	uls_set_line(sample_lex, _T("hello world"), -1, 0); // ULS_WANT_EOFTOK

	while (1) {
		tok_id = uls_get_tok(sample_lex);
		if (tok_id == tokEOI) break;

		uls_dumpln_tok(sample_lex);
	}

	uls_push_file(sample_lex, input_file, 0);
	uls_want_eof(sample_lex);

	while (1) {
		tok_id = uls_get_tok(sample_lex);
		if (tok_id == tokEOI) break;

		uls_dumpln_tok(sample_lex);
	}
}

static void
dump_next_tok(int linefeed)
{
	LPCTSTR suff = NULL;

	if (linefeed) suff = _T("\n");

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T("\t"), suff);
}

int
test_unget(void)
{
	void *xdef;
	int rc;

	err_log(_T("setting tok-info of tokID:%d ..."), tokID);
	if ((rc = uls_set_extra_tokdef(sample_lex, tokID, (void *) 0x777)) < 0) {
		err_log(_T("can't find the tok-info of tokID:%d"), tokID);
		return -1;
	}

	uls_set_line(sample_lex, _T("hello world"), -1, 0);

	/* 1 */
	dump_next_tok(0);
	xdef = uls_get_current_extra_tokdef(sample_lex);
	uls_printf(_T(" --- xdef[tokID] = 0x%X\n"), xdef);

	/* 2 */
	uls_unget_tok(sample_lex);
	dump_next_tok(0);
	xdef = uls_get_current_extra_tokdef(sample_lex);
	uls_printf(_T(" --- xdef[tokID] = 0x%X\n"), xdef);

	/* 3 */
	uls_skip_blanks(sample_lex);
	uls_unget_str(sample_lex, _T("hello"));
	dump_next_tok(1);

	/* 4 */
	uls_unget_str(sample_lex, _T("hello"));
	uls_unget_ch(sample_lex, _T('w'));
	dump_next_tok(1);

	/* 5 */
	uls_unget_lexeme(sample_lex, _T(".314"), tokNUM);
	dump_next_tok(1);

	/* 6 */
	uls_unget_lexeme(sample_lex, _T(" world hello "), tokDQUOTE);
	dump_next_tok(1);

	uls_dismiss_input(sample_lex);

	return 0;
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

	if (i0 < n_targv) {
		input_file = targv[i0];
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't create uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_get_ch();
		break;
	case 1:
		test_want_eof();
		break;
	case 2:
		test_unget();
		break;
	default:
		break;
	}

	uls_destroy(sample_lex);

	return 0;
}

int
main(int argc, char *argv[])
{
	LPTSTR *targv;
	int stat;

	ULS_GET_WARGS_LIST(argc, argv, targv);
	stat = _tmain(argc, targv);
	ULS_PUT_WARGS_LIST(argc, targv);

	return stat;
}
