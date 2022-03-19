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
 * unget.c -- unget_* routines, unget_ch, unget_tok --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#define __ULS_UNGET__
#include "uls/unget.h"
#include "uls/uls_core.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC char*
__find_first_space_char(const char* lptr, const char* lptr_end)
{
	char ch;

	for ( ; lptr < lptr_end; lptr++) {
		if ((ch=*lptr) == ' ')
			break;
	}

	return (char *) lptr;
}

ULS_DECL_STATIC int
__numof_lfs(uls_outparam_ptr_t parms)
{
	const char* str = parms->lptr;
	int i, len, n = 0;
	const char* ptr;
	char ch;

	if ((len = parms->len) < 0) {
		for (ptr=str; (ch=*ptr) != '\0'; ptr++) {
			if (ch == '\n') ++n;
		}
	} else {
		for (ptr=str, i=0; (ch=*ptr) != '\0' && i<len; ptr++, i++) {
			if (ch == '\n') ++n;
		}
		for ( ; *ptr != '\0'; ptr++) {
		}
	}

	parms->n = n;
	return (int) (ptr - str);
}

ULS_DECL_STATIC int
__alloc_lexseg_and_zbuf(uls_context_ptr_t ctx, uls_lexseg_ptr_t lexseg, int len,
	uls_tokdef_vx_ptr_t e_vx, const char *qstr, int qlen)
{
	char *lptr;
	int k1, siz1, k2;

	k1 = csz_length(uls_ptr(ctx->zbuf1));

	siz1 = ULS_UNGETS_BUFSIZ + len;
	if (siz1 < 0) siz1 = 0;
	ctx->line = lptr = csz_modify(uls_ptr(ctx->zbuf1), k1, NULL, siz1 + 1);

	ctx->lptr = lptr += ULS_UNGETS_BUFSIZ;
	ctx->line_end = lptr += len;
	*lptr = '\0';

	if (e_vx != nilptr) {
		if (qstr != NULL) {
			k2 = csz_length(uls_ptr(ctx->zbuf2));
			lptr = csz_modify(uls_ptr(ctx->zbuf2), k2, NULL, qlen + 1);
			if (qlen < 0) qlen = uls_strlen(qstr);
			uls_memcopy(lptr, qstr, qlen);
		} else {
			k2 = 0;
			qlen = 0;
		}
	} else {
		k2 = 0;
		qlen = -1;
	}

	uls_reset_lexseg(lexseg, k1, siz1, k2, qlen, e_vx);
	return k1 + ULS_UNGETS_BUFSIZ;
}

ULS_DECL_STATIC uls_context_ptr_t
__push_and_alloc_line_right(uls_lex_ptr_t uls, int len,
	uls_tokdef_vx_ptr_t e_vx, const char *qstr, int qlen, int num_lfs)
{
	uls_context_ptr_t ctx;
	uls_input_ptr_t   inp;
	uls_lexseg_ptr_t lexseg;
	const char *old_tag;
	int old_lno;

	old_tag = __uls_get_tag(uls);
	old_lno = __uls_get_lineno(uls);

	ctx = uls_push_context(uls, nilptr);
	uls_context_set_tag(ctx, old_tag, old_lno);

	inp = ctx->input;
	inp->rawbuf_ptr = "";
	inp->rawbuf_bytes = 0;
	inp->isource.flags |= ULS_ISRC_FL_EOF;

	ctx->flags |= ULS_CTX_FL_UNGET_CONTEXT;
	ctx->flags &= ~ULS_CTX_FL_WANT_EOFTOK;

	ctx->n_lexsegs = ctx->lexsegs.n - 1;

	if (qstr == NULL) {
		ctx->i_lexsegs = ctx->n_lexsegs;
		lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs);
		__alloc_lexseg_and_zbuf(ctx, lexseg, len, nilptr, NULL, -1);

	} else {
		ctx->i_lexsegs = ctx->n_lexsegs - 1;
		lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs);
		lexseg->n_lfs_raw = num_lfs;
		__alloc_lexseg_and_zbuf(ctx, lexseg, len, e_vx, qstr, qlen);

		lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->n_lexsegs);
		uls_reset_lexseg(lexseg, 0, 0, -1, -1, nilptr);
	}

	return ctx;
}

ULS_DECL_STATIC int
__advance_upto_nonspace(uls_context_ptr_t ctx)
{
	const char *lptr2;
	int len1;

	lptr2 = __find_first_space_char(ctx->lptr, ctx->line_end);
	len1 = (int) (lptr2 - ctx->lptr);
	ctx->lptr = lptr2;

	return len1;
}

