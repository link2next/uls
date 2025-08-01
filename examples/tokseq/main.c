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
  <file> token_stream.c </file>
  <brief>
    After installing ULS libraries, build this program to check that ULS correctly works.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#include <uls.h>
#include <uls/uls_auw.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "tokseq_lex.h"

#define N_CMD_ARGS  16
#define N_TMPLVAL_ARRAY 8

LPCTSTR progname;
LPCTSTR input_file;

int  opt_verbose;

uls_lex_t *sample_lex;
uls_tmpl_list_t *tmpl_list;

int
print_uls_file(uls_lex_t *uls, LPCTSTR fpath, uls_ostream_t *ostr)
{
	int t, l_lxm, stat=0;
	LPCTSTR lxm;

	if (uls_push_file(uls, fpath, 0) < 0) {
		err_log(_T("%hs: fail to prepare input-stream %s."), __func__, fpath);
		return -1;
	}

	for ( ; ; ) {
		t = uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1; break;
		}

		if (uls_is_eof(uls) || uls_is_eoi(uls)) break;

		lxm = uls_lexeme(uls);

		if (uls_is_id(uls) && uls_get_tmpl_value(tmpl_list, lxm) != NULL) {
			t = uls_toknum_tmpl(uls);
		}

		for (l_lxm = 0; lxm[l_lxm] != _T('\0'); l_lxm++)
			/* NOTHING */;

		if (__uls_print_tok(ostr, t, lxm, l_lxm) < 0) {
			stat = -1; break;
		}
	}

	return stat;
}

int
read_uls_file(uls_lex_t *uls, LPCTSTR fpath)
{
	int stat=0;
	uls_istream_t *istr;
	LPCTSTR lxm;

	if ((istr = uls_open_istream_file(fpath)) == NULL) {
		err_log(_T("can't read %s"), fpath);
		return -1;
	}

	if (uls_push_istream(uls, istr, tmpl_list, 0) < 0) {
		err_log(_T("%hs: fail to prepare input-stream %s."), __func__, fpath);
		uls_destroy_istream(istr);
		return -1;
	}

	for ( ; ; ) {
		uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1; break;
		}

		if (uls_is_eoi(uls)) break;

		lxm = uls_lexeme(uls);

		uls_printf(_T("\t[%7t] %s\n"), uls, lxm);
	}

	uls_destroy_istream(istr);
	return stat;
}

int
run_test_scenario(LPCTSTR out_dir)
{
	TCHAR uls_fname[128];
	LPCTSTR tptr, tname;
	uls_ostream_t *ostr;
	int stat = 0;

	uls_snprintf(uls_fname, sizeof(uls_fname)/sizeof(TCHAR), _T("%s%c%s"), out_dir, ULS_FILEPATH_DELIM, _T("a1.uls"));

	// Step-1: Instantiate template vars <TVAR1,TVAR2>
	tname = _T("AVAR");
	tptr = _T("unsigned long long");
	uls_add_tmpl(tmpl_list, tname, tptr);

	tname = _T("BVAR");
	tptr = _T("long double");
	uls_add_tmpl(tmpl_list, tname, tptr);

	// Step-2: writing stream to output file 'a1.uls'
	if ((ostr = uls_create_ostream_file(uls_fname, sample_lex, _T("<tag_name>"))) == NULL) {
		err_log(_T("can't set uls-stream to %s"), uls_fname);
		return -1;
	}

	print_uls_file(sample_lex, input_file, ostr);
	uls_destroy_ostream(ostr);

	// Step-3: re-read the uls file "a1.uls" with the template vars.
	read_uls_file(sample_lex, uls_fname);

	return stat;
}

static void usage(void)
{
	err_log(_T("usage(%s): dumping the tokens defined as in 'simple.ulc'"), progname);
	err_log(_T("\t%s -c <config-file> <file1> ..."), progname);
}

static int
options(int opt, LPTSTR opttarg)
{
	int   stat = 0;

	switch (opt) {
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
__test_wstr_fprintf(FILE *fp)
{
	double x;
	int i, len;

	i = 2025;
	len = uls_fprintf(fp, _T("hello 0x%08X world\n"), i);
	uls_fprintf(fp, _T("len = %d\n"), len);

	i = 7;
	len = uls_fprintf(fp, _T("%+5d\n"), i);

	i = -9;
	len = uls_fprintf(fp, _T("%5d<hello>\n"), i);


	x = 0.0;
	len = uls_fprintf(fp, _T("%f -- %.3f -- %0.3f -- %#.3f\n"), x, x, x, x);

	x = 0.31;
	len = uls_fprintf(fp, _T("%f -- %.3f -- %0.3f -- %#.3f\n"), x, x, x, x);

	x = 31.158;
	len = uls_fprintf(fp, _T("%f -- %.2f -- %0.2f -- %#.2f\n"), x, x, x, x);

}

int
test_wstr_fprintf()
{
	int stat = 0;
	LPCTSTR out_file = _T("C:\\Temp\\wstr_prn_12.txt");
	FILE* fp;

	fp = uls_fp_open(out_file, ULS_FIO_WRITE);
	if (fp == NULL) {
		stat = -1;
	} else {
		__test_wstr_fprintf(fp);
		fclose(fp);
	}
	
	return stat;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	LPTSTR out_dir;
	int i0;

	progname = targv[0];
	tmpl_list = uls_create_tmpls(N_TMPLVAL_ARRAY, 0);

	// opt_verbose = 1;
	out_dir = _T(".");

	if ((i0=uls_getopts(n_targv, targv, _T("vhV"), options)) <= 0) {
		err_log(_T("Fail to parse command options."));
		uls_destroy_tmpls(tmpl_list);
		return i0;
	}

	if (i0 < n_targv) {
		input_file = targv[i0];
		if (uls_dirent_exist(input_file) != ST_MODE_FILE) {
			err_panic(_T("%s: not a file'"), input_file);
		}
	} else {
		input_file = _T("tokseq_sam.txt");
	}

	if (i0 + 1 < n_targv) {
		out_dir = targv[i0 + 1];
		if (uls_dirent_exist(out_dir) != ST_MODE_DIR) {
			err_panic(_T("%s: not a directory'"), out_dir);
		}
	}

	if ((sample_lex = uls_create_tokseq()) == NULL) {
//	if ((sample_lex = uls_create(_T("./tokseq.uld"))) == NULL) {
		err_log(_T("%hs: can't init uls-object!"), __func__);
		uls_destroy_tmpls(tmpl_list);
		return -1;
	}

	run_test_scenario(out_dir);

	uls_destroy_tokseq(sample_lex);
//	uls_destroy(sample_lex);
	uls_destroy_tmpls(tmpl_list);

	return 0;
}

#ifndef __ULS_WINDOWS__
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
#endif
