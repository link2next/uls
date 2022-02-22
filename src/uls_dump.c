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
 * uls_dump.c -- Dumping uls token setting --
 *     written by Stanley J. Hong <link2next@gmail.com>, June 2018.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#define __ULS_DUMP__
#include "uls/uls_dump.h"
#include "uls/uls_lex.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"

void
ULS_QUALIFIED_METHOD(ulc_dump_tokdef_sorted)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t  e_vx, e2_vx;
	const char *str;
	int i;

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (e_vx->base != nilptr) {
			_uls_log_(printf)("\t[%2d]", i);
			_uls_log_(printf)(" %d keyw'%s'\n", e_vx->tok_id, uls_get_namebuf_value(e_vx->base->keyword));
		} else {
			_uls_log_(printf)("\t[%2d] %d\n", i, e_vx->tok_id);
		}
	}

	_uls_log_(printf)("****************** ****************** ******************\n");

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		e2_vx = uls_find_tokdef_vx(uls, e_vx->tok_id);

		// test the correctness of uls_find_tokdef_vx.
		if (e2_vx != e_vx) {
			_uls_log(err_panic)("[%3d] e_vx :: e2_vx !!\n", e_vx->tok_id);
		}

		str = uls_tok2keyw(uls, e_vx->tok_id);
		if (str == NULL) str = "???";

		if (e2_vx->base != nilptr) {
			_uls_log_(printf)("\t[%3d] keyw'%s' ==  %s\n", e_vx->tok_id,
				uls_get_namebuf_value(e_vx->base->keyword), str);
		} else {
			_uls_log_(printf)("\t[%3d] keyw'%s'\n", e_vx->tok_id, str);
		}
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_char_context)(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	int i, j, ch;

	for (i=0; i < 32; i++) {
		_uls_log_(printf)("%02X: ", 4*i);
		for (j=0; j < 4; j++) {
			ch = 4*i + j;

			if (_uls_tool_(isgraph)(ch))
				_uls_log_(printf)("  <%c> ", ch);
			else
				_uls_log_(printf)("  < > ");

			_uls_tool_(print_bytes)(ch_ctx + ch, 1);
		}
		_uls_log_(printf)("\n");
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_quote)(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt;
	int i, ch;

	_uls_log_(printf)("Literal Strings:\n");
	_uls_log_(printf)("\tQuote chars:\n\t\t");
	for (ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (ch_ctx[ch] & ULS_CH_QUOTE) {
			_uls_log_(printf)("%c ", ch);
		}
	}
	_uls_log_(printf)("\n");

	_uls_log_(printf)("\tQuote types:\n");

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		_uls_log_(printf)("\t\t%d] %s ~ %s", i,
			uls_get_namebuf_value(qmt->start_mark), uls_get_namebuf_value(qmt->end_mark));
		_uls_log_(printf)(" '%s' %d\n", uls_get_namebuf_value(qmt->tokdef_vx->name), qmt->tok_id);
	}

	_uls_log_(printf)("\n");
}