ULS_DECL_STATIC uls_context_ptr_t
__uls_unget_str(uls_lex_ptr_t uls, const char* str, int len)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_lexseg_ptr_t lexseg;
	int len1, k1;
	const char *lptr1;
	char *lptr;

	if (ctx->flags & ULS_CTX_FL_GETTOK_RAW) {
		if (ctx->flags & ULS_CTX_FL_UNGET_CONTEXT) {
			if (ctx->line + len <= ctx->lptr) {
				lptr1 = NULL; len1 = 0;
				ctx->lptr = ctx->lptr - len;
			} else if (ctx->i_lexsegs > 0) {
				k1 = (int) (ctx->lptr - csz_data_ptr(uls_ptr(ctx->zbuf1)));
				len1 = __advance_upto_nonspace(ctx);
				lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), --ctx->i_lexsegs);
				__alloc_lexseg_and_zbuf(ctx, lexseg, len + len1, nilptr, NULL, -1);
				lptr1 = csz_data_ptr(uls_ptr(ctx->zbuf1)) + k1;
			} else {
				lptr1 = ctx->lptr;
				len1 = __advance_upto_nonspace(ctx);
				ctx = __push_and_alloc_line_right(uls, len + len1, nilptr, NULL, -1, 0);
			}
		} else {
			lptr1 = ctx->lptr;
			len1 = __advance_upto_nonspace(ctx);
			ctx = __push_and_alloc_line_right(uls, len + len1, nilptr, NULL, -1, 0);
		}

		lptr = (char *) ctx->lptr;
		if (str != NULL) {
			uls_memmove(lptr, str, len);
		}
		lptr += len;

		if (len1 > 0) {
			uls_memcopy(lptr, lptr1, len1);
		}

	} else {
		ctx = __push_and_alloc_line_right(uls, len, nilptr, NULL, -1, 0);
		lptr = (char *) ctx->lptr;
		if (str != NULL) {
			uls_memmove(lptr, str, len);
		}
	}

	return ctx;
}

ULS_DECL_STATIC uls_context_ptr_t
__uls_unget_quote(uls_lex_ptr_t uls,
	const char* qstr, int qlen, uls_tokdef_vx_ptr_t e_vx, int lf_delta)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_lexseg_ptr_t lexseg;

	if (ctx->flags & ULS_CTX_FL_GETTOK_RAW) {
		if (ctx->flags & ULS_CTX_FL_UNGET_CONTEXT) {
			if (ctx->i_lexsegs > 0) {
				lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), --ctx->i_lexsegs);
				__alloc_lexseg_and_zbuf(ctx, lexseg, 0, e_vx, qstr, qlen);
				lexseg->n_lfs_raw = lf_delta;
			} else {
				ctx = __push_and_alloc_line_right(uls, 0, e_vx, qstr, qlen, lf_delta);
			}
		} else {
			ctx = __push_and_alloc_line_right(uls, 0, e_vx, qstr, qlen, lf_delta);
		}
	} else {
		ctx = __push_and_alloc_line_right(uls, 0, e_vx, qstr, qlen, lf_delta);
	}

	return ctx;
}

uls_context_ptr_t
__uls_unget_tok(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_outparam_t parms;
	uls_lexseg_ptr_t lexseg_prev;
	int n_lfs, l_lxm;
	const char *lxm;
	char *lptr;

	if (ctx->flags & ULS_CTX_FL_QTOK) {
		lexseg_prev = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs - 1);
		n_lfs = lexseg_prev->n_lfs_raw;
		ctx = __uls_unget_quote(uls, __uls_lexeme(uls), __uls_lexeme_len(uls), uls->tokdef_vx, n_lfs);
		uls_context_inc_lineno(ctx, -n_lfs);

	} else {
		lxm = __uls_lexeme(uls);
		l_lxm = __uls_lexeme_len(uls);

		parms.lptr = lxm;
		parms.len = l_lxm;
		__numof_lfs(uls_ptr(parms));
		n_lfs = parms.n;

		if (uls_is_int(uls)) {
			ctx = __uls_unget_str(uls, NULL, 2 + l_lxm + 1);
			lptr = (char *) ctx->lptr;
			lptr[0] = '0'; lptr[1] = 'x'; lptr += 2;
		} else {
			ctx = __uls_unget_str(uls, NULL, l_lxm + 1);
			lptr = (char *) ctx->lptr;
		}

		uls_memmove(lptr, lxm, l_lxm);
		lptr += l_lxm;
		*lptr = ' ';

		uls_context_inc_lineno(ctx, -n_lfs);
	}

	return ctx;
}

