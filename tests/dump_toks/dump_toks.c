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
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/uls_lex.h"
#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "sample_lex.h"

LPCTSTR progname;
int  opt_verbose;

LPCTSTR config_name;
LPCTSTR input_file;
int   test_mode = -1;

uls_lex_ptr_t sample_lex;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps tokens in the inputfiles"));
	err_log(_T("  according to the token defintions in %s"), config_name);
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("  %s [-c <config-file>] <inputfile>"), progname);
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
		config_name = uls_strdup(optarg, -1);
		break;

	case _T('m'):
		test_mode = uls_str2int(optarg);
		break;

	case _T('v'):
		++opt_verbose;
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
test_initial_state(LPCTSTR fpath)
{
	uls_lex_ptr_t uls = sample_lex;
	double f_val;
	uls_int64 ii_val;
	LPCTSTR tagstr;
	int t, taglen;

	if (uls_push_file(uls, fpath, ULS_WANT_EOFTOK) < 0) {
		err_log(_T("file open error"));
		return -1;
	}

	uls_printf(_T("initial-state: %3d : %3d  '%s'\n"),
		uls_get_lineno(uls), uls_tok(uls), uls_tokstr(uls));

	for ( ; ; ) {
		if ((t = uls_get_tok(uls)) == TOK_EOI) break;

		uls_printf(_T(" %3d : %3d  '%s'\n"),
			uls_get_lineno(uls), uls_tok(uls), uls_tokstr(uls));

		if (t == TOK_NUMBER) {
			if (uls_is_real(uls)) {
				f_val = uls_lexeme_double(uls);
				uls_printf(_T("\tf_val = %f\n"), f_val);
			} else {
				ii_val = uls_lexeme_lld(uls);
				uls_printf(_T("\ti_val = %lld\n"), ii_val);
			}
		} else if (t == TOK_EOF) {
			tagstr = uls_get_eoftag(uls, &taglen);
			uls_printf(_T("\tEOF : '%s'%d\n"), tagstr, taglen);
		}
	}

	uls_printf(_T("final-state: %3d : %3d  '%s'\n"),
		uls_get_lineno(uls), uls_tok(uls), uls_tokstr(uls));

	uls_get_tok(uls);
	uls_printf(_T("After EOI: %3d : %3d  '%s'\n"),
		uls_get_lineno(uls), uls_tok(uls), uls_tokstr(uls));

	uls_push_line(uls, _T("hello world"), -1, 0);
	for ( ; ; ) {
		if (uls_get_tok(uls) == TOK_EOI) break;
		uls_printf(_T(" %3d : %3d  '%s'\n"),
			uls_get_lineno(uls), uls_tok(uls), uls_tokstr(uls));
	}

	return 0;
}

int
test_uls(LPCTSTR fpath)
{
	uls_lex_ptr_t uls = sample_lex;
	int fd, t;

	if ((fd = uls_fd_open(fpath, ULS_FIO_READ)) < 0) {
		err_log(_T("%s: file open error"), fpath);
		return -1;
	}

	if (uls_set_fd(uls, fd, ULS_DO_DUP) < 0) {
		err_log(_T("Can't set the istream, %s."), fpath);
		return -1;
	}

	uls_set_tag(uls, fpath, 1);

	for ( ; ; ) {
		if ((t=uls_get_tok(uls)) == TOK_ERR) {
			err_log(_T("TOK_ERR!"));
			break;
		} else if (t == TOK_EOI) {
			break;
		}

		uls_printf(_T("%3d"), uls_get_lineno(uls));
		uls_dump_tok(uls, _T("\t"), _T("\n"));
	}

	close(fd);
	return 0;
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

static int
my_gettok(void)
{
	uls_lex_ptr_t uls = sample_lex;
	int tok_id;

	tok_id = uls_get_tok(uls);
	if (tok_id == '@') {
		uls_get_tok(uls);
		tok_id = TOK_USERDEF4;
		uls_set_tok(uls, tok_id, NULL, 0);
	}

	return tok_id;
}

void
test_uls_xdef(LPCTSTR fpath)
{
	uls_lex_ptr_t uls = sample_lex;
	sample_xdef_t *xdef;
	LPCTSTR ptr;
	TCHAR buff[64];
	int i;

	for (i=0; i<5; i++) {
		if (uls_set_extra_tokdef(uls, xdefs[i].tok_id, xdefs + i) < 0) {
			ptr = uls_tok2keyw(uls, xdefs[i].tok_id);
			if (ptr == NULL) ptr = _T("???");
			err_log(_T("fail to set x-tokdef for token %s."), ptr);
		}
	}

	if (uls_push_file(uls, fpath, 0) < 0) {
		err_log(_T(" file open error"));
		return;
	}

	for ( ; ; ) {
		if (my_gettok() == TOK_EOI) break;

		xdef = (sample_xdef_t *) uls_get_current_extra_tokdef(uls);

		uls_printf(_T("%3d"), uls_get_lineno(uls));

		if (xdef != NULL) {
			uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T(" prec=%d node_id=%d\n"),
				xdef->prec, xdef->node_id);
		} else {
			buff[0] = _T('\n'); buff[1] = _T('\0');
		}

		uls_dump_tok(uls, _T("\t"), buff);
	}
}

