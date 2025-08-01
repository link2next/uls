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
 * uls_twoplus.c -- manipulating tokens having more than 1-chars --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_TWOPLUS__
#include "uls/twoplus.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(__twoplus_bi_search)(const char *line_keyw,
	uls_ref_parray(ary,tokdef), int n_ary)
{
	uls_decl_parray_slots_init(slots_ary, tokdef, ary);
	int   low, high, mid, cmp;
	uls_tokdef_ptr_t e;

	const char *lptr, *lptr2;
	int ch1, ch2;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e = slots_ary[mid];

		lptr = line_keyw;
		lptr2 = uls_get_namebuf_value(e->keyword);
		for (cmp = 0; (ch2 = *lptr2) != '\0'; lptr++, lptr2++) {
			ch1 = *lptr;
			if (ch1 != ch2) {
				cmp = ch1 - ch2;
				break;
			}
		}

		if (cmp < 0) {
			high = mid - 1;
		} else if (cmp > 0) {
			low = mid + 1;
		} else {
			return e;
		}
	}

	return nilptr;
}

int
ULS_QUALIFIED_METHOD(cmp_twoplus_by_length)(uls_const_voidptr_t a, uls_const_voidptr_t b)
{
	const uls_tokdef_ptr_t e1 = (const uls_tokdef_ptr_t) a;
	const uls_tokdef_ptr_t e2 = (const uls_tokdef_ptr_t) b;
	int stat;

	if (e1->wlen_keyword < e2->wlen_keyword) stat = 1;
	else if (e1->wlen_keyword > e2->wlen_keyword) stat = -1;
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(cmp_twoplus_vx_by_keyword)(uls_const_voidptr_t a, uls_const_voidptr_t b)
{
	const uls_tokdef_ptr_t e1 = (const uls_tokdef_ptr_t) a;
	const uls_tokdef_ptr_t e2 = (const uls_tokdef_ptr_t) b;
	// return -1 if keyword1 < keyword2
	// return  1 if keyword1 > keyword2
	// return  0 if keyword1 == keyword2
	return _uls_tool_(strcmp)(uls_get_namebuf_value(e1->keyword), uls_get_namebuf_value(e2->keyword));
}

void
ULS_QUALIFIED_METHOD(uls_init_2char_tree)(uls_twoplus_tree_ptr_t tree)
{
	uls_initial_zerofy_object(tree);
	uls_init_parray(uls_ptr(tree->twoplus_sorted), tokdef, 0);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_2char_tree)(uls_twoplus_tree_ptr_t tree)
{
	uls_deinit_parray(uls_ptr(tree->twoplus_sorted));
}

void
ULS_QUALIFIED_METHOD(uls_init_2char_table)(uls_kwtable_twoplus_ptr_t tbl)
{
	uls_twoplus_tree_ptr_t tree;
	int i;

	uls_initial_zerofy_object(tbl);
	uls_init_array_type00(uls_ptr(tbl->tree_array), twoplus_tree, ULS_KWTABLE_TWOPLUS_SIZE);

	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_get_array_slot_type00(uls_ptr(tbl->tree_array), i);
		uls_init_2char_tree(tree);
	}
	uls_init_parray(uls_ptr(tbl->twoplus_mempool), tokdef, 0);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_2char_table)(uls_kwtable_twoplus_ptr_t tbl)
{
	uls_twoplus_tree_ptr_t tree;
	int i;

	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_get_array_slot_type00(uls_ptr(tbl->tree_array), i);
		uls_deinit_2char_tree(tree);
	}

	uls_deinit_array_type00(uls_ptr(tbl->tree_array), twoplus_tree);
	uls_deinit_parray(uls_ptr(tbl->twoplus_mempool));
}

