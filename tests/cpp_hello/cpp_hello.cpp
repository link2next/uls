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
  <file> cpp_hello.cpp </file>
  <brief>
    testing the ULS-API in C++.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include <uls/UlsLex.h>
#include <uls/UlsIStream.h>
#include <uls/UlsOStream.h>
#include <uls/UlsUtils.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "Sample2Lex.h"

#ifdef __ULS_WINDOWS__
#define MSG_STR_10 _T(" -> Hello Hangul !")
#define FMT_STR_11 _T("Hangul Hello %d!\n")
#define PTH_STR_12 _T("D:\\Temp\\ansi_Hangul_file.txt")
#define TAG_STR_13 _T("Hangul")
#define LOG_STR_14 _T("Hello Hangul!")
#define PTH_FMT_15 _T("C:\\Temp\\res_ansi_%d.txt")
#define PTH_FMT_16 _T("C:\\Temp\\res_ansi_sysprn_%d.txt")
#else
#define MSG_STR_10 _T(" -> Hello Hangul !")
#define FMT_STR_11 _T("Hangul Hello %d!\n")
#define PTH_STR_12 _T("/tmp/ansi_ms_file.txt")
#define TAG_STR_13 _T("tag1")
#define LOG_STR_14 _T("Hello world!")
#define PTH_FMT_15 _T("/tmp/res_utf8_%d.txt")
#define PTH_FMT_16 _T("/tmp/res_utf8_sysprn_%d.txt")
#endif

void ult_putstr(const char *austr, FILE *fp = NULL);
void ult_putstr(const wchar_t *wstr, FILE *fp = NULL);

void ult_printf(const char* fmt, ...);
void ult_printf(const wchar_t *wfmt, ...);

void ult_log(const char* fmt, ...);
void ult_log(const wchar_t *wfmt, ...);

using namespace std;
using namespace uls::crux;

using tstring = uls::tstring;
using namespace AAA::BBB;

class sample_xdef {
public:
	int tok_id;
	int prec;
	int node_id;

	sample_xdef() {
		tok_id = 0;
		prec = 1;
		node_id = -1;
	}

	void set(int t, int p, int n)
	{
		tok_id = t;
		prec = p;
		node_id = n;
	}
};

LPCTSTR progname;
tstring out_dir = _T(".");
int  opt_verbose;
int  test_mode;

sample_xdef *xdefs_list;

static void
usage(void)
{
	ult_log(_T("usage: %s [-m<0|1>] <input-file>"), progname);
}

static int
cpphello_options(int opt, LPTSTR optarg)
{
	int   stat = 0;

	switch (opt) {
	case _T('m'):
		test_mode = uls::strToInt(optarg);
		break;

	case _T('v'):
		opt_verbose = 1;
		break;

	default:
		ult_log(_T("undefined option -%c"), opt);
		stat = -1;
		break;
	}

	return stat;
}

void
test_input_string(Sample2Lex *sam_lex)
{
	sam_lex->pushLine(_T("C++ hello world\n\t\n"));

	while (sam_lex->getTok() != Sample2Lex::EOI) {
		sam_lex->dumpTok();
	}
}

void
test_input_file(Sample2Lex *sam_lex, tstring& filename)
{
	tstring lxm;
	string numsuff;

	sam_lex->printf(_T("Welcome to ULS World!\n"));
	sam_lex->printf(_T("Testing ULS in C++ ...\n"));

	sam_lex->pushFile(filename);
	sam_lex->setTag(filename);

	while (sam_lex->getTok() != Sample2Lex::EOI) {
		sam_lex->getTokStr(lxm);
		sam_lex->printf(_T("\t[%8t] %10s"), lxm.c_str());

		numsuff = sam_lex->lexemeNumberSuffix();
		if (!numsuff.empty()) {
			sam_lex->printf(_T(" %hs"), numsuff.c_str());
		}

		sam_lex->printf(_T(" at %w\n"));
	}
}

void init_xdefs(void)
{
	sample_xdef *xdef;

	xdef = &xdefs_list[0];
	xdef->set('*', 27, 1);

	xdef = &xdefs_list[1];
	xdef->set('+', 26, 2);

	xdef = &xdefs_list[2];
	xdef->set('/', 25, 3);

	xdef = &xdefs_list[3];
	xdef->set('&', 24, 4);

	xdef = &xdefs_list[4];
	xdef->set('^', 23, 5);
}

