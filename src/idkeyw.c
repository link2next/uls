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
 * idkeyw.c -- manipulating id-style keywords. --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_IDKEYW__
#include "uls/idkeyw.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

void
ULS_QUALIFIED_METHOD(uls_init_hash_stat)(uls_hash_stat_ptr_t hs)
{
	int i;
	uls_init_intarray(hs->weight, 3);
	for (i = 0; i < 3; i++) hs->weight[i] = 1;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_hash_stat)(uls_hash_stat_ptr_t hs)
{
	uls_deinit_intarray(hs->weight);
}

void
ULS_QUALIFIED_METHOD(uls_copy_hash_stat)(uls_hash_stat_ptr_t hs_src,
	uls_hash_stat_ptr_t hs_dst)
{
	uls_ref_intarray(weights_src, hs_src->weight);
	uls_ref_intarray(weights_dst, hs_dst->weight);
	int i;

	for (i = 0; i < 3; i++) {
		weights_dst[i] = weights_src[i];
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__keyw_strncmp_case_sensitive)(const char* str1, const char* str2, int len)
{
	int i, ch1, ch2, stat=0;

	for (i=0; i<len; i++) {
		ch1 = str1[i];
		ch2 = str2[i];

		if (ch1 != ch2) {
			stat = ch1 - ch2;
			break;
		}
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__keyw_hashfunc_case_sensitive)(uls_hash_stat_ptr_t hs, const char *name)
{
	uls_ref_intarray(weights, hs->weight);
	int i, j, len = _uls_tool_(strlen)(name);
	int hash = 0;

	for (i = 0; i < len; i += 3) {
		if (i + 2 >= len) {
			for (j = 0; i < len; i++, j++) {
				hash += name[i] * weights[j];
			}
			break;
		}

		hash += name[i] * weights[0] +
			name[i+1] * weights[1] +
			name[i+2] * weights[2];
	}

	hash = (hash % ULF_HASH_TABLE_SIZE) + ULF_HASH_TABLE_SIZE;
	return hash % ULF_HASH_TABLE_SIZE;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__keyw_strncmp_case_insensitive)(const char* wrd, const char* keyw, int len)
{
	int i, ch1, ch2, stat=0;

	for (i=0; i<len; i++) {
		ch1 = _uls_tool_(toupper)(wrd[i]);
		ch2 = keyw[i];

		if (ch1 != ch2) {
			stat = ch1 - ch2;
			break;
		}
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__keyw_hashfunc_case_insensitive)(uls_hash_stat_ptr_t hs, const char *name)
{
	uls_ref_intarray(weights, hs->weight);
	int i, j, len = _uls_tool_(strlen)(name);
	int hash = 0;

	for (i = 0; i < len; i += 3) {
		if (i + 2 >= len) {
			for (j = 0; i < len; i++, j++) {
				hash += _uls_tool_(toupper)(name[i]) * weights[j];
			}
			break;
		}

		hash += _uls_tool_(toupper)(name[i]) * weights[0] +
			_uls_tool_(toupper)(name[i+1]) * weights[1] +
			_uls_tool_(toupper)(name[i+2]) * weights[2];
	}

	hash = (hash % ULF_HASH_TABLE_SIZE) + ULF_HASH_TABLE_SIZE;
	return hash % ULF_HASH_TABLE_SIZE;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__init_kwtable_buckets)(uls_kwtable_ptr_t tbl)
{
	uls_decl_parray_slots(slots_bh, tokdef);
	int i;

	uls_init_parray(uls_ptr(tbl->bucket_head), tokdef, ULF_HASH_TABLE_SIZE);

	slots_bh = uls_parray_slots(uls_ptr(tbl->bucket_head));
	for (i = 0; i < ULF_HASH_TABLE_SIZE; i++) slots_bh[i] = nilptr;
	tbl->bucket_head.n = ULF_HASH_TABLE_SIZE;

	uls_init_hash_stat(uls_ptr(tbl->hash_stat));
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__export_kwtable)(uls_kwtable_ptr_t tbl, uls_ref_parray(lst, keyw_stat), int n_lst)
{
	uls_decl_parray_slots_init(slots_bh, tokdef, uls_ptr(tbl->bucket_head));
	uls_decl_parray_slots_init(slots_lst, keyw_stat, lst);
	uls_tokdef_ptr_t e;
	uls_keyw_stat_ptr_t kwstat;
	int i, k=0;

	if (n_lst == 0) return 0;

	for (i=0; i<tbl->bucket_head.n; i++) {
		if (slots_bh[i] == nilptr) continue;

		for (e=slots_bh[i]; e!=nilptr; e=e->link) {
			if (k >= n_lst) return -n_lst;

			kwstat = slots_lst[k] = uls_alloc_object(uls_keyw_stat_t);
			kwstat->keyw = uls_get_namebuf_value(e->keyword);
			kwstat->freq = 0; // initializing frequency to 0
			kwstat->keyw_info = e;

			++k;
		}
	}

	lst->n = k;
	_uls_quicksort_vptr(slots_lst, k, keyw_stat_comp_by_keyw);

	return k;
}

void
ULS_QUALIFIED_METHOD(uls_init_kwtable)(uls_kwtable_ptr_t tbl)
{
	__init_kwtable_buckets(tbl);

	tbl->str_ncmp = uls_ref_callback_this(__keyw_strncmp_case_sensitive);
	tbl->hashfunc = uls_ref_callback_this(__keyw_hashfunc_case_sensitive);
}

void
ULS_QUALIFIED_METHOD(uls_reset_kwtable)(uls_kwtable_ptr_t tbl, int case_insensitive)
{
	if (case_insensitive) {
		tbl->str_ncmp = uls_ref_callback_this(__keyw_strncmp_case_insensitive);
		tbl->hashfunc = uls_ref_callback_this(__keyw_hashfunc_case_insensitive);
	} else {
		tbl->str_ncmp = uls_ref_callback_this(__keyw_strncmp_case_sensitive);
		tbl->hashfunc = uls_ref_callback_this(__keyw_hashfunc_case_sensitive);
	}
}

void
ULS_QUALIFIED_METHOD(uls_deinit_kwtable)(uls_kwtable_ptr_t tbl)
{
	uls_deinit_parray(uls_ptr(tbl->bucket_head));
	uls_deinit_hash_stat(uls_ptr(tbl->hash_stat));
	tbl->hashfunc = nilptr;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_kw)(uls_kwtable_ptr_t tbl, uls_ptrtype_tool(outparam) parms)
{
	uls_decl_parray_slots_init(slots_bh, tokdef, uls_ptr(tbl->bucket_head));
	const char *idstr = parms->lptr;
	int hash_id, l_idstr = parms->len;
	uls_tokdef_ptr_t   e, e_found=nilptr;

	hash_id = tbl->hashfunc(uls_ptr(tbl->hash_stat), idstr);
	parms->n = hash_id;

	for (e = slots_bh[hash_id]; e != nilptr; e = e->link) {
		if (l_idstr == e->ulen_keyword &&
			tbl->str_ncmp(idstr, uls_get_namebuf_value(e->keyword), l_idstr) == 0) {
			e_found = e;
			break;
		}
	}

	return e_found;
}

int
ULS_QUALIFIED_METHOD(uls_add_kw)(uls_kwtable_ptr_t tbl, uls_tokdef_ptr_t e)
{
	uls_decl_parray_slots_init(slots_bh, tokdef, uls_ptr(tbl->bucket_head));
	uls_type_tool(outparam) parms;
	uls_tokdef_ptr_t e2;
	int hash_id;

	parms.lptr = uls_get_namebuf_value(e->keyword);
	parms.len = e->ulen_keyword;
	e2 = uls_find_kw(tbl, uls_ptr(parms));
	hash_id = parms.n;

	if (e2 == nilptr) {
		e->link = slots_bh[hash_id];
		slots_bh[hash_id] = e;
		return 1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(sizeof_kwtable)(uls_kwtable_ptr_t tbl)
{
	uls_decl_parray_slots_init(slots_bh, tokdef, uls_ptr(tbl->bucket_head));
	uls_tokdef_ptr_t  e;
	int i, n=0;

	for (i=0; i<tbl->bucket_head.n; i++) {
		if (slots_bh[i] == nilptr) continue;
		for (e=slots_bh[i]; e!=nilptr; e=e->link) {
			++n;
		}
	}

	return n;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(is_keyword_idstr)(uls_kwtable_ptr_t tbl, const char* keyw, int l_keyw)
{
	uls_type_tool(outparam) parms;

	parms.lptr = keyw;
	parms.len =  l_keyw;

	return uls_find_kw(tbl, uls_ptr(parms));
}

int
ULS_QUALIFIED_METHOD(keyw_stat_comp_by_keyw)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	uls_keyw_stat_ptr_t a1 = (uls_keyw_stat_ptr_t ) a;
	uls_keyw_stat_ptr_t b1 = (uls_keyw_stat_ptr_t ) b;

	return _uls_tool_(strcmp)(a1->keyw, b1->keyw);
}

ULS_QUALIFIED_RETTYP(uls_keyw_stat_list_ptr_t)
ULS_QUALIFIED_METHOD(ulc_export_kwtable)(uls_kwtable_ptr_t tbl)
{
	uls_keyw_stat_list_ptr_t kwslst;
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	int   i, n_lst;

	kwslst = uls_alloc_object(uls_keyw_stat_list_t);

	n_lst = sizeof_kwtable(tbl);
	lst = uls_ptr(kwslst->lst);
	uls_init_parray(lst, keyw_stat, n_lst);

	// lst is sorted by lst[].keyw
	__export_kwtable(tbl, lst, n_lst);
	kwslst->lst.n = n_lst;

	slots_lst = uls_parray_slots(lst);
	for (i=0; i < kwslst->lst.n; i++) {
		slots_lst[i]->freq = 0;
	}

	return kwslst;
}

void
ULS_QUALIFIED_METHOD(ulc_free_kwstat_list)(uls_keyw_stat_list_ptr_t kwslst)
{
	uls_ref_parray_init(lst, keyw_stat, uls_ptr(kwslst->lst));
	uls_decl_parray_slots_init(slots_lst, keyw_stat, lst);
	int i;

	for (i=0; i<kwslst->lst.n; i++) {
		uls_dealloc_object(slots_lst[i]);
	}

	uls_deinit_parray(uls_ptr(kwslst->lst));
	uls_dealloc_object(kwslst);
}

ULS_QUALIFIED_RETTYP(uls_hashfunc_t)
ULS_QUALIFIED_METHOD(uls_get_hashfunc)(const char* hashname, int case_insensitive)
{
	uls_hashfunc_t hashfunc;

	if (!(uls_streql(hashname, ULS_HASH_ALGORITHM) || uls_streql(hashname, "ULF-HASH"))) {
		_uls_log(err_log)("%s: unsupported hash algorithm!", hashname);
		return nilptr;
	}

	if (case_insensitive) {
		hashfunc = uls_ref_callback_this(__keyw_hashfunc_case_insensitive);
	} else {
		hashfunc = uls_ref_callback_this(__keyw_hashfunc_case_sensitive);
	}

	return hashfunc;
}
