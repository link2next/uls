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
  <file> logging.c </file>
  <brief>
    Test the logging APIs in ULS-LF.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#define ULS_DECL_PROTECTED_PROC
#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_init.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sample_lex.h"

#define N_THREADS   8
#define N_CMD_ARGS  16

LPCTSTR progname;
int  test_mode = -1;
int  opt_verbose;

LPCTSTR config_name;
uls_lex_t *sample_lex;
uls_log_t *sample_log;

static void usage(void)
{
	err_log(_T("%s v1.1"), progname);
	err_log(_T("  Demonstrates the use of ULS-LF, the ULS Logging Framework."));
	err_log(_T(" Just type"));
	err_log(_T("  %s input1.txt"), progname);
}

static int
options(int opt, LPTSTR optarg)
{
	int stat = 0;

	switch (opt) {
	case 'a':
		uls_sysprn(_T("-a-option: %s(TSTR)\n"), optarg);
		break;
	case 'm':
		test_mode = atoi((char *) optarg);
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'h':
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

static void
dump_tstr(LPCTSTR tstr, int len)
{
	TCHAR numbuf[32], tch;
	int i, m, n, n_digits;

	for (n_digits=0, n=len; n!=0; n/=10) {
		m = n % 10;
		numbuf[n_digits++] = _T('0') + m;
	}

	for (i=0; i<n_digits/2; i++) {
		m = n_digits - 1 - i;
		tch = numbuf[i];
		numbuf[i] = numbuf[m];
		numbuf[m] = tch;
	}

	if (n_digits <= 0) {
		numbuf[n_digits++] = _T('0');
	}
	numbuf[n_digits] = _T('\0');

	uls_putstr(numbuf);
	uls_putstr(_T(" -- '"));
	uls_putstr(tstr);
	uls_putstr(_T("'\n"));
}

void
test_sprintf(uls_lex_t* uls, uls_log_t* log)
{
	TCHAR buff[128];
	int len, i, ui;
	long long ll;
	unsigned long long ull;
	int tok;
	LPCTSTR str = _T("hello");
	double x;
	long double xx;

	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T(""));
	dump_tstr(buff, len);

	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("'%s' '%8s' %% '%-8s'"), str, str, str);
	dump_tstr(buff, len);

	buff[0] = 'X'; buff[1] = 'Y'; buff[2] = 'Z';
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%c he-%c-llo %c"),
		buff[0], buff[1], buff[2]);
	dump_tstr(buff, len);

	i = 2008;
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("hello 0x%08X world"), i);
	dump_tstr(buff, len);

	i = 7;
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%+5d"), i);
	dump_tstr(buff, len);

	i = -9;
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%5d<hello>"), i);
	dump_tstr(buff, len);

	ui = 7;
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%-3u"), ui);
	dump_tstr(buff, len);

	ui = 2008;
	ll = -35;
	tok = uls_get_tok(uls);
	uls_log(log, _T("buff = '%lld', tok=%d:'%s'"), ll, tok, uls_lexeme(uls));

	tok = uls_get_tok(uls);
	ull = (unsigned long long) ll;
	uls_log(log, _T("buff = '%llu+%u', tok=%d:'%s'"), ull, ui, tok, uls_lexeme(uls));

	i = 2008;
	tok = uls_get_tok(uls);
	uls_log(log, _T("buff = '%-12d, tok=%d:'%s'"), i, tok, uls_lexeme(uls));

	x = 314159.2653589;
	// use %f for printing 'double'
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%.10f %.10e %.10g"), x, x, x);

	dump_tstr(buff, len);

	xx = -0.00031415926535897;
	len = uls_snprintf(buff, sizeof(buff)/sizeof(TCHAR), _T("%.10lf %.10le %.10lg"), xx, xx, xx);

	dump_tstr(buff, len);
}

