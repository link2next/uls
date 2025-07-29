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
#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"

#include <stdlib.h>
#include <string.h>

#include "gcc_lex.h"

LPCTSTR progname;
int  opt_verbose;

int   test_mode = -1;
LPCTSTR config_name;
LPCTSTR input_file;

uls_lex_ptr_t gcc_lex;

static LPTSTR lbuff;
static int lbuff_siz;

char *ult_skip_blanks(const char *lptr);
wchar_t *ult_skip_blanks_wstr(const wchar_t *lptr);
char *ult_skip_blanks_astr(const char *lptr);

char* ult_split_litstr(char *str, char qch);
wchar_t* ult_split_litstr_wstr(wchar_t *wstr, wchar_t qch);
char* ult_split_litstr_astr(char *astr, char qch);

char* ult_splitstr(char** p_str, int *p_len);
wchar_t* ult_splitstr_wstr(wchar_t **p_wstr, int *p_wlen);
char* ult_splitstr_astr(char **p_astr, int *p_alen);

#if defined(ULS_USE_WSTR)
#define ult_skip_blanks ult_skip_blanks_wstr
#define ult_split_litstr ult_split_litstr_wstr
#define ult_splitstr ult_splitstr_wstr
#elif defined(ULS_USE_ASTR)
#define ult_skip_blanks ult_skip_blanks_astr
#define ult_split_litstr ult_split_litstr_astr
#define ult_splitstr ult_splitstr_astr
#endif

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
proc_file_coord(LPCTSTR buf)
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
	wrd = ult_splitstr(&lptr, NULL);
	lno = uls_str2int(wrd);

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
	uls_lex_ptr_t uls = gcc_lex;
	LPCTSTR tokstr;
	FILE   *fp;
	int t, stat = 0;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(_T(" file open error"));
		return -1;
	}

	uls_push_isrc(uls, (void *) fp,
		uls_fill_FILE_source, uls_ungrab_FILE_source);

	uls_set_tag(uls, fpath, -1);

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
			proc_file_coord(tokstr);
			continue;
		}

		uls_printf(_T("%s:%3d"), uls_get_tag(uls), uls_get_lineno(uls));
		uls_dump_tok(uls, _T(" "), _T("\n"));
	}

	return stat;
}

int
test_gnu_c(LPTSTR *args, int n_args)
{
	LPCTSTR fpath;
	int i, stat=0;

	if (n_args < 1) {
		err_log(_T("%hs: invalid parameter!"), __func__);
		return -1;
	}

	for (i=0; i<n_args; i++) {
		fpath = args[i];

		if (test_uls_isrc(fpath) < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
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

	progname = uls_filename(targv[0], NULL);
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
	case 2:
		lex_input_file(targv[i0]);
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

char*
ult_skip_blanks(const char *lptr)
{
	char ch;

	for ( ; (ch=*lptr)==' ' || ch=='\t'; lptr++)
		/* nothing */;
	return (char *) lptr;
}

wchar_t*
ult_skip_blanks_wstr(const wchar_t *lptr)
{
	wchar_t wch;

	for ( ; (wch=*lptr)==L' ' || wch==L'\t'; lptr++)
		/* nothing */;
	return (wchar_t *) lptr;
}

char*
ult_skip_blanks_astr(const char *lptr)
{
	return ult_skip_blanks(lptr);
}

char*
ult_split_litstr(char *str, char qch)
{
	char   *ptr, *ptr1;
	char   ch, esc_ch=0;

	for (ptr1=ptr=str; ; ptr++) {
		if (!uls_isprint(ch=*ptr)) { // ctrl-ch or '\0'
			if (ch != '\0') ++ptr;
			break;
		}

		if (esc_ch) {
			*ptr1++ = ch;
			esc_ch = 0;
		} else if (ch == '\\') {
			esc_ch = 1;
		} else if (ch == qch) {
			++ptr;
			break;
		} else {
			*ptr1++ = ch;
		}
	}

	*ptr1 = '\0';
	return ptr;
}

wchar_t*
ult_split_litstr_wstr(wchar_t *wstr, wchar_t qch)
{
	wchar_t   wch, *wptr, *wptr1;
	int esc_ch = 0;

	for (wptr1 = wptr = wstr; ; wptr++) {
		wch = *wptr;
		if (!uls_isprint(wch)) { // ctrl-ch or '\0'
			if (wch != '\0') ++wptr;
			break;
		}

		if (esc_ch) {
			*wptr1++ = wch;
			esc_ch = 0;
		} else if (wch == L'\\') {
			esc_ch = 1;
		} else if (wch == qch) {
			++wptr;
			break;
		} else {
			*wptr1++ = wch;
		}
	}

	*wptr1 = L'\0';
	return wptr;
}

char*
ult_split_litstr_astr(char *astr, char qch)
{
	return ult_split_litstr(astr, qch);
}

char*
ult_splitstr(char** p_str, int *p_len)
{
	char   *str = *p_str;
	char   ch, *ptr, *ptr0;
	int len;

	ptr0 = ptr = ult_skip_blanks_astr(str);

	for (len = 0; (ch=*ptr) != '\0'; ptr++) {
		if (ch == ' ' || ch == '\t') {
			len = (int) (ptr - ptr0);
			*ptr++ = '\0';
			break;
		}
	}

	if (p_len != NULL) *p_len = len;
	*p_str = ptr;
	return ptr0;
}

wchar_t*
ult_splitstr_wstr(wchar_t **p_wstr, int *p_wlen)
{
	wchar_t   *wstr = *p_wstr;
	wchar_t   wch, *ptr, *ptr0;
	int wlen;

	ptr0 = ptr = ult_skip_blanks_wstr(wstr);

	for (wlen = 0; (wch=*ptr) != L'\0'; ptr++) {
		if (wch == L' ' || wch == L'\t') {
			wlen = (int) (ptr - ptr0);
			*ptr++ = L'\0';
			break;
		}
	}

	if (p_wlen != NULL) *p_wlen = wlen;
	*p_wstr = ptr;
	return ptr0;
}

char*
ult_splitstr_astr(char **p_astr, int *p_alen)
{
	return ult_splitstr(p_astr, p_alen);
}
