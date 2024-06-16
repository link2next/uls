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
test_get_ch(uls_lex_ptr_t uls)
{
	int tok_id, is_quote;
	uls_nextch_detail_t detail;
	uls_wch_t wch;

	uls_set_file(uls, input_file, 0);
	// uls_set_line(uls, "hello world", -1, 0);

	while (1) {
		is_quote = 0;
		if ((wch = uls_get_uch(uls, &detail)) == ULS_UCH_NONE) {
			if (detail.tok == tokEOI || detail.tok == tokEOF) {
				break;
			}
			if (detail.qmt != NULL) {
				is_quote = 1;
			}
		}

		if (is_quote) {
			tok_id = uls_get_tok(uls);
			uls_printf(_T(" str = '%s' (%d)\n"), uls_lexeme(uls), tok_id);
		} else {
			uls_printf(_T(" ch = '%c'\n"), (char) wch);
		}
	}
	uls_pop_all(uls);
}

void
test_want_eof(uls_lex_ptr_t uls)
{
	int tok_id;

	uls_set_line(uls, _T("hello world"), -1, 0); // ULS_WANT_EOFTOK

	while (1) {
		tok_id = uls_get_tok(uls);
		if (tok_id == tokEOI) break;

		uls_dumpln_tok(uls);
	}

	uls_push_file(uls, input_file, 0);
	uls_want_eof(uls);

	while (1) {
		tok_id = uls_get_tok(uls);
		if (tok_id == tokEOI) break;

		uls_dumpln_tok(uls);
	}
}

static void
dump_next_tok(uls_lex_ptr_t uls, int linefeed)
{
	LPCTSTR suff = NULL;

	if (linefeed) suff = _T("\n");

	uls_get_tok(uls);
	uls_dump_tok(uls, _T("\t"), suff);
}

int
test_unget(uls_lex_ptr_t uls)
{
	void *xdef;
	int rc;

	err_log(_T("setting tok-info of tokID:%d ..."), tokID);
	if ((rc = uls_set_extra_tokdef(uls, tokID, (void *) 0x777)) < 0) {
		err_log(_T("can't find the tok-info of tokID:%d"), tokID);
		return -1;
	}

	uls_set_line(uls, _T("hello world"), -1, 0);

	/* 1 */
	dump_next_tok(uls, 0);
	xdef = uls_get_current_extra_tokdef(uls);
	uls_printf(_T(" --- xdef[tokID] = 0x%X\n"), xdef);

	/* 2 */
	uls_unget_tok(uls);
	dump_next_tok(uls, 0);
	xdef = uls_get_current_extra_tokdef(uls);
	uls_printf(_T(" --- xdef[tokID] = 0x%X\n"), xdef);

	/* 3 */
	uls_skip_blanks(uls);
	uls_unget_str(uls, _T("hello"));
	dump_next_tok(uls, 1);

	/* 4 */
	uls_unget_str(uls, _T("hello"));
	uls_unget_ch(uls, _T('w'));
	dump_next_tok(uls, 1);

	/* 5 */
	uls_unget_lexeme(uls, _T(".314"), tokNUM);
	dump_next_tok(uls, 1);

	/* 6 */
	uls_unget_lexeme(uls, _T(" world hello "), tokDQUOTE);
	dump_next_tok(uls, 1);

	return 0;
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

	if (i0 < n_targv) {
		input_file = targv[i0];
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't create uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_get_ch(sample_lex);
		break;
	case 1:
		test_want_eof(sample_lex);
		break;
	case 2:
		test_unget(sample_lex);
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
