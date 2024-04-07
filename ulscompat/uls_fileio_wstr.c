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
  <file> uls_util_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"

#include "uls/uls_wlog.h"

int
uls_dirent_exist_wstr(const wchar_t *wfpath)
{
	char *ustr;
	int  rval;
	csz_str_t csz;

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
uls_fp_wopen(const wchar_t* wfpath, int mode)
{
	char *ustr;
	csz_str_t csz;
	FILE *fp;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		fp = NULL;
	} else {
		fp = uls_fp_open(ustr, mode);
	}

	csz_deinit(uls_ptr(csz));
	return fp;
}

int
uls_fp_gets_wstr(FILE *fp, wchar_t *wbuf, int wbuf_siz, int flags)
{
	char *abuf = NULL;
	int asiz=0, alen;

	wchar_t *wstr;
	int wlen;
	csz_str_t csz_wstr;

	asiz = (1 + wbuf_siz) * 4;
	abuf = uls_malloc(asiz);
	csz_init(uls_ptr(csz_wstr), asiz);

	if ((alen = uls_fp_gets(fp, abuf, asiz, flags)) <= ULS_EOF) {
		csz_deinit(uls_ptr(csz_wstr));
		uls_mfree(abuf);
		return alen;
	}

	if ((wstr = uls_ustr2wstr(abuf, alen, uls_ptr(csz_wstr))) == NULL) {
		err_wlog(L"encoding error!");
		csz_deinit(uls_ptr(csz_wstr));
		uls_mfree(abuf);
		return ULS_EOF - 4;
	}

	uls_mfree(abuf);

	if ((wlen = auw_csz_wlen(uls_ptr(csz_wstr))) >= wbuf_siz) {
		csz_deinit(uls_ptr(csz_wstr));
		return ULS_EOF - 5;
	}

	uls_memcopy(wbuf, wstr, wlen * sizeof(wchar_t));
	wbuf[wlen] = L'\0';

	csz_deinit(uls_ptr(csz_wstr));

	return wlen;
}

int
uls_close_tempfile_wstr(uls_tempfile_ptr_t tmpfile, const wchar_t* wfilepath)
{
	char *ustr;
	int  rval;
	csz_str_t csz;

	if (wfilepath == NULL) {
		return uls_close_tempfile(tmpfile, NULL);
	}

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfilepath, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		rval = -1;
	} else {
		rval = uls_close_tempfile(tmpfile, ustr);
	}

	csz_deinit(uls_ptr(csz));
	return rval;
}

int
uls_fd_open_wstr(const wchar_t* wfpath, int mode)
{
	char *ustr;
	int  rval;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"invalid param!");
		rval = -1;
	} else {
		rval = uls_fd_open(ustr, mode);
	}

	csz_deinit(uls_ptr(csz));
	return rval;
}
