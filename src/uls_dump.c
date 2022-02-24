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

#include "uls/uls_dump.h"
#include "uls/uls_lex.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"

void
ulc_dump_tokdef_sorted(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t  e_vx, e2_vx;
	const char *str;
	int i;

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (e_vx->base != nilptr) {
			uls_printf("\t[%2d]", i);
			uls_printf(" %d keyw'%s'\n", e_vx->tok_id, uls_get_namebuf_value(e_vx->base->keyword));
		} else {
			uls_printf("\t[%2d] %d\n", i, e_vx->tok_id);
		}
	}

	uls_printf("****************** ****************** ******************\n");

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		e2_vx = uls_find_tokdef_vx(uls, e_vx->tok_id);

		// test the correctness of uls_find_tokdef_vx.
		if (e2_vx != e_vx) {
			err_panic("[%3d] e_vx :: e2_vx !!\n", e_vx->tok_id);
		}

		str = uls_tok2keyw(uls, e_vx->tok_id);
		if (str == NULL) str = "???";

		if (e2_vx->base != nilptr) {
			uls_printf("\t[%3d] keyw'%s' ==  %s\n", e_vx->tok_id,
				uls_get_namebuf_value(e_vx->base->keyword), str);
		} else {
			uls_printf("\t[%3d] keyw'%s'\n", e_vx->tok_id, str);
		}
	}
}

void
uls_dump_char_context(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	int i, j, ch;

	for (i=0; i < 32; i++) {
		uls_printf("%02X: ", 4*i);
		for (j=0; j < 4; j++) {
			ch = 4*i + j;

			if (uls_isgraph(ch))
				uls_printf("  <%c> ", ch);
			else
				uls_printf("  < > ");

			uls_print_bytes(ch_ctx + ch, 1);
		}
		uls_printf("\n");
	}
}

void
uls_dump_quote(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt;
	int i, ch;

	uls_printf("Literal Strings:\n");
	uls_printf("\tQuote chars:\n\t\t");
	for (ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (ch_ctx[ch] & ULS_CH_QUOTE) {
			uls_printf("%c ", ch);
		}
	}
	uls_printf("\n");

	uls_printf("\tQuote types:\n");

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		uls_printf("\t\t%d] %s ~ %s", i,
			uls_get_namebuf_value(qmt->start_mark), uls_get_namebuf_value(qmt->end_mark));
		uls_printf(" '%s' %d\n", uls_get_namebuf_value(qmt->tokdef_vx->name), qmt->tok_id);
	}

	uls_printf("\n");
}


