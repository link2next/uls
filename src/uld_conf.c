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
  <file> uld_conf.c </file>
  <brief>
    Loading the config spec file (*.uld) of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2014.
  </author>
*/
#define __ULD_CONF__
#include "uls/uld_conf.h"
#include "uls/uls_core.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC int
comp_vx_by_toknam(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1 = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2 = (const uls_tokdef_vx_ptr_t) b;

	return uls_strcmp(uls_get_namebuf_value(e1->name), uls_get_namebuf_value(e2->name));
}

ULS_DECL_STATIC int
srch_vx_by_toknam(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_outparam_ptr_t parms = (const uls_outparam_ptr_t) b;
	const char *name = parms->lptr;
	const uls_tokdef_vx_ptr_t e = (const uls_tokdef_vx_ptr_t ) a;

	return uls_strcmp(uls_get_namebuf_value(e->name), name);
}

ULS_DECL_STATIC void
__change_tok_id(uls_tokdef_vx_ptr_t e_vx, int tok_id)
{
	if (e_vx->tok_id != tok_id) {
		e_vx->flags |= ULS_VX_TOKID_CHANGED;
		e_vx->tok_id = tok_id;
	}
}

ULS_DECL_STATIC int
__change_tok_nam(uls_tokdef_vx_ptr_t e0_vx, const char* name, const char* name2)
{
	int stat = 0;
	uls_tokdef_name_ptr_t e_nam;

	if (canbe_tokname(name2) <= 0) {
		err_log("%s: not token name!", name2);
		return -1;
	}

	if (e0_vx == nilptr || (name != NULL && uls_streql(name, name2))) { // the current token leader
		err_log("%s: can't find the token leader of '%s'", __FUNCTION__, name);
		return -1;
	}

	if (name == NULL || uls_streql(uls_get_namebuf_value(e0_vx->name), name)) {
		uls_set_namebuf_value(e0_vx->name, name2);
		stat = 1; // found & changed!

	} else if ((e_nam = find_tokdef_name(e0_vx, name, nilptr)) != nilptr) {
		uls_set_namebuf_value(e_nam->name, name2);
		e_nam->flags |= ULS_VX_TOKNAM_CHANGED;
		stat = 1; // found & changed!
	}

	return stat;
}

ULS_DECL_STATIC int
add_aliases_to_token(uls_tokdef_vx_ptr_t e0_vx, const char *wrd, uls_wrd_ptr_t wrdx)
{
	uls_tokdef_name_ptr_t e_nam, e_nam_prev;
	uls_outparam_t parms1;
	int n;

	for (n=0; *wrd != '\0'; n++) {
		if (canbe_tokname(wrd) <= 0) {
			n = -1; break;
		}

		e_nam = find_tokdef_name(e0_vx, wrd, uls_ptr(parms1));
		e_nam_prev = (uls_tokdef_name_ptr_t) parms1.data;

		if (e_nam == nilptr) {
			e_nam = alloc_tokdef_name(wrd, e0_vx);
			e_nam->flags |= ULS_VX_TOKNAM_CHANGED;
			insert_tokdef_name_to_group(e0_vx, e_nam_prev, e_nam);
		}

		wrd = _uls_splitstr(wrdx);
	}

	return n;
}

int
uld_pars_line(int lno, uls_wrd_ptr_t wrdx, uld_line_ptr_t tok_names)
{
	const char *name, *name2, *wrd;
	int tok_id, tok_id_changed, stat=0;

	name = _uls_splitstr(wrdx); // token-name

	if (*(wrd = _uls_splitstr(wrdx)) == '\0') {
		err_log("#%d: can't change the token name", lno);
		err_log("\t:'%s' to ''", name);
		return -1;
	}

	if (canbe_tokname(wrd) <= 0) {
		name2 = NULL;
	} else {
		name2 = wrd;
		wrd = _uls_splitstr(wrdx);
	}

	if (is_pure_int_number(wrd) > 0) {
		tok_id = uls_atoi(wrd);
		tok_id_changed = 1;
		wrd = _uls_splitstr(wrdx);
	} else {
		tok_id = 0;
		tok_id_changed = 0;
	}

	tok_names->name = name;
	tok_names->name2 = name2;
	tok_names->tokid_changed = tok_id_changed;
	tok_names->tokid = tok_id;
	tok_names->aliases = wrd;

	return stat;
}

