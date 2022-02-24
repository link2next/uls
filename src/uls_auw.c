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
  <file> uls_auw.c </file>
  <brief>
    Converting string among MS-MBCS, utf8, and wide string encodings.
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/
#define __ULS_AUW__
#include "uls/uls_auw.h"
#include "uls/uls_prim.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"

#include <stdlib.h>
#include <string.h>

#ifdef ULS_WINDOWS
ULS_DECL_STATIC char*
wstr2mbs(const wchar_t* wstr, int wlen, int is_utf8, csz_str_ptr_t csz)
{
	UINT codepage = is_utf8 ? CP_UTF8 : CP_ACP;
	DWORD  errnum;
	int asiz, rc;
	char *astr;

	if (wlen == 0) {
		csz_reset(csz);
		astr = csz_text(csz);
		return astr;
	}

	if ((rc = WideCharToMultiByte(codepage, 0, wstr, wlen, NULL, 0, NULL, NULL)) == 0 || (is_utf8 && rc == 0xFFFD)) {
		if ((errnum=GetLastError()) == ERROR_INVALID_PARAMETER || errnum == ERROR_NO_UNICODE_TRANSLATION) {
			return NULL;
		}
		err_log_primitive("Error: WideCharToMultiByte!");
		return NULL;
	}

	asiz = rc + 1;
	csz_modify(csz, 0, NULL, asiz);
	astr = csz_data_ptr(csz);

	if ((rc = WideCharToMultiByte(codepage, 0, wstr, wlen, astr, asiz, NULL, NULL)) <= 0) {
		err_log_primitive("Error: WideCharToMultiByte!");
		csz_reset(csz);
		astr = NULL;
	} else {
		csz_truncate(csz, rc);
		astr = csz_text(csz);
	}

	return astr;
}

ULS_DECL_STATIC wchar_t*
mbs2wstr(const char* astr, int alen, int is_utf8, csz_str_ptr_t csz)
{
	UINT codepage = is_utf8 ? CP_UTF8 : CP_ACP;
	DWORD  errnum;
	int rc, wlen, wsiz;
	wchar_t *wstr;

	if (alen == 0) {
		wchar_t nil_wstr[1] = { L'\0' };
		csz_reset(csz);
		csz_append(csz, (char *) nil_wstr, sizeof(wchar_t));
		csz_truncate(csz, 0);
		return (wchar_t *) csz_data_ptr(csz);
	}

	if ((rc = MultiByteToWideChar(codepage, 0, astr, alen, NULL, 0)) == 0 || (is_utf8 && rc == 0xFFFD)) {
		if ((errnum=GetLastError()) == ERROR_INVALID_PARAMETER || errnum == ERROR_NO_UNICODE_TRANSLATION) {
			return NULL;
		}
  		err_log_primitive("Error: MultiByteToWideChar!");
  		return NULL;
	}

  	wsiz = rc + 1;
  	csz_modify(csz, 0, NULL, wsiz * sizeof(wchar_t));
	wstr = (wchar_t *) csz_data_ptr(csz);

	if ((rc = MultiByteToWideChar(codepage, 0, astr, alen, wstr, wsiz)) <= 0) {
		err_log_primitive("Error: MultiByteToWideChar!");
		csz_reset(csz);
		wstr = NULL;
	} else {
		wlen = rc;
		wstr[wlen] = L'\0';
		csz_truncate(csz, (int) (wlen * sizeof(wchar_t)));
		csz_text(csz);
	}

	return wstr;
}

ULS_DECL_STATIC int
__uls_astr2ustr_ptr(uls_outparam_ptr_t parms)
{
	const char *astr = parms->lptr;
	int alen = parms->len;
	int n_chars, stat;
	uls_outparam_t parms1;

	if (alen >= 0) {
		return -1;
	}

	if (alen == -1) {
		n_chars = astr_num_chars(astr, -1, uls_ptr(parms1));
		alen = parms1.len;
	} else {
		alen = -alen;
		n_chars = astr_num_chars(astr, alen, nilptr);
	}

	if (n_chars == alen) {
		stat = 0;
	} else {
		stat = -1;
	}

	parms->len = alen;
	return stat;
}

