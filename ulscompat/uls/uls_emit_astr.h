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
  <file> uls_emit_astr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jun 2015.
  </author>
*/
#ifndef __ULS_EMIT_ASTR_H__
#define __ULS_EMIT_ASTR_H__

#include "uls/uls_auw.h"
#include "uls/uls_emit.h"
#include "uls/uls_lex.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define N_PARMS_EMIT_ASTR 7

ULS_DEFINE_STRUCT(emit_ashell)
{
	const char *wrdlst[N_PARMS_EMIT_ASTR];
	auw_outparam_t buf_csz[N_PARMS_EMIT_ASTR];
	const char *ustr[N_PARMS_EMIT_ASTR];
};

ULS_DLL_EXTERN int uls_init_parms_emit_astr(uls_parms_emit_ptr_t emit_parm,
	const char *out_dpath, const char *out_fname, const char *fpath_uld,
	const char *ulc_name, const char *class_name, const char *enum_name,
	const char *tok_pfx, int flags);
ULS_DLL_EXTERN int uls_deinit_parms_emit_astr(uls_parms_emit_ptr_t emit_parm);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_init_parms_emit uls_init_parms_emit_astr
#define uls_deinit_parms_emit uls_deinit_parms_emit_astr
#endif
#endif

#endif // __ULS_EMIT_ASTR_H__