int
uld_proc_line(const char *tag, int lno,
	char* lptr, uls_lex_ptr_t uls, int n2_vx_namelist)
{
	int stat=0;
	uls_tokdef_vx_ptr_t e0_vx;
	uld_line_t tok_names;
	uls_wrd_t wrdx;

	wrdx.lptr = lptr;

	if (uld_pars_line(lno, uls_ptr(wrdx), uls_ptr(tok_names)) < 0) {
		err_log("%s<%d>: can't change the token name", tag, lno);
		return -1;
	}

	if ((e0_vx = uld_find_tokdef_vx(uls, n2_vx_namelist, tok_names.name)) == nilptr) {
		err_log("%s<%d>: can't find tok named", tag, lno);
		err_log("\t:'%s'", tok_names.name);
		return -1;
	}

	if (tok_names.name2 != NULL && __change_tok_nam(e0_vx, NULL, tok_names.name2) < 0) {
		err_log("%s<%d>: can't change the token name", tag, lno);
		err_log("\t:'%s' to '%s'", tok_names.name, tok_names.name2);
		return -1;
	}

	if (tok_names.tokid_changed) {
		__change_tok_id(e0_vx, tok_names.tokid);
	}

	if (add_aliases_to_token(e0_vx, tok_names.aliases, uls_ptr(wrdx)) < 0) {
		err_log("%s<%d>: column is not name!", tag, lno);
		stat = -1;
	}

	return stat;
}

uls_tokdef_vx_ptr_t
uld_find_tokdef_vx(uls_lex_ptr_t uls, int n_slots_vx, const char* name)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e_vx;
	uls_outparam_t parms1;

	parms1.lptr = name;
	e_vx = (uls_tokdef_vx_ptr_t ) uls_bi_search_vptr((uls_voidptr_t) uls_ptr(parms1),
		(_uls_type_array(uls_voidptr_t)) slots_vx, n_slots_vx,
		uls_ref_callback(srch_vx_by_toknam));

	return e_vx;
}

int
uld_add_aliases(uls_tokdef_vx_ptr_t e0_vx, const char *line_aliases)
{
	uls_wrd_t wrdx;
	char *line, *wrd;
	int stat = 0;

	if (line_aliases == NULL || *line_aliases == '\0') {
		return 0;
	}

	line = uls_strdup(line_aliases, -1);

	wrdx.lptr = line;
	wrd = _uls_splitstr(uls_ptr(wrdx));

	if (add_aliases_to_token(e0_vx, wrd, uls_ptr(wrdx)) < 0) {
		stat = -1;
	}

	uls_mfree(line);
	return stat;
}

int
uld_load_fp(uls_lex_ptr_t uls, FILE *fin_uld, const char *tag)
{
	char     linebuff[ULS_LINEBUFF_SIZ__ULD+1], *lptr;
	int      n2_vx_namelist, linelen, lno=0, stat=0;
	uld_names_map_ptr_t names_map;

	names_map = uld_prepare_names(uls);
	n2_vx_namelist = names_map->n_vx_namelist;

	while (1) {
		if ((linelen=uls_fp_gets(fin_uld, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				err_log("%s: ulc file i/o error at %d", __FUNCTION__, lno);
				stat = -1;
			}
			break;
		}
		++lno;

		if (*(lptr = skip_blanks(linebuff)) == '\0' ||
			*lptr == '#' || (lptr[0]=='/' && lptr[1]=='/'))
			continue;

		if (uld_proc_line(tag, lno, lptr, uls, n2_vx_namelist) < 0) {
			stat = -1;
			break;
		}
	}

	if (uld_post_names(names_map) < 0) {
		err_log("can't process uld-file %s", tag);
		stat = -1;
	}

	return stat;
}

void
uls_init_nam_tok(uls_nam_tok_ptr_t nam_tok)
{
}

void
uls_deinit_nam_tok(uls_nam_tok_ptr_t nam_tok)
{
}

uld_names_map_ptr_t
uld_prepare_names(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	int i, k, n, n_slots_vx=uls->tokdef_vx_array.n;
	uls_tokdef_vx_ptr_t e_vx, e2_vx;
	uld_names_map_ptr_t names_map;

	for (n=n_slots_vx,i=k=0; i<n; ) {
		e_vx = slots_vx[i];

		if (uls_get_namebuf_value(e_vx->name)[0] == '\0') {
			// exchange slots_vx[i](==e_vx) and slots_vx[n-1](==e2_vx).
			e2_vx = slots_vx[--n];
			slots_vx[n] = e_vx;
			slots_vx[i] = e2_vx;
		} else {
			slots_vx[k++] = e_vx;
			i++;
		}
	}

	_uls_quicksort_vptr(slots_vx, n, comp_vx_by_toknam);

	names_map = uls_alloc_object(uld_names_map_t);
	names_map->uls = uls;
	names_map->n_vx_namelist = n;

	return names_map;
}

int
uld_change_names(uld_names_map_ptr_t names_map, uld_line_ptr_t tnd)
{
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = uld_find_tokdef_vx(names_map->uls, names_map->n_vx_namelist, tnd->name)) == nilptr) {
		return -1;
	}

	if (uld_change_tok_names(e_vx, tnd->name2, tnd->aliases) < 0) {
		return -1;
	}

	if (tnd->tokid_changed) {
		uld_change_tok_id(e_vx, tnd->tokid);
	}

	return 0;
}

