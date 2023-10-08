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
  <file> gcc_toks.c </file>
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
#include <string.h>

#include "gcc_lex.h"

LPTSTR progname;
int  opt_verbose;

int   test_mode = -1;
LPCTSTR config_name;
LPCTSTR input_file;

uls_lex_t *gcc_lex;

static LPTSTR lbuff;
static int lbuff_siz;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps tokens in the inputfiles"));
	err_log(_T("  according to the token defintions of %s"), config_name);
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("  %s <target-dir> <inputfile ...>"), progname);
	err_log(_T(""));
	err_log(_T("  For example,"));
	err_log(_T("      %s kernel input1.txt"), progname);
	err_log(_T("  A default config-file used, %s if you don't specifiy the config-file."), config_name);
}

static int
options(int opt, LPTSTR optarg)
{
	int stat = 0;

	switch (opt) {
	case _T('m'):
		test_mode = ult_str2int(optarg);
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
proc_file_coord(LPCTSTR buf)
{
	int lno, len, siz;
	LPTSTR lptr, wrd;

	if ( (len = ult_str_length(buf))>= lbuff_siz) {
		siz = len + 128;
		lbuff = (LPTSTR) uls_mrealloc(lbuff, siz * sizeof(TCHAR));
		lbuff_siz = siz;
	}
	// len  < buff_siz

	uls_memcopy(lbuff, buf, len * sizeof(TCHAR));
	lbuff[len] = _T('\0');
	lptr = lbuff;

	// line
	wrd = ult_splitstr(&lptr, NULL);
	lno = ult_str2int(wrd);

	// filename
	lptr = wrd = ult_skip_blanks(lptr);
	if (*wrd == _T('\0')) {
		wrd = NULL;
	} else {
		++wrd;
		lptr = ult_split_litstr(wrd, *lptr);
	}

	uls_set_tag(gcc_lex, wrd, lno - 1);

	return 1;
}

int
lex_input_file(LPCTSTR fpath)
{
	LPCTSTR tokstr;
	int t;

	if (uls_push_file(gcc_lex, fpath, 0) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	uls_set_tag(gcc_lex, fpath, -1);

	for ( ; ; ) {
		if ((t = uls_get_tok(gcc_lex)) == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log(_T("program abnormally terminated!"));
			}
			break;
		}

		if (t == TOK_EOL) {
			continue;
		}

		tokstr = uls_tokstr(gcc_lex);
		if (t == TOK_WCOORD) {
			proc_file_coord(tokstr);
			continue;
		}

		uls_printf(_T("%s:%3d"), uls_get_tag(gcc_lex), uls_get_lineno(gcc_lex));
		uls_dump_tok(gcc_lex, _T(" "), _T("\n"));
	}

	return 0;
}

int
test_gnu_c(LPTSTR *args, int n_args)
{
	LPCTSTR fpath;
	int i, stat=0;

	if (n_args < 1) {
		err_log(_T("%s: invalid parameter!"), __func__);
		return -1;
	}

	for (i=0; i<n_args; i++) {
		fpath = args[i];

		if (lex_input_file(fpath) < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
}

int
lex_input_line()
{
	LPCTSTR tokstr, tagstr;
	int t;

	if (uls_push_line(gcc_lex, _T("// comment here!\nTo parse\ngcc pre-processed files"), -1, 0) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	for ( ; ; ) {
		if ((t=uls_get_tok(gcc_lex)) == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log(_T("program abnormally terminated!"));
			}
			break;
		}

		tokstr = uls_tokstr(gcc_lex);
		if (t == TOK_WCOORD) {
			proc_file_coord(tokstr);
			continue;
		}

		tagstr = uls_get_tag(gcc_lex);
		if (*tagstr != '\0') uls_printf(_T("%s:"), tagstr);
		uls_printf(_T("%3d"), uls_get_lineno(gcc_lex));

		if (t == TOK_EOL) {
			uls_printf(_T(" [    EOL]\n"));
		} else {
			uls_dump_tok(gcc_lex, _T(" "), _T("\n"));
		}
	}

	return 0;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i0;

	progname = uls_split_filepath(targv[0], NULL);
	config_name = _T("gcc.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("m:vhz"), options)) <= 0) {
		return i0;
	}

	if (i0 < n_targv) {
		input_file = targv[i0];
	}

	if ((gcc_lex=uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_gnu_c(targv + i0, n_targv - i0);
		break;
	case 1:
		lex_input_line();
		break;

	default:
		break;
	}

	uls_destroy(gcc_lex);

	uls_mfree(lbuff);

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