ULS_DECL_STATIC int
__uls_ustr2astr_ptr(uls_outparam_ptr_t parms)
{
	const char *ustr = parms->lptr;
	int ulen = parms->len;
	int n_chars, stat;
	uls_outparam_t parms1;

	if (ulen >= 0) {
		return -1;
	}

	if (ulen == -1) {
		n_chars = ustr_num_chars(ustr, -1, uls_ptr(parms1));
		ulen = parms1.len;
	} else {
		ulen = -ulen;
		n_chars = ustr_num_chars(ustr, ulen, nilptr);
	}

	if (n_chars == ulen) {
		stat = 0;
	} else {
		stat = -1;
	}

	parms->len =  ulen;
	return stat;
}
#endif // ULS_WINDOWS

void
auw_init_outparam(auw_outparam_ptr_t auw)
{
	csz_init(uls_ptr(auw->csz), -1);
	auw->outlen = 0;
}

void
auw_deinit_outparam(auw_outparam_ptr_t auw)
{
	csz_deinit(uls_ptr(auw->csz));
	auw->outlen = 0;
}

int
astr_lengthof_char(const char* str)
{
	char ch;
	int n;

	if ((ch=*str) & 0x80) {
		n = _uls_sysinfo_(multibytes);
	} else if (ch=='\0') {
		n = 0;
	} else {
		n = 1;
	}

	return n;
}

int
astr_num_chars(const char* str, int len, uls_outparam_ptr_t parms)
{
	const char *ptr;
	int n=0, i, rc;

	if (len < 0) {
		for (ptr=str; (rc=astr_lengthof_char(ptr)) > 0; ptr+=rc) {
			++n;
		}
		len = (int) (ptr - str);

	} else {
		for (i=0,ptr=str; i<len; i+=rc,ptr+=rc) {
			if (ptr[0] == '\0') {
				len = (int) (ptr - str);
				break;
			}

			rc = astr_lengthof_char(ptr);
			++n;
		}
	}

	if (parms != nilptr) {
		parms->n = n;
		parms->len = len;
	}

	return n;
}

#ifdef ULS_WINDOWS
wchar_t*
uls_astr2wstr(const char *astr, int alen, csz_str_ptr_t csz_wstr)
{
	int  has_nil=0;
	wchar_t *wstr;
	uls_outparam_t parms1;

	if (astr == NULL) {
		return NULL;
	}

	if (alen < 0) {
		has_nil = 1;
		if (alen == -1) {
			astr_num_chars(astr, -1, uls_ptr(parms1));
			alen = parms1.len;
		} else {
			alen = -alen;
		}
	}

	wstr = mbs2wstr(astr, alen, 0, csz_wstr);
	return wstr;
}

char*
uls_wstr2astr(const wchar_t* wstr, int wlen, csz_str_ptr_t csz)
{
	int  has_nil=0;
	char *astr;

	if (wstr == NULL) {
		return NULL;
	}

	if (wlen < 0) {
		has_nil = 1;
		if (wlen == -1) {
			wlen = uls_wcslen(wstr);
		} else {
			wlen = -wlen;
		}
	}

	astr = wstr2mbs(wstr, wlen, 0, csz);
	return astr;
}

wchar_t*
uls_ustr2wstr(const char *ustr, int ulen, csz_str_ptr_t csz_wstr)
{
	int has_nil=0;
	wchar_t *wstr;
	uls_outparam_t parms1;

	if (ustr == NULL) {
		return NULL;
	}

	if (ulen < 0) {
		has_nil = 1;
		if (ulen == -1) {
			ustr_num_chars(ustr, -1, uls_ptr(parms1));
			ulen = parms1.len;
		} else {
			ulen = -ulen;
		}
	}

	wstr = mbs2wstr(ustr, ulen, 1, csz_wstr);
	return wstr;
}

char*
uls_wstr2ustr(const wchar_t* wstr, int wlen, csz_str_ptr_t csz)
{
	int  has_nil=0;
	char *ustr;

	if (wstr == NULL) {
		return NULL;
	}

	if (wlen < 0) {
		has_nil = 1;
		if (wlen == -1) {
			wlen = uls_wcslen(wstr);
		} else {
			wlen = -wlen;
		}
	}

	ustr = wstr2mbs(wstr, wlen, 1, csz);
	return ustr;
}

