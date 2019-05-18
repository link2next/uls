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
  <file> uls_util.h </file>
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

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"

#include <stdlib.h>
#include <errno.h>

#ifndef ULS_WINDOWS
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#endif // ULS_EXCLUDE_HFILES

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define uls_heaparray_parent(i) ((((i)+1)>>1)-1)
#define uls_heaparray_lchild(i) ((((i)+1)<<1)-1)
#define uls_heaparray_rchild(i) (uls_heaparray_lchild(i)+1)
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
#define _uls_quicksort_vptr(ary,n,cmpfunc) \
	_uls_tool_(quick_sort_vptr)(_uls_cast_array_type(uls_voidptr_t)ary,n,_uls_ref_callback_this(cmpfunc))

#define _uls_bisearch_vptr(keyw,ary,n,cmpfunc) \
	_uls_tool_(bi_search_vptr)(keyw,(_uls_type_array(uls_voidptr_t))ary,n,_uls_ref_callback_this(cmpfunc))

ULS_DECLARE_STRUCT(uls_isp);
ULS_DECLARE_STRUCT(uls_cmd);
#endif // ULS_DECL_GLOBAL_TYPES

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(sort_cmpfunc,int)(const uls_voidptr_t a, const uls_voidptr_t b);
ULS_DEFINE_DELEGATE_END(sort_cmpfunc);

ULS_DEFINE_DELEGATE_BEGIN(bi_comp,int)(const uls_voidptr_t a, const uls_voidptr_t keyw);
ULS_DEFINE_DELEGATE_END(bi_comp);

ULS_DEFINE_DELEGATE_BEGIN(cmd_proc,int)(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DEFINE_DELEGATE_END(cmd_proc);
#endif

#ifdef ULS_DEF_PROTECTED_TYPE
ULS_DEFINE_STRUCT(uls_heaparray)
{
  _uls_decl_array(ary,uls_voidptr_t);
  unsigned int n_ary, ary_siz;
  uls_sort_cmpfunc_t cmpfunc;
};

ULS_DEFINE_STRUCT(uls_obj4sort)
{
	int idx;
	char *vptr;
	uls_sort_cmpfunc_t cmpfunc;
};
ULS_DEF_PARRAY_THIS(obj4sort);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(uls_isp)
{
	char *buff;
	int  len_strpool, siz_strpool;
	int  align_strpool;
};

ULS_DEFINE_STRUCT(uls_cmd)
{
	const char     *name;
	uls_cmd_proc_t proc;
	uls_voidptr_t  user_data;
};
#endif

#if defined(__ULS_MISC__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC unsigned int uls_gauss_log2(unsigned int n, _uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC void downheap_vptr(uls_heaparray_ptr_t hh, unsigned int i0);
ULS_DECL_STATIC void extract_top_vptr(uls_heaparray_ptr_t hh);
ULS_DECL_STATIC int get_ms_codepage(_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int replace_cr_guard(char *buff, int n, char crlf2chlf);
ULS_DECL_STATIC char check_eof_cr_lf(char *ptr_ch);

#ifndef ULS_DOTNET
ULS_DECL_STATIC int sortcmp_obj4sort(const uls_voidptr_t a, const uls_voidptr_t b);
#endif
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void build_heaptree_vptr(uls_heaparray_ptr_t hh,
	_uls_decl_array(ary,uls_voidptr_t), unsigned int n, uls_sort_cmpfunc_t cmpfunc);
int __initialize_uls_misc(void);
void __finalize_uls_misc(void);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
int uls_get_simple_escape_char(uls_outparam_ptr_t parms);
int uls_get_simple_escape_str(char quote_ch, uls_outparam_ptr_t parms);

void isp_init(uls_isp_ptr_t isp, int init_size);
void isp_reset(uls_isp_ptr_t isp);
void isp_deinit(uls_isp_ptr_t isp);
char* isp_find(uls_isp_ptr_t isp, const char* str, int len);
char* isp_insert(uls_isp_ptr_t isp, const char* str, int len);

int splitint(const char* line, _uls_tool_ptrtype_(outparam) parms);
int canbe_tokname(const char *str);

const char* uls_skip_multiline_comment(_uls_tool_ptrtype_(parm_line) parm_ln);
const char* uls_skip_singleline_comment(_uls_tool_ptrtype_(parm_line) parm_ln);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN void uls_quick_sort(uls_native_vptr_t ary, int n_ary, int elmt_size, uls_sort_cmpfunc_t cmpfunc);
ULS_DLL_EXTERN uls_voidptr_t uls_bi_search(const uls_voidptr_t keyw, uls_native_vptr_t ary, int n_ary, int elmt_size, uls_bi_comp_t cmpfunc);
#endif

ULS_DLL_EXTERN void uls_quick_sort_vptr(_uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_sort_cmpfunc_t cmpfunc);
ULS_DLL_EXTERN uls_voidptr_t uls_bi_search_vptr(const uls_voidptr_t kwrd, _uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_bi_comp_t cmpfunc);
ULS_DLL_EXTERN int uls_cmd_run(uls_array_ref_slots_this_type01(cmdlst,cmd), int n_cmdlst, const char* kwrd, char *line, uls_voidptr_t data);

ULS_DLL_EXTERN char uls_replace_cr_carry(char ch_carry, char *buf, int n);
#if defined(ULS_WINDOWS) && !defined(ULS_DOTNET)
ULS_DLL_EXTERN char* uls_win32_lookup_regval(wchar_t* reg_dir, _uls_tool_ptrtype_(outparam) parms);
#endif

#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_MISC_H__
