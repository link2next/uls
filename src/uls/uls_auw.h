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
#ifndef __ULS_AUW_H__
#define __ULS_AUW_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#include <wchar.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#ifndef __ULS_WINDOWS__
#ifdef ULS_USE_WSTR
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif
typedef TCHAR *LPTSTR;
typedef const TCHAR *LPCTSTR;
#endif
#define uls_wcslen(wstr) ((int)wcslen(wstr))
#define auw_csz_wstr(csz) ((wchar_t *)csz_data_ptr(csz))
#define auw_csz_wlen(csz) (csz_length(csz)/sizeof(wchar_t))
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
_ULS_DEFINE_STRUCT(auw_outparam)
{
	csz_str_t  csz;
	int outlen;
};
#endif

#if defined(__ULS_AUW__) || defined(ULS_DECL_PRIVATE_PROC)
#ifdef __ULS_WINDOWS__
ULS_DECL_STATIC char *wstr2mbs(const wchar_t *wstr, int wlen, int is_utf8, csz_str_ptr_t csz);
ULS_DECL_STATIC wchar_t *mbs2wstr(const char *astr, int alen, int is_utf8, csz_str_ptr_t csz_wstr);
ULS_DECL_STATIC int __uls_astr2ustr_ptr(uls_outparam_ptr_t parms);
ULS_DECL_STATIC int __uls_ustr2astr_ptr(uls_outparam_ptr_t parms);
#endif
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int astr_lengthof_char(const char *str);
ULS_DLL_EXTERN int astr_num_wchars(const char *str, int len, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN wchar_t *uls_astr2wstr(const char *astr, int alen, csz_str_ptr_t csz_wstr);
ULS_DLL_EXTERN char *uls_wstr2astr(const wchar_t *wstr, int wlen, csz_str_ptr_t csz);

ULS_DLL_EXTERN wchar_t *uls_ustr2wstr(const char *ustr, int ulen, csz_str_ptr_t csz_wstr);
ULS_DLL_EXTERN char *uls_wstr2ustr(const wchar_t *wstr, int wlen, csz_str_ptr_t csz);

ULS_DLL_EXTERN char *uls_ustr2astr(const char *ubuf, int ulen, csz_str_ptr_t csz);
ULS_DLL_EXTERN char *uls_astr2ustr(const char *astr, int alen, csz_str_ptr_t csz);

ULS_DLL_EXTERN void auw_init_outparam(auw_outparam_ptr_t auw);
ULS_DLL_EXTERN void auw_deinit_outparam(auw_outparam_ptr_t auw);

ULS_DLL_EXTERN const char *uls_ustr2astr_ptr(const char *ustr, int ulen, auw_outparam_ptr_t auw);
ULS_DLL_EXTERN const char *uls_astr2ustr_ptr(const char *astr, int alen, auw_outparam_ptr_t auw);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_AUW_H__
