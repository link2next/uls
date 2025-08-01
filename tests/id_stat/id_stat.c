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
  <file> id_stat.c </file>
  <brief>
    Read from input file and gather statistics of identifiers.
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

#include "sample_lex.h"

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;
LPCTSTR config_name;
LPCTSTR input_file;

LPCTSTR target_dir;
TCHAR home_dir[1024];
FILE *fp_list;
LPCTSTR filelist;

uls_lex_t sample_lex;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps the identifiers in inputfiles"));
	err_log(_T("  according to the token defintions in %s"), config_name);
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

	case _T('l'):
		filelist = optarg;
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

typedef struct _id_stat id_stat_t;
struct _id_stat {
	TCHAR name[64];
	int freq;
};

id_stat_t *id_stat_array;
int n_id_stat_array;
int n_alloc_id_stat_array;

id_stat_t*
find_id_stat(LPCTSTR name)
{
	id_stat_t *e;
	int i;

	for (i=0; i<n_id_stat_array; i++) {
		e = id_stat_array + i;
		if (uls_str_equal(e->name, name))
			return e;
	}

	return NULL;
}

id_stat_t*
append_id_stat(LPCTSTR name)
{
	id_stat_t *id_stat;

	if (n_id_stat_array >= n_alloc_id_stat_array) {
		n_alloc_id_stat_array += 128;
		id_stat_array = uls_mrealloc(id_stat_array, n_alloc_id_stat_array * sizeof(id_stat_t));
	}

	id_stat = id_stat_array + n_id_stat_array++;
	uls_str_copy(id_stat->name, name);
	id_stat->freq = 0;

	return id_stat;
}

int
proc_file(LPCTSTR fpath)
{
	id_stat_t *e;
	int tok;

	if (fpath == NULL) return -1;

	uls_set_file(&sample_lex, fpath, 0);

	for ( ; ; ) {
		tok = uls_get_tok(&sample_lex);

		if (tok == TOK_EOI) break;

		if (tok == TOK_ID) {
			if ((e=find_id_stat(uls_lexeme(&sample_lex))) == NULL) {
				e = append_id_stat(uls_lexeme(&sample_lex));
			}

			++e->freq;
		}
	}

	return 0;
}

int
test_id_stat(LPCTSTR fpath)
{
	int tok;

	if (fpath == NULL) return -1;

	uls_push_file(&sample_lex, fpath, 0);

	for ( ; ; ) {
		tok = uls_get_tok(&sample_lex);
		if (tok == TOK_EOI) break;

		uls_printf(_T("%3d: <%3d> "), uls_get_lineno(&sample_lex), tok);
		uls_dump_tok(&sample_lex, NULL, _T("\n"));
	}

	return 0;
}

int
proc_filelist(FILE *fin)
{
	TCHAR  ch, linebuff[1024];
	LPTSTR lptr, fpath;
	int   i, len, lno = 0, stat=0;

	while (1) {
		if ((len=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log(_T("%hs: error to read a line"), __func__);
				stat =-1;
			}
			break;
		}

		++lno;

		for (lptr = linebuff; (ch=*lptr) == _T(' ') || ch == _T('\t'); lptr++)
			/* nothing */;

		if (*lptr == _T('\0') || *lptr == _T('#')) continue;

		fpath = lptr;
		for (i = uls_str_length(fpath)-1; i>=0; i--) {
			if (!uls_isspace(fpath[i])) break;
		}
		fpath[++i] = _T('\0');

		if (proc_file(fpath) < 0) {
			err_log(_T("fail to process '%s'"), fpath);
			stat = -1;
			break;
		}
	}

	return stat;
}

static int
comp_by_idname(const void *a, const void *b)
{
	id_stat_t *a1 = (id_stat_t *) a;
	id_stat_t *b1 = (id_stat_t *) b;

	return uls_str_compare(a1->name, b1->name);
}

int
get_id_stats(int argc, LPTSTR argv[], int i0)
{
	id_stat_t *id_stat;
	int i;

	if (filelist != NULL ) {
		if (i0 < argc) {
			target_dir = argv[i0++];
		}

		if ((fp_list=uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
			err_log(_T("%hs: fail to read '%s'"), __func__, filelist);
			return -1;
		}

		if (uls_getcwd(home_dir, sizeof(home_dir)) < 0) {
			err_panic(_T("can't get the pwd!"));
		}

		if (target_dir != NULL) {
			if (uls_chdir(target_dir) < 0) {
				err_log(_T("can't change to %s"), target_dir);
				uls_fp_close(fp_list);
				return -1;
			}
		}

		proc_filelist(fp_list);
		uls_fp_close(fp_list);
		uls_chdir(home_dir);

	} else {
		for (i=i0; i<argc; i++) {
			proc_file(argv[i]);
		}
	}

	qsort(id_stat_array, n_id_stat_array, sizeof(id_stat_t), comp_by_idname);

	for (i=0; i<n_id_stat_array; i++) {
		id_stat = id_stat_array + i;
		uls_printf(_T("%24s %8d\n"), id_stat->name, id_stat->freq);
	}

	return 0;
}

int
test_initial_tok(uls_lex_ptr_t uls)
{
	int i, tok;

	for (i=0; i<3; i++) {
		tok = uls_get_tok(uls);
		uls_printf(_T("[%d] tok_id=%d, lexeme='%s'\n"), i, tok, uls_lexeme(uls));
	}

	return 0;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	int i0, i, rc;

	progname = uls_filename(targv[0], NULL);
	target_dir = NULL;
	config_name = _T("sample.ulc");
	input_file = _T("input1.txt");

	if ((i0=uls_getopts(n_targv, targv, _T("c:l:m:vh"), options)) <= 0) {
		return i0;
	}

	if (uls_init(&sample_lex, config_name) < 0) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		rc = get_id_stats(n_targv, targv, i0);
		break;
	case 1:
		for (i=i0; i<n_targv; i++) {
			rc = test_id_stat(targv[i]);
			if (rc < 0) break;
		}
		break;
	case 2:
		rc = test_initial_tok(&sample_lex);
		break;
	default:
		rc = 0;
		break;
	}

	uls_destroy(&sample_lex);
	uls_mfree(id_stat_array);

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
