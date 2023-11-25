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
 * uls_tokdef.h -- token definitions of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_TOKDEF_H__
#define __ULS_TOKDEF_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
#define ULS_KEYW_TYPE_IDSTR      0
#define ULS_KEYW_TYPE_TWOPLUS    1
#define ULS_KEYW_TYPE_1CHAR      2
#define ULS_KEYW_TYPE_RESERVED   3
#define ULS_KEYW_TYPE_LITERAL    4
#define ULS_KEYW_TYPE_USER       5

#define ULS_VX_RSVD              0x01
#define ULS_VX_TOKID_CHANGED     0x02
#define ULS_VX_TOKNAM_CHANGED    0x04
#define ULS_VX_ANONYMOUS         0x08

ULS_DECLARE_STRUCT(tokdef_vx);
ULS_DECLARE_STRUCT(tokdef);
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(strcmp_proc,int)(const char* wrd1, const char* wrd2, int len);
ULS_DEFINE_DELEGATE_END(strcmp_proc);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(tokdef_name)
{
	uls_flags_t flags; // ULS_VX_TOKNAM_CHANGED
	uls_def_namebuf(name, ULS_LEXSTR_MAXSIZ);
	uls_tokdef_vx_ptr_t view;
	uls_tokdef_name_ptr_t prev;
};
ULS_DEF_PARRAY(tokdef_name);

ULS_DEFINE_STRUCT_BEGIN(tokdef_vx)
{
	uls_flags_t flags;
	int  tok_id; // primary

	uls_def_namebuf(name, ULS_LEXSTR_MAXSIZ);
	int  l_name;

	uls_voidptr_t extra_tokdef;
	uls_tokdef_ptr_t base; // list of tokdef
	uls_tokdef_name_ptr_t tokdef_names; // list of aliases
};
ULS_DEF_PARRAY(tokdef_vx);

ULS_DEFINE_STRUCT_BEGIN(tokdef)
{
	uls_def_namebuf(keyword, ULS_TWOPLUS_WMAXLEN*ULS_UTF8_CH_MAXLEN); // primary
	int  ulen_keyword, wlen_keyword;
	int  keyw_type;

	uls_tokdef_vx_ptr_t view;

	const char *name;

	// Hash link for same hash-value(keyword,ulen_keyword)
	uls_tokdef_ptr_t link;

	// The link for grounping the elements with same view->tok_id via tokdef_vx.base.
	uls_tokdef_ptr_t next;
};
ULS_DEF_PARRAY(tokdef);

#endif // ULS_DEF_PUBLIC_TYPE

#ifdef ULS_DECL_PROTECTED_PROC
void print_tokdef_vx_char(uls_wch_t wch, uls_tokdef_vx_ptr_t e_vx);
void uls_init_tokdef_vx(uls_tokdef_vx_ptr_t e_vx, int tok_id, const char* name, uls_tokdef_ptr_t e);
void uls_deinit_tokdef_vx(uls_tokdef_vx_ptr_t e_vx);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
int __is_in_ilist(int *ilst, int n_ilst, int val);

uls_tokdef_ptr_t uls_create_tokdef(void);
void uls_destroy_tokdef(uls_tokdef_ptr_t e);

void __init_tokdef_vx(uls_tokdef_vx_ptr_t e_vx);
uls_tokdef_vx_ptr_t uls_create_tokdef_vx(int tok_id, const char* name, uls_tokdef_ptr_t e);
void uls_destroy_tokdef_vx(uls_tokdef_vx_ptr_t e_vx);

uls_tokdef_name_ptr_t alloc_tokdef_name(const char *name, uls_tokdef_vx_ptr_t view);
void dealloc_tokdef_name(uls_tokdef_name_ptr_t e_nam);

uls_tokdef_name_ptr_t find_tokdef_name(uls_tokdef_vx_ptr_t e_vx_leader, const char* name, uls_ptrtype_tool(outparam) parms);
void insert_tokdef_name_to_group(uls_tokdef_vx_ptr_t e_vx_leader, uls_tokdef_name_ptr_t e_nam_prev, uls_tokdef_name_ptr_t e_nam);
int append_tokdef_name_to_group(uls_tokdef_vx_ptr_t e_vx_leader, uls_tokdef_name_ptr_t e_nam);

uls_tokdef_ptr_t search_tokdef_group(uls_tokdef_vx_ptr_t e_vx_leader, const char* keyw);
void append_tokdef_to_group(uls_tokdef_vx_ptr_t e_vx_leader, uls_tokdef_ptr_t e_target);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_tokdef_wstr.h"
#endif

#endif // __ULS_TOKDEF_H__
