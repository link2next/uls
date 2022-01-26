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
  <file> dump_toks.c </file>
  <brief>
    Read from input file and just dump tokens.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2015.
  </author>
*/

#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_fileio.h"
#include "uls/uls_util.h"
#include "uls/uls_init.h"
#ifdef ULS_USE_WSTR
#include "uls/ulscompat.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "sample_lex.h"

#define N_CMD_ARGS  16

LPCTSTR progname;
int  opt_verbose;

LPCTSTR config_name;
LPTSTR input_file;
int   test_mode = -1;

uls_lex_t *sample_lex;

static void usage(void)
{
	err_wlog(_T("%s v1.0"), progname);
	err_wlog(_T("  Dumps tokens in the inputfiles"));
	err_wlog(_T("  according to the token defintions in %s"), config_name);
	err_wlog(_T(""));
	err_wlog(_T(" Usage:"));
	err_wlog(_T("  %s [-c <config-file>] <inputfile>"), progname);
	err_wlog(_T(""));
	err_wlog(_T("  For example,"));
	err_wlog(_T("      %s input1.txt"), progname);
	err_wlog(_T("  A default config-file used, %s if you don't specifiy the config-file."), config_name);
}

static int
options(int opt, LPTSTR optwarg)
{
	int stat = 0;

	switch (opt) {
	case 'c':
		config_name = uls_wstrdup(optwarg, -1);
		break;

	case 'm':
		test_mode = atoi((char *) optwarg);
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'h':
		usage();
		stat = 1;
		break;

	default:
		err_wlog(_T("undefined option -%c"), opt);
		usage();
		stat = -1;
		break;
	}

	return stat;
}

void
test_initial_uls(LPTSTR fpath)
{
	double f_val;
	int i_val;

	if (uls_push_file_wstr(sample_lex, fpath, 0) < 0) {
		err_wlog(_T(" file open error"));
		return;
	}

	for ( ; ; ) {
		if (uls_get_wtok(sample_lex) == TOK_EOI) break;
		uls_wprintf(_T(" %3d : %3d  '%s'\n"),
			uls_get_lineno(sample_lex), uls_tok(sample_lex), uls_lexeme_wstr(sample_lex));

		if (uls_tok(sample_lex) == TOK_NUMBER) {
			if (uls_is_real(sample_lex)) {
				f_val = uls_lexeme_double(sample_lex);
				uls_wprintf(_T("\tf_val = %f\n"), f_val);
			} else {
				i_val = uls_lexeme_d(sample_lex);
				uls_wprintf(_T("\ti_val = %d\n"), i_val);
			}
		}
	}
}

void
test_uls(LPTSTR fpath)
{
	if (uls_set_file_wstr(sample_lex, fpath, ULS_DO_DUP) < 0) {
		err_wlog(_T("can't set the istream!"));
		return;
	}

	uls_set_tag_wstr(sample_lex, fpath, 1);
	for ( ; ; ) {
		if (uls_get_wtok(sample_lex) == TOK_EOI) break;

		uls_wprintf(_T("%3d"), uls_get_lineno(sample_lex));
		uls_dump_tok_wstr(sample_lex, _T("\t"), _T("\n"));
	}
}

typedef struct {
	int tok_id;
	int prec;
	int node_id;
	void (*node_op)(void* a, void* b);
} sample_xdef_t;

void mul_nodetree(void* a, void* b) {}
void add_nodetree(void* a, void* b) {}
void div_nodetree(void* a, void* b) {}
void and_nodetree(void* a, void* b) {}
void xor_nodetree(void* a, void* b) {}

sample_xdef_t xdefs[5] = {
	{ '*', 27, 1, mul_nodetree },
	{ '+', 26, 2, add_nodetree },
	{ '/', 25, 3, div_nodetree },
	{ '&', 24, 4, and_nodetree },
	{ '^', 23, 5, xor_nodetree }
};

void
test_uls_xdef(LPTSTR fpath)
{
	sample_xdef_t *xdef;
	LPCTSTR ptr;
	int i;

	for (i=0; i<5; i++) {
		if (uls_set_extra_tokdef(sample_lex, xdefs[i].tok_id, xdefs + i) < 0) {
			ptr = uls_tok2keyw_wstr(sample_lex, xdefs[i].tok_id);
			if (ptr == NULL) ptr = _T("???");
			err_wlog(_T("fail to set x-tokdef for tok %s."), ptr);
		}
	}

	if (uls_push_file_wstr(sample_lex, fpath, 0) < 0) {
		err_wlog(_T(" file open error"));
		return;
	}

	for ( ; ; ) {
		if (uls_get_wtok(sample_lex) == TOK_EOI) break;

		xdef = (sample_xdef_t *) uls_get_current_extra_tokdef(sample_lex);

		uls_wprintf(_T("%3d\t"), uls_get_lineno(sample_lex));

		uls_dump_tok_wstr(sample_lex, NULL, NULL);
		if (xdef != NULL) {
			uls_wprintf(_T(" prec=%d node_id=%d"), xdef->prec, xdef->node_id);
		}

		uls_wprintf(_T("\n"));
	}
}

int
uls_fill_FILE_source(uls_source_t* isrc, char* buf, int buflen, int bufsiz)
{
	FILE  *fp = (FILE *) isrc->usrc;
	return fread(buf + buflen, sizeof(char), bufsiz - buflen, fp);
}

static void
uls_ungrab_FILE_source(uls_source_t* isrc)
{
	FILE  *fp = (FILE *) isrc->usrc;
	uls_fp_close(fp);
}

void
test_uls_isrc(LPTSTR fpath)
{
	FILE   *fp;

	if ((fp=uls_fp_wopen(fpath, ULS_FIO_READ)) == NULL) {
		err_wlog(_T(" file open error"));
		return;
	}

	uls_push_isrc(sample_lex, (void *) fp,
		uls_fill_FILE_source, uls_ungrab_FILE_source);

	uls_set_tag_wstr(sample_lex, fpath, 1);

	for ( ; ; ) {
		if (uls_get_wtok(sample_lex) == TOK_EOI) break;

		uls_wprintf(_T("%3d"), uls_get_lineno(sample_lex));
		uls_dump_tok_wstr(sample_lex, _T("\t"), _T("\n"));
	}
}

int
main(int argc, char *argv[])
{
	LPTSTR *targv;
	int i0;

#ifdef ULS_USE_WSTR
	initialize_ulscompat();
#else
	initialize_uls();
#endif

	ULS_GET_WARGS_LIST(argc, argv, targv);

	progname = targv[0];
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts_wstr(argc, targv, _T("c:vm:hz"), options)) <= 0 || i0 >= argc) {
		return i0;
	}

	input_file = targv[i0];

	if ((sample_lex=uls_create_wstr(config_name)) == NULL) {
		err_wlog(_T("can't init uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_uls(input_file);
		break;
	case 1:
		test_initial_uls(input_file);
		break;
	case 2:
		test_uls_xdef(input_file);
		break;
	case 3:
		test_uls_isrc(input_file);
		break;
	default:
		break;
	}

	uls_destroy(sample_lex);
	ULS_PUT_WARGS_LIST(argc, targv);

	return 0;
}
