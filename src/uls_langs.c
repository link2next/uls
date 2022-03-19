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
  <file> uls_langs.c </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/
#define __ULS_LANGS__
#include "uls/uls_langs.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC unsigned int
ulc_alias_hashfunc(const char *str, int n)
{
	unsigned int hash_val = 0xE7AFAEB;
	int i;

	for (i=0; i<n; i++) {
		hash_val += str[i];
	}

	return hash_val;
}

ULS_DECL_STATIC int
strdup_cnst(uls_lang_list_ptr_t tbl, const char *str)
{
	int i0, len, siz;

	len = uls_strlen(str);

	if ((i0 = tbl->n_str_pool) + len >= tbl->siz_str_pool) {
		siz = uls_ceil_log2(i0 + len + 1, 10);
		tbl->str_pool = (char *) uls_mrealloc(tbl->str_pool, siz);
		tbl->siz_str_pool = siz;
	}

	uls_strcpy(tbl->str_pool + i0, str);
	tbl->n_str_pool += len + 1; // +1 for '\0'

	return i0;
}

ULS_DECL_STATIC uls_lang_ptr_t
uls_append_lang(uls_lang_list_ptr_t tbl, uls_outparam_ptr_t parms)
{
	const char *wrd = parms->lptr;
	uls_lang_ptr_t lang;
	int siz, k;

	if ((k=tbl->langs.n) >= tbl->langs.n_alloc) {
		siz = uls_ceil_log2(k+1, 9);
		uls_resize_array_type10(uls_ptr(tbl->langs), lang, siz);
	}

	uls_array_alloc_slot_type10(uls_ptr(tbl->langs), lang, k);
	lang = uls_array_get_slot_type10(uls_ptr(tbl->langs), k);
	lang->parent = tbl;

	lang->idx_alias_list = tbl->alias_pool.n;
	new_ulc_alias(tbl, wrd, k);
	lang->n_alias_list = 0;

	tbl->langs.n = k + 1;

	parms->n = k;
	return lang;
}

ULS_DECL_STATIC int
langs_proc_line(uls_lang_list_ptr_t tbl, char* line)
{
	uls_lang_ptr_t lang;
	char *lptr=line, *wrd0, *wrd;
	int len, lst_id;
	uls_outparam_t parms;
	uls_wrd_t wrdx;

	wrdx.lptr = lptr;
	wrd0 = _uls_splitstr(uls_ptr(wrdx));
	lptr = wrdx.lptr;

	if ((len = uls_strlen(wrd0)) <= 1 || wrd0[len-1] != ':') {
		err_log("%s: incorrect format!", __func__);
		return -1;
	}

	// wrd0[len-1] == ':'
	wrd0[--len] = '\0';

	parms.lptr = wrd0;
	lang = uls_append_lang(tbl, uls_ptr(parms));
	lst_id = parms.n;

	while (1) {
		parms.line = lptr;
		wrd = split_clause(uls_ptr(parms));
		lptr = parms.line;

		if (*wrd == '\0') {
			break;
		}

		new_ulc_alias(tbl, wrd, lst_id);
		++lang->n_alias_list;
	}

	return 0;
}

