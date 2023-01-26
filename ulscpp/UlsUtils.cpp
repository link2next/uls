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
  <file> UlsUtils.cpp </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2018.
  </author>
*/

#include "uls/UlsUtils.h"
#include <uls/uls_auw.h>
#include <uls/uls_fileio.h>
#include <uls/uls_log.h>

#include <string>
#include <stdlib.h>
#include <memory.h>

using namespace std;
using namespace uls::crux;
//
//
// UlsAuw
//
//
UlsAuw::UlsAuw(int size)
{
	if (size < 0) {
		size = ULSCPP_NUM_CSZ_BUFFS;
	}

	auwstr_buf = (csz_str_t *) malloc(size * sizeof(csz_str_t));
	siz_auwstr_buf = size;

	for (int i = 0; i < siz_auwstr_buf; i++) {
		csz_init(auwstr_buf + i, ULSCPP_DFL_CSZ_BUFFSIZE);
	}
}

// <brief>
// The destructor of UlsLex.
// </brief>
// <return>none</return>
UlsAuw::~UlsAuw()
{
	int i;

	for (i = 0; i < siz_auwstr_buf; i++) {
		csz_deinit(auwstr_buf + i);
	}

	free(auwstr_buf);
}

char *
UlsAuw::wstr2mbstr(const wchar_t *wstr, int mode, int slot_no)
{
	char *mbstr2;

	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	if (mode == CVT_MBSTR_USTR) {
		mbstr2 = uls_wstr2ustr(wstr, -1, auwstr_buf + slot_no);
	} else {
		mbstr2 = NULL;
	}

	return mbstr2;
}

wchar_t *
UlsAuw::mbstr2wstr(const char *mbstr, int mode, int slot_no)
{
	wchar_t *wstr2;

	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	if (mode == CVT_MBSTR_USTR) {
		wstr2 = uls_ustr2wstr(mbstr, -1, auwstr_buf + slot_no);
	} else {
		wstr2 = NULL;
	}

	return wstr2;
}

int
UlsAuw::get_slot_len(int slot_no)
{
	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	return auwstr_buf[slot_no].len;
}

// <brief>
// This is a constructor of ArgListW
// </brief>
ArgListW::ArgListW()
{
	wargs = NULL;
	n_wargs = 0;
}

// <brief>
// The destructor of ArgListW
// </brief>
ArgListW::~ArgListW()
{
	reset();
}

// <brief>
// convert the list of string to wide strings.
// </brief>
// <parm name="args">list of string</parm>
// <parm name="n_args"># of strings in the list</parm>
// <return>list of wide-char string</return>
bool
ArgListW::setWArgList(char **args, int n_args)
{
	reset();

	wchar_t *wstr;
	int wlen;

	if (n_args <= 0) {
		return false;
	}

	UlsAuw *auw_converter = new UlsAuw(n_args);

	n_wargs = n_args;
	wargs = (wchar_t **) uls_malloc(n_args * sizeof(wchar_t *));

	for (int i=0; i < n_args; i++) {
		wstr = auw_converter->mbstr2wstr(args[i], UlsAuw::CVT_MBSTR_USTR, i);
		wlen = auw_converter->get_slot_len(i) / sizeof(wchar_t);
		wargs[i] = (wchar_t *) uls_malloc((wlen + 1) * sizeof(wchar_t));
		uls_memcopy(wargs[i], wstr, wlen * sizeof(wchar_t));
		wargs[i][wlen] = L'\0';
	}

	delete auw_converter;
	return true;
}

wchar_t*
ArgListW::getWArg(int i)
{
	if (i >= n_wargs) {
		return NULL;
	}

	return wargs[i];
}

wchar_t **
ArgListW::exportWArgs(int *ptr_n_args)
{
	wchar_t **wlist = wargs;

	if (ptr_n_args != NULL) {
		*ptr_n_args = n_wargs;
	}

	wargs = NULL;
	n_wargs = 0;

	return wlist;
}

void
ArgListW::reset()
{
	int i;

	for (i=0; i < n_wargs; i++) {
		uls_mfree(wargs[i]);
	}

	uls_mfree(wargs);
}

void
uls::memcopy(void *dst, const void *src, int n)
{
	memmove(dst, src, n);
}