void
ULS_QUALIFIED_METHOD(uls_dump_2char)(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	uls_twoplus_tree_ptr_t tree;
	uls_decl_parray_slots(slots_tp, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	int i, ch;

	_uls_log_(printf)("2+CHAR TOKEN:\n");
	_uls_log_(printf)("\tfirst_chars:");
	for (i=0, ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if ((ch_ctx[ch] & ULS_CH_2PLUS)==0)
			continue;
		_uls_log_(printf)(" %3d(%c)", ch, ch);

		if (((++i) % 8)==0) _uls_log_(printf)("\n");
	}
	_uls_log_(printf)("\n");

	_uls_log_(printf)("2+CHAR types:");
	for (tree=uls->twoplus_table.start; tree != nilptr; tree=tree->prev) {
		_uls_log_(printf)("\t[LEN=%d]\n", tree->len_keyw);

		slots_tp = uls_parray_slots(uls_ptr(tree->twoplus_sorted));
		for (i=0; i<tree->twoplus_sorted.n; i++) {
			e_vx = slots_tp[i];

			if ((e = e_vx->base) == nilptr) {
				_uls_log_(printf)("\t'%s' --> %d\n", uls_get_namebuf_value(e->keyword), e_vx->tok_id);
			} else {
				_uls_log_(printf)("\t'%s' --> %-10s %d\n",
					uls_get_namebuf_value(e->keyword), uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
			}
		}
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_utf8)(uls_lex_ptr_t uls)
{
	int i, j;
	int ch, ch0;
	uls_ptrtype_tool(uch_range) ran;

	for (i=0; i < 16; i++) {
		ch0 = 128 + 8*i;
		_uls_log_(printf)("%02X: ", ch0);

		for (j=0; j<8; j++) {
			ch = ch0 + j;

//			_uls_tool_(print_bytes)(&ch, 1);
			if ((ch & 0xF8) == 0xF0 || (ch & 0xF0) == 0xE0 || (ch & 0xE0) == 0xC0)
				_uls_log_(printf)("  <U> ");
			else
				_uls_log_(printf)("  < > ");
		}
		_uls_log_(printf)("\n");
	}

	_uls_log_(printf)(" ********** (Unicode) ID-FIRST charset ranges **************\n");
	for (i=0; i<uls->idfirst_charset.n; i++) {
		ran = uls_get_array_slot_type01(uls_ptr(uls->idfirst_charset), i);
		_uls_log_(printf)("%2d] 0x%04X - 0x%04X\n", i, ran->x1, ran->x2);
	}

	_uls_log_(printf)(" ********** (Unicode) ID charset ranges **************\n");
	for (i=0; i<uls->id_charset.n; i++) {
		ran = uls_get_array_slot_type01(uls_ptr(uls->id_charset), i);
		_uls_log_(printf)("%2d] 0x%04X - 0x%04X\n", i, ran->x1, ran->x2);
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_char_tokmap)(uls_lex_ptr_t uls)
{
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_onechar_tokdef_etc_ptr_t  e_etc;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;

	int i, j;
	uls_uch_t uch, uch0;

	_uls_log_(printf)("1-CHAR TOKEN MAP(map):\n");
	for (i=0; i < ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(uls->onechar_table.tokgrps), i);
		uch0 = tokgrp->uch0;

		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		for (j=0; j < tokgrp->tokdef_vx_1char.n; j++) {
			uch = uch0 + j;

			if ((e_vx = slots_vx[j]) != nilptr) {
				print_tokdef_vx_char(uch, e_vx);
			}
		}
	}

	_uls_log_(printf)("1-CHAR TOKEN MAP(list):\n");
	for (e_etc = uls->onechar_table.tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		print_tokdef_vx_char(e_etc->uch, e_etc->tokdef_vx);
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_1char)(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	int i, ch;

	_uls_log_(printf)("1-CHAR TOKEN:\n");
	for (i=0, ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if ((ch_ctx[ch] & ULS_CH_1) == 0)
			continue;

		if (_uls_tool_(isgraph)(ch))
			_uls_log_(printf)("\t%3d(%c)", ch, ch);
		else
			_uls_log_(printf)("\t%3d   ", ch);

		if (((++i) % 8)==0) _uls_log_(printf)("\n");
	}
	_uls_log_(printf)("\n");

	uls_dump_char_tokmap(uls);
}

void
ULS_QUALIFIED_METHOD(uls_dump_tokdef_rsvd)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots(slots_rsv, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
	for (i=0; i < N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		_uls_log_(printf)("\t%-10s: %2d\n", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_tokdef_vx)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	uls_tokdef_ptr_t e;
	int i;

	_uls_log_(printf)(" ********** The list of tokens by tok-id **************\n");
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e0_vx = slots_vx[i];

		if ((e = e0_vx->base) != nilptr) {
			_uls_log_(printf)("%3d] %s '%s' :", e0_vx->tok_id,
				uls_get_namebuf_value(e0_vx->name), uls_get_namebuf_value(e->keyword));
			e = e->next;
		} else {
			_uls_log_(printf)("%3d] %s :", e0_vx->tok_id, uls_get_namebuf_value(e0_vx->name));
		}

		if ((e_nam = e0_vx->tokdef_names) != nilptr) {
			for ( ; e_nam != nilptr; e_nam = e_nam->prev) {
				_uls_log_(printf)(" %s", uls_get_namebuf_value(e_nam->name));
			}
		}
		_uls_log_(printf)("\n");

		for ( ; e != nilptr; e = e->next) {
			_uls_log_(printf)("\t'%s'\n", uls_get_namebuf_value(e->keyword));
		}
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_tokdef_names)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_name_ptr_t e_nam;
	uls_tokdef_vx_ptr_t e0_vx;
	int i;

	_uls_log_(printf)(" ********** The list of tokens by tok-id **************\n");
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e0_vx = slots_vx[i];

		_uls_log_(printf)("%3d] %s\n", e0_vx->tok_id, uls_get_namebuf_value(e0_vx->name));

		if ((e_nam = e0_vx->tokdef_names) != nilptr) {
			_uls_log_(printf)("\tNAMES:");
			for ( ; e_nam != nilptr; e_nam = e_nam->prev) {
				_uls_log_(printf)(" %s", uls_get_namebuf_value(e_nam->name));
			}
			_uls_log_(printf)("\n");
		}
	}
}

void
ULS_QUALIFIED_METHOD(dump_fd_tower)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx;
	int tower_level = 0;

	_uls_log_(printf)("+) --------------------------------------\n");
	for (ctx=uls->xcontext.context; ctx!=nilptr;
		ctx = ctx->prev, --tower_level) {
		_uls_log_(printf)("level=%d] lptr0='%s'\n", tower_level, ctx->lptr);
	}
	_uls_log_(printf)("-) --------------------------------------\n");
}

void
ULS_QUALIFIED_METHOD(uls_dump_kwtable)(uls_kwtable_ptr_t tbl)
{
	uls_decl_parray_slots(slots_bh, tokdef);
	uls_tokdef_ptr_t  e;
	uls_tokdef_vx_ptr_t  e_vx;
	int i;

	_uls_log_(printf)(" HASH_TABLE(keyw) size = %d\n", tbl->bucket_head.n);

	slots_bh = uls_parray_slots(uls_ptr(tbl->bucket_head));
	for (i=0; i<tbl->bucket_head.n; i++) {
		if ((e = slots_bh[i]) == nilptr) continue;
		_uls_log_(printf)("bucket-%d]\n", i);

		for ( ; e != nilptr; e = e->link) {
			e_vx = e->view;
			_uls_log_(printf)("\t'%s' --> %-10s %d\n",
				uls_get_namebuf_value(e->keyword), uls_get_namebuf_value(e->view->name), e_vx->tok_id);
		}
	}
}

void
ULS_QUALIFIED_METHOD(dump_tokdef_vx__yaml_commtype)(int ind, uls_lex_ptr_t uls, uls_commtype_ptr_t cmt)
{
	uls_type_tool(outparam) parms1;
	char outbuf_keyw[128];

	uls_sysprn_tabs(1, "%d:\n", ind);

	parms1.lptr = uls_get_namebuf_value(cmt->start_mark);
	parms1.line = outbuf_keyw;
	uls_get_simple_unescape_str(uls_ptr(parms1));
	uls_sysprn_tabs(2, "start: %s\n", outbuf_keyw);

	parms1.lptr = uls_get_namebuf_value(cmt->end_mark);
	parms1.line = outbuf_keyw;
	uls_get_simple_unescape_str(uls_ptr(parms1));
	uls_sysprn_tabs(2, "end: %s\n", outbuf_keyw);

	if (cmt->flags & (ULS_COMM_ONELINE | ULS_COMM_COLUMN0 | ULS_COMM_NESTED)) {
		uls_sysprn_tabs(2, "options:\n");

		if (cmt->flags & ULS_COMM_ONELINE) {
			uls_sysprn_tabs(3, "- oneline\n");
		}

		if (cmt->flags & ULS_COMM_COLUMN0) {
			uls_sysprn_tabs(3, "- column0\n");
		}

		if (cmt->flags & ULS_COMM_NESTED) {
			uls_sysprn_tabs(3, "- nested\n");
		}
	}
}

void
ULS_QUALIFIED_METHOD(dump_tokdef_vx__yaml_quotetype)(int ind, uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt)
{
	int tok_id;
	uls_type_tool(outparam) parms1;
	uls_escmap_ptr_t esc_map;
	char outbuf_keyw[128];
	const char *tok_nam;

	tok_id = qmt->tok_id;
	tok_nam = uls_get_namebuf_value(qmt->tokdef_vx->name);
	uls_sysprn_tabs(1, "%d:\n", ind);
	uls_sysprn_tabs(2, "id: %d\n", tok_id);
	uls_sysprn_tabs(2, "name: %s\n", tok_nam);

	parms1.lptr = uls_get_namebuf_value(qmt->start_mark);
	parms1.line = outbuf_keyw;
	uls_get_simple_unescape_str(uls_ptr(parms1));
	uls_sysprn_tabs(2, "start: %s\n", outbuf_keyw);

	parms1.lptr = uls_get_namebuf_value(qmt->end_mark);
	parms1.line = outbuf_keyw;
	uls_get_simple_unescape_str(uls_ptr(parms1));
	uls_sysprn_tabs(2, "end: %s\n", outbuf_keyw);

	if (qmt->flags & (ULS_QSTR_ASYMMETRIC | ULS_QSTR_OPEN | ULS_QSTR_MULTILINE | ULS_QSTR_R_EXCLUSIVE) ) {
		uls_sysprn_tabs(2, "options:\n");

		if (qmt->flags & ULS_QSTR_ASYMMETRIC) {
			uls_sysprn_tabs(3, "- asymmetric\n");
		}
		if (qmt->flags & ULS_QSTR_NOTHING) {
			uls_sysprn_tabs(3, "- nothing\n");
		}
		if (qmt->flags & ULS_QSTR_OPEN) {
			uls_sysprn_tabs(3, "- open\n");
		}
		if (qmt->flags & ULS_QSTR_ASYMMETRIC) {
			uls_sysprn_tabs(3, "- right_exclusive\n");
		}
	}

	esc_map = qmt->escmap;
	if (esc_map->flags & ULS_ESCMAP_MODE__MASK) {
		uls_sysprn_tabs(2, "mode:\n");

		if (esc_map->flags & ULS_ESCMAP_MODE__LEGACY) {
			uls_sysprn_tabs(3, "- legacy\n");
		}
		if (esc_map->flags & ULS_ESCMAP_MODE__MODERN) {
			uls_sysprn_tabs(3, "- modern\n");
		}
		if (esc_map->flags & ULS_ESCMAP_MODE__VERBATIM) {
			uls_sysprn_tabs(3, "- verbatim\n");
		}
		if (esc_map->flags & ULS_ESCMAP_MODE__LEGACY_FULL) {
			uls_sysprn_tabs(3, "- legacy_full\n");
		}
		if (esc_map->flags & ULS_ESCMAP_MODE__VERBATIM_MODERATE) {
			uls_sysprn_tabs(3, "- verbatim_moderate\n");
		}
	}
}

void
ULS_QUALIFIED_METHOD(dump_tokdef_vx__yaml_rename)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots(slots_rsv, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	const char *wrd, *wrd2;

	// check the attribute 'rename:'
	slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));

	/* LINENUM */
	e_vx = slots_rsv[LINENUM_TOK_IDX];
	wrd = "LINENUM";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* NONE */
	e_vx = slots_rsv[NONE_TOK_IDX];
	wrd = "NONE";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* ERR */
	e_vx = slots_rsv[ERR_TOK_IDX];
	wrd = "ERR";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* EOI */
	e_vx = slots_rsv[EOI_TOK_IDX];
	wrd = "EOI";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* EOF */
	e_vx = slots_rsv[EOF_TOK_IDX];
	wrd = "EOF";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* ID */
	e_vx = slots_rsv[ID_TOK_IDX];
	wrd = "ID";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* NUMBER */
	e_vx = slots_rsv[NUM_TOK_IDX];
	wrd = "NUMBER";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* TEMPLATE */
	e_vx = slots_rsv[TMPL_TOK_IDX];
	wrd = "TMPL";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}

	/* LINK */
	e_vx = slots_rsv[LINK_TOK_IDX];
	wrd = "LINK";
	wrd2 = uls_get_namebuf_value(e_vx->name);
	if (!uls_streql(wrd, wrd2)) {
		uls_sysprn_tabs(0, "rename: %s %s\n", wrd, wrd2);
	}
}