ULS_DECL_STATIC void
construct_ulc_lang_db(uls_lang_list_ptr_t tbl)
{
	uls_decl_parray_slots(slots_ht, alias);
	uls_lang_ptr_t lang;
	uls_alias_ptr_t e;
	unsigned int hval;
	int i, siz;

	if ((siz = tbl->n_str_pool) < tbl->siz_str_pool) {
		tbl->str_pool = (char *) uls_mrealloc(tbl->str_pool, siz);
		tbl->siz_str_pool = siz;
	}

	if ((siz = tbl->alias_pool.n) < tbl->alias_pool.n_alloc) {
		uls_resize_array_type10(uls_ptr(tbl->alias_pool), alias, siz);
	}

	if ((siz = tbl->langs.n) < tbl->langs.n_alloc) {
		uls_resize_array_type10(uls_ptr(tbl->langs), lang, siz);
	}

	slots_ht = uls_parray_slots(uls_ptr(tbl->hashtbl));
	for (i=0; i<tbl->alias_pool.n; i++) {
		e = uls_array_get_slot_type10(uls_ptr(tbl->alias_pool), i);

		e->name = tbl->str_pool + e->len;
		e->len = uls_strlen(e->name);
		hval = ulc_alias_hashfunc(e->name, e->len) % ULC_ALIAS_HASHTABLE_SIZ;
		e->next = slots_ht[hval];
		slots_ht[hval] = e;
	}

	for (i=0; i<tbl->langs.n; i++) {
		lang = uls_array_get_slot_type10(uls_ptr(tbl->langs), i);
		lang->e0 = uls_array_get_slot_type10(uls_ptr(tbl->alias_pool), lang->idx_alias_list);
		++lang->idx_alias_list;
	}
}

ULS_DECL_STATIC _ULS_INLINE int
__is_langs_needed_quote(const char* name)
{
	int stat=0;
	const char *cptr;
	char ch;

	for (cptr=name; (ch=*cptr) != '\0'; cptr++) {
		if (ch==' ') {
			stat = 1;
			break;
		}
	}

	return stat;
}

ULS_DECL_STATIC void
uls_dump_lang(uls_lang_ptr_t lang)
{
	uls_lang_list_ptr_t tbl = lang->parent;
	int i, i0=lang->idx_alias_list, i1=i0 + lang->n_alias_list;
	uls_alias_ptr_t e, e0;

	e0 = lang->e0;
	uls_printf("\t%s", e0->name);

	for (i=i0; i<i1; i++) {
		e = uls_array_get_slot_type10(uls_ptr(tbl->alias_pool), i);

		if (__is_langs_needed_quote(e->name)) {
			uls_printf(" '%s'", e->name);
		} else {
			uls_printf(" %s", e->name);
		}
	}
	uls_printf("\n");
}

void
uls_init_alias(uls_alias_ptr_t alias)
{
}

void
uls_deinit_alias(uls_alias_ptr_t alias)
{
}

uls_alias_ptr_t
new_ulc_alias(uls_lang_list_ptr_t tbl, const char *wrd, int lst_id)
{
	uls_ref_array_init_type10(aliases, alias, uls_ptr(tbl->alias_pool));
	int n_aliases = tbl->alias_pool.n, siz;
	uls_alias_ptr_t e;

	if (n_aliases >= tbl->alias_pool.n_alloc) {
		siz = uls_ceil_log2(n_aliases+1, 8);
		uls_resize_array_type10(uls_ptr(tbl->alias_pool), alias, siz);
		aliases = uls_ptr(tbl->alias_pool);
	}

	uls_array_alloc_slot_type10(aliases, alias, n_aliases);
	e = uls_array_get_slot_type10(aliases, n_aliases);

	e->name = NULL;
	e->len = strdup_cnst(tbl, wrd);
	e->lst_id = lst_id;
	e->next = nilptr;

	++tbl->alias_pool.n;

	return e;
}

void
uls_init_lang(uls_lang_ptr_t lang)
{
	lang->e0 = nilptr;
	lang->idx_alias_list = -1;
	lang->n_alias_list = 0;
	lang->parent = nilptr;
}

void
uls_deinit_lang(uls_lang_ptr_t lang)
{
}

void
uls_init_lang_list(uls_lang_list_ptr_t tbl)
{
	uls_decl_parray_slots(slots_ht, alias);
	int i;

	uls_initial_zerofy_object(tbl);

	uls_init_parray(uls_ptr(tbl->hashtbl), alias, ULC_ALIAS_HASHTABLE_SIZ);
	slots_ht = uls_parray_slots(uls_ptr(tbl->hashtbl));
	for (i=0; i<ULC_ALIAS_HASHTABLE_SIZ; i++) {
		slots_ht[i] = nilptr;
	}
	tbl->hashtbl.n = ULC_ALIAS_HASHTABLE_SIZ;

	uls_init_array_type10(uls_ptr(tbl->langs), lang, 0);
	uls_init_array_type10(uls_ptr(tbl->alias_pool), alias, 0);
}

