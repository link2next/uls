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
  <file> uls_lf_asprintf.h </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..) for MS-MBCS encoding.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/
#ifndef __ULS_LF_SAPRINTF_H__
#define __ULS_LF_SAPRINTF_H__

#include "uls/uls_lf_sprintf.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(uls_alf_shell)
{
  uls_lf_ptr_t uls_lf;
  csz_str_t fmtstr;
};

void initialize_uls_alf(void);
void finalize_uls_alf(void);
void alf_shell_init(uls_alf_shell_ptr_t alf, uls_lf_ptr_t uls_lf);
void alf_shell_deinit(uls_alf_shell_ptr_t alf);

ULS_DLL_EXTERN int uls_lf_init_convspec_amap(uls_lf_map_ptr_t lf_map, int flags);
ULS_DLL_EXTERN void uls_lf_deinit_convspec_amap(uls_lf_map_ptr_t lf_map);
ULS_DLL_EXTERN uls_lf_map_ptr_t uls_lf_create_convspec_amap(int flags);
ULS_DLL_EXTERN void uls_lf_destroy_convspec_amap(uls_lf_map_ptr_t lf_map);

ULS_DLL_EXTERN int uls_alf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_alf_deinit(uls_lf_ptr_t uls_lf);
ULS_DLL_EXTERN uls_lf_ptr_t uls_alf_create(uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_alf_destroy(uls_lf_ptr_t uls_lf);

ULS_DLL_EXTERN int __uls_lf_vxaprintf(uls_lf_ptr_t uls_lf, const char* afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxaprintf(uls_lf_ptr_t uls_lf, const char* afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xaprintf(uls_lf_ptr_t uls_lf, const char* afmt, ...);
ULS_DLL_EXTERN int uls_lf_xaprintf(uls_lf_ptr_t uls_lf, const char* afmt, ...);

ULS_DLL_EXTERN int __uls_lf_vxaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, ...);
ULS_DLL_EXTERN int uls_lf_xaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_lf_init_convspec_map uls_lf_init_convspec_amap
#define uls_lf_create_convspec_map uls_lf_create_convspec_amap

#define uls_lf_init uls_alf_init
#define uls_lf_deinit uls_alf_deinit
#define uls_lf_create uls_alf_create
#define uls_lf_destroy uls_alf_destroy

#define __uls_lf_vxprintf __uls_lf_vxaprintf
#define uls_lf_vxprintf uls_lf_vxaprintf
#define __uls_lf_xprintf __uls_lf_xaprintf
#define uls_lf_xprintf uls_lf_xaprintf

#define __uls_lf_vxprintf_generic __uls_lf_vxaprintf_generic
#define uls_lf_vxprintf_generic uls_lf_vxaprintf_generic
#define __uls_lf_xprintf_generic __uls_lf_xaprintf_generic
#define uls_lf_xprintf_generic uls_lf_xaprintf_generic
#endif
#endif

#endif // __ULS_LF_SAPRINTF_H__