void
test_log_primitive(void)
{
	char buff[128];
	char *str = "hello";
	unsigned int ui;
	int i;

	err_log_primitive("'%s'", str);
	err_log_primitive("");

	buff[0] = 'Z'; buff[1] = 'Y'; buff[2] = 'X';
	err_log_primitive("%c he-%c-llo %c", buff[0], buff[1], buff[2]);

	i = 2017;
	err_log_primitive("hello 0x%X world", i);
	err_log_primitive("i = %d", i);

	ui = 7;
	err_log_primitive("%u", ui);

	i = -8;
	err_log_primitive("%d<hello>%u", i, i);
}

void sample_lex_log(LPCTSTR fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	uls_vlog(sample_log, fmt, args);
	va_end(args);
}

void
test_log(uls_lex_t *uls, uls_log_t *log)
{
	unsigned int ui;
	long long ll;
	int i;
	int tok;

	i = -3;
	ui = 3;
	err_log(_T("mesg = '%+5d' '%-3u'"), i, ui);

	tok = uls_get_tok(uls);
	uls_printf(_T("stdout: <%w>: %t %k\n"), uls, uls, uls);
	err_log(_T("err<%w>: %t %k"), uls, uls, uls);

	i = 2011;
	tok = uls_get_tok(uls);
	uls_log(log, _T("buff = '%-12d, tok=%d:'%s'"), i, tok, uls_lexeme(uls));

	ui = 2015;
	ll = -35;
	tok = uls_get_tok(uls);
	uls_log(log, _T("buff = '%lld', tok=%d(%-5k) '%8t' at '%-12w' $"), ll, tok);

	uls_log(log, _T("buff = '%-12d, tok=%d:'%s'"), i, tok, uls_lexeme(uls));
	uls_log(log, _T("buff = '%lld', tok=%d: %t at %w"), ll, tok);

	tok = uls_get_tok(uls);
	sample_lex_log(_T("buff = '%lld+%u', tok=%d: at %w"), ll, ui, tok);
}

void *thr_task(void *param)
{
	long tid = (long) param;
	int   i;

	for (i = 0; i < 100; i++) {
		if (i % 2 == 0)
			uls_log(sample_log, _T("<tid=%ld> says hello #%d"), tid, i);
		else
			err_log(_T("\t<tid=%ld> says hello #%d"), tid, i);
	}

	return NULL;
}

void
test_log_threads(void)
{
	pthread_attr_t	thread_attr;
	pthread_t tid[N_THREADS];
	int i, n_thr_ary = N_THREADS;

	pthread_attr_init(&thread_attr);

	for (i=0; i<n_thr_ary; i++) {
		pthread_create(&tid[i], &thread_attr, thr_task, (void *) (long) i);
	}
	pthread_attr_destroy(&thread_attr);

	for (i=0; i<n_thr_ary; i++) {
		pthread_join(tid[i], NULL);
	}
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	LPTSTR input_file;
	int i0;

	progname = targv[0];
	config_name = _T("sample.ulc");

	if ((i0=uls_getopts(n_targv, targv, _T("a:m:vh"), options)) <= 0) {
		return i0;
	}

	if (i0 >= n_targv) {
		err_log(_T("need an input file"));
		return -1;
	}

	input_file = targv[i0];

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log(_T("can't create uls-object"));
		usage();
		return -1;
	}

	if ((sample_log = uls_create_log(NULL, sample_lex)) == NULL) {
		uls_destroy(sample_lex);
		return -1;
	}

	if (uls_set_file(sample_lex, input_file, 0) < 0) {
		err_log(_T("can't find input file or error!"));
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_sprintf(sample_lex, sample_log);
		break;
	case 1:
		test_log(sample_lex, sample_log);
		break;
	case 2:
		test_log_threads();
		break;
	case 3:
		test_log_primitive();
		break;
	default:
		break;
	}

	uls_destroy_log(sample_log);

	uls_dismiss_input(sample_lex);
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
