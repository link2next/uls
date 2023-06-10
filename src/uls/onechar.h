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
 * uls_onechar.h -- manipulating 1-chars tokens. --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_ONECHAR_H__
#define __ULS_ONECHAR_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_tokdef.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE

ULS_DEFINE_STRUCT(uls_onechar_tokgrp)
{
	uls_uch_t uch0;
	uls_decl_parray(tokdef_vx_1char, tokdef_vx);
};
ULS_DEF_ARRAY_THIS_TYPE01(onechar_tokgrp, N_ONECHAR_TOKGRPS);

ULS_DEFINE_STRUCT(uls_onechar_tokdef_etc)
{
	uls_uch_t uch;
	uls_tokdef_vx_ptr_t tokdef_vx;
	uls_onechar_tokdef_etc_ptr_t next;
};

ULS_DEFINE_STRUCT(uls_onechar_table)
{
	uls_decl_array_this_type01(tokgrps, onechar_tokgrp, N_ONECHAR_TOKGRPS);
	uls_onechar_tokdef_etc_ptr_t tokdefs_etc_list;

	uls_decl_parray(tokdef_vx_pool_1ch, tokdef_vx);
};

ULS_DEFINE_STRUCT(uls_tokdef_outparam)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_onechar_tokgrp_ptr_t tokgrp;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_ONECHAR__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC void __init_onechar_tokgrp(uls_onechar_table_ptr_t tbl, int grp_id, uls_uch_t uch0, int n);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_onechar_tokgrp(uls_onechar_tokgrp_ptr_t tokgrp);
void uls_deinit_onechar_tokgrp(uls_onechar_tokgrp_ptr_t tokgrp);

void uls_init_onechar_table(uls_onechar_table_ptr_t tbl);
void uls_deinit_onechar_table(uls_onechar_table_ptr_t tbl);

uls_tokdef_vx_ptr_t uls_find_1char_tokdef_map(uls_onechar_table_ptr_t tbl, uls_uch_t uch, uls_tokdef_outparam_ptr_t outparam);
void uls_insert_onechar_tokdef_map(uls_onechar_tokgrp_ptr_t tokgrp, uls_uch_t uch, uls_tokdef_vx_ptr_t e_vx);

uls_tokdef_vx_ptr_t uls_find_1char_tokdef_etc(uls_onechar_table_ptr_t tbl, uls_uch_t uch);
void uls_insert_onechar_tokdef_etc(uls_onechar_table_ptr_t tbl, uls_uch_t uch, uls_tokdef_vx_ptr_t e_vx);

uls_tokdef_vx_ptr_t uls_find_1char_tokdef_vx(uls_onechar_table_ptr_t tbl, uls_uch_t uch,
	uls_tokdef_outparam_ptr_t outparam);
int uls_insert_1char_tokdef_vx(uls_onechar_table_ptr_t tbl, uls_uch_t uch, uls_tokdef_vx_ptr_t e_vx);
uls_tokdef_vx_ptr_t uls_insert_1char_tokdef_uch(uls_onechar_table_ptr_t tbl, uls_uch_t uch);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_ONECHAR_H__
