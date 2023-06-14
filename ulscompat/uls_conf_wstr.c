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
  <file> uls_conf_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/
#include "uls/uls_conf.h"
#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_wlog.h"


int
uls_is_valid_specpath_wstr(const wchar_t* wconfname)
{
	char *ustr;
	int rval;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wconfname, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		rval = -1;
	} else {
		rval = uls_is_valid_specpath(ustr);
	}

	csz_deinit(uls_ptr(csz));

	return rval;
}

int
uls_get_spectype_wstr(const wchar_t *wfpath, uls_outparam_ptr_t parms)
{
	wchar_t* specwname = (wchar_t *) parms->line;
	char *ufpath;
	wchar_t *wstr;
	int wlen, stat, len_dpath_ustr, l_specname_ustr;
	char specname_ustr[ULC_LONGNAME_MAXSIZ+1];
	csz_str_t csz1, csz2;
	uls_outparam_t parms1;

	csz_init(uls_ptr(csz1), -1);

	if ((ufpath = uls_wstr2ustr(wfpath, -1, uls_ptr(csz1))) == NULL) {
		err_wlog(L"encoding error!");
		csz_deinit(uls_ptr(csz1));
		return -1;
	}

	parms1.line = specname_ustr;
	stat = uls_get_spectype(ufpath, uls_ptr(parms1));
	len_dpath_ustr = parms1.n;
	l_specname_ustr = parms1.len;

	csz_init(uls_ptr(csz2), -1);
	if ((wstr = uls_ustr2wstr(specname_ustr, -l_specname_ustr, uls_ptr(csz2))) == NULL) {
		err_wlog(L"encoding error!");
		csz_deinit(uls_ptr(csz1)); csz_deinit(uls_ptr(csz2));
		return -1;
	}

	wlen = auw_csz_wlen(uls_ptr(csz2));
	uls_memcopy(specwname, wstr, wlen*sizeof(wchar_t));
	specwname[wlen] = L'\0';
	parms->len = wlen;

	if ((wstr = uls_ustr2wstr(ufpath, len_dpath_ustr, uls_ptr(csz2))) == NULL) {
		err_wlog(L"encoding error!");
		stat = -1;
	} else {
		parms->n = wlen = auw_csz_wlen(uls_ptr(csz2));
	}

	csz_deinit(uls_ptr(csz1)); csz_deinit(uls_ptr(csz2));
	return stat;
}

void
ulc_list_searchpath_wstr(const wchar_t* confname)
{
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(confname, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		ulc_list_searchpath(ustr);
	}

	csz_deinit(uls_ptr(csz));
}

void
ulc_set_searchpath_wstr(const wchar_t *pathlist)
{
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(pathlist, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		ulc_set_searchpath(ustr);
	}

	csz_deinit(uls_ptr(csz));
}

int
ulc_add_searchpath_wstr(const wchar_t *pathlist, int front)
{
	char *ustr;
	int  rval;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(pathlist, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		rval = -1;
	} else {
		rval = ulc_add_searchpath(ustr, front);
	}

	csz_deinit(uls_ptr(csz));
	return rval;
}