ULS_QUALIFIED_RETTYP(uls_twoplus_tree_ptr_t)
ULS_QUALIFIED_METHOD(uls_get_ind_twoplus_tree)(uls_kwtable_twoplus_ptr_t tbl, int wlen_keyw, uls_ptrtype_tool(outparam) parms)
{
	// wlen_keyw >= 1 and ind < ULS_TWOPLUS_WMAXLEN
	int ind;

	if ((ind = wlen_keyw - 1) < 0 || ind >= ULS_KWTABLE_TWOPLUS_SIZE) {
		return nilptr;
	}

	if (parms != nilptr) {
		parms->n = ind;
	}

	return uls_get_array_slot_type00(uls_ptr(tbl->tree_array), ind);
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(is_keyword_twoplus)(uls_kwtable_twoplus_ptr_t tbl, const char *ch_ctx, const char *line)
{
	uls_twoplus_tree_ptr_t tree;
	uls_tokdef_vx_ptr_t e_vx_ret;
	uls_tokdef_ptr_t e;
	int i, n, wlen;
	char ch;

	if ((tree = tbl->start) == nilptr)
		return nilptr;

	n = tree->wlen_keyw * ULS_UTF8_CH_MAXLEN;
	for (i = 0; ; i++) {
		if (i >= n) {
			wlen = ULS_TWOPLUS_WMAXLEN;
			break;
		}

		ch = line[i];
		if (uls_canbe_ch_space(ch_ctx, ch)) {
			wlen = _uls_tool(ustr_num_wchars)(line, i, nilptr);
			if (wlen > ULS_TWOPLUS_WMAXLEN) wlen = ULS_TWOPLUS_WMAXLEN;
			break;
		}
	}

	tree = uls_get_ind_twoplus_tree(tbl, wlen, nilptr);
	if (tree->wlen_keyw <= 0) {
		tree = tree->prev;
	}

	for (e_vx_ret = nilptr; tree != nilptr; tree = tree->prev) {
		e = __twoplus_bi_search(line, uls_ptr(tree->twoplus_sorted), tree->twoplus_sorted.n);
		if (e != nilptr) {
			e_vx_ret = e->view;
			break;
		}
	}

	return e_vx_ret;
}

void
ULS_QUALIFIED_METHOD(distribute_twoplus_toks)(uls_kwtable_twoplus_ptr_t tbl, char *ch_ctx)
{
	uls_decl_parray_slots_init(slots_keyw, tokdef, uls_ptr(tbl->twoplus_mempool));
	uls_decl_parray_slots(slots_tp, tokdef);
	int n_slots_keyw = tbl->twoplus_mempool.n;

	uls_twoplus_tree_ptr_t tree, tree_prev;
	uls_twoplus_tree_ptr_t tree_list, tree_listtail;

	uls_tokdef_ptr_t e;
	int i, i0, j, n, wlen0_keyw;
	char ch_kwd;

	if (n_slots_keyw <= 0) return;
	_uls_quicksort_vptr(slots_keyw, n_slots_keyw, cmp_twoplus_by_length);

	for (i=0; i<n_slots_keyw; i++) {
		e = slots_keyw[i];
		if ((ch_kwd = uls_get_namebuf_value(e->keyword)[0]) < ULS_SYNTAX_TABLE_SIZE) {
			ch_ctx[ch_kwd] |= ULS_CH_2PLUS;
		}
	}

	tree_list = tree_listtail = nilptr;
	for (i=0; i<n_slots_keyw; ) {
		e = slots_keyw[i];

		wlen0_keyw = e->wlen_keyword;
		tree = uls_get_ind_twoplus_tree(tbl, wlen0_keyw, nilptr);

		tree->wlen_keyw = wlen0_keyw;
		for (i0 = i++; i < n_slots_keyw; i++) {
			e = slots_keyw[i];
			if (e->wlen_keyword != wlen0_keyw) break;
		}

		n = i - i0;
		uls_resize_parray(uls_ptr(tree->twoplus_sorted), tokdef, n);
		slots_tp = uls_parray_slots(uls_ptr(tree->twoplus_sorted));
		for (j = 0; j < n; j++) {
			slots_tp[j] = slots_keyw[i0 + j];
		}
		_uls_quicksort_vptr(slots_tp, n, cmp_twoplus_vx_by_keyword);
		tree->twoplus_sorted.n = n;

		if (tree_listtail != nilptr) {
			tree_listtail->prev = tree;
			tree_listtail = tree;
			tree_listtail->prev = nilptr;
		} else {
			tree_list = tree_listtail = tree;
		}
	}

	// set tbl->start to the list [tree_list..tree_listtail]
	tbl->start = tree_list;

	// Let tree_array[i] have a valid 'prev' for all i
	tree_prev = nilptr;
	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_get_array_slot_type00(uls_ptr(tbl->tree_array), i);

		if (tree->wlen_keyw <= 0) {
			tree->prev = tree_prev;
		} else {
			tree_prev = tree;
		}
	}
}