int
uls_fill_FILE_source(uls_source_t* isrc, char *buf, int buflen, int bufsiz)
{
	static unsigned char utf8buf[ULS_UTF8_CH_MAXLEN];
	static int len_utf8buf;

	int buflen0 = buflen;
	FILE  *fp = (FILE *) isrc->usrc;
	int i, j, rc, n_bytes;

	if (len_utf8buf > 0) {
		for (j = 0 ; j < len_utf8buf; j++) {
			buf[buflen + j] = utf8buf[j];
		}
	}

	if ((buflen += len_utf8buf) >= bufsiz) {
		return buflen - buflen0;
	}

	if ((n_bytes = fread(buf + buflen, sizeof(char), bufsiz - buflen, fp)) <= 0) {
		if (n_bytes < 0 || ferror(fp)) {
			n_bytes = -1;
		} else if (feof(fp)) {
			isrc->flags |= ULS_ISRC_FL_EOF;
		}
		return n_bytes;
	}

	for (i = buflen; i < n_bytes; i += rc) {
		if ((rc = uls_decode_utf8(buf + i, n_bytes - i, NULL)) <= 0) {
			if (rc < -ULS_UTF8_CH_MAXLEN || feof(fp)) {
				return -2;
			}
			len_utf8buf = n_bytes - i;
			for (j = 0; j < len_utf8buf; j++) {
				utf8buf[j] = buf[i + j];
			}
			break;
		}
	}

	return i - buflen0;
}

static void
uls_ungrab_FILE_source(uls_source_t* isrc)
{
	FILE  *fp = (FILE *) isrc->usrc;
	uls_fp_close(fp);
}

int
test_uls_isrc(LPCTSTR fpath)
{
	uls_lex_ptr_t uls = sample_lex;
	LPCTSTR tokstr;
	FILE   *fp;
	int t, stat = 0;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T(" file open error"));
		return -1;
	}

	uls_push_isrc(uls, (void *) fp,
		uls_fill_FILE_source, uls_ungrab_FILE_source);

	uls_set_tag(uls, fpath, 1);

	for ( ; (t = uls_get_tok(uls)) != TOK_EOI; ) {
		tokstr = uls_tokstr(uls);
		if (t == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), tokstr);
			stat = -1;
			break;
		}

		uls_printf(_T("#%d(%6d) :"), uls_get_lineno(uls), t);
		uls_dump_utf8str(tokstr);
		uls_printf(_T("\n"));
	}

	return stat;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i0;

	progname = targv[0];
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:vm:hz"), options)) <= 0) {
		return i0;
	}

	if (i0 < n_targv) {
		input_file = targv[i0];
	}

	if ((sample_lex=uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_uls(input_file);
		break;
	case 1:
		test_initial_state(input_file);
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
