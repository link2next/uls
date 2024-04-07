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
#ifndef ULS_EXCLUDE_HFILES
#define __ULD_CONF__
#include "uls/uld_conf.h"
#include "uls/uls_core.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#endif

int
ULS_QUALIFIED_METHOD(uld_calc_filebuff_size)(int filesize)
{
	return uls_ceil_log2(filesize + 1, 3);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(comp_vx_by_toknam)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1 = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2 = (const uls_tokdef_vx_ptr_t) b;

	return _uls_tool_(strcmp)(uls_get_namebuf_value(e1->name), uls_get_namebuf_value(e2->name));
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(srch_vx_by_toknam)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_ptrtype_tool(outparam) parms = (const uls_ptrtype_tool(outparam)) b;
	const char *name = parms->lptr;
	const uls_tokdef_vx_ptr_t e_vx = (const uls_tokdef_vx_ptr_t ) a;

	return _uls_tool_(strcmp)(uls_get_namebuf_value(e_vx->name), name);
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__change_tok_id)(uls_tokdef_vx_ptr_t e_vx, int tok_id)
{
	if (e_vx->tok_id != tok_id) {
		e_vx->flags |= ULS_VX_TOKID_CHANGED;
		e_vx->tok_id = tok_id;
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(add_aliases_to_token)(uls_tokdef_vx_ptr_t e_vx, uls_ptrtype_tool(wrd) wrdx)
{
	char *wrd;
	int n;

	for (n=0; *(wrd = __uls_tool_(splitstr)(wrdx)) != '\0'; n++) {
		if (canbe_tokname(wrd) <= 0) {
			_uls_log(err_log)("%s: not good as token name!", wrd);
			n = -1; break;
		}
		if (!uls_add_tokdef_vx_name(e_vx, wrd)) {
			n = -1; break;
		}
	}

	return n;
}

int
ULS_QUALIFIED_METHOD(uld_pars_line)(uls_ptrtype_tool(wrd) wrdx, uld_line_ptr_t tok_names)
{
	uls_type_tool(outparam) parms1;
	const char *name, *name2;
	int tok_id, changed, rc, stat=0;
	char *lptr, *wrd;

	name = __uls_tool_(splitstr)(wrdx); // token-name
	if (canbe_tokname(name) <= 0) {
		_uls_log(err_log)("%s can't be used as token-name.", name);
		return -1;
	}

	if (*(wrd = __uls_tool_(splitstr)(wrdx)) == '\0') {
		_uls_log(err_log)("Can't change the token name");
		_uls_log(err_log)("\t:'%s' to ''", name);
		return -1;
	}

	wrdx->lptr = lptr = _uls_tool(skip_blanks)(wrdx->lptr);

	if (_uls_tool(is_pure_integer)(wrd, nilptr) > 0) {
		tok_id = _uls_tool_(atoi)(wrd);
		changed = 1;
		name2 = NULL;

	} else {
		if (canbe_tokname(wrd) <= 0) {
			_uls_log(err_log)("%s can't be used as token-name.", wrd);
			return -1;
		}
		name2 = wrd;
		tok_id = 0;
		changed = 0;

		if ((rc=_uls_tool(is_pure_integer)(lptr, uls_ptr(parms1))) != 0) {
			tok_id = parms1.n;
			changed = 1;
			if (rc < 0) rc = -rc;
			lptr += rc;
		}
	}

	tok_names->name = name;
	tok_names->name2 = name2;
	tok_names->tokid_changed = changed;
	tok_names->tokid = tok_id;
	tok_names->aliases = wrdx->lptr = lptr;

	return stat;
}

int
ULS_QUALIFIED_METHOD(uld_proc_line)(char* lptr, uls_lex_ptr_t uls, int n2_vx_namelist)
{
	int stat = 0;
	uls_tokdef_vx_ptr_t e_vx;
	uld_line_t tok_names;
	uls_type_tool(wrd) wrdx;

	wrdx.lptr = lptr;
	if (uld_pars_line(uls_ptr(wrdx), uls_ptr(tok_names)) < 0) {
		_uls_log(err_log)("Can't change the token name");
		return -1;
	}

	if ((e_vx = uld_find_tokdef_vx(uls, n2_vx_namelist, tok_names.name)) == nilptr) {
		_uls_log(err_log)("Can't find tok named");
		_uls_log(err_log)("\t:'%s'", tok_names.name);
		return -1;
	}

	if (tok_names.name2 != NULL && uls_change_tokdef_vx_name(e_vx, NULL, tok_names.name2) < 0) {
		_uls_log(err_log)("Can't change the token name");
		_uls_log(err_log)("\t:'%s' to '%s'", tok_names.name, tok_names.name2);
		return -1;
	}

	if (tok_names.tokid_changed) {
		__change_tok_id(e_vx, tok_names.tokid);
	}

	if (add_aliases_to_token(e_vx, uls_ptr(wrdx)) < 0) {
		_uls_log(err_log)("Column is not name!");
		stat = -1;
	}

	return stat;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uld_find_tokdef_vx)(uls_lex_ptr_t uls, int n_slots_vx, const char* name)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e_vx;
	uls_type_tool(outparam) parms1;

	parms1.lptr = name;
	e_vx = (uls_tokdef_vx_ptr_t ) uls_bi_search_vptr((uls_voidptr_t) uls_ptr(parms1),
		(_uls_type_array(uls_voidptr_t)) slots_vx, n_slots_vx,
		uls_ref_callback_this(srch_vx_by_toknam));

	return e_vx;
}

int
ULS_QUALIFIED_METHOD(uld_add_aliases)(uls_tokdef_vx_ptr_t e_vx, const char *line_aliases)
{
	uls_type_tool(wrd) wrdx;
	char *line;
	int stat = 0;

	if (line_aliases == NULL || *line_aliases == '\0') {
		return 0;
	}

	line = _uls_tool_(strdup)(line_aliases, -1);
	wrdx.lptr = line;

	if (add_aliases_to_token(e_vx, uls_ptr(wrdx)) < 0) {
		stat = -1;
	}

	uls_mfree(line);
	return stat;
}


ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(writeline_uld_backup)(uls_xcontext_ptr_t xctx, const char *line, int linelen)
{
	char *buf = xctx->uldfile_buf;
	int k = xctx->uldfile_buflen;
	int bufsiz = xctx->uldfile_bufsiz;

	if (k + linelen + 1 >= bufsiz) {
		_uls_log(err_log)("%s: overflow occurred when backing up uld file content!", __func__);
		return;
	}

	_uls_tool_(memcopy)(buf + k, line, linelen);
	k += linelen;
	buf[k++] = '\n';

	xctx->uldfile_buflen = k;
	++xctx->uldfile_nlines;
}

int
ULS_QUALIFIED_METHOD(uld_load_fp)(uls_lex_ptr_t uls, FILE *fin_uld, int siz_uld_filebuff)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	char linebuff[ULS_LINEBUFF_SIZ__ULD+1], *lptr;
	int  n2_vx_namelist, linelen, lno = 1, stat = 0;
	uld_names_map_ptr_t names_map;

	names_map = uld_prepare_names(uls, siz_uld_filebuff);
	n2_vx_namelist = names_map->n_vx_namelist;

	xctx->uldfile_buflen = xctx->uldfile_nlines = 0;
	while (1) {
		if ((linelen=_uls_tool_(fp_gets)(fin_uld, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				_uls_log(err_log)("%s: ulc file i/o error at %d", __func__, lno);
				stat = -1;
			}
			break;
		}
		++lno;

		linelen = _uls_tool(str_trim_end)(linebuff, linelen);
		if (linebuff[0] == '#' || *(lptr = _uls_tool(skip_blanks)(linebuff)) == '\0') 
			continue;

		linelen = linelen - (int) (lptr - linebuff);
		writeline_uld_backup(xctx, lptr, linelen);

		if (uld_proc_line(lptr, uls, n2_vx_namelist) < 0) {
			_uls_log(err_log)("#%d: Failed to process uld-line", lno);
			stat = -1;
			break;
		}
	}

	xctx->uldfile_buf[xctx->uldfile_buflen] = '\0';
	xctx->uldfile_bufsiz = uld_calc_filebuff_size(xctx->uldfile_buflen + 1);
	xctx->uldfile_buf = (char *) _uls_tool_(mrealloc)(xctx->uldfile_buf, xctx->uldfile_bufsiz);

	if (uld_post_names(names_map) < 0) {
		_uls_log(err_log)("can't process uld-file");
		stat = -1;
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_init_nam_tok)(uls_nam_tok_ptr_t nam_tok)
{
}

void
ULS_QUALIFIED_METHOD(uls_deinit_nam_tok)(uls_nam_tok_ptr_t nam_tok)
{
}

ULS_QUALIFIED_RETTYP(uld_names_map_ptr_t)
ULS_QUALIFIED_METHOD(uld_prepare_names)(uls_lex_ptr_t uls, int siz_uldfile)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
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

	xctx->uldfile_buf = (char *) _uls_tool_(malloc)(siz_uldfile);
	xctx->uldfile_bufsiz = siz_uldfile;
	xctx->uldfile_buflen = 0;

	return names_map;
}

int
ULS_QUALIFIED_METHOD(uld_change_names)(uld_names_map_ptr_t names_map, uld_line_ptr_t tnd)
{
	uls_lex_ptr_t uls = names_map->uls;
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_tokdef_vx_ptr_t e_vx;
	char linebuff[ULS_LINEBUFF_SIZ__ULD + 1];
	int k;

	if ((e_vx = uld_find_tokdef_vx(uls, names_map->n_vx_namelist, tnd->name)) == nilptr) {
		return -1;
	}

	if (uld_change_tok_names(e_vx, tnd->name2, tnd->aliases) < 0) {
		return -1;
	}

	if (tnd->tokid_changed) {
		uld_change_tok_id(e_vx, tnd->tokid);
	}

	k = _uls_tool_(strcpy)(linebuff, tnd->name);

	if (tnd->name2 != NULL) {
		linebuff[k++] = ' ';
		k += _uls_tool_(strcpy)(linebuff + k, tnd->name2);
	}

	if (tnd->tokid_changed) {
		k += _uls_log_(snprintf)(linebuff + k, 128 - k, " %d", tnd->tokid);
	}

	if (*tnd->aliases != '\0') {
		k += _uls_log_(snprintf)(linebuff + k, 128 - k, " %s", tnd->aliases);
	}

	k = _uls_tool(str_trim_end)(linebuff, k);
	linebuff[k++] = '\n';

	_uls_tool_(memcopy)(xctx->uldfile_buf + xctx->uldfile_buflen, linebuff, k);
	xctx->uldfile_buflen += k;

	return 0;
}

int
ULS_QUALIFIED_METHOD(__uld_post_names)(uld_names_map_ptr_t names_map)
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

	for (i=1; i < n_slots_vx; i++) {
		e_vx = slots_vx[i];

		if (e_vx->tok_id == prev_tok_id) {
			_uls_log(err_log)("tok-id confliction: %s :: %s", uls_get_namebuf_value(e_vx->name), prev_tok_nam);
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
ULS_QUALIFIED_METHOD(uld_post_names)(uld_names_map_ptr_t names_map)
{
	int stat = 0;
	uls_lex_ptr_t uls = names_map->uls;

	if (__uld_post_names(names_map) < 0) {
		stat = -1;
	} else {
		if (uls_classify_tok_group(uls) < 0) {
			_uls_log(err_log)("%s: lex-conf file not consistent!", __func__);
			stat = -1;
		}
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uld_change_tok_id)(uls_tokdef_vx_ptr_t e_vx, int tok_id)
{
	__change_tok_id(e_vx, tok_id);
	return 0;
}

int
ULS_QUALIFIED_METHOD(uld_change_tok_names)(uls_tokdef_vx_ptr_t e_vx, const char* name2, const char *line_aliases)
{
	if (name2 != NULL && uls_change_tokdef_vx_name(e_vx, NULL, name2) < 0) {
		return -1;
	}

	if (uld_add_aliases(e_vx, line_aliases) < 0) {
		return -1;
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(uld_dump_sample)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e_vx;
	const char *cptr;
	int i;

	_uls_log_(printf)("#@%s\n\n", uls_get_namebuf_value(uls->ulc_name));
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if ((cptr = uls_get_namebuf_value(e_vx->name))[0] != '\0')
			_uls_log_(printf)("#%-16s %-16s %3d\n", cptr, cptr, e_vx->tok_id);
	}
}