void test_extra_tokdefs(UlsLex *lex)
{
	sample_xdef *xdef;
	int i, t;

	xdefs_list = new sample_xdef[5];
	init_xdefs();

	for (i = 0; i < 5; i++) {
		xdef = xdefs_list + i;
		ult_printf(_T("\txdefs[%d] = %d\n"), i, xdef->tok_id);
		lex->setExtraTokdef(xdef->tok_id, xdef);
	}

	tstring line = _T("C++ *hello/ &world^\n\t\n");
	lex->pushLine(line.c_str());

	while ((t=lex->getTok()) != lex->toknum_EOI) {
		lex->dumpTok(_T("\t"), _T(""));

		xdef = (sample_xdef *) lex->getExtraTokdef();
		if (xdef != NULL)
			lex->printf(_T(" prec=%d node_id=%d"), xdef->prec, xdef->node_id);
		lex->printf(_T("\n"));
	}

	delete [] xdefs_list;
}

bool test_simple_streaming(tstring& input_file, tstring& out_dir)
{
	UlsLex *lex;
	tstring uls_file;
	tstring lxm;
	int t;

	try {
		lex = new UlsLex(_T("c++"));
	} catch (const invalid_argument& ex) {
		ult_log(_T("%s: Failed to create UlsLex!"), progname);
		throw ex;
	}

	uls_file = out_dir;
	uls_file += ULS_FILEPATH_DELIM;
	uls_file += _T("a2.uls");

	UlsOStream *ofile = new UlsOStream(uls_file, lex, _T("<<tag>>"));
	UlsIStream *ifile = new UlsIStream(input_file);

	ofile->startStream(*ifile);

	delete ifile;
	delete ofile;

	// Read the written input-stream previously.
	ifile = new UlsIStream(uls_file, NULL);

	lex->pushInput(*ifile);

	while (1) {
		t = lex->getTok();
		if (t == lex->toknum_EOI) break;

		lex->getTokStr(lxm);
		lex->printf(_T("\t[%7t]  %s\n"), lxm.c_str());
	}

	delete ifile;
	delete lex;

	return true;
}

bool test_tokseq(UlsLex *lex, tstring& input_file, tstring& out_dir)
{
	UlsTmplList  tmpl_list;
	tstring uls_file;
	tstring lxm;
	int t, stat=0;

	uls_file = out_dir;
	uls_file += ULS_FILEPATH_DELIM;
	uls_file += _T("a3.uls");

	tmpl_list.setValue(_T("TVAR1"), _T("unsigned long long"));
	tmpl_list.setValue(_T("TVAR2"), _T("long double"));

	// Write a output-stream
	UlsOStream *ofile = new UlsOStream(uls_file, lex, _T("<<tag>>"));
	lex->pushFile(input_file);

	while (1) {
		t = lex->getTok();

		if (t == lex->toknum_ERR) {
			stat = -1; break;
		}

		if (t == lex->toknum_EOF || t == lex->toknum_EOI) {
			break;
		}

		lex->getTokStr(lxm);
		if (t == lex->toknum_ID && tmpl_list.exist(lxm) == true) {
			t = lex->toknum_TMPL;
		}

		ofile->printTok(t, lxm);
	}

	delete ofile;
	if (stat < 0) return false;

	// Read the written input-stream previously.
	UlsIStream *ifile = new UlsIStream(uls_file, &tmpl_list);

	lex->pushInput(*ifile);

	while (1) {
		t = lex->getTok();
		if (t == lex->toknum_EOI) break;

		lex->getTokStr(lxm);
		lex->printf(_T("\t[%7t] %s\n"), lxm.c_str());
	}

	delete ifile;

	return true;
}

int
cpp_test_printf(UlsLex *lex, int seqno)
{
	TCHAR tbuf[128];
	TCHAR filebuff[128];
	int len = 123, stat = 0;
	FILE* fp;

	lex->snprintf(tbuf, sizeof(tbuf)/sizeof(TCHAR), MSG_STR_10);
	ult_printf(_T("buf = '"));
	ult_putstr(tbuf);
	ult_printf(_T("'\n"));

	lex->snprintf(filebuff, sizeof(filebuff)/sizeof(TCHAR), PTH_FMT_15, seqno);
	ult_printf(_T("Writing to "));
	ult_putstr(filebuff);
	ult_printf(_T(" ...\n"));

	fp = uls::fileOpenWriteolnly(filebuff);
	if (fp) {
		len = lex->fprintf(fp, FMT_STR_11, seqno);
		ult_printf(_T("buf --> len(chars) = %d\n"), len);
		fclose(fp);
	}
	else {
		stat = -1;
	}

	return stat;
}

