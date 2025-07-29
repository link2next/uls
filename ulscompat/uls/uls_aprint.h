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
  <file> uls_aprint.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_APRINT_H__
#define __ULS_APRINT_H__

#include "uls/uls_auw.h"
#include "uls/uls_print.h"

#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

void initialize_uls_aprint(void);
void finalize_uls_aprint(void);

ULS_DLL_EXTERN int uls_sysprn_aopen(uls_voidptr_t data, uls_lf_puts_t proc);
ULS_DLL_EXTERN void uls_sysprn_aclose(void);
ULS_DLL_EXTERN int uls_vasysprn(const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_asysprn(const char *afmt, ...);

/* string aprintf */
ULS_DLL_EXTERN int __uls_lf_vsnaprintf(char *abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vsnaprintf(char *abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_snaprintf(char *abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, ...);
ULS_DLL_EXTERN int uls_lf_snaprintf(char *abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, ...);

ULS_DLL_EXTERN int __uls_vsnaprintf(char *abuf, int abuf_siz, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_vsnaprintf(char *abuf, int abuf_siz, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_snaprintf(char *abuf, int abuf_siz, const char *afmt, ...);
ULS_DLL_EXTERN int uls_snaprintf(char *abuf, int abuf_siz, const char *afmt, ...);

/* csz aprintf */
ULS_DLL_EXTERN int __uls_lf_vzaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vzaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_zaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...);
ULS_DLL_EXTERN int uls_lf_zaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...);

ULS_DLL_EXTERN int __uls_vzaprintf(csz_str_ptr_t csz, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_vzaprintf(csz_str_ptr_t csz, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_zaprintf(csz_str_ptr_t csz, const char *afmt, ...);
ULS_DLL_EXTERN int uls_zaprintf(csz_str_ptr_t csz, const char *afmt, ...);

/* file aprintf */
ULS_DLL_EXTERN int __uls_lf_vfaprintf(FILE *fp, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vfaprintf(FILE *fp, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_faprintf(FILE *fp, uls_lf_ptr_t uls_lf, const char *afmt, ...);
ULS_DLL_EXTERN int uls_lf_faprintf(FILE *fp, uls_lf_ptr_t uls_lf, const char *afmt, ...);

ULS_DLL_EXTERN int __uls_vfaprintf(FILE *fp, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_vfaprintf(FILE *fp, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_faprintf(FILE *fp, const char *afmt, ...);
ULS_DLL_EXTERN int uls_faprintf(FILE *fp, const char *afmt, ...);

ULS_DLL_EXTERN int __uls_lf_vaprintf(uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vaprintf(uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_aprintf(uls_lf_ptr_t uls_lf, const char *afmt, ...);
ULS_DLL_EXTERN int uls_lf_aprintf(uls_lf_ptr_t uls_lf, const char *afmt, ...);

ULS_DLL_EXTERN int __uls_vaprintf(const char *afmt, va_list args);
ULS_DLL_EXTERN int uls_vaprintf(const char *afmt, va_list args);
ULS_DLL_EXTERN int __uls_aprintf(const char *afmt, ...);
ULS_DLL_EXTERN int uls_aprintf(const char *afmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_sysprn_open uls_sysprn_aopen
#define uls_sysprn_close uls_sysprn_aclose
#define uls_vsysprn uls_vasysprn
#define uls_sysprn uls_asysprn

#define __uls_lf_vsnprintf __uls_lf_vsnaprintf
#define uls_lf_vsnprintf uls_lf_vsnaprintf
#define __uls_lf_snprintf __uls_lf_snaprintf
#define uls_lf_snprintf uls_lf_snaprintf

#define __uls_vsnprintf __uls_vsnaprintf
#define uls_vsnprintf uls_vsnaprintf
#define __uls_snprintf __uls_snaprintf
#define uls_snprintf uls_snaprintf

#define __uls_lf_vzprintf __uls_lf_vzaprintf
#define uls_lf_vzprintf uls_lf_vzaprintf
#define __uls_lf_zprintf __uls_lf_zaprintf
#define uls_lf_zprintf uls_lf_zaprintf

#define __uls_vzprintf __uls_vzaprintf
#define uls_vzprintf uls_vzaprintf
#define __uls_zprintf __uls_zaprintf
#define uls_zprintf uls_zaprintf

#define __uls_lf_vfprintf __uls_lf_vfaprintf
#define uls_lf_vfprintf uls_lf_vfaprintf
#define __uls_lf_fprintf __uls_lf_faprintf
#define uls_lf_fprintf uls_lf_faprintf

#define __uls_vfprintf __uls_vfaprintf
#define uls_vfprintf uls_vfaprintf
#define __uls_fprintf __uls_faprintf
#define uls_fprintf uls_faprintf

#define __uls_lf_vprintf __uls_lf_vaprintf
#define uls_lf_vprintf uls_lf_vaprintf
#define __uls_lf_printf __uls_lf_aprintf
#define uls_lf_printf uls_lf_aprintf

#define __uls_vprintf __uls_vaprintf
#define uls_vprintf uls_vaprintf
#define __uls_printf __uls_aprintf
#define uls_printf uls_aprintf
#endif
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_APRINT_H__
