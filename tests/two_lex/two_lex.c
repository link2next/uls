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
  <file> two_lex.c </file>
  <brief>
    Read from 2 instances of uls simultaenously.
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

#include "sam1_lex.h"
#include "sam2_lex.h"

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;

uls_lex_ptr_t sam1_lex, sam2_lex;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  ULS can instantiate many uls lexical objects."));
	err_log(_T("  '%s' demonstrates this feature of ULS"), progname);
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("  %s <inputfile> ..."), progname);
	err_log(_T(""));
	err_log(_T("  For example,"));
	err_log(_T("      %s input1.txt"), progname);
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
test_uls_2(LPCTSTR fpath)
{
	int i;

	if (uls_push_file(sam2_lex, fpath, 0) < 0) {
		err_log(_T(" file open error"));
		return;
	}

	for (i=0; ; ) {
		if (uls_get_tok(sam2_lex) == T2_EOI) break;

		uls_printf(_T("\t\t%3d"), uls_get_lineno(sam2_lex));
		uls_dump_tok(sam2_lex, NULL, _T("\n"));

		++i;
	}
}

int
test_uls(LPCTSTR fpath)
{
	int fd;
	int i;

	if ((fd = uls_fd_open(fpath, O_RDONLY)) < 0) {
		err_log(_T(" file open error"));
		return -1;
	}

	if (uls_push_fd(sam1_lex, fd, 0) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	uls_set_tag(sam1_lex, fpath, 1);

	for (i=0; ; ) {
		if (uls_get_tok(sam1_lex) == T1_EOI) break;

		uls_printf(_T("\t%3d "), uls_get_lineno(sam1_lex));
		uls_dumpln_tok(sam1_lex);

		if (uls_tok(sam1_lex) == T1_ID && ult_str_equal(uls_lexeme(sam1_lex), _T("WWW"))) {
			test_uls_2(_T("input2.txt"));
		}

		++i;
	}

	close(fd);
	return 0;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	LPCTSTR conf_name1 = _T("sam1.ulc");
	LPCTSTR conf_name2 = _T("sam2.ulc");
	LPCTSTR input_file;
	int  i, i0;

	progname = uls_split_filepath(targv[0], NULL);

	if ((i0=uls_getopts(n_targv, targv, _T("m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sam1_lex=uls_create(conf_name1)) == NULL) {
		err_log(_T("can't init uls-object"));
		return 1;
	}

	if ((sam2_lex=uls_create(conf_name2)) == NULL) {
		err_log(_T("can't init uls-object"));
		uls_destroy(sam1_lex);
		return 1;
	}

	for (i=i0; i<n_targv; i++) {
		input_file = targv[i];
		test_uls(input_file);
	}

	uls_destroy(sam1_lex);
	uls_destroy(sam2_lex);

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
