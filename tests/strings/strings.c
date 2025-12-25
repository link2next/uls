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
  <file> strings.c </file>
  <brief>
    Extract the literal strings from input-file.
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
#ifndef __ULS_WINDOWS__
#include <locale.h>
#endif

#include "sample_lex.h"

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;

LPCTSTR config_name;
LPTSTR input_file;

static void usage(void)
{
	err_log(_T("%s v1.0"), progname);
	err_log(_T("  Dumps all the literal strings in inputfiles"));
	err_log(_T("  according to the token defintions in %s"), config_name);
	err_log(_T(""));
	err_log(_T(" Usage:"));
	err_log(_T("\t%s [-c <config-file>]  <inputfile> ..."), progname);
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

int
test_escchar_map(uls_lex_ptr_t uls, LPCTSTR fpath)
{
	int i, rc, len, tok, stat = 0;
	unsigned char ch, ch1, ch2;
	LPCTSTR lxm, qstr_type;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ | ULS_FIO_NO_UTF8BOM)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return -1;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return -1;
	}

	for ( ; (tok = uls_get_tok(uls)) != TOK_EOI; ) {
		lxm = uls_lexeme(uls);
		if (tok == TOK_ERR) {
			err_log(_T("%s: error-token detected in %s"), lxm, fpath);
			stat = -1;
			break;
		}

		if (tok == TOK_CS_QUOTE) {
			qstr_type = _T("RAW");
		} else if (tok == TOK_SQUOTE) {
			qstr_type = _T("SQ");
		} else if (tok == TOK_DQUOTE) {
			qstr_type = _T("DQ");
		} else if (tok == TOK_MQUOTE) {
			qstr_type = _T("MQ");
		} else if (tok == TOK_NQUOTE) {
			qstr_type = _T("NQ");
		} else {
			qstr_type = NULL;
		}

		if (qstr_type != NULL) {
			len = uls_lexeme_len(uls);
			uls_printf(_T("#%-2d: %3s(%2d)"), uls_get_lineno(uls), qstr_type, len);

			for (i = 0; i < len; i += rc) {
				ch = (unsigned char) lxm[i];
				ch1 = ch2 = '\0';
				if (i + 1 < len) ch1 = (unsigned char) lxm[i + 1];
				if (i + 2 < len) ch2 = (unsigned char) lxm[i + 2];

				if (ch == '\\') {
					rc = 1;
				} else if (ch == 0xE2 && ch1 == 0x82 && ch2 == 0xAC) { // EuroSign
					rc = 3;
				} else {
					rc = 0;
				}

				if (rc > 0) {
					uls_printf(_T("  -"));
				} else {
					uls_printf(_T(" %02X"), ch);
					rc = 1;
				}
			}
			uls_printf(_T("\n"));
		}
	}

	uls_fp_close(fp);
	return stat;
}

int
test_escaped_eol_1(uls_lex_ptr_t uls, LPCTSTR fpath)
{
	int i, len, tok, stat = 0;
	LPCTSTR lxm;
	unsigned char ch;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ | ULS_FIO_NO_UTF8BOM)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return -1;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return -1;
	}

	for ( ; (tok = uls_get_tok(uls)) != TOK_EOI; ) {
		lxm = uls_lexeme(uls);
		len = uls_lexeme_len(uls);

		if (tok == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), lxm);
			stat = -1;
			break;
		}

		if (tok == TOK_NQUOTE) {
			uls_printf(_T(" #%-02d(NQ) : \""), uls_get_lineno(uls));
			for (i = 0; i < len; i++) {
				ch = (unsigned char) lxm[i];
				if (uls_isprint(ch)) {
					uls_printf(_T("%c"), ch);
				} else {
					uls_printf(_T("(%02X)"), ch);
				}
			}
			uls_printf(_T("\"\n"));
		}
	}

	uls_fp_close(fp);
	return stat;
}