void
ULS_QUALIFIED_METHOD(dump_tokdef_vx__yaml)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	uls_tokdef_ptr_t e;

	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt;
	uls_commtype_ptr_t cmt;

	uls_number_prefix_ptr_t numpfx;

	uls_type_tool(outparam) parms1;
	const char *tok_nam, *keyw;
	char outbuf_keyw[128];
	int i, tok_id;

	uls_sysprn_tabs(0, "ulc-format-ver: 2.9\n");

	if (uls->flags & ULS_FL_CASE_INSENSITIVE) {
		keyw = "false";
	} else {
		keyw = "true";
	}
	uls_sysprn_tabs(0, "case-sensitive: %s\n", keyw);

	tok_id = uls->numcnst_separator;
	if (tok_id != ULS_DECIMAL_SEPARATOR_DFL) {
		uls_sysprn_tabs(0, "number-separator: %c\n", (char) tok_id);
	}

	if ((i=uls->id_max_bytes) != INT_MAX) {
		uls_sysprn_tabs(0, "id-max-bytes: %d\n", i);
	}

	if ((i=uls->id_max_uchars) != INT_MAX) {
		uls_sysprn_tabs(0, "id_max_uchars: %d\n", i);
	}

	if (uls->n_numcnst_prefixes > 0) {
		uls_sysprn_tabs(0, "number-prefixes:\n");
		for (i=0; i<uls->n_numcnst_prefixes; i++) {
			numpfx = uls_get_array_slot_type00(uls_ptr(uls->numcnst_prefixes), i);
			keyw = uls_get_namebuf_value(numpfx->prefix);

			uls_sysprn_tabs(1, "- prefix: %s\n", keyw);
			uls_sysprn_tabs(2, "radix: %d\n", numpfx->radix);
		}
	}

	if (*(keyw = uls->numcnst_suffixes) != '\0') {
		uls_sysprn_tabs(0, "number-sufficies:\n");
		do {
			i = _uls_tool_(strlen)(keyw);
			uls_sysprn_tabs(1, "- %s\n", keyw);
			keyw += i + 1;
		} while (*keyw != '\0');
	}

	if (uls->n1_commtypes > 0) {
		uls_sysprn_tabs(0, "comm-types:\n");

		for (i=0; i<uls->n1_commtypes; i++) {
			cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
			dump_tokdef_vx__yaml_commtype(i + 1, uls, cmt);
		}
		uls_sysprn_tabs(0, "\n");
	}

	if (uls->quotetypes.n > 0) {
		uls_sysprn_tabs(0, "quote-types:\n");
		slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
		for (i=0; i<uls->quotetypes.n; i++) {
			qmt = slots_qmt[i];

			dump_tokdef_vx__yaml_quotetype(i + 1, uls, qmt);
		}
		uls_sysprn_tabs(0, "\n");
	}

	dump_tokdef_vx__yaml_rename(uls);
	uls_sysprn_tabs(0, "\n");

	// the token list
	uls_sysprn_tabs(0, "tokens:\n");
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e0_vx = slots_vx[i];

		tok_id = e0_vx->tok_id;
		tok_nam = uls_get_namebuf_value(e0_vx->name);

		if ((e = e0_vx->base) != nilptr) {
			keyw = uls_get_namebuf_value(e->keyword);

			parms1.lptr = keyw;
			parms1.line = outbuf_keyw;
			uls_get_simple_unescape_str(uls_ptr(parms1));
			keyw = outbuf_keyw;

			e = e->next;
		} else {
			keyw = NULL;
		}

		uls_sysprn_tabs(1, "- id: %d\n", tok_id);
		uls_sysprn_tabs(2, "name: %s\n", tok_nam);
		if (keyw != NULL) {
			uls_sysprn_tabs(2, "keyw: %s\n", keyw);
		}

		if ((e_nam = e0_vx->tokdef_names) != nilptr) {
			for ( ; e_nam != nilptr; e_nam = e_nam->prev) {
				tok_nam = uls_get_namebuf_value(e_nam->name);
				uls_sysprn_tabs(1, "- id: %d\n", tok_id);
				uls_sysprn_tabs(2, "name: %s\n", tok_nam);
				uls_sysprn_tabs(2, "keyw: %s\n", keyw);
			}
		}

		for ( ; e != nilptr; e = e->next) {
			keyw = uls_get_namebuf_value(e->keyword);
			uls_sysprn_tabs(1, "- id: %d\n", tok_id);
			uls_sysprn_tabs(2, "keyw: %s\n", keyw);
		}
	}
}

int
ULS_QUALIFIED_METHOD(uls_dump_tokdef_vx__yaml)(const char *ulc_config, int flags)
{
	int signed_yaml = 0, stat = 0;
	FILE *fout = uls_get_cvt2yaml();
	uls_lex_ptr_t uls;

	if (flags & 0x01) {
		signed_yaml = 1;
	}

	if (_uls_log_(sysprn_open)(fout, nilptr) < 0) {
		_uls_log(err_log)("%s: create an output file", __func__);
		return -1;

	}

	_uls_log_(sysprn_set_tabsiz)(2);
	if (signed_yaml) {
		uls_sysprn_tabs(0, "%%YAML 1.2\n");
		uls_sysprn_tabs(0, "---\n");
	}

	if ((uls=uls_create(ulc_config)) == uls_nil) {
		_uls_log(err_log)("%s: Failed to open the configuration file %s.", __func__, ulc_config);
		stat = -1;
	} else {
		dump_tokdef_vx__yaml(uls);
		uls_destroy(uls);
	}

	if (signed_yaml) {
		uls_sysprn_tabs(0, "...\n");
	}
	_uls_log_(sysprn_close)();

	return stat;
}
