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
  <file> line.c </file>
  <brief>
    Testing input stream of which source is char array with length.
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

#define FILEBUFF_SIZ  4095

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;
LPCTSTR config_name;
LPTSTR input_file;
uls_lex_ptr_t sample_lex;

static FILE   *cur_fin;
static TCHAR   filebuff[FILEBUFF_SIZ+1];

int
proc_file(LPCTSTR filepath)
{
	int len, stat = 0, lno = 0;

	if ((cur_fin=uls_fp_open(filepath, ULS_FIO_READ)) == NULL) {
		err_log(_T("%hs: fail to open '%s'"), __func__, filepath);
		return -1;
	}

	while (1) {
		if ((len=uls_fp_gets(cur_fin, filebuff, sizeof(filebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log(_T("%hs: error to read a line"), __func__);
				stat =-1;
			}
			break;
		}
		filebuff[len++] = '\n'; filebuff[len] = '\0';

		uls_push_line(sample_lex, filebuff, ult_str_length(filebuff), ULS_DO_DUP);
		uls_set_lineno(sample_lex, ++lno);

		while (uls_get_tok(sample_lex) != tokEOI) {
			uls_dumpln_tok(sample_lex);
		}
	}

	uls_fp_close(cur_fin);

	return stat;
}

static void
dump_toks(void)
{
	while (uls_get_tok(sample_lex) != tokEOI) {
		uls_dumpln_tok(sample_lex);
	}
}

void
test_stage_1(void)
{
	TCHAR linebuff[81];
	int  len, i;

	ult_str_copy(linebuff, _T("hello ULS world"));
	len = ult_str_length(linebuff);

	for (i=0; i<len; i+=3) {
		uls_printf(_T("***** session-%d:\n"), i);
		uls_set_line(sample_lex, linebuff, i, 0);
		dump_toks();
	}

	linebuff[2] = _T('\0');
	uls_printf(_T("***** session-x:\n"));
	uls_set_line(sample_lex, linebuff, len, 0);
	dump_toks();
}

void
test_stage_3(void)
{
	TCHAR linebuff[81];

	ult_str_copy(linebuff, _T("hello ULS-world"));

	uls_set_line(sample_lex, linebuff, -1, 0);

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 1stToken:\n\t"), _T("\n"));

	uls_unget_str(sample_lex, _T("ungetc"));

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 2ndToken:\n\t"), _T("\n"));

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 3rdToken:\n\t"), _T("\n"));
}

void
test_stage_4(void)
{
	TCHAR linebuff[81];
	uls_wch_t wch;

	ult_str_copy(linebuff, _T("7094ULS-world"));

	uls_set_line(sample_lex, linebuff, -1, 0);

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 1stToken:\n\t"), _T("\n"));

	uls_unget_tok(sample_lex);

	wch = uls_getch(sample_lex, NULL);
	uls_printf(_T(" getch: '%c'\n"), (char) wch);

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 2ndToken:\n\t"), _T("\n"));

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 3rdToken:\n\t"), _T("\n"));
}

void
test_stage_5(void)
{
	TCHAR linebuff[81];

	ult_str_copy(linebuff, _T("hello ULS-world"));
	uls_set_line(sample_lex, linebuff, -1, 0);

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 1stToken:\n\t"), _T("\n"));

	uls_unget_lexeme(sample_lex, _T("ga"), tokNONE);
	uls_unget_lexeme(sample_lex, _T("gb gc"), tokID);

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 2ndToken:\n\t"), _T("\n"));

	uls_get_tok(sample_lex);
	uls_dump_tok(sample_lex, _T(" 3rdToken:\n\t"), _T("\n"));
}

void
test_uls_line(uls_lex_ptr_t uls)
{
	LPTSTR linebuff;

	/* 1 */
	uls_push_line(uls, _T("hello world"), -1, 0);

	for ( ; ; ) {
		if (uls_get_tok(uls) == tokEOI) break;

		uls_printf(_T("%3d"), uls_get_lineno(uls));
		uls_dumpln_tok(uls);
	}

	/* 2 */
	uls_push_line(uls, _T("hello world"), -1, ULS_DO_DUP);

	for ( ; ; ) {
		if (uls_get_tok(uls) == tokEOI) break;

		uls_printf(_T("%3d"), uls_get_lineno(uls));
		uls_dumpln_tok(uls);
	}

	/* 3 */
	linebuff = (LPTSTR) malloc(128 * sizeof(TCHAR));
	ult_str_copy(linebuff, _T("hello world"));
	uls_push_line(uls, linebuff, -1, ULS_MEMFREE_LINE);

	for ( ; ; ) {
		if (uls_get_tok(uls) == tokEOI) break;

		uls_printf(_T("%3d"), uls_get_lineno(uls));
		uls_dumpln_tok(uls);
	}
}

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps tokens in inputfiles"));
	err_log(_T("  according to the token defintions in %s"), config_name);
	err_log(_T("  Uses uls_set_line() to set the input source"));
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("\t%s [-c <config-file>]  <inputfile>"), progname);
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

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i, i0;

	progname = uls_split_filepath(targv[0], NULL);
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't create uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		for (i=i0; i<n_targv; i++) {
			input_file = targv[i];
			uls_set_tag(sample_lex, input_file, 1);
			proc_file(input_file);
		}
		break;

	case 1:
		test_stage_1();
		break;

	case 2:
		for (i=i0; i<n_targv; i++) {
			// shell style
			input_file = targv[i];
			uls_set_tag(sample_lex, input_file, 1);
			proc_file(input_file);
		}
		break;

	case 3:
		test_stage_3();
		break;

	case 4:
		test_stage_4();
		break;

	case 5:
		test_stage_5();
		break;

	case 6:
		test_uls_line(sample_lex);
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