int
test_uls_2(uls_lex_ptr_t uls, LPTSTR fpath)
{
	int tok, stat = 0;
	LPCTSTR lxm;
	FILE *fp;

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ | ULS_FIO_NO_UTF8BOM)) == NULL) {
		err_log(_T("can't open the file '%s'"), fpath);
		return -1;
	}

	if (uls_push_fp(uls, fp, 0) < 0) {
		err_log(_T("can't set the input '%s' to uls"), fpath);
		return -1;
	}

	for ( ; (tok = uls_get_tok(uls)) != TOK_EOI; ) {
		lxm = uls_lexeme(uls);
		if (tok == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), lxm);
			stat = -1;
			break;
		}

		if (tok == TOK_SQUOTE) {
			uls_printf(_T("\t'%s'\n"), lxm);
		} else if (tok == '"') {
			uls_printf(_T("\t\"%s\"\n"), lxm);
		} else if (tok == TOK_CS_QUOTE) {
			uls_printf(_T("\t\"\"\"%s\"\"\"\n"), lxm);
		}
	}

	uls_fp_close(fp);
	return stat;
}

int
test_uls_3(uls_lex_ptr_t uls, LPCTSTR fpath)
{
	int fd, tok, stat = 0;
	LPCTSTR lxm;

	if ((fd = uls_fd_open(fpath, ULS_FIO_READ)) < 0) {
		err_log(_T("%s: file open error"), fpath);
		return -1;
	}

	if (uls_set_fd(uls, fd, ULS_DO_DUP) < 0) {
		err_log(_T("Can't set the istream, %s."), fpath);
		return -1;
	}

	uls_set_tag(uls, fpath, 1);

	for ( ; (tok = uls_get_tok(uls)) != TOK_EOI; ) {
		lxm = uls_lexeme(uls);
		if (tok == TOK_ERR) {
			err_log(_T("ErrorToken: %s"), lxm);
			stat = -1;
			break;
		}

		uls_printf(_T("%3d"), uls_get_lineno(uls));
		uls_dump_tok(uls, _T("\t"), _T("\n"));
	}

	close(fd);
	return stat;
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	uls_lex_ptr_t sample_lex;
	int i0;

	progname = uls_filename(targv[0], NULL);
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("c:m:vh"), options)) <= 0) {
		return i0;
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't init uls-object"));
		return -1;
	}

	input_file = targv[i0];

	switch (test_mode) {
	case 0:
		test_escchar_map(sample_lex, input_file);
		break;
	case 1:
		test_escaped_eol_1(sample_lex, input_file);
		break;
	case 2:
		test_uls_2(sample_lex, input_file);
		break;
	case 3:
		test_uls_3(sample_lex, input_file);
		break;
	default:
		break;
	}

	uls_destroy(sample_lex);
	return 0;
}

#ifndef ULS_WINDOWS
static int
set_uls_locale(void)
{
	const char *cptr0, *cptr;
	char lang_entry[16], lang_buff[16];
	const char *locale_list[] = { "", "C", "en_US", NULL };
	const char *encoding_suffs[] = { "UTF-8", "utf8", NULL };
	int i, j, len, bReset = 0;

	if ((cptr0 = getenv("LANG")) != NULL) {
		if ((cptr = strchr(cptr0, '.')) != NULL &&
			(len=(int)(cptr-cptr0)) > 0 && len < 8) {
			for (j=0; j<len; j++) lang_entry[j] = cptr0[j];
			lang_entry[len] = '\0';
			locale_list[0] = lang_entry;
		}
	}

	for (i = 0; (cptr0 = locale_list[i]) != NULL; i++) {
		if ((len = strlen(cptr0)) <= 0) continue;
		strcpy(lang_buff, cptr0);

		for (j=0; (cptr = encoding_suffs[j]) != NULL; j++) {
			lang_buff[len] = '.';
			strcpy(lang_buff+len+1, cptr);
			if (setlocale(LC_ALL, lang_buff) != NULL) {
				bReset = 1;
				break;
			}
		}
		if (bReset > 0) break;
	}

	return bReset;
}
#endif

int
main(int argc, char *argv[])
{
	LPTSTR *targv;
	int stat;
#ifndef __ULS_WINDOWS__
	set_uls_locale();
#endif
	ULS_GET_WARGS_LIST(argc, argv, targv);
	stat = _tmain(argc, targv);
	ULS_PUT_WARGS_LIST(argc, targv);

	return stat;
}