int
cpp_test_sysprn(UlsLex *lex, int seqno)
{
	TCHAR filebuff[128];
	int i;

	lex->snprintf(filebuff, sizeof(filebuff)/sizeof(TCHAR), PTH_FMT_16, seqno);
	ult_printf(_T("Writing to "));
	ult_putstr(filebuff);
	ult_printf(_T(" ...\n"));

	lex->openOutput(filebuff);

	for (i = 0; i < 5; i++) {
		lex->print(_T("-> %d .. %s..\n"), i, filebuff);
	}

	lex->closeOutput();
	return 0;
}

int
cpp_test_logging(UlsLex *lex, int n)
{
	lex->setTag(TAG_STR_13);
	for (int i = 0; i < n; i++) {
		lex->log(LOG_STR_14);
	}
	return 0;
}

int
cpp_test_pushfile(UlsLex& lex, int lex_no, tstring& input_file)
{
	int tok_eoi;

	if (uls::direntExist(input_file.c_str()) != ST_MODE_FILE) {
		return 0;
	}

	if (lex_no == 1) tok_eoi = lex.toknum_EOI;
	else if (lex_no == 2) tok_eoi = Sample2Lex::EOI;
	else return -1;

	if (lex.pushFile(input_file) == false) {
		ult_log(_T("Can't open the input file!\n"));
		return -1;
	}

	while (lex.getTok() != tok_eoi) {
		lex.dumpTok(_T("\t"), _T("\n"));
	}

	return -1;
}

void
cpp_test_auw_pushfile(UlsLex *lex1, UlsLex *lex2, tstring& input_file)
{
	cpp_test_pushfile(*lex1, 1, input_file);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));
	cpp_test_pushfile(*lex2, 2, input_file);
}

int
cpp_test_tagstr(UlsLex *lex)
{
	tstring tagstr;
	lex->getTag(tagstr);
	ult_printf(_T("1. getTag = '"));
	ult_putstr(tagstr.c_str());
	ult_printf(_T("'\n"));

	tagstr = TAG_STR_13;
	lex->setTag(tagstr);

	lex->getTag(tagstr);
	ult_printf(_T("2. getTag = '"));
	ult_putstr(tagstr.c_str());
	ult_printf(_T("'\n"));

	return 0;
}

int
cpp_test_settok(UlsLex *lex)
{
	tstring tokstr;

	ult_printf(_T("0. tokid = %d, tokstr = '"), lex->getTokNum());
	ult_putstr(tokstr.c_str());
	ult_printf(_T("'\n"));

	lex->getTok();

	lex->getTokStr(tokstr);
	ult_printf(_T("1. tokid = %d, tokstr = '"), lex->getTokNum());
	ult_putstr(tokstr.c_str());
	ult_printf(_T("'\n"));

	tokstr = TAG_STR_13;
	lex->setTok(lex->toknum_ID, tokstr);

	lex->getTokStr(tokstr);
	ult_printf(_T("2. tokid = %d, tokstr = '"), lex->getTokNum());
	ult_putstr(tokstr.c_str());
	ult_printf(_T("'\n"));

	return 0;
}

int
cpp_test_dumptok(UlsLex *lex)
{
	lex->pushLine(_T("C++ hello world\n\t\n"));

	lex->getTok();
	lex->dumpTok(_T("\t^^"), _T("$$\n"));

	lex->getTok();
	lex->dumpTok(_T("\t^^"), _T("$$\n"));

	return 0;
}

void
test_awstr_api(UlsLex *lex1, int lex_no, int file_idx, int n)
{
	// snprintf, printf
	cpp_test_printf(lex1, file_idx);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// sysprn
	cpp_test_sysprn(lex1, file_idx);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// logging
	cpp_test_logging(lex1, n);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// pushFile
	tstring input_file = PTH_STR_12;
	cpp_test_pushfile(*lex1, lex_no, input_file);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// tagstr
	cpp_test_tagstr(lex1);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// settok
	cpp_test_settok(lex1);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));

	// dumptok
	cpp_test_dumptok(lex1);
	ult_printf(_T("\n--------------------------------------------------------------\n\n"));
}

