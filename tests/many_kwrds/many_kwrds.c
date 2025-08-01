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
  <file> many_keys.c </file>
  <brief>
    Test the case of many keywords in ULC-spec.
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
#include <string.h>

#include "sample_lex.h"

#define FILEBUFF_SIZ  4095

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;
LPCTSTR config_name;
LPCTSTR input_file;

uls_lex_ptr_t sample_lex;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Demonstrates the lexical scanner having many keywords"));
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
		test_mode = uls_str2int(optarg);
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

static FILE     *cur_fin;
static TCHAR   filebuff[FILEBUFF_SIZ+1];

int
proc_file(LPCTSTR fpath)
{
	int len, stat = 0, lno = 0;

	if ((cur_fin=uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T("%hs: fail to open '%s'"), __func__, fpath);
		return -1;
	}

	while (1) {
		if ((len=uls_fp_gets(cur_fin, filebuff, sizeof(filebuff)/sizeof(TCHAR), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log(_T("%hs: error to read a line"), __func__);
				stat =-1;
			}
			break;
		}

		uls_set_line(sample_lex, filebuff, uls_str_length(filebuff), 0);
		uls_set_lineno(sample_lex, ++lno);

		if (uls_get_tok(sample_lex) != TOK_EOI) {
			uls_dumpln_tok(sample_lex);
		}
	}

	uls_fp_close(cur_fin);

	return stat;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i, i0;

	progname = uls_filename(targv[0], NULL);
	config_name = _T("sample.ulc");
	input_file = _T("input1.txt");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	for (i=i0; i<n_targv; i++) {
		input_file = targv[i];
		proc_file(input_file);
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