int
uld_post_names(uld_names_map_ptr_t names_map)
{
	uls_lex_ptr_t uls = names_map->uls;
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	int n_slots_vx = uls->tokdef_vx_array.n, prev_tok_id, i, stat=0;
	uls_tokdef_vx_ptr_t e_vx;
	char *prev_tok_nam;

	// n_vx_namelist > 0
	_uls_quicksort_vptr(slots_vx, n_slots_vx, comp_vx_by_tokid);

	e_vx = slots_vx[0];
	prev_tok_id = e_vx->tok_id;
	prev_tok_nam = uls_get_namebuf_value(e_vx->name);

	for (i=1; ; i++) {
		if (i >= n_slots_vx) {
			if (classify_tok_group(uls) < 0) {
				err_log("%s: lex-conf file not consistent!", __FUNCTION__);
				stat = -1;
			} else {
				uls->xcontext.context->tok = uls->xcontext.toknum_EOI;
			}

			break;
		}

		e_vx = slots_vx[i];

		if (e_vx->tok_id == prev_tok_id) {
			err_log("tok-id confliction: %s :: %s", uls_get_namebuf_value(e_vx->name), prev_tok_nam);
			stat = -1;
			break;
		}

		prev_tok_id = e_vx->tok_id;
		prev_tok_nam = uls_get_namebuf_value(e_vx->name);
	}

	uls_dealloc_object(names_map);

	return stat;
}

int
uld_change_tok_id(uls_tokdef_vx_ptr_t e_vx, int tok_id)
{
	__change_tok_id(e_vx, tok_id);
	return 0;
}

int
uld_change_tok_names(uls_tokdef_vx_ptr_t e_vx, const char* name2, const char *line_aliases)
{
	if (name2 != NULL && __change_tok_nam(e_vx, NULL, name2) < 0) {
		return -1;
	}

	if (uld_add_aliases(e_vx, line_aliases) < 0) {
		return -1;
	}

	return 0;
}

void
uld_export_names(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	uls_printf("#@%s\n#\n", uls_get_namebuf_value(uls->ulc_name));
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (uls_get_namebuf_value(e_vx->name)[0] != '\0')
			uls_printf("#%16s %d\n", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
	}
}

int
uld_export_extra_names(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	int i, k, n = uls->tokdef_vx_array.n;
	uls_tokdef_vx_ptr_t e_vx;
	uls_nam_tok_ptr_t nam_tok;
	uls_decl_array_alloc_type10(lst_names, nam_tok);

	uls_init_array_type10(lst_names, nam_tok, n);

	for (i=k=0; i<n; i++) {
		e_vx = slots_vx[i];

		if (uls_get_namebuf_value(e_vx->name)[0] != '\0' && (e_vx->flags & ULS_VX_TOKID_CHANGED)) {
			uls_array_alloc_slot_type10(lst_names, nam_tok, k);

			nam_tok = uls_array_get_slot_type10(lst_names, k);
			nam_tok->name = uls_get_namebuf_value(e_vx->name);
			nam_tok->tok_id = e_vx->tok_id;
			k++;
		}
	}

	parms->n = lst_names->n = k;
	parms->data = (uls_voidptr_t) lst_names;

	return 0;
}

void
uld_unexport_extra_names(uls_ref_array_type10(lst_names,nam_tok))
{
	uls_deinit_array_type10(lst_names, nam_tok);
	uls_dealloc_object(lst_names);
}
