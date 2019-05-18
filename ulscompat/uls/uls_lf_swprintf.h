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
  <file> uls_lf_swprintf.h </file>
  <brief>
    An wide-char string version of uls_lf_sprintf.c.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef __ULS_LF_SWPRINTF_H__
#define __ULS_LF_SWPRINTF_H__

#include "uls/uls_lf_sprintf.h"
#include <wchar.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(uls_buf4wstr)
{
  unsigned int flags;
  wchar_t *wbuf, *wbufptr;
  int wbuflen;
};

ULS_DEFINE_STRUCT(uls_wlf_shell)
{
  uls_lf_ptr_t uls_lf;
  csz_str_t fmtstr;

};

void initialize_uls_wlf(void);
void finalize_uls_wlf(void);

void wlf_shell_init(uls_wlf_shell_ptr_t wlf, uls_lf_ptr_t uls_lf);
void wlf_shell_deinit(uls_wlf_shell_ptr_t wlf);

ULS_DLL_EXTERN int uls_lf_init_convspec_wmap(uls_lf_map_ptr_t lf_map, int flags);
ULS_DLL_EXTERN void uls_lf_deinit_convspec_wmap(uls_lf_map_ptr_t lf_map);
ULS_DLL_EXTERN uls_lf_map_ptr_t uls_lf_create_convspec_wmap(int flags);
ULS_DLL_EXTERN void uls_lf_destroy_convspec_wmap(uls_lf_map_ptr_t lf_map);

ULS_DLL_EXTERN int uls_wlf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_wlf_deinit(uls_lf_ptr_t uls_lf);
ULS_DLL_EXTERN uls_lf_ptr_t uls_wlf_create(uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_wlf_destroy(uls_lf_ptr_t uls_lf);

ULS_DLL_EXTERN int __uls_lf_vxwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...);
ULS_DLL_EXTERN int uls_lf_xwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...);

ULS_DLL_EXTERN int __uls_lf_vxwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...);
ULS_DLL_EXTERN int uls_lf_xwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define uls_lf_init_convspec_map uls_lf_init_convspec_wmap
#define uls_lf_create_convspec_map uls_lf_create_convspec_wmap

#define uls_lf_init uls_wlf_init
#define uls_lf_deinit uls_wlf_deinit
#define uls_lf_create uls_wlf_create
#define uls_lf_destroy uls_wlf_destroy

#define __uls_lf_vxprintf __uls_lf_vxwprintf
#define uls_lf_vxprintf uls_lf_vxwprintf
#define __uls_lf_xprintf __uls_lf_xwprintf
#define uls_lf_xprintf uls_lf_xwprintf

#define __uls_lf_vxprintf_generic __uls_lf_vxwprintf_generic
#define uls_lf_vxprintf_generic uls_lf_vxwprintf_generic
#define __uls_lf_xprintf_generic __uls_lf_xwprintf_generic
#define uls_lf_xprintf_generic uls_lf_xwprintf_generic
#endif
#endif

#endif // __ULS_LF_SWPRINTF_H__
