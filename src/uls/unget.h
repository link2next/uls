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
 * unget.h -- unget_* routines, unget_ch, unget_tok --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_UNGET_H__
#define __ULS_UNGET_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_conf.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(nextch_detail)
{
	uls_wch_t wch;
	int len_uch;
	uls_quotetype_ptr_t qmt;
	int tok;
};
#endif

#if defined(__ULS_UNGET__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC char *__find_first_space_char(const char *lptr, const char *lptr_end);
ULS_DECL_STATIC int __numof_lfs(uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int __alloc_lexseg_and_zbuf(uls_context_ptr_t ctx, uls_lexseg_ptr_t lexseg, int len,
	uls_tokdef_vx_ptr_t e_vx, const char *qstr, int qlen);
ULS_DECL_STATIC uls_context_ptr_t __push_and_alloc_line_right(uls_lex_ptr_t uls, int len,
	uls_tokdef_vx_ptr_t e_vx, const char *qstr, int qlen, int num_lfs);
ULS_DECL_STATIC int __advance_upto_nonspace(uls_context_ptr_t ctx);
ULS_DECL_STATIC uls_context_ptr_t __uls_unget_str(uls_lex_ptr_t uls, const char *str, int len);
ULS_DECL_STATIC uls_context_ptr_t __uls_unget_quote(uls_lex_ptr_t uls,
	const char *qstr, int qlen, uls_tokdef_vx_ptr_t e_vx, int lf_delta);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
uls_context_ptr_t __uls_unget_current(uls_lex_ptr_t uls);
uls_wch_t uls_peekch_detail(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int uls_unget_current(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_unget_tok(uls_lex_ptr_t uls, int tok_id, const char *lxm);
ULS_DLL_EXTERN int uls_unget_str(uls_lex_ptr_t uls, const char *str);
ULS_DLL_EXTERN int uls_unget_ch(uls_lex_ptr_t uls, uls_wch_t wch);

ULS_DLL_EXTERN uls_wch_t uls_peek_ch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch);
ULS_DLL_EXTERN uls_wch_t uls_get_ch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch);

ULS_DLL_EXTERN int ulsjava_unget_str(uls_lex_ptr_t uls, const uls_native_vptr_t str, int len_str);
ULS_DLL_EXTERN int ulsjava_unget_lexeme(uls_lex_ptr_t uls, int tok_id, const uls_native_vptr_t lxm, int len_lxm);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN int ulsjava_peek_ch(uls_lex_ptr_t uls, int* tok_peek);
ULS_DLL_EXTERN int ulsjava_get_ch(uls_lex_ptr_t uls, int* tok_peek);

ULS_DLL_EXTERN uls_nextch_detail_ptr_t ulsjava_peek_nextch_info(uls_lex_ptr_t uls);
ULS_DLL_EXTERN uls_nextch_detail_ptr_t ulsjava_get_nextch_info(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void ulsjava_put_nextch_info(uls_nextch_detail_ptr_t detail_ch);
ULS_DLL_EXTERN int ulsjava_get_ch_from_nextch(uls_nextch_detail_ptr_t detail_ch);
ULS_DLL_EXTERN int ulsjava_get_tok_from_nextch(uls_nextch_detail_ptr_t detail_ch);
#endif
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_UNGET_H__