char*
uls_ustr2astr(const char *ustr, int ulen, csz_str_ptr_t csz)
{
	csz_str_t csz_wstr;
	wchar_t *wstr;
	char *astr;
	int wlen;

	if (ustr == NULL) {
		return NULL;
	}

	if (ulen == 0) {
		csz_reset(csz);
		return csz_text(csz);
	}

	csz_init(uls_ptr(csz_wstr), (ulen+1) * sizeof(wchar_t));

	if ((wstr = uls_ustr2wstr(ustr, ulen, uls_ptr(csz_wstr))) == NULL) {
		astr = NULL;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		astr = uls_wstr2astr(wstr, -wlen, csz);
	}

	csz_deinit(uls_ptr(csz_wstr));
	return astr;
}

char*
uls_astr2ustr(const char *astr, int alen, csz_str_ptr_t csz)
{
	csz_str_t csz_wstr;
	wchar_t *wstr;
	int wlen;
	char *ustr;

	if (astr == NULL) {
		return NULL;
	}

	if (alen == 0) {
		csz_reset(csz);
		return csz_text(csz);
	}

	csz_init(uls_ptr(csz_wstr), (alen+1) * sizeof(wchar_t));

	if ((wstr = uls_astr2wstr(astr, alen, uls_ptr(csz_wstr))) == NULL) {
		ustr = NULL;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		ustr = uls_wstr2ustr(wstr, -wlen, csz);
	}

	csz_deinit(uls_ptr(csz_wstr));
	return ustr;
}

const char*
uls_ustr2astr_ptr(const char *ustr, int ulen, auw_outparam_ptr_t auw)
{
	csz_str_ptr_t csz = uls_ptr(auw->csz);
	const char *astr;
	int alen, rval;
	uls_outparam_t parms;

	if (ustr == NULL) {
		return NULL;
	}

	parms.lptr = ustr;
	parms.len = ulen;
	rval = __uls_ustr2astr_ptr(uls_ptr(parms));
	ulen = parms.len;

	if (rval < 0) {
		if ((astr = uls_ustr2astr(ustr, ulen, csz)) == NULL) {
			alen = 0;
		} else {
			alen = csz_length(csz);
		}
	} else {
		alen = ulen;
		astr = ustr;
	}

	auw->outlen = alen;
	return astr;
}

const char*
uls_astr2ustr_ptr(const char *astr, int alen, auw_outparam_ptr_t auw)
{
	csz_str_ptr_t csz = uls_ptr(auw->csz);
	const char *ustr;
	int ulen, rval;
	uls_outparam_t parms;

	if (astr == NULL) {
		return NULL;
	}

	parms.lptr = astr;
	parms.len = alen;
	rval = __uls_astr2ustr_ptr(uls_ptr(parms));
	alen = parms.len;

	if (rval < 0) {
		if ((ustr = uls_astr2ustr(astr, alen, csz)) == NULL) {
			ulen = 0;
		} else {
			ulen = csz_length(csz);
		}
	} else {
		ulen = alen;
		ustr = astr;
	}

	auw->outlen = ulen;
	return ustr;
}

#else // ~ULS_WINDOWS

wchar_t*
uls_astr2wstr(const char *astr, int alen, csz_str_ptr_t csz_wstr)
{
	err_panic("%s: Not Supported on this platform", __FUNCTION__);
	return NULL;
}

char*
uls_wstr2astr(const wchar_t *wstr, int wlen, csz_str_ptr_t csz)
{
	err_panic("%s: Not Supported on this platform", __FUNCTION__);
	return NULL;
}

