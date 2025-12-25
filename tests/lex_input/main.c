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
  <file> lex_input.c </file>
  <brief>
    Read from input file and just dump tokens.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/uls_lex.h"
#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"

#include <stdlib.h>
#include <string.h>

#include "sample_g_lex.h"

LPCTSTR progname;
int  opt_verbose;

int   test_mode = -1;
LPCTSTR config_name;
LPCTSTR input_file;

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
proc_file_coord(uls_lex_ptr_t uls, LPCTSTR buf)
{
	int lno, len, siz;
	LPTSTR lptr, wrd;

	if ( (len = uls_str_length(buf))>= lbuff_siz) {
		siz = len + 128;
		lbuff = (LPTSTR) uls_mrealloc(lbuff, siz * sizeof(TCHAR));
		lbuff_siz = siz;
	}
	// len  < buff_siz

	uls_memcopy(lbuff, buf, len * sizeof(TCHAR));
	lbuff[len] = _T('\0');
	lptr = lbuff;

	// line
	wrd = uls_str_splitstr(&lptr, NULL);
	lno = uls_str2int(wrd);

	// filename
	lptr = wrd = uls_str_skip_blanks(lptr);
	if (*wrd == _T('\0')) {
		wrd = NULL;
	} else {
		++wrd;
		lptr = uls_str_split_litstr(wrd, *lptr);
	}

	uls_set_tag(uls, wrd, lno - 1);
	return 1;
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
test_uls_isrc(uls_lex_ptr_t uls, LPCTSTR filepath)
{
	LPCTSTR tokstr;
	FILE   *fp;
	int t, stat = 0;

	if ((fp = uls_fp_open(filepath, ULS_FIO_READ)) == NULL) {
		err_log(_T(" file open error"));
		return -1;
	}

	uls_push_isrc(uls, (void *) fp,
		uls_fill_FILE_source, uls_ungrab_FILE_source);

	uls_set_tag(uls, filepath, -1);

	for ( ; (t = uls_get_tok(uls)) != TOK_EOI; ) {
		tokstr = uls_tokstr(uls);

		if (t == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), tokstr);
			stat = -1;
			break;
		}

		if (t == TOK_EOL) {
			continue;
		}

		if (t == TOK_WCOORD) {
			proc_file_coord(uls, tokstr);
			continue;
		}

		uls_printf(_T("%s:%3d"), uls_get_tag(uls), uls_get_lineno(uls));
		uls_dump_tok(uls, _T(" "), _T("\n"));
	}

	return stat;
}

int
test_gnu_c(uls_lex_ptr_t uls, LPTSTR *args, int n_args)
{
	LPCTSTR filepath;
	int i, stat=0;

	if (n_args < 1) {
		err_log(_T("%hs: invalid parameter!"), __func__);
		return -1;
	}

	for (i=0; i<n_args; i++) {
		filepath = args[i];

		if (test_uls_isrc(uls, filepath) < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
}

int
lex_input_file(uls_lex_ptr_t uls, LPCTSTR filepath)
{
	LPCTSTR tokstr;
	int t;

	if (uls_push_file(uls, filepath, 0) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	uls_set_tag(uls, filepath, -1);

	for ( ; ; ) {
		if ((t = uls_get_tok(uls)) == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log(_T("program abnormally terminated!"));
			}
			break;
		}

		if (t == TOK_EOL) {
			continue;
		}

		tokstr = uls_tokstr(uls);
		if (t == TOK_WCOORD) {
			proc_file_coord(uls, tokstr);
			continue;
		}

		uls_printf(_T("%s:%3d"), uls_get_tag(uls), uls_get_lineno(uls));
		uls_dump_tok(uls, _T(" "), _T("\n"));
	}

	return 0;
}

int
lex_input_line(uls_lex_ptr_t uls)
{
	LPCTSTR line = _T("// comment here!\nTo parse\ngcc pre-processed files");
	LPCTSTR tokstr, tagstr;
	int t;

	if (uls_push_line(uls, line, -1, ULS_WANT_EOFTOK) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	for ( ; ; ) {
		if ((t = uls_get_tok(uls)) == TOK_EOF || t == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log(_T("program abnormally terminated!"));
			}
			break;
		}

		tokstr = uls_tokstr(uls);
		if (t == TOK_WCOORD) {
			proc_file_coord(uls, tokstr);
			continue;
		}

		tagstr = uls_get_tag(uls);
		if (*tagstr != '\0') uls_printf(_T("%s:"), tagstr);
		uls_printf(_T("%3d"), uls_get_lineno(uls));

		if (t == TOK_EOL) {
			uls_printf(_T(" [    EOL]\n"));
		} else {
			uls_dump_tok(uls, _T(" "), _T("\n"));
		}
	}

	return 0;
}

int
lex_gcc_line(uls_lex_ptr_t uls, LPCTSTR line)
{
	int stat = 0;
	LPCTSTR tokstr;
	int t;

	if (uls_push_line(uls, line, -1, ULS_WANT_EOFTOK) < 0) {
		err_log(_T("can't set the istream!"));
		return -1;
	}

	for ( ; ; ) {
		if ((t = uls_get_tok(uls)) == TOK_EOF || t == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log(_T("program abnormally terminated!"));
				stat = -1;
			}
			break;
		}

		tokstr = uls_tokstr(uls);
		uls_printf(_T("\t%d : '%s'\n"), t, tokstr);
	}

	return stat;
}

int
lex_gcc_preprocd(uls_lex_ptr_t uls, LPCTSTR filepath)
{
	char linebuff[256];
	int linelen, lno = 0, stat = 0;
	FILE *fp;
	LPCTSTR line;
#ifdef ULS_USE_WSTR
	csz_str_t csz_wstr2;
	csz_init(uls_ptr(csz_wstr2), -1);
#endif

	if ((fp = uls_fp_open(filepath, ULS_FIO_READ)) == NULL) {
		err_log(_T(" file open error"));
		return -1;
	}

	while (1) {
		if (fgets(linebuff, sizeof(linebuff), fp) == NULL) {
			break;
		}

		++lno;
		if ((linelen = strlen(linebuff)) > 0 && linebuff[linelen-1] == '\n') {
			linebuff[--linelen] = '\0';
		}

		if (linebuff[0] == '#') continue;

#ifdef ULS_USE_WSTR
		line = uls_ustr2wstr(linebuff, linelen, uls_ptr(csz_wstr2));
		if (line == NULL) {
			err_log(_T("encoding error!"));
			stat = -1;
			break;
		}
#else
		line = linebuff;
#endif
		uls_printf(_T("#%d] '%s'\n"), lno, line);
		if (lex_gcc_line(uls, line) < 0) {
			err_log(_T("can't proc the istream!"));
			stat = -1;
			break;
		}
	}

	fclose(fp);
#ifdef ULS_USE_WSTR
	csz_deinit(uls_ptr(csz_wstr2));
#endif
	return stat;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	uls_lex_ptr_t gcc_lex;
	int i0;

	progname = uls_filename(targv[0], NULL);
	config_name = _T("sample_g.ulc");

	if ((i0 = uls_getopts(n_targv, targv, _T("m:vhz"), options)) <= 0) {
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
		test_gnu_c(gcc_lex, targv + i0, n_targv - i0);
		break;
	case 1:
		lex_input_line(gcc_lex);
		break;
	case 2:
		lex_input_file(gcc_lex, targv[i0]);
		break;
	case 3:
		lex_gcc_preprocd(gcc_lex, targv[i0]);
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
