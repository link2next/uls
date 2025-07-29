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
  <file> uls_lf_xputs.h </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..).
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2025.
  </author>
*/
#if !defined(ULS_DOTNET) && !defined(__ULS_LF_XPUTS_H__)
#define __ULS_LF_XPUTS_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_TYPE

#define ULS_LF_PERCPROC_object  0

#define ULS_LF_PERCPROC_s       1
#define ULS_LF_PERCPROC_c       2

#define ULS_LF_PERCPROC_d       3
#define ULS_LF_PERCPROC_u       4
#define ULS_LF_PERCPROC_f       5

#define ULS_LF_PERCPROC_ld      6
#define ULS_LF_PERCPROC_lu      7
#define ULS_LF_PERCPROC_lf      8

#define ULS_LF_PERCPROC_lld     9
#define ULS_LF_PERCPROC_llu     10
#define ULS_LF_PERCPROC_llf     11

#define ULS_LF_PERCPROC_Ld      12
#define ULS_LF_PERCPROC_Lu      13
#define ULS_LF_PERCPROC_Lf      14

#define ULS_LF_PERCPROC_p       15

ULS_DECLARE_STRUCT(lf_context);

ULS_DEFINE_DELEGATE_BEGIN(lf_puts, int)(uls_voidptr_t dat, const char *str, int len);
ULS_DEFINE_DELEGATE_END(lf_puts);

ULS_DEFINE_DELEGATE_BEGIN(lf_convspec, int)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DEFINE_DELEGATE_END(lf_convspec);

#endif // ULS_DECL_PUBLIC_TYPE

#ifdef ULS_DEF_PROTECTED_TYPE
ULS_DEFINE_STRUCT(lf_convflag)
{
	uls_flags_t  flags;
	int    width, precision;
};
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT_BEGIN(lf_context)
{
	void           *g_dat;
	uls_lf_convflag_t perfmt;
	csz_str_ptr_t   numbuf1, numbuf2;
	void           *u_dat;
	va_list        args;
};

ULS_DEFINE_STRUCT(buf4str)
{
	unsigned int flags;
	char *buf, *bufptr;
	int bufsiz;
};

ULS_DEFINE_STRUCT(buf4wstr)
{
  unsigned int flags;
  wchar_t *wbuf, *wbufptr;
  int wbufsiz;
};
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void __uls_lf_sysputs(const char *msg);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int uls_lf_puts_csz(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_str(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_file(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_aufile(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_cons(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_null(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);

#ifndef ULS_DOTNET
#ifdef __ULS_WINDOWS__
ULS_DLL_EXTERN int uls_lf_aputs_csz(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_aputs_str(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_aputs_file(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int fmtproc_as(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_t* lf_ctx);
#endif
ULS_DLL_EXTERN int uls_lf_wputs_csz(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_wputs_str(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_wputs_file(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen);
#endif

#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_LF_XPUTS_H__