uls_uch_t
uls_peekch_detail(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	const char *ch_ctx = uls->ch_context;
	uls_quotetype_ptr_t qmt;
	uls_lexseg_ptr_t lexseg;

	const char *lptr;
	int         k, rc;
	uls_uch_t   uch;
	char ch;

again_1:
	lptr = xctx->context->lptr;

	if (*lptr == '\0') {
		if ((k=xctx->context->i_lexsegs) >= xctx->context->n_lexsegs) {
			if ((rc=uls_clear_and_fillbuff(uls)) < 0) {
				parms->len = -2; // ERR
				parms->data = nilptr;
				return ULS_UCH_NONE;

			} else if (rc == 0) {
				parms->len = -1;
				parms->data = nilptr;
				return ULS_UCH_NONE; // EOF
			}
			goto again_1;

		} else {
			lexseg = uls_array_get_slot_type10(uls_ptr(xctx->context->lexsegs), k);

			if ((qmt = uls_find_quotetype_by_tokid(
				uls_ptr(uls->quotetypes), uls->quotetypes.n, lexseg->tokdef_vx->tok_id)) == nilptr) {
				err_panic("No matching quote-string for tok-id(%d) found",
					lexseg->tokdef_vx->tok_id);
			}

			parms->len = 0;
			parms->data = qmt;

			return ULS_UCH_NONE;
		}
	}

	if ((ch=*lptr) < ULS_SYNTAX_TABLE_SIZE) {
		if (ch_ctx[ch] == 0) {
			if (ch != '\t' && ch != '\n') ch = ' ';
		}
		uch = ch;
		rc = 1;
	} else {
		if ((rc = uls_decode_utf8(lptr, -1, &uch)) <= 0) {
			parms->len = -3; // ERR
			parms->data = nilptr;
			return ULS_UCH_NONE;
		}
	}

	parms->len = rc;
	parms->data = nilptr;

	return uch;
}

uls_uch_t
uls_peek_uch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_quotetype_ptr_t qmt;
	int      qtok, len_uch;
	uls_uch_t uch;
	uls_outparam_t parms1;

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		ctx = __uls_unget_tok(uls);
	}

	uch = uls_peekch_detail(uls, uls_ptr(parms1));
	qmt = (uls_quotetype_ptr_t) parms1.data;
	len_uch = parms1.len;

	if (uch == ULS_UCH_NONE) {
		if (qmt != nilptr) {
			qtok = qmt->tok_id;
		} else { // len_uch < 0
			if (uls_is_context_initial(uls)) {
				qtok = uls->xcontext.toknum_EOI;
			} else {
				qtok = uls->xcontext.toknum_EOF;
			}
		}
	} else { // normal case!
		qtok = uls->xcontext.toknum_NONE;
	}

	if (detail_ch != nilptr) {
		detail_ch->uch = uch;
		detail_ch->len_uch = len_uch;
		detail_ch->qmt = qmt;
		detail_ch->tok = qtok;
	}

	return uch;
}

uls_uch_t
uls_get_uch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch)
{
	uls_context_ptr_t ctx;
	uls_uch_t uch;
	uls_nextch_detail_t detail2;

	if (detail_ch == nilptr) {
		detail_ch = uls_ptr(detail2);
	}

	if ((uch = uls_peek_uch(uls, detail_ch)) != ULS_UCH_NONE) {
		uls->xcontext.context->lptr += detail_ch->len_uch;
	}

	ctx = uls->xcontext.context;
	if (ctx->tok != uls->xcontext.toknum_NONE) {
		ctx->tok = uls->xcontext.toknum_NONE;
		ctx->tokbuf.buf[0] = '\0';
		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = 0;
	}

	return uch;
}

void
uls_unget_tok(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	if ((ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) || __uls_tok(uls) == uls->xcontext.toknum_NONE) {
		err_log("%s: called twice!, ignoring ...", __func__);
	} else {
		ctx->flags |= ULS_CTX_FL_TOKEN_UNGOT;
	}
}