int
uls::strLength(const char *str)
{
	const char *ptr;

	for (ptr=str; *ptr != '\0'; ptr++)
		/* NOTHING */;

	return (int) (ptr - str);
}

int
uls::strLength(const wchar_t *wstr)
{
	const wchar_t *ptr;

	for (ptr=wstr; *ptr != L'\0'; ptr++)
		/* NOTHING */;

	return (int) (ptr - wstr);
}

int
uls::strFindIndex(const char *line, char ch0)
{
	int i;

	if (ch0 >= 0x80) return -1;

	for (i=0; line[i] != '\0'; i++) {
		if (line[i] == ch0) return i;
	}

	if (ch0 == '\0') return i;
	return -1;
}

int
uls::strFindIndex(const wchar_t* wline, wchar_t wch0)
{
	wchar_t wch;
	int i;

	for (i=0; (wch=wline[i]) != L'\0'; i++) {
		if (wch == wch0) return i;
	}

	if (wch0 == L'\0') return i;
	return-1;
}

int
uls::direntExist(const char *fpath)
{
	return uls_dirent_exist(fpath);
}

int
uls::direntExist(const wchar_t *wfpath)
{
	csz_str_t csz;
	char *ustr;
	int  rval;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		rval = -1;
	} else {
		rval = uls_dirent_exist(ustr);
	}

	csz_deinit(uls_ptr(csz));
	return rval;
}

FILE*
uls::fileOpenReadolnly(const char *fpath)
{
	return fopen(fpath, "r");
}

FILE*
uls::fileOpenReadolnly(const wchar_t *wfpath)
{
	csz_str_t csz;
	char *ustr;
	FILE *fp;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		fp = NULL;
	} else {
		fp = fileOpenReadolnly(ustr);
	}

	csz_deinit(uls_ptr(csz));
	return fp;
}


wchar_t**
uls::getWargList(char **argv, int n_argv)
{
	wchar_t **wargv;

	uls_warg_list_t wlist;

	uls_init_warg_list(&wlist);
	uls_set_warg_list(&wlist, argv, n_argv);

	wargv = uls_export_warg_list(&wlist, NULL);
	uls_deinit_warg_list(&wlist);

	return wargv;

}

void
uls::putWargList(wchar_t **wargv, int n_wargv)
{
	int i;

	for (i=0; i < n_wargv; i++) {
		uls_mfree(wargv[i]);
	}

	uls_mfree(wargv);
}

int
uls::parseCommandOptions(int n_args, char *args[], const char *optfmt, uls::optproc_t proc)
{
	const char  *cptr;
	char *optarg, *optstr, nullbuff[4] = { '\0', };
	int         rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (args[i][0] != '-') break;

		optstr = uls_ptr(args[i][1]);
		for (k=i,j=0; (opt=optstr[j]) != '\0'; ) {
			if (opt == '?') {
				return 0; // call usage();
			}

			if ((rc = strFindIndex(optfmt, opt)) < 0) {
				_uls_log(err_log)("parseCommandOptions: undefined option -%c", opt);
				return -1;
			}

			cptr = optfmt + rc;
			if (cptr[1] == ':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					_uls_log(err_log)("parseCommandOptions: option -%c requires an arg.", opt);
					return -1;
				}

				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("Error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = nullbuff;
				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("parseCommandOptions: error in -%c.", opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}

int
uls::parseCommandOptions(int n_args, wchar_t *args[], const wchar_t *optfmt, woptproc_t wproc)
{
	const wchar_t  *cptr;
	wchar_t *optarg, *optstr, nullbuff[4] = { L'\0', };
	int         rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (args[i][0] != L'-') break;

		optstr = uls_ptr(args[i][1]);
		for (k=i,j=0; (opt=optstr[j]) != '\0'; ) {
			if (opt == L'?') {
				return 0; // call usage();
			}

			if ((rc = strFindIndex(optfmt, opt)) < 0) {
				_uls_log(err_log)("parseCommandOptions: undefined option -%c", opt);
				return -1;
			}

			cptr = optfmt + rc;
			if (cptr[1] == L':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					_uls_log(err_log)("parseCommandOptions: option -%c requires an arg.", opt);
					return -1;
				}

				if ((rc = wproc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("Error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = nullbuff;
				if ((rc = wproc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("parseCommandOptions: error in -%c.", opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}
