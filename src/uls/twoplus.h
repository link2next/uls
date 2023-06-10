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
 * uls_twoplus.h -- manipulating tokens having more than 1-chars --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_TWOPLUS_H__
#define __ULS_TWOPLUS_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_tokdef.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(uls_twoplus_tree)
{
	int len_keyw;

	uls_decl_parray(twoplus_sorted, tokdef_vx);
	uls_twoplus_tree_ptr_t prev;
};
ULS_DEF_ARRAY_THIS_TYPE01(twoplus_tree, ULS_KWTABLE_TWOPLUS_SIZE);

ULS_DEFINE_STRUCT(uls_kwtable_twoplus)
{
	uls_decl_array_this_type01(tree_array, twoplus_tree, ULS_KWTABLE_TWOPLUS_SIZE);

	uls_twoplus_tree_ptr_t  start;
	_uls_callback_type_this_(strcmp_proc) str_ncmp;

	uls_decl_parray(twoplus_mempool, tokdef_vx);
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_TWOPLUS__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __twoplus_bi_search(uls_kwtable_twoplus_ptr_t tbl, const char* kwrd,
	uls_ref_parray(ary,tokdef_vx), int n_ary);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_twoplus_tree(uls_twoplus_tree_ptr_t tree);
void uls_deinit_twoplus_tree(uls_twoplus_tree_ptr_t tree);

void uls_init_kwtable_twoplus(uls_kwtable_twoplus_ptr_t tbl);
void uls_deinit_kwtable_twoplus(uls_kwtable_twoplus_ptr_t tbl);
uls_twoplus_tree_ptr_t uls_get_ind_twoplus_tree(uls_kwtable_twoplus_ptr_t tbl, int len_twoplus, _uls_tool_ptrtype_(outparam) parms);

int cmp_twoplus_by_length(const uls_voidptr_t a, const uls_voidptr_t b);
int cmp_twoplus_vx_by_keyword(const uls_voidptr_t a, const uls_voidptr_t b);

void distribute_twoplus_toks(uls_kwtable_twoplus_ptr_t tbl, uls_strcmp_proc_t cmpfunc);
uls_tokdef_ptr_t is_keyword_twoplus(uls_kwtable_twoplus_ptr_t tbl, const char *ch_ctx, const char* str);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_TWOPLUS_H__
