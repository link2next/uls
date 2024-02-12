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
 * uls_onechar.c -- manipulating 1-chars tokens. --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_ONECHAR__
#include "uls/onechar.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__init_onechar_tokgrp)(uls_onechar_table_ptr_t tbl, int grp_id, int ch0, int n)
{
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int i;

	tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), grp_id);
	tokgrp->ch0 = ch0;

	uls_init_parray(uls_ptr(tokgrp->tokdef_vx_1char), tokdef_vx, n);
	slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
	for (i = 0; i < n; i++) {
		slots_vx[i] = nilptr;
	}
	tokgrp->tokdef_vx_1char.n = n;
}

void
ULS_QUALIFIED_METHOD(uls_init_onechar_table)(uls_onechar_table_ptr_t tbl)
{
	uls_init_array_type00(uls_ptr(tbl->tokgrps), onechar_tokgrp, ULS_N_ONECHAR_TOKGRPS);

	// GROUP-0: '!' ~ '/'
	__init_onechar_tokgrp(tbl, 0, '!', 15);

	// GROUP-1: ':' ~ '@'
	__init_onechar_tokgrp(tbl, 1, ':', 7);

	// GROUP-2: '[' ~ '`'
	__init_onechar_tokgrp(tbl, 2, '[', 6);

	// GROUP-3: '{' ~ '~'
	__init_onechar_tokgrp(tbl, 3, '{', 4);

	tbl->tokdefs_etc_list = nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_onechar_table)(uls_onechar_table_ptr_t tbl)
{
	uls_onechar_tokdef_etc_ptr_t  e_etc, e_etc_next;
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	int i, j;

	for (e_etc = tbl->tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc_next) {
		e_etc_next = e_etc->next;
		e_vx = e_etc->tokdef_vx;
		uls_destroy_tokdef_vx(e_vx);
		uls_dealloc_object(e_etc);
	}
	tbl->tokdefs_etc_list = nilptr;

	for (i = 0; i < ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), i);

		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		for (j=0; j < tokgrp->tokdef_vx_1char.n; j++) {
			if ((e_vx = slots_vx[j]) != nilptr) {
				uls_destroy_tokdef_vx(e_vx);
			}
		}
		uls_deinit_parray(uls_ptr(tokgrp->tokdef_vx_1char));
	}
	uls_deinit_array_type00(uls_ptr(tbl->tokgrps), onechar_tokgrp);
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(find_1char_tokdef_map)(uls_onechar_table_ptr_t tbl,
	int ch, uls_tokdef_outparam_ptr_t outparam)
{
	//
	// group[0] = { '!' ... '/' }
	// group[1] = { ':' ... '@' }
	// group[2] = { '[' ... '`' }
	// group[3] = { '{' ... '~' }
	//
	//                           group  ...  group
	//                             0   1   2   3
	static int   left_child[4] = { -1, -1,  1, -1 };
	static int  right_child[4] = {  2, -1,  3, -1 };

	uls_tokdef_vx_ptr_t e_vx_ret = nilptr;
	int  i_grp, n_tok_array;
	uls_onechar_tokgrp_ptr_t tokgrp, tokgrp_ret = nilptr;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int ch0;

	for (i_grp = 0; i_grp >= 0; ) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), i_grp);

		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		n_tok_array = tokgrp->tokdef_vx_1char.n;
		ch0 = tokgrp->ch0;

		if (ch < ch0) {
			i_grp = left_child[i_grp];
		} else if (ch >= ch0 + n_tok_array) {
			i_grp = right_child[i_grp];
		} else {
			e_vx_ret = slots_vx[ch - ch0];
			tokgrp_ret = tokgrp;
			break;
		}
	}

	if (outparam != nilptr) {
		outparam->tokgrp = tokgrp_ret;
		outparam->e_vx = e_vx_ret;
	}

	return e_vx_ret;
}

void
ULS_QUALIFIED_METHOD(insert_1char_tokdef_map)(uls_onechar_tokgrp_ptr_t tokgrp,
	int ch, uls_tokdef_vx_ptr_t e_vx)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(tokgrp->tokdef_vx_1char));
	int j;

	if ((j = ch - tokgrp->ch0) >= tokgrp->tokdef_vx_1char.n || ch < tokgrp->ch0) {
		_uls_log(err_log)("%s: InternalError, %c out of tokgrp", __func__, ch);
		return;
	}

	if (slots_vx[j] != nilptr) {
		_uls_log(err_log)("%s: the slot for '0x%X already occupied!", __func__, ch);
	}

	slots_vx[j] = e_vx;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(find_1char_tokdef_etc)(uls_onechar_table_ptr_t tbl, uls_wch_t wch)
{
	uls_onechar_tokdef_etc_ptr_t e_etc;

	for (e_etc = tbl->tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		if (e_etc->wch == wch) {
			uls_assert(e_etc->tokdef_vx != nilptr);
			return e_etc->tokdef_vx;
		}
	}

	return nilptr;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(find_1char_tokdef_etc_by_tokid)(uls_onechar_table_ptr_t tbl, int tok_id)
{
	uls_onechar_tokdef_etc_ptr_t e_etc;
	uls_tokdef_vx_ptr_t e_vx, e_vx_ret = nilptr;

	for (e_etc = tbl->tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		e_vx = e_etc->tokdef_vx;
		if (e_vx->tok_id == tok_id) {
			e_vx_ret = e_vx;
			break;
		}
	}

	return e_vx_ret;
}

void
ULS_QUALIFIED_METHOD(insert_1char_tokdef_etc)(uls_onechar_table_ptr_t tbl,
	uls_wch_t wch, uls_tokdef_vx_ptr_t e_vx)
{
	uls_onechar_tokdef_etc_ptr_t  e_etc;

	e_etc = uls_alloc_object(uls_onechar_tokdef_etc_t);
	e_etc->wch = wch;
	e_etc->tokdef_vx = e_vx;

	e_etc->next = tbl->tokdefs_etc_list;
	tbl->tokdefs_etc_list = e_etc;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_1char_tokdef_vx)(uls_onechar_table_ptr_t tbl, int ch,
	uls_tokdef_outparam_ptr_t outparam)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_outparam_t parms1;

	if (ch < 0) return nilptr;
	if ((e_vx = find_1char_tokdef_map(tbl, ch, uls_ptr(parms1))) == nilptr &&
		parms1.tokgrp == nilptr) {
		e_vx = find_1char_tokdef_etc(tbl, ch);
	}

	if (outparam != nilptr) {
		outparam->e_vx = e_vx;
		outparam->tokgrp = parms1.tokgrp;
	}

	return e_vx;
}
