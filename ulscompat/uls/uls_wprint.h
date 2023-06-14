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
  <file> uls_wprint.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_WPRINT_H__
#define __ULS_WPRINT_H__

#include "uls/uls_lf_swprintf.h"
#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

void initialize_uls_wprint(void);
void finalize_uls_wprint(void);

ULS_DLL_EXTERN int uls_sysprn_wopen(uls_voidptr_t data, uls_lf_puts_t proc);
ULS_DLL_EXTERN void uls_sysprn_wclose(void);
ULS_DLL_EXTERN int uls_vwsysprn(const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN int uls_wsysprn(const wchar_t* wfmt, ...);

/* string wprintf */
ULS_DLL_EXTERN int __uls_lf_vsnwprintf(wchar_t* wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vsnwprintf(wchar_t* wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_snwprintf(wchar_t* wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_lf_snwprintf(wchar_t* wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);

ULS_DLL_EXTERN int __uls_vsnwprintf(wchar_t* wbuf, int wbuf_siz, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_vsnwprintf(wchar_t* wbuf, int wbuf_siz, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_snwprintf(wchar_t* wbuf, int wbuf_siz, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_snwprintf(wchar_t* wbuf, int wbuf_siz, const wchar_t *wfmt, ...);

/* csz wprintf */
ULS_DLL_EXTERN int __uls_lf_vzwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vzwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_zwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_lf_zwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);

ULS_DLL_EXTERN int __uls_vzwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_vzwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_zwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_zwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, ...);

/* file wprintf */
ULS_DLL_EXTERN int __uls_lf_vfwprintf(FILE* fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vfwprintf(FILE* fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_fwprintf(FILE* fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_lf_fwprintf(FILE* fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);

ULS_DLL_EXTERN int __uls_vfwprintf(FILE* fp, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_vfwprintf(FILE* fp, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_fwprintf(FILE* fp, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_fwprintf(FILE* fp, const wchar_t *wfmt, ...);

ULS_DLL_EXTERN int __uls_lf_vwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_wprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_lf_wprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);

ULS_DLL_EXTERN int __uls_vwprintf(const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_vwprintf(const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int __uls_wprintf(const wchar_t *wfmt, ...);
ULS_DLL_EXTERN int uls_wprintf(const wchar_t *wfmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_WPRINT_H__
