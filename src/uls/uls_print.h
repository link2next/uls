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
  <file> uls_print.h </file>
  <brief>
    Vararg routines for printing output to files.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#ifndef __ULS_PRINT_H__
#define __ULS_PRINT_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_lf_sprintf.h"
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#if defined(__ULS_PRINT__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_lf_ptr_t dfl_str_lf;
ULS_DECL_STATIC uls_lf_ptr_t dfl_file_lf;
ULS_DECL_STATIC uls_lf_ptr_t dfl_csz_lf;
ULS_DECL_STATIC uls_lf_ptr_t dfl_sysprn_lf;
ULS_DECL_STATIC int sysprn_opened;
#define ULS_SYSPRN_TABBUF_SIZE  64
ULS_DECL_STATIC char sysprn_tabbuf[ULS_SYSPRN_TABBUF_SIZE];
ULS_DECL_STATIC int  sysprn_tabsiz, sysprn_ntabs, sysprn_tabbuf_len;
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void initialize_uls_sysprn(void);
void finalize_uls_sysprn(void);
#endif

#if defined(__ULS_PRINT__) || defined(ULS_DECL_PUBLIC_PROC)
ULS_DLL_EXTERN int uls_sysprn_open(uls_voidptr_t data, uls_lf_puts_t proc);
ULS_DLL_EXTERN void uls_sysprn_set_tabsiz(int tabsiz);
ULS_DLL_EXTERN void uls_sysprn_close(void);
ULS_DLL_EXTERN int uls_vsysprn(const char* fmt, va_list args);
ULS_DLL_EXTERN int uls_sysprn(const char* fmt, ...);

ULS_DLL_EXTERN void uls_sysprn_puttabs(int n);
ULS_DLL_EXTERN void uls_sysprn_tabs(int n_tabs, char *fmt, ...);

/* string printf */
ULS_DLL_EXTERN int __uls_lf_vsnprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vsnprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_snprintf(char *buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, ...);
ULS_DLL_EXTERN int uls_lf_snprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, ...);

ULS_DLL_EXTERN int __uls_vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_snprintf(char* buf, int bufsiz, const char *fmt, ...);
ULS_DLL_EXTERN int uls_snprintf(char *buf, int bufsiz, const char *fmt, ...);

/* csz printf */
ULS_DLL_EXTERN int __uls_lf_vzprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vzprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_zprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, ...);
ULS_DLL_EXTERN int uls_lf_zprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, ...);

ULS_DLL_EXTERN int __uls_vzprintf(csz_str_ptr_t csz, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_vzprintf(csz_str_ptr_t csz, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_zprintf(csz_str_ptr_t csz, const char *fmt, ...);
ULS_DLL_EXTERN int uls_zprintf(csz_str_ptr_t csz, const char *fmt, ...);

/* file printf */
ULS_DLL_EXTERN int __uls_lf_vfprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vfprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_fprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, ...);
ULS_DLL_EXTERN int uls_lf_fprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, ...);

ULS_DLL_EXTERN int __uls_vfprintf(FILE* fp, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_vfprintf(FILE* fp, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_fprintf(FILE* fp, const char *fmt, ...);
ULS_DLL_EXTERN int uls_fprintf(FILE* fp, const char *fmt, ...);

ULS_DLL_EXTERN int __uls_lf_vprintf(uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vprintf(uls_lf_ptr_t uls_lf, const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_printf(uls_lf_ptr_t uls_lf, const char *fmt, ...);
ULS_DLL_EXTERN int uls_lf_printf(uls_lf_ptr_t uls_lf, const char *fmt, ...);

ULS_DLL_EXTERN int __uls_vprintf(const char *fmt, va_list args);
ULS_DLL_EXTERN int uls_vprintf(const char *fmt, va_list args);
ULS_DLL_EXTERN int __uls_printf(const char *fmt, ...);
ULS_DLL_EXTERN int uls_printf(const char *fmt, ...);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_wprint.h"
#endif

#endif // __ULS_PRINT_H__
