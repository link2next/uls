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
 * idkeyw.h -- manipulating id-style keywords. --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_IDKEYW_H__
#define __ULS_IDKEYW_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_tokdef.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(hashfunc, int)(uls_voidptr_t tbl_info, const char *name);
ULS_DEFINE_DELEGATE_END(hashfunc);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(uls_dflhash_state)
{
	int        n_slots;
	int        n_shifts;
	uls_uint32     init_hcode;
};

ULS_DEFINE_STRUCT(uls_kwtable)
{
	uls_voidptr_t  hash_stat;
	uls_decl_parray(bucket_head, tokdef);

	_uls_callback_type_this_(strcmp_proc) str_ncmp;
	_uls_callback_type_this_(hashfunc) hashfunc;

	uls_dflhash_state_t dflhash_stat;
};

ULS_DEFINE_STRUCT(uls_keyw_stat)
{
	const char *keyw;
	int freq;
	uls_tokdef_ptr_t keyw_info;
};
ULS_DEF_PARRAY_THIS(keyw_stat);

ULS_DEFINE_STRUCT(uls_keyw_stat_list)
{
	uls_decl_parray(lst, keyw_stat);
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_IDKEYW__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __keyw_strncmp_case_sensitive(const char* str1, const char* str2, int len);
ULS_DECL_STATIC int __keyw_hashfunc_case_sensitive(uls_voidptr_t tbl_info, const char *name);
ULS_DECL_STATIC int __keyw_strncmp_case_insensitive(const char* wrd, const char* keyw, int len);
ULS_DECL_STATIC int __keyw_hashfunc_case_insensitive(uls_voidptr_t tbl_info, const char *name);
ULS_DECL_STATIC void __init_kwtable_buckets(uls_kwtable_ptr_t tbl, int n_slots);
ULS_DECL_STATIC int __export_kwtable(uls_kwtable_ptr_t tbl, uls_ref_parray(lst,keyw_stat), int n_lst);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void __init_kwtable(uls_kwtable_ptr_t tbl);
void uls_init_kwtable(uls_kwtable_ptr_t tbl, int n_slots, int case_insensitive);
void uls_reset_kwtable(uls_kwtable_ptr_t tbl, int n_slots, uls_hashfunc_t hashfunc, uls_voidptr_t hash_stat);
void uls_deinit_kwtable(uls_kwtable_ptr_t tbl);

uls_tokdef_ptr_t uls_find_kw(uls_kwtable_ptr_t tbl, _uls_tool_ptrtype_(outparam) parms);
int uls_add_kw(uls_kwtable_ptr_t tbl, uls_tokdef_ptr_t e);

int sizeof_kwtable(uls_kwtable_ptr_t tbl);
uls_tokdef_ptr_t is_keyword_idstr(uls_kwtable_ptr_t tbl, const char* keyw, int l_keyw);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
int keyw_stat_comp_by_keyw(const uls_voidptr_t a, const uls_voidptr_t b);

ULS_DLL_EXTERN uls_hashfunc_t uls_get_hashfunc(const char* hashname, int case_insensitive);
ULS_DLL_EXTERN uls_keyw_stat_list_ptr_t ulc_export_kwtable(uls_kwtable_ptr_t tbl);
ULS_DLL_EXTERN uls_keyw_stat_ptr_t ulc_search_kwstat_list(uls_keyw_stat_list_ptr_t kwslst, const char* str);
ULS_DLL_EXTERN void ulc_free_kwstat_list(uls_keyw_stat_list_ptr_t kwslst);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_IDKEYW_H__
