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
  <file> uls_emit_wstr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jun 2015.
  </author>
*/
#ifndef __ULS_EMIT_WSTR_H__
#define __ULS_EMIT_WSTR_H__

#include "uls/uls_auw.h"
#ifdef _ULS_INTERNAL_USE_ONLY
#include "uls/uls_emit.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define N_PARMS_EMIT_WSTR 7

ULS_DEFINE_STRUCT(emit_wshell)
{
	const wchar_t *wrdlst[N_PARMS_EMIT_WSTR];
	csz_str_t csz_wrd_ary[N_PARMS_EMIT_WSTR];
	char *ustr[N_PARMS_EMIT_WSTR];
};

ULS_DLL_EXTERN int uls_init_parms_emit_wstr(uls_parms_emit_ptr_t emit_parm,
	const wchar_t *out_wdpath, const wchar_t *out_wfname, const wchar_t *wfpath_uld,
	const wchar_t *ulc_wname, const wchar_t *class_wname, const wchar_t *enum_wname,
	const wchar_t *tok_wpfx, uls_flags_t flags);

ULS_DLL_EXTERN int uls_deinit_parms_emit_wstr(uls_parms_emit_ptr_t emit_parm);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define uls_init_parms_emit uls_init_parms_emit_wstr
#define uls_deinit_parms_emit uls_deinit_parms_emit_wstr
#endif
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_EMIT_WSTR_H__