int
_tmain(int n_targv, LPTSTR *targv)
{
	UlsLex *sample1_lex;
	Sample2Lex *sample2_lex;
	tstring *input_fpath = NULL;
	int i0, stat = 0;

	progname = uls::getFilename(targv[0], NULL);
	test_mode = 0;

	if ((i0 = uls::parseCommandOptions(n_targv, targv, _T("m:v"), cpphello_options)) <= 0) {
		usage();
		return -1;
	}

	if (i0 < n_targv) {
		input_fpath = new tstring(targv[i0]);
	}

	sample1_lex = new UlsLex(_T("dumptoks.ulc"));
	sample2_lex = new Sample2Lex();

	switch (test_mode) {
	case 0:
		test_input_string(sample2_lex);
		break;

	case 1:
		if (input_fpath != NULL) {
			test_input_file(sample2_lex, *input_fpath);
		}
		break;

	case 2:
		if (input_fpath != NULL) {
			cpp_test_auw_pushfile(sample1_lex, sample2_lex, *input_fpath);
		} else {
			ult_printf(_T("+) =============================================================\n"));
			test_awstr_api(sample1_lex, 1, 10, 3);
			ult_printf(_T("   =============================================================\n"));
			test_awstr_api(sample2_lex, 2, 11, 2);
			ult_printf(_T("-) =============================================================\n"));
		}
		break;

	case 3:
		test_extra_tokdefs(sample1_lex);
		break;

	case 4:
		if (input_fpath != NULL) {
			test_simple_streaming(*input_fpath, out_dir);
		}
		break;

	case 5:
		if (input_fpath != NULL) {
			test_tokseq(sample1_lex, *input_fpath, out_dir);
		}
		break;

	default:
		break;
	}

	delete sample1_lex;
	delete sample2_lex;

	return stat;
}

#ifndef __ULS_WINDOWS__
int
main(int argc, char *argv[])
{
	uls::ArgListT targlst;
	LPTSTR *targv;
	int stat;

	ULSCPP_GET_WARGS_LIST(targlst, argc, argv, targv);
	stat = _tmain(argc, targv);
	ULSCPP_PUT_WARGS_LIST(targlst);

	return stat;
}
#endif

void
ult_putstr(const char *austr, FILE *fp)
{
	if (fp == NULL) fp = stdout;
	fprintf(fp, "%s", austr);
}

void
ult_putstr(const wchar_t *wstr, FILE *fp)
{
	csz_str_t csz;
	const char *austr;

	csz_init(uls_ptr(csz), -1);
#ifdef __ULS_WINDOWS__
	austr = uls_wstr2astr(wstr, -1, uls_ptr(csz));
#else
	austr = uls_wstr2ustr(wstr, -1, uls_ptr(csz));
#endif
	if (austr != NULL) {
		ult_putstr(austr, fp);
	}

	csz_deinit(uls_ptr(csz));
}

static void
ult_vfprintf(FILE *fp, const char *fmt, va_list args)
{
	char buf[512];
	int len;

	len = vsnprintf(buf, sizeof(buf), fmt, args);
	if (len > 0) fputs(buf, fp);
}

void
ult_printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	ult_vfprintf(stdout, fmt, args);
	va_end(args);
}

void
ult_log(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	ult_vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
}

static void
ult_vfwprintf(FILE *fp, const wchar_t *wfmt, va_list args)
{
	wchar_t wbuf[512];
	int wlen;

	wlen = vswprintf(wbuf, sizeof(wbuf) / sizeof(wchar_t), wfmt, args);
	if (wlen > 0) ult_putstr(wbuf, fp);
}

void
ult_printf(const wchar_t *wfmt, ...)
{
	va_list args;

	va_start(args, wfmt);
	ult_vfwprintf(stdout, wfmt, args);
	va_end(args);
}

void
ult_log(const wchar_t *wfmt, ...)
{
	va_list args;

	va_start(args, wfmt);
	ult_vfwprintf(stderr, wfmt, args);
	va_end(args);
	fputc('\n', stderr);
}