void
uls_unget_lexeme(uls_lex_ptr_t uls, const char *lxm, int tok_id)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_outparam_t parms;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	int l_lxm, n_lfs;
	char *lptr;

	if (lxm == NULL) {
		err_log("%s: lxm == NULL", __func__);
		return;
	}

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		ctx = __uls_unget_tok(uls);
	}

	parms.lptr = lxm;
	parms.len = -1;
	l_lxm = __numof_lfs(uls_ptr(parms));
	n_lfs = parms.n;

	if (tok_id == uls->xcontext.toknum_NONE) {
		if (l_lxm > 0) {
			ctx = __uls_unget_str(uls, NULL, l_lxm + 1);
			lptr = (char *) ctx->lptr;
			uls_memmove(lptr, lxm, l_lxm); lptr += l_lxm;
			*lptr = ' ';
		}
	} else {
		ctx = __uls_unget_quote(uls, lxm, l_lxm, uls_find_tokdef_vx(uls, tok_id), n_lfs);
	}

	uls->tokdef_vx = slots_rsv[NONE_TOK_IDX];
}

void
uls_unget_ch(uls_lex_ptr_t uls, uls_uch_t uch)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	char ch_str[ULS_UTF8_CH_MAXLEN];
	int rc;

	if (uch == '\0')
		return;

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		ctx = __uls_unget_tok(uls);
	}

	if ((rc = uls_encode_utf8(uch, ch_str, ULS_UTF8_CH_MAXLEN)) <= 0) {
		err_log("%s: incorrect unicode!", __func__);
		return;
	}

	ctx = __uls_unget_str(uls, ch_str, rc);
	if (uch == '\n') {
		uls_context_inc_lineno(ctx, -1);
	}
}

void
uls_unget_str(uls_lex_ptr_t uls, const char* str)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	int len;

	if (str == NULL)
		err_panic("%s: null string NOT permitted!", __func__);
	if ((len=uls_strlen(str)) == 0) return;

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		ctx = __uls_unget_tok(uls);
	}

	ctx = __uls_unget_str(uls, str, len);
	uls->tokdef_vx = slots_rsv[NONE_TOK_IDX];
}

void
ulsjava_unget_str(uls_lex_ptr_t uls, const uls_native_vptr_t str, int len_str)
{
	char *ustr = uls_strdup((const char *)str, len_str);

	uls_unget_str(uls, ustr);
	uls_mfree(ustr);
}

void
ulsjava_unget_lexeme(uls_lex_ptr_t uls, const uls_native_vptr_t lxm, int len_lxm, int tok_id)
{
	char *ustr = uls_strdup((const char *)lxm, len_lxm);

	uls_unget_lexeme(uls, ustr, tok_id);
	uls_mfree(ustr);
}

int
ulsjava_peek_ch(uls_lex_t* uls, int* tok_peek)
{
	uls_nextch_detail_t detail_ch;
	uls_uch_t uch;

	uch = uls_peek_uch(uls, uls_ptr(detail_ch));
	if (tok_peek) {
		*tok_peek = detail_ch.tok;
	}

	return (int) uch;
}

int
ulsjava_get_ch(uls_lex_t* uls, int* tok_peek)
{
	uls_nextch_detail_t detail_ch;
	uls_uch_t uch;

	uch = uls_get_uch(uls, uls_ptr(detail_ch));
	if (tok_peek) {
		*tok_peek = detail_ch.tok;
	}

	return (int) uch;
}

uls_nextch_detail_ptr_t
ulsjava_peek_nextch_info(uls_lex_ptr_t uls)
{
	uls_nextch_detail_ptr_t detail_ch;

	detail_ch = (uls_nextch_detail_ptr_t)uls_alloc_object(uls_nextch_detail_t);
	uls_peek_uch(uls, detail_ch);

	return detail_ch;
}

uls_nextch_detail_ptr_t
ulsjava_get_nextch_info(uls_lex_ptr_t uls)
{
	uls_nextch_detail_ptr_t detail_ch;

	detail_ch = (uls_nextch_detail_ptr_t)uls_alloc_object(uls_nextch_detail_t);
	uls_get_uch(uls, detail_ch);

	return detail_ch;
}

void
ulsjava_put_nextch_info(uls_nextch_detail_ptr_t detail_ch)
{
	uls_dealloc_object(detail_ch);
}

int
ulsjava_get_uch_from_nextch(uls_nextch_detail_ptr_t detail_ch)
{
	return detail_ch->uch;
}

int
ulsjava_get_tok_from_nextch(uls_nextch_detail_t* detail_ch)
{
	return detail_ch->tok;
}