wchar_t*
uls_ustr2wstr(const char *ustr, int ulen, csz_str_ptr_t csz_wstr)
{
	wchar_t *wstr;
	int wsiz, wlen, has_nil=0;
	size_t siz, siz2;
	char *ustr1_buff = NULL;
	const char *ustr1;
	uls_outparam_t parms1;

	if (ustr == NULL) {
		return NULL;
	}

	if (ulen < 0) {
		has_nil = 1;
		if (ulen == -1) {
			ustr_num_chars(ustr, -1, uls_ptr(parms1));
			ulen = parms1.len;
		} else {
			ulen = -ulen;
		}
	}

	if (ulen == 0) {
		wchar_t nil_wstr[1] = { L'\0' };
		csz_reset(csz_wstr);
		csz_append(csz_wstr, (char *) nil_wstr, sizeof(wchar_t));
		csz_truncate(csz_wstr, 0);
		return auw_csz_wstr(csz_wstr);
	}

	if (has_nil) {
		ustr1 = ustr;
	} else {
		ustr1_buff = uls_strdup(ustr, ulen);
		ustr1 = ustr1_buff;
	}

 	if ((siz = mbstowcs(NULL, ustr1, 0)) == (size_t) -1) {
 		uls_mfree(ustr1_buff);
 		csz_reset(csz_wstr);
		return NULL;
  	}

  	wsiz = ++siz;
  	csz_modify(csz_wstr, 0, NULL, wsiz * sizeof(wchar_t));
	wstr = (wchar_t *) csz_data_ptr(csz_wstr);

	if ((siz2 = mbstowcs(wstr, ustr1, wsiz)) != (size_t) -1) {
		wlen = (int) siz2;
		wstr[wlen] = L'\0';
		csz_truncate(csz_wstr, wlen * sizeof(wchar_t));
	} else {
		csz_reset(csz_wstr);
		wstr = NULL;
	}

 	uls_mfree(ustr1_buff);
	return wstr;
}

char*
uls_wstr2ustr(const wchar_t *wstr, int wlen, csz_str_ptr_t csz)
{
	char *ustr;
	int usiz, ulen, has_nil=0;
	size_t siz, siz2;
	wchar_t *wstr1_buff = NULL;
	const wchar_t *wstr1;

	if (wstr == NULL) {
		return NULL;
	}

	if (wlen < 0) {
		has_nil = 1;
		if (wlen == -1) {
			wlen = uls_wcslen(wstr);
		} else {
			wlen = -wlen;
		}
	}

	if (wlen == 0) {
		csz_reset(csz);
		return csz_text(csz);
	}

	if (has_nil) {
		wstr1 = wstr;
	} else {
		wstr1_buff = (wchar_t *) uls_malloc((wlen+1)*sizeof(wchar_t));
		uls_memcopy(wstr1_buff, wstr, wlen*sizeof(wchar_t));
		wstr1_buff[wlen] = L'\0';
		wstr1 = wstr1_buff;
	}

	if ((siz = wcstombs(NULL, wstr1, 0)) == (size_t) -1) {
		csz_reset(csz);
		uls_mfree(wstr1_buff);
		return NULL;
	}

	usiz = ++siz;
	csz_modify(csz, 0, NULL, usiz);
	ustr = csz_data_ptr(csz);

	if ((siz2 = wcstombs(ustr, wstr1, usiz)) != (size_t) -1) {
		ulen = (int) siz2;
		csz_truncate(csz, ulen);
		ustr = csz_text(csz);
	} else {
		csz_reset(csz);
		ustr = NULL;
	}

	uls_mfree(wstr1_buff);
	return ustr;
}

char*
uls_ustr2astr(const char *ustr, int ulen, csz_str_ptr_t csz)
{
	err_panic("%s: Not Supported on the platform", __FUNCTION__);
	return NULL;
}

char*
uls_astr2ustr(const char *astr, int alen, csz_str_ptr_t csz)
{
	err_panic("%s: Not Supported on the platform", __FUNCTION__);
	return NULL;
}


const char*
uls_ustr2astr_ptr(const char *ustr, int ulen, auw_outparam_ptr_t auw)
{
	err_panic("%s: Not Supported on the platform", __FUNCTION__);
	return NULL;
}

const char*
uls_astr2ustr_ptr(const char *astr, int alen, auw_outparam_ptr_t auw)
{
	err_panic("%s: Not Supported on the platform", __FUNCTION__);
	return NULL;
}

#endif
