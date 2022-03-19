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
#define __ULS_TWOPLUS__
#include "uls/twoplus.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC uls_tokdef_vx_ptr_t
__twoplus_bi_search(uls_kwtable_twoplus_ptr_t tbl, const char* keyw,
	uls_ref_parray(ary,tokdef_vx), int n_ary)
{
	uls_decl_parray_slots_init(slots_ary, tokdef_vx, ary);
	int   low, high, mid, cond;
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e_vx = slots_ary[mid];
		e = e_vx->base;

		if ((cond = tbl->str_ncmp(e->keyword, keyw, e->l_keyword)) < 0) {
			low = mid + 1;
		} else if (cond > 0) {
			high = mid - 1;
		} else {
			return e_vx;
		}
	}

	return nilptr;
}

int
cmp_twoplus_by_length(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1_vx = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2_vx = (const uls_tokdef_vx_ptr_t) b;
	uls_tokdef_ptr_t e1, e2;
	int stat;

	e1 = e1_vx->base;
	e2 = e2_vx->base;

	if (e1->l_keyword < e2->l_keyword) stat = 1;
	else if (e1->l_keyword > e2->l_keyword) stat = -1;
	else stat = 0;

	return stat;
}

int
cmp_twoplus_vx_by_keyword(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1_vx = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2_vx = (const uls_tokdef_vx_ptr_t) b;
	uls_tokdef_ptr_t e1, e2;

	e1 = e1_vx->base;
	e2 = e2_vx->base;

	// return -1 if keyword1 < keyword2
	// return  1 if keyword1 > keyword2
	// return  0 if keyword1 == keyword2
	return uls_strcmp(e1->keyword, e2->keyword);
}

void
uls_init_2char_tree(uls_twoplus_tree_ptr_t tree)
{
	uls_initial_zerofy_object(tree);
	uls_init_parray(uls_ptr(tree->twoplus_sorted), tokdef_vx, 0);
}

void
uls_deinit_2char_tree(uls_twoplus_tree_ptr_t tree)
{
	uls_deinit_parray(uls_ptr(tree->twoplus_sorted));
}

void
uls_init_2char_table(uls_kwtable_twoplus_ptr_t tbl)
{
	uls_twoplus_tree_ptr_t tree;
	int i;

	uls_initial_zerofy_object(tbl);

	uls_init_array_type00(uls_ptr(tbl->tree_array), twoplus_tree, ULS_KWTABLE_TWOPLUS_SIZE);
	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_array_get_slot_type00(uls_ptr(tbl->tree_array), i);
		uls_init_2char_tree(tree);
	}
}

void
uls_deinit_2char_table(uls_kwtable_twoplus_ptr_t tbl)
{
	uls_twoplus_tree_ptr_t tree;
	int i;

	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_array_get_slot_type00(uls_ptr(tbl->tree_array), i);
		uls_deinit_2char_tree(tree);
	}

	uls_deinit_array_type00(uls_ptr(tbl->tree_array), twoplus_tree);
	uls_deinit_parray(uls_ptr(tbl->twoplus_mempool));
}

uls_twoplus_tree_ptr_t
uls_get_ind_twoplus_tree(uls_kwtable_twoplus_ptr_t tbl, int len_twoplus, uls_outparam_ptr_t parms)
{
	int ind;

	if ((ind=len_twoplus-2) < 0 || ind >= ULS_KWTABLE_TWOPLUS_SIZE) {
		return nilptr;
	}

	if (parms != nilptr) {
		parms->n = ind;
	}

	return uls_array_get_slot_type00(uls_ptr(tbl->tree_array), ind);
}

uls_tokdef_ptr_t
is_keyword_twoplus(uls_kwtable_twoplus_ptr_t tbl, const char *ch_ctx, const char* line)
{
	uls_twoplus_tree_ptr_t tree;
	uls_tokdef_vx_ptr_t e_vx;
	char ch;
	int i;

	if ((tree=tbl->start) == nilptr)
		return nilptr;

	// tree->len_keyw >= 2 AND ch_ctx['\0'] == 0
	for (i=0; i < tree->len_keyw; i++) {
		ch = line[i];
		if (ch < ULS_SYNTAX_TABLE_SIZE && ch_ctx[ch] == 0) {
			tree = uls_get_ind_twoplus_tree(tbl, i, nilptr);
			if (tree != nilptr && tree->len_keyw <= 0)
				tree = tree->next;
			break;
			// what if tree == nilptr ?
		}
	}

	// If line[0] == '\0', then i == 0 AND tree == NULL
	for ( ; tree != nilptr; tree=tree->next) {
		e_vx = __twoplus_bi_search(tbl, line, uls_ptr(tree->twoplus_sorted), tree->twoplus_sorted.n);
		if (e_vx != nilptr) {
			return e_vx->base;
		}
	}

	return nilptr;
}

void
distribute_2char_toks(uls_kwtable_twoplus_ptr_t tbl, uls_strcmp_proc_t cmpfunc)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(tbl->twoplus_mempool));
	int n_tokdefs_vx = tbl->twoplus_mempool.n;

	uls_twoplus_tree_ptr_t tree, tree_next;
	uls_twoplus_tree_ptr_t tree_list, tree_listtail;
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_tp, tokdef_vx);

	uls_tokdef_ptr_t e;
	int i, i0, j, len_keyw; tbl->str_ncmp = cmpfunc;

	if (n_tokdefs_vx <= 0) {
		return;
	}

	_uls_quicksort_vptr(slots_vx, n_tokdefs_vx, cmp_twoplus_by_length);

	tree_list = tree_listtail = nilptr;

	for (i=0; i<n_tokdefs_vx; ) {
		e_vx = slots_vx[i];
		e = e_vx->base;

		len_keyw = e->l_keyword;

		if ((tree=uls_get_ind_twoplus_tree(tbl, len_keyw, nilptr)) == nilptr)
			err_panic("%s: get tree index for '%s'", e_vx->name);

		tree->len_keyw = len_keyw;

		for (i0=i++; i<n_tokdefs_vx; i++) {
			e_vx = slots_vx[i];
			e = e_vx->base;
			if (len_keyw != e->l_keyword) break;
		}

		uls_resize_parray(uls_ptr(tree->twoplus_sorted), tokdef_vx, i - i0);
		tree->twoplus_sorted.n = i - i0;
		slots_tp = uls_parray_slots(uls_ptr(tree->twoplus_sorted));
		for (j=0; j<tree->twoplus_sorted.n; j++) {
			slots_tp[j] = slots_vx[i0+j];
		}

		if (tree_listtail != nilptr) {
			tree_listtail->next = tree;
			tree_listtail = tree;
		} else {
			tree_list = tree_listtail = tree;
		}

		_uls_quicksort_vptr(slots_tp, tree->twoplus_sorted.n, cmp_twoplus_vx_by_keyword);
	}

	tree_listtail->next = nilptr;
	tbl->start = tree_list;

	tree_next = nilptr;
	for (i=0; i < ULS_KWTABLE_TWOPLUS_SIZE; i++) {
		tree = uls_array_get_slot_type00(uls_ptr(tbl->tree_array), i);

		if (tree->len_keyw <= 0) {
			tree->next = tree_next;
		} else {
			tree_next = tree;
		}
	}
}