void
uls_deinit_lang_list(uls_lang_list_ptr_t tbl)
{
	uls_deinit_parray(uls_ptr(tbl->hashtbl));

	uls_deinit_array_type10(uls_ptr(tbl->langs), lang);
	uls_deinit_array_type10(uls_ptr(tbl->alias_pool), alias);

	uls_mfree(tbl->str_pool);
}

uls_lang_list_ptr_t
uls_load_langdb(const char *fpath)
{
	uls_lang_list_ptr_t tbl;
	char linebuff[ULS_LINEBUFF_SIZ+1], *lptr;
	int len, stat=0;
	uls_file_ptr_t filp;

	tbl = uls_alloc_object(uls_lang_list_t);
	uls_init_lang_list(tbl);

	if ((filp = uls_open_filp(fpath, ULS_FIO_READ|ULS_FIO_MULTLINE)) == nilptr) {
		uls_deinit_lang_list(tbl);
		uls_dealloc_object(tbl);
		return nilptr;
	}

	while (1) {
		if ((len=uls_filp_gets(filp, linebuff, sizeof(linebuff))) <= ULS_EOF) {
			if (len < ULS_EOF) stat = -1;
			break;
		}

		if (*(lptr = skip_blanks(linebuff)) == '\0' || *lptr == '#')
			continue;

		if (langs_proc_line(tbl, lptr) < 0) {
			stat = -1;
			break;
		}
	}

	uls_close_filp(filp);

	if (stat < 0) {
		uls_deinit_lang_list(tbl);
		uls_dealloc_object(tbl);
		return nilptr;
	}

	construct_ulc_lang_db(tbl);
	return tbl;
}

void
uls_destroy_lang_list(uls_lang_list_ptr_t tbl)
{
	uls_deinit_lang_list(tbl);
	uls_dealloc_object(tbl);
}

uls_lang_ptr_t
uls_find_lang(uls_lang_list_ptr_t tbl, const char* alias)
{
	uls_decl_parray_slots(slots_ht, alias);
	unsigned int hval;
	uls_alias_ptr_t e;
	int len;

	if (alias == NULL || *alias == '\0') {
		err_log("%s: invalid parameter!", __func__);
		return nilptr;
	}
	len = uls_strlen(alias);

	slots_ht = uls_parray_slots(uls_ptr(tbl->hashtbl));
	hval = ulc_alias_hashfunc(alias, len) % ULC_ALIAS_HASHTABLE_SIZ;
	for (e=slots_ht[hval]; e!=nilptr; e=e->next) {
		if (len == e->len && uls_streql(e->name, alias)) {
			return uls_array_get_slot_type10(uls_ptr(tbl->langs), e->lst_id);
		}
	}

	return nilptr;
}

const char*
uls_find_lang_name(const char* alias)
{
	uls_alias_ptr_t e0;
	uls_lang_ptr_t lang;

	if ((lang = uls_find_lang(uls_langs, alias)) == nilptr) {
		return NULL;
	}

	e0 = lang->e0;
	return e0->name;
}

int
uls_list_names_of_lang(const char* alias)
{
	uls_lang_ptr_t lang;

	if ((lang = uls_find_lang(uls_langs, alias)) == nilptr) {
		return -1;
	}

	uls_dump_lang(lang);

	return 0;
}

void
uls_list_langs(void)
{
	uls_lang_list_ptr_t tbl = uls_langs;
	uls_lang_ptr_t lang;
	int i;

	for (i=0; i<tbl->langs.n; i++) {
		lang = uls_array_get_slot_type10(uls_ptr(tbl->langs), i);
		uls_dump_lang(lang);
	}
}
