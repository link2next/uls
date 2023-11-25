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
ULS_QUALIFIED_METHOD(__init_onechar_tokgrp)(uls_onechar_table_ptr_t tbl, int grp_id, uls_wch_t uch0, int n)
{
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int i;

	tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), grp_id);
	tokgrp->uch0 = uch0;

	uls_init_parray(uls_ptr(tokgrp->tokdef_vx_1char), tokdef_vx, n);
	slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
	for (i=0; i<n; i++) {
		slots_vx[i] = nilptr;
	}
	tokgrp->tokdef_vx_1char.n = n;
}

void
ULS_QUALIFIED_METHOD(uls_init_onechar_table)(uls_onechar_table_ptr_t tbl)
{
	uls_init_array_type00(uls_ptr(tbl->tokgrps), onechar_tokgrp, ULS_N_ONECHAR_TOKGRPS);

	uls_init_parray(uls_ptr(tbl->tokdef_vx_pool_1ch), tokdef_vx, 0);

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
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	for (e_etc = tbl->tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc_next) {
		e_etc_next = e_etc->next;
		uls_dealloc_object(e_etc);
	}

	tbl->tokdefs_etc_list = nilptr;

	slots_vx = uls_parray_slots(uls_ptr(tbl->tokdef_vx_pool_1ch));
	for (i=0; i < tbl->tokdef_vx_pool_1ch.n; i++) {
		e_vx = slots_vx[i];
		uls_destroy_tokdef_vx(e_vx);
	}

	uls_deinit_parray(uls_ptr(tbl->tokdef_vx_pool_1ch));

	for (i=0; i<ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), i);
		uls_deinit_parray(uls_ptr(tokgrp->tokdef_vx_1char));
	}
	uls_deinit_array_type00(uls_ptr(tbl->tokgrps), onechar_tokgrp);
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_1char_tokdef_map)(uls_onechar_table_ptr_t tbl,
	uls_wch_t wch, uls_tokdef_outparam_ptr_t outparam)
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
	uls_wch_t uch0;

	for (i_grp = 0; i_grp >= 0; ) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(tbl->tokgrps), i_grp);

		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		n_tok_array = tokgrp->tokdef_vx_1char.n;
		uch0 = tokgrp->uch0;

		if (wch < uch0) {
			i_grp = left_child[i_grp];
		} else if (wch >= uch0 + n_tok_array) {
			i_grp = right_child[i_grp];
		} else {
			e_vx_ret = slots_vx[wch - uch0];
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
ULS_QUALIFIED_METHOD(uls_insert_onechar_tokdef_map)(uls_onechar_tokgrp_ptr_t tokgrp,
	uls_wch_t wch, uls_tokdef_vx_ptr_t e_vx)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(tokgrp->tokdef_vx_1char));
	int j;

	uls_assert(tokgrp->uch0 <= wch);

	j = wch - tokgrp->uch0;
	uls_assert(j < tokgrp->tokdef_vx_1char.n);

	if (slots_vx[j] != nilptr) {
		_uls_log(err_log)("%s: the slot for '0x%X already occupied!", __func__, wch);
	}

	slots_vx[j] = e_vx;
}

void
ULS_QUALIFIED_METHOD(uls_insert_onechar_tokdef_etc)
(uls_onechar_table_ptr_t tbl, uls_wch_t wch, uls_tokdef_vx_ptr_t e_vx)
{
	uls_onechar_tokdef_etc_ptr_t  e_etc;

	uls_assert(e_vx != nilptr);

	e_etc = uls_alloc_object(uls_onechar_tokdef_etc_t);

	e_etc->wch = wch;
	e_etc->tokdef_vx = e_vx;
	e_etc->next = tbl->tokdefs_etc_list;
	tbl->tokdefs_etc_list = e_etc;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_1char_tokdef_etc)(uls_onechar_table_ptr_t tbl, uls_wch_t wch)
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
ULS_QUALIFIED_METHOD(uls_find_1char_tokdef_vx)(uls_onechar_table_ptr_t tbl, int ch,
	uls_tokdef_outparam_ptr_t outparam)
{
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx=uls_find_1char_tokdef_map(tbl, ch, outparam)) == nilptr) {
		e_vx = uls_find_1char_tokdef_etc(tbl, ch);

		if (outparam != nilptr) {
			outparam->e_vx = e_vx;
			outparam->tokgrp = nilptr;
		}
	}

	return e_vx;
}

int
ULS_QUALIFIED_METHOD(uls_insert_1char_tokdef_vx)(uls_onechar_table_ptr_t tbl,
	uls_wch_t wch, uls_tokdef_vx_ptr_t e_vx)
{
	uls_tokdef_outparam_t outparam;
	int stat;

	// why wch is needed? Can we use the e_vx->tok_id instead!?
	if (uls_find_1char_tokdef_vx(tbl, wch, uls_ptr(outparam)) != nilptr) {
		return 0;
	}

	if (outparam.tokgrp != nilptr) {
		uls_insert_onechar_tokdef_map(outparam.tokgrp, wch, e_vx);
		stat = 1;
	} else {
		uls_insert_onechar_tokdef_etc(tbl, wch, e_vx);
		stat = 2;
	}

	return stat;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_insert_1char_tokdef_uch)(uls_onechar_table_ptr_t tbl, uls_wch_t wch)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int siz;

	if (tbl->tokdef_vx_pool_1ch.n_alloc < (siz = tbl->tokdef_vx_pool_1ch.n + 1)) {
		siz = uls_roundup(siz, TOKDEF_LINES_DELTA2);
		uls_resize_parray(uls_ptr(tbl->tokdef_vx_pool_1ch), tokdef_vx, siz);
	}

	e_vx = uls_create_tokdef_vx((int) wch, "", nilptr);
	uls_insert_1char_tokdef_vx(tbl, wch, e_vx);

	slots_vx = uls_parray_slots(uls_ptr(tbl->tokdef_vx_pool_1ch));
	slots_vx[tbl->tokdef_vx_pool_1ch.n++] = e_vx;

	return e_vx;
}
