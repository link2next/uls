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
  <file> uls_misc.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef __ULS_MISC_H__
#define __ULS_MISC_H__

#include "uls/uls_prim.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define OPT_NOT_OPTCHAR   0
#define OPT_USAGE_BRIEF   1
#define OPT_USAGE_DESC    2

#define uls_heaparray_parent(i) ((((i)+1)>>1)-1)
#define uls_heaparray_lchild(i) ((((i)+1)<<1)-1)
#define uls_heaparray_rchild(i) (uls_heaparray_lchild(i)+1)

#define _uls_quicksort_vptr(ary,n,cmpfunc) \
	uls_quick_sort_vptr(_uls_cast_array_type(uls_voidptr_t)ary,n,uls_ref_callback(cmpfunc))

#define _uls_bisearch_vptr(keyw,ary,n,cmpfunc) \
	uls_bi_search_vptr(keyw,(_uls_type_array(uls_voidptr_t))ary,n,uls_ref_callback(cmpfunc))

ULS_DEFINE_DELEGATE_BEGIN(sort_cmpfunc,int)(const uls_voidptr_t a, const uls_voidptr_t b);
ULS_DEFINE_DELEGATE_END(sort_cmpfunc);

ULS_DEFINE_DELEGATE_BEGIN(bi_comp,int)(const uls_voidptr_t a, const uls_voidptr_t keyw);
ULS_DEFINE_DELEGATE_END(bi_comp);

ULS_DECLARE_STRUCT(cmd);
ULS_DEFINE_DELEGATE_BEGIN(cmd_proc,int)(char *line, uls_cmd_ptr_t cmd);
ULS_DEFINE_DELEGATE_END(cmd_proc);

ULS_DEFINE_STRUCT(heaparray)
{
  _uls_decl_array(ary,uls_voidptr_t);
  unsigned int n_ary, ary_siz;
  uls_sort_cmpfunc_t cmpfunc;
};

ULS_DEFINE_STRUCT(obj4sort)
{
	int idx;
	char *vptr;
	uls_sort_cmpfunc_t cmpfunc;
};
ULS_DEF_PARRAY(obj4sort);

ULS_DEFINE_STRUCT(cmd)
{
	const char     *name;
	uls_cmd_proc_t proc;
	uls_voidptr_t  user_data;
};
ULS_DEF_ARRAY_TYPE00(cmd, CMD_TYPE00_ULC_N_LEXATTRS, ULC_N_LEXATTRS);

#if defined(__ULS_MISC__)
ULS_DECL_STATIC unsigned int uls_gauss_log2(unsigned int n, uls_outparam_ptr_t parms);
ULS_DECL_STATIC void downheap_vptr(uls_heaparray_ptr_t hh, unsigned int i0);
ULS_DECL_STATIC void extract_top_vptr(uls_heaparray_ptr_t hh);
ULS_DECL_STATIC int sortcmp_obj4sort(const uls_voidptr_t a, const uls_voidptr_t b);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int splitint(const char* line, uls_outparam_ptr_t parms);

const char* uls_skip_multiline_comment(uls_parm_line_ptr_t parm_ln);
const char* uls_skip_singleline_comment(uls_parm_line_ptr_t parm_ln);

void build_heaptree_vptr(uls_heaparray_ptr_t hh,
	_uls_decl_array(ary,uls_voidptr_t), unsigned int n, uls_sort_cmpfunc_t cmpfunc);
ULS_DLL_EXTERN void uls_quick_sort_vptr(_uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_sort_cmpfunc_t cmpfunc);
ULS_DLL_EXTERN uls_voidptr_t uls_bi_search_vptr(const uls_voidptr_t keyw, _uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_bi_comp_t cmpfunc);

int uls_get_simple_escape_char(uls_outparam_ptr_t parms);
int uls_get_simple_escape_str(char quote_ch, uls_outparam_ptr_t parms);

int uls_get_simple_unescape_char(int ch);
int uls_get_simple_unescape_str(uls_outparam_ptr_t parms);

FILE* uls_get_spec_fp(const char* dirpath_list, const char* fpath, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int uls_cmd_run(uls_array_ref_slots_type00(cmdlst,cmd), int n_cmdlst, const char* keyw, char *line, uls_voidptr_t data);

ULS_DLL_EXTERN void uls_quick_sort(uls_native_vptr_t ary, int n_ary, int elmt_size, uls_sort_cmpfunc_t cmpfunc);
ULS_DLL_EXTERN uls_voidptr_t uls_bi_search(const uls_voidptr_t keyw, uls_native_vptr_t ary, int n_ary, int elmt_size, uls_bi_comp_t cmpfunc);
#if defined(ULS_WINDOWS)
ULS_DLL_EXTERN char* uls_win32_lookup_regval(wchar_t* reg_dir, uls_outparam_ptr_t parms);
#endif

#endif // ULS_DECL_PROTECTED_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_MISC_H__