void
uls_dump_2char(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	uls_twoplus_tree_ptr_t tree;
	uls_decl_parray_slots(slots_tp, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	int i, ch;

	uls_printf("2+CHAR TOKEN:\n");
	uls_printf("\tfirst_chars:");
	for (i=0, ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if ((ch_ctx[ch] & ULS_CH_2PLUS)==0)
			continue;
		uls_printf(" %3d(%c)", ch, ch);

		if (((++i) % 8)==0) uls_printf("\n");
	}
	uls_printf("\n");

	uls_printf("2+CHAR types:");
	for (tree=uls->twoplus_table.start; tree != nilptr; tree=tree->prev) {
		uls_printf("\t[LEN=%d]\n", tree->len_keyw);

		slots_tp = uls_parray_slots(uls_ptr(tree->twoplus_sorted));
		for (i=0; i<tree->twoplus_sorted.n; i++) {
			e_vx = slots_tp[i];

			if ((e = e_vx->base) == nilptr) {
				uls_printf("\t'%s' --> %d\n", uls_get_namebuf_value(e->keyword), e_vx->tok_id);
			} else {
				uls_printf("\t'%s' --> %-10s %d\n",
					uls_get_namebuf_value(e->keyword), uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
			}
		}
	}
}

void
uls_dump_utf8(uls_lex_ptr_t uls)
{
	int i, j;
	int ch, ch0;
	uls_uch_range_ptr_t ran;

	for (i=0; i < 16; i++) {
		ch0 = 128 + 8*i;
		uls_printf("%02X: ", ch0);

		for (j=0; j<8; j++) {
			ch = ch0 + j;

//			uls_print_bytes(&ch, 1);
			if ((ch & 0xF8) == 0xF0 || (ch & 0xF0) == 0xE0 || (ch & 0xE0) == 0xC0)
				uls_printf("  <U> ");
			else
				uls_printf("  < > ");
		}
		uls_printf("\n");
	}

	uls_printf(" ********** (Unicode) ID-FIRST charset ranges **************\n");
	for (i=0; i<uls->idfirst_charset.n; i++) {
		ran = uls_array_get_slot_type01(uls_ptr(uls->idfirst_charset), i);
		uls_printf("%2d] 0x%04X - 0x%04X\n", i, ran->x1, ran->x2);
	}

	uls_printf(" ********** (Unicode) ID charset ranges **************\n");
	for (i=0; i<uls->id_charset.n; i++) {
		ran = uls_array_get_slot_type01(uls_ptr(uls->id_charset), i);
		uls_printf("%2d] 0x%04X - 0x%04X\n", i, ran->x1, ran->x2);
	}
}

void
uls_dump_char_tokmap(uls_lex_ptr_t uls)
{
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_onechar_tokdef_etc_ptr_t  e_etc;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;

	int i, j;
	uls_uch_t uch, uch0;

	uls_printf("1-CHAR TOKEN MAP(map):\n");
	for (i=0; i < ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_array_get_slot_type00(uls_ptr(uls->onechar_table.tokgrps), i);
		uch0 = tokgrp->uch0;

		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		for (j=0; j < tokgrp->tokdef_vx_1char.n; j++) {
			uch = uch0 + j;

			if ((e_vx = slots_vx[j]) != nilptr) {
				print_tokdef_vx_char(uch, e_vx);
			}
		}
	}

	uls_printf("1-CHAR TOKEN MAP(list):\n");
	for (e_etc = uls->onechar_table.tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		print_tokdef_vx_char(e_etc->uch, e_etc->tokdef_vx);
	}
}

void
uls_dump_1char(uls_lex_ptr_t uls)
{
	const char *ch_ctx = uls->xcontext.ch_context;
	int i, ch;

	uls_printf("1-CHAR TOKEN:\n");
	for (i=0, ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if ((ch_ctx[ch] & ULS_CH_1) == 0)
			continue;

		if (uls_isgraph(ch))
			uls_printf("\t%3d(%c)", ch, ch);
		else
			uls_printf("\t%3d   ", ch);

		if (((++i) % 8)==0) uls_printf("\n");
	}
	uls_printf("\n");

	uls_dump_char_tokmap(uls);
}

void
uls_dump_tokdef_rsvd(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots(slots_rsv, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
	for (i=0; i < N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		uls_printf("\t%-10s: %2d\n", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
	}
}

void
uls_dump_tokdef_vx(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	uls_tokdef_ptr_t e;
	int i;

	uls_printf(" ********** The list of tokens by tok-id **************\n");
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e0_vx = slots_vx[i];

		if ((e = e0_vx->base) != nilptr) {
			uls_printf("%3d] %s '%s' :", e0_vx->tok_id,
				uls_get_namebuf_value(e0_vx->name), uls_get_namebuf_value(e->keyword));
			e = e->next;
		} else {
			uls_printf("%3d] %s :", e0_vx->tok_id, uls_get_namebuf_value(e0_vx->name));
		}

		if ((e_nam = e0_vx->tokdef_names) != nilptr) {
			for ( ; e_nam != nilptr; e_nam = e_nam->prev) {
				uls_printf(" %s", uls_get_namebuf_value(e_nam->name));
			}
		}
		uls_printf("\n");

		for ( ; e != nilptr; e = e->next) {
			uls_printf("\t'%s'\n", uls_get_namebuf_value(e->keyword));
		}
	}
}

void
uls_dump_tokdef_names(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_name_ptr_t e_nam;
	uls_tokdef_vx_ptr_t e0_vx;
	int i;

	uls_printf(" ********** The list of tokens by tok-id **************\n");
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e0_vx = slots_vx[i];

		uls_printf("%3d] %s\n", e0_vx->tok_id, uls_get_namebuf_value(e0_vx->name));

		if ((e_nam = e0_vx->tokdef_names) != nilptr) {
			uls_printf("\tNAMES:");
			for ( ; e_nam != nilptr; e_nam = e_nam->prev) {
				uls_printf(" %s", uls_get_namebuf_value(e_nam->name));
			}
			uls_printf("\n");
		}
	}
}

void
dump_fd_tower(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx;
	int tower_level = 0;

	uls_printf("+) --------------------------------------\n");
	for (ctx=uls->xcontext.context; ctx!=nilptr;
		ctx = ctx->prev, --tower_level) {
		uls_printf("level=%d] lptr0='%s'\n", tower_level, ctx->lptr);
	}
	uls_printf("-) --------------------------------------\n");
}

void
uls_dump_kwtable(uls_kwtable_ptr_t tbl)
{
	uls_decl_parray_slots(slots_bh, tokdef);
	uls_tokdef_ptr_t  e;
	uls_tokdef_vx_ptr_t  e_vx;
	int i;

	uls_printf(" HASH_TABLE(keyw) size = %d\n", tbl->bucket_head.n);

	slots_bh = uls_parray_slots(uls_ptr(tbl->bucket_head));
	for (i=0; i<tbl->bucket_head.n; i++) {
		if ((e = slots_bh[i]) == nilptr) continue;
		uls_printf("bucket-%d]\n", i);

		for ( ; e != nilptr; e = e->link) {
			e_vx = e->view;
			uls_printf("\t'%s' --> %-10s %d\n",
				uls_get_namebuf_value(e->keyword), uls_get_namebuf_value(e->view->name), e_vx->tok_id);
		}
	}
}
