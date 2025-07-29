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
 * uls_context.c -- the input manipulating procedures of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, April 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_CONTEXT__
#include "uls/uls_context.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__xcontext_binfd_filler)(uls_xcontext_ptr_t xctx)
{
	uls_input_ptr_t inp = xctx->context->input;
	int  n, rc;

	uls_regulate_rawbuf(inp);

	if ((rc = inp->rawbuf.siz - inp->rawbuf_bytes) <= 0) {
		_uls_log(err_log)("%s: InternalError: No space to fill in buffer!", __func__);
		return 0;
	}

	if ((n=uls_input_read(uls_ptr(inp->isource), inp->rawbuf.buf, inp->rawbuf_bytes, inp->rawbuf.siz)) <= 0) {
		if (n == 0) {
			xctx->context->flags |= ULS_CTX_FL_EOF;
		} else {
			return -1;
		}
	}

	inp->rawbuf_bytes += n;

	return inp->rawbuf_bytes;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(add_bin_packet_to_zbuf)(int tok_id, int txtlen, const char *txtptr, _uls_ptrtype_tool(csz_str) ss_dst)
{
	uls_int32 hdrbuf[2];
	char *pktptr;
	int k0;

	k0 = csz_length(ss_dst);
	_uls_tool(csz_modify)(ss_dst, k0, nilptr, ULS_RDPKT_SIZE);

	hdrbuf[0] = tok_id;
	hdrbuf[1] = txtlen;
	pktptr = (char *) _uls_tool_(memcopy)((csz_data_ptr(ss_dst) + k0), hdrbuf, sizeof(hdrbuf));
	_uls_tool_(memcopy)(pktptr, &txtptr, sizeof(const char *));
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(insert_txt_record_into_stream)(
	int tokid, int tokid_TMPL, const char *txtptr, int txtlen,
	uls_context_ptr_t ctx, _uls_ptrtype_tool(csz_str)  ss_dst)
{
	uls_tmplvar_ptr_t tmpl;
	uls_rd_packet_ptr_t pkt;
	int i;

	if (tokid == tokid_TMPL) {
		if ((tmpl = uls_search_tmpls(uls_ptr(ctx->tmpls_pool->tmplvars), txtptr)) == nilptr) {
			add_bin_packet_to_zbuf(tokid, txtlen, txtptr, ss_dst);
		} else {
			for (i=0; i<tmpl->n_pkt_ary; i++) {
				pkt = uls_get_array_slot_type10(uls_ptr(ctx->tmpls_pool->pkt_ary), tmpl->i0_pkt_ary + i);
				add_bin_packet_to_zbuf(pkt->tok_id, pkt->len_tokstr, pkt->tokstr, ss_dst);
			}
		}
	} else {
		add_bin_packet_to_zbuf(tokid, txtlen, txtptr, ss_dst);
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_txthdr_1)(uls_ptrtype_tool(outparam) parms)
{
	char *lptr = parms->line;
	const char *lptr_end = parms->lptr_end;
	const char *txtptr;
	int i, txtlen;
	uls_type_tool(outparam) parms2;

	for (i = 0; lptr[i]==' '; i++)
		/* NOTHING */;

	parms2.n1 = i;
	parms->n = splitint(lptr, uls_ptr(parms2));
	parms->len = txtlen = splitint(lptr, uls_ptr(parms2));

	i = parms2.n1 + 1; // next to ' '
	txtptr = lptr + i;

	lptr += i + txtlen;
	if (lptr >= lptr_end) return -1;

	*lptr++ = '\0';
	parms->line = lptr;
	parms->lptr = txtptr;

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_txthdr_2)(uls_ptrtype_tool(outparam) parms)
{
	char *lptr = parms->line, *txtptr, *wrd;
	const char *lptr_end = parms->lptr_end;
	uls_type_tool(wrd) wrdx;
	int txtlen;

	wrdx.lptr = lptr;

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0') {
		return -1;
	}
	parms->n = _uls_tool_(atoi)(wrd);

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0') {
		return -1;
	}
	parms->len = txtlen = _uls_tool_(atoi)(wrd);
	txtptr = lptr;

	if (lptr_end < txtptr + txtlen) {
		return -1;
	}

	lptr = txtptr + txtlen;

	*lptr++ = '\0';
	parms->line = lptr;
	parms->lptr = txtptr;

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__xcontext_txtfd_filler)(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(outparam) parms)
{
	uls_input_ptr_t   inp = xctx->context->input;
	_uls_ptrtype_tool(csz_str) ss_dst = uls_ptr(xctx->context->zbuf1);
	uls_context_ptr_t ctx = xctx->context;
	int   tok_id, n, rc, rc2, txtlen, n_recs=0;
	char *lptr0, *lptr, *lptr_end;
	const char *txtptr;
	uls_type_tool(outparam) parms2;

	uls_regulate_rawbuf(inp);

again_1:
	if ((rc=uls_input_read(uls_ptr(inp->isource), inp->rawbuf.buf, inp->rawbuf_bytes, inp->rawbuf.siz)) < 0) {
		if (inp->rawbuf_bytes > 0)
			_uls_log(err_log)("%s: redundant %d-bytes exist!", __func__, inp->rawbuf_bytes);
		return -1;
	}

	if (rc == 0) {
		if (inp->rawbuf_bytes == 0) {
			parms->n = n_recs;
			return 0;
		}

		lptr = inp->rawbuf.buf;
		lptr_end = lptr + inp->rawbuf_bytes;
		*lptr_end = '\0';

		for ( ; lptr < lptr_end; ) {
			// No worry about the end of record
			parms2.line = lptr;
			parms2.lptr_end = lptr_end;

			rc2 = get_txthdr_2(uls_ptr(parms2));

			lptr = parms2.line;
			tok_id = parms2.n;
			txtlen = parms2.len;
			txtptr = parms2.lptr;

			if (rc2 < 0) {
				_uls_log(err_log)("%s: record truncated!", __func__);
				return -1;
			}

			insert_txt_record_into_stream(tok_id, xctx->toknum_TMPL, txtptr, txtlen,
				ctx, ss_dst);

			++n_recs;
		}

		inp->rawbuf_ptr = inp->rawbuf.buf;
		inp->rawbuf_bytes = 0;

		parms->n = n_recs;
		return 1;
	}

	inp->rawbuf_ptr = lptr = inp->rawbuf.buf;
	inp->rawbuf_bytes += rc;

	lptr_end = lptr + inp->rawbuf_bytes;

	for ( ; (n = (int) (lptr_end - lptr)) >= ULS_TXT_RECHDR_SZ; ) {
		lptr0 = lptr;

		parms2.line = lptr;
		parms2.lptr_end = lptr_end;

		rc2 = get_txthdr_1(uls_ptr(parms2));

		lptr = parms2.line;
		tok_id = parms2.n;
		txtlen = parms2.len;
		txtptr = parms2.lptr;

		if (rc2 < 0) {
			if (n_recs > 0) break;

			// 'line' must include at least one record.
			inp->rawbuf_ptr = lptr0;
			inp->rawbuf_bytes = n;
			uls_regulate_rawbuf(inp);
			uls_resize_rawbuf(inp, txtlen + 1);
			goto again_1;
		}

		insert_txt_record_into_stream(tok_id, xctx->toknum_TMPL, txtptr, txtlen,
			ctx, ss_dst);

		++n_recs;
	}

	inp->rawbuf_ptr = lptr;
	inp->rawbuf_bytes = n;

	parms->n = n_recs;
	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__check_rec_boundary_host_order)(uls_xcontext_ptr_t xctx, char *buf, int n)
{
	uls_context_ptr_t ctx = xctx->context;
	_uls_ptrtype_tool(csz_str)  ss_dst = uls_ptr(ctx->zbuf1);

	uls_rd_packet_ptr_t pkt;
	uls_tmplvar_ptr_t tmpl;
	int    i, tok_id, txtlen, reclen;
	const char *txtptr;

	if (n < ULS_BIN_RECHDR_SZ) {
		return 0;
	}

	tok_id = ((uls_int32 *) buf)[0];
	txtlen = ((uls_int32 *) buf)[1];
	txtptr = buf + ULS_BIN_RECHDR_SZ;
	reclen = ULS_BIN_REC_SZ(txtlen);

	if (reclen > n) {
		return -reclen;
	}

	if (tok_id == xctx->toknum_TMPL) {
		if ((tmpl = uls_search_tmpls(uls_ptr(ctx->tmpls_pool->tmplvars), txtptr)) == nilptr || tmpl->n_pkt_ary < 0) {
			add_bin_packet_to_zbuf(tok_id, txtlen, txtptr, ss_dst);
		} else {
			for (i=0; i<tmpl->n_pkt_ary; i++) {
				pkt = uls_get_array_slot_type10(uls_ptr(ctx->tmpls_pool->pkt_ary), tmpl->i0_pkt_ary + i);
				add_bin_packet_to_zbuf(pkt->tok_id, pkt->len_tokstr, pkt->tokstr, ss_dst);
			}
		}
	} else {
		add_bin_packet_to_zbuf(tok_id, txtlen, txtptr, ss_dst);
	}

	// reclen > ULS_BIN_RECHDR_SZ > 0
	return reclen;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__check_rec_boundary_reverse_order)(uls_xcontext_ptr_t xctx, char *buf, int n)
{
	uls_context_ptr_t ctx = xctx->context;
	_uls_ptrtype_tool(csz_str)  ss_dst = uls_ptr(ctx->zbuf1);

	uls_rd_packet_ptr_t pkt;
	uls_tmplvar_ptr_t tmpl;
	int    i, tok_id, txtlen, reclen;
	const char *txtptr;

	if (n < ULS_BIN_RECHDR_SZ) {
		return 0;
	}

	tok_id = ((uls_int32 *) buf)[0];
	_uls_tool_(reverse_bytes)((char *) &tok_id, sizeof(uls_int32));

	txtlen = ((uls_int32 *) buf)[1];
	_uls_tool_(reverse_bytes)((char *) &txtlen, sizeof(uls_int32));

	if (txtlen < 0)
		_uls_log(err_panic)("%s: corrupt stream!", __func__);

	txtptr = buf + ULS_BIN_RECHDR_SZ;
	reclen = ULS_BIN_REC_SZ(txtlen);

	if (reclen > n) {
		return -reclen;
	}

	if (tok_id == xctx->toknum_TMPL) {
		if ((tmpl = uls_search_tmpls(uls_ptr(ctx->tmpls_pool->tmplvars), txtptr)) == nilptr || tmpl->n_pkt_ary < 0) {
			add_bin_packet_to_zbuf(tok_id, txtlen, txtptr, ss_dst);
		} else {
			for (i=0; i<tmpl->n_pkt_ary; i++) {
				pkt = uls_get_array_slot_type10(uls_ptr(ctx->tmpls_pool->pkt_ary), tmpl->i0_pkt_ary + i);
				add_bin_packet_to_zbuf(pkt->tok_id, pkt->len_tokstr, pkt->tokstr, ss_dst);
			}
		}
	} else {
		add_bin_packet_to_zbuf(tok_id, txtlen, txtptr, ss_dst);
	}

	// reclen > ULS_BIN_RECHDR_SZ > 0
	return reclen;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__check_rec_boundary_bin)(uls_xcontext_ptr_t xctx, uls_xctx_boundary_checker2_t checker)
{
	uls_input_ptr_t inp = xctx->context->input;
	int i0 = 0, reclen, n_recs, n, m2;

	if ((n=inp->rawbuf_bytes) < ULS_BIN_RECHDR_SZ) {
		_uls_log(err_log)("%s: incorrect format error!", __func__);
		return -1;
	}

	for (n_recs=0; ; n_recs++) {
		if ((reclen = checker(xctx, inp->rawbuf.buf + i0, n)) > 0) {
			i0 += reclen;
			n -= reclen;

		} else if (reclen < 0 && n_recs == 0) {
			reclen = -reclen;

			m2 = reclen - n;  // need m2 more bytes to complete a record.

			_uls_tool(str_modify)(uls_ptr(inp->rawbuf), inp->rawbuf_bytes, NULL, m2);
			if (uls_input_read(uls_ptr(inp->isource), inp->rawbuf.buf, inp->rawbuf_bytes, reclen) < m2) {
				_uls_log(err_log)("%s: file error", __func__);
				return -1;
			}

			n = reclen;

		} else {
			break;
		}
	}

	inp->rawbuf_bytes = n; // n may be zero!
	inp->rawbuf_ptr = inp->rawbuf.buf + i0;

	return n_recs;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_commtype_ptr_t)
ULS_QUALIFIED_METHOD(is_commtype_start)(uls_xcontext_ptr_t xctx, const char *ptr, int len)
{
	uls_commtype_ptr_t cmt;
	const char *str;
	int i;

	for (i=0; i<xctx->n_commtypes; i++) {
		cmt = uls_get_array_slot_type01(xctx->commtypes, i);

		if (len < cmt->len_start_mark) continue;

		str = uls_get_namebuf_value(cmt->start_mark);
		if (ptr[0] == str[0]) {
			if (cmt->len_start_mark == 1 || !_uls_tool_(strncmp)(ptr+1,str+1, cmt->len_start_mark-1)) {
				return cmt;
			}
		}
	}

	return nilptr;
}

const char*
ULS_QUALIFIED_METHOD(uls_xcontext_quotetype_start_mark)(uls_xcontext_ptr_t xctx, int tok_id)
{
	uls_quotetype_ptr_t qmt;

	if ((qmt = uls_find_quotetype_by_tokid(xctx->quotetypes, xctx->quotetypes->n, tok_id)) == nilptr) {
		_uls_log(err_log)("No matching quote-string for tok-id(%d) found", tok_id);
		return NULL;
	}

	return uls_get_namebuf_value(qmt->start_mark);
}

ULS_QUALIFIED_RETTYP(uls_quotetype_ptr_t)
ULS_QUALIFIED_METHOD(uls_xcontext_find_quotetype)(uls_xcontext_ptr_t xctx, const char *ptr, int len)
{
	uls_decl_parray_slots_init(slots_qmt, quotetype, xctx->quotetypes);
	uls_quotetype_ptr_t qmt;
	const char *str;
	int i;

	for (i=0; i<xctx->quotetypes->n; i++) {
		qmt = slots_qmt[i];

		if (len < qmt->len_start_mark) continue;

		str = uls_get_namebuf_value(qmt->start_mark);
		if (ptr[0] == str[0]) {
			if (qmt->len_start_mark == 1 || !_uls_tool_(strncmp)(ptr+1,str+1, qmt->len_start_mark-1)) {
				return qmt;
			}
		}
	}

	return nilptr;
}

int
ULS_QUALIFIED_METHOD(check_rec_boundary_null)(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln)
{
	uls_lexseg_ptr_t lexseg;

	xctx->context->n_lexsegs = 0;
	lexseg = uls_get_array_slot_type10(uls_ptr(xctx->context->lexsegs), 0);
	uls_reset_lexseg(lexseg, 0, 0, -1, -1, nilptr);

	if (parm_ln != nilptr) {
		parm_ln->lptr = "";
	}
	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_init_lexseg)(uls_lexseg_ptr_t lexseg)
{
}

void
ULS_QUALIFIED_METHOD(uls_deinit_lexseg)(uls_lexseg_ptr_t lexseg)
{
}

void
ULS_QUALIFIED_METHOD(uls_reset_lexseg)(uls_lexseg_ptr_t lexseg,
	int offset1, int len1, int offset2, int len2, uls_tokdef_vx_ptr_t e_vx)
{
	lexseg->offset1 = offset1;
	lexseg->len1 = len1;

	if (offset2 < 0) len2 = -1;
	lexseg->offset2 = offset2;
	lexseg->len_text = len2;

	lexseg->tokdef_vx = e_vx;
	lexseg->n_lfs_raw = 0;
}

void
ULS_QUALIFIED_METHOD(uls_init_context)(uls_context_ptr_t ctx, uls_gettok_t gettok, int tok0)
{
	uls_input_ptr_t   inp;
	uls_lexseg_ptr_t lexseg;
	int i, n;

	ctx->flags = 0;
	_uls_tool(csz_init)(uls_ptr(ctx->tag), 128);
	ctx->lineno = 1;
	ctx->delta_lineno = 0;

	uls_init_bytespool(ctx->cnst_nilstr, ULS_CNST_NILSTR_SIZE, 0);
	ctx->input = inp = uls_create_input();

	_uls_tool(csz_init)(uls_ptr(ctx->zbuf1), ULS_ZBUF1_INITSIZE);
	_uls_tool(csz_init)(uls_ptr(ctx->zbuf2), ULS_ZBUF1_INITSIZE);

	ctx->lptr = ctx->line = ctx->cnst_nilstr;
	ctx->line_end = ctx->line;

	uls_init_array_type10(uls_ptr(ctx->lexsegs), lexseg, ULS_LEXSEGS_MAX + 1);
	for (i=0; i < ctx->lexsegs.n_alloc; i++) {
		uls_alloc_array_slot_type10(uls_ptr(ctx->lexsegs), lexseg, i);
	}
	ctx->lexsegs.n = ctx->lexsegs.n_alloc;

	ctx->i_lexsegs = ctx->n_lexsegs = 0;
	lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), 0);
	uls_reset_lexseg(lexseg, 0, 0, -1, -1, nilptr);

	ctx->gettok = gettok;
	ctx->fill_proc = uls_ref_callback_this(xcontext_raw_filler);
	ctx->flags |= ULS_CTX_FL_FILL_RAW;
	ctx->record_boundary_checker = uls_ref_callback_this(check_rec_boundary_null);

	ctx->tmpls_pool = nilptr;

	ctx->tok = tok0;
	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = ctx->s_val_wchars = 0;

	n = (ULS_LEXSTR_MAXSIZ+1) << 1;
	_uls_tool(str_init)(uls_ptr(ctx->tokbuf), n);
	ctx->n_digits = ctx->n_expo = 0;

	_uls_tool(str_init)(uls_ptr(ctx->tokbuf_aux), n);
	ctx->l_tokbuf_aux = -1;

	ctx->anonymous_uchar_vx = uls_create_tokdef_vx(0, NULL, nilptr); // 0: nonsense
	ctx->anonymous_uchar_vx->flags |= ULS_VX_ANONYMOUS;
	ctx->user_data = nilptr;
	ctx->prev = nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_context)(uls_context_ptr_t ctx)
{
	uls_destroy_input(ctx->input);
	ctx->input = nilptr;

	_uls_tool(csz_deinit)(uls_ptr(ctx->zbuf1));
	_uls_tool(csz_deinit)(uls_ptr(ctx->zbuf2));

	ctx->fill_proc = uls_ref_callback_this(xcontext_raw_filler);
	ctx->flags |= ULS_CTX_FL_FILL_RAW;
	ctx->record_boundary_checker = uls_ref_callback_this(check_rec_boundary_null);

	_uls_tool(csz_deinit)(uls_ptr(ctx->tag));
	_uls_tool(str_free)(uls_ptr(ctx->tokbuf));
	_uls_tool(str_free)(uls_ptr(ctx->tokbuf_aux));
	ctx->gettok = nilptr;
	ctx->prev = nilptr;

	uls_destroy_tokdef_vx(ctx->anonymous_uchar_vx);
	ctx->anonymous_uchar_vx = nilptr;

	ctx->flags = 0;
	ctx->lineno = 1;
	ctx->delta_lineno = 0;
	ctx->line = ctx->lptr = ctx->line_end = ctx->cnst_nilstr;

	uls_deinit_array_type10(uls_ptr(ctx->lexsegs), lexseg);
	ctx->i_lexsegs =  ctx->n_lexsegs =  0;

	uls_destroy_tmpl_pool(ctx->tmpls_pool);
	ctx->tmpls_pool = nilptr;

	uls_deinit_bytespool(ctx->cnst_nilstr);
}

void
ULS_QUALIFIED_METHOD(uls_xcontext_init)(uls_xcontext_ptr_t xctx, uls_gettok_t gettok)
{
	uls_initial_zerofy_object(xctx);
	xctx->flags = ULS_FL_STATIC;

	xctx->toknum_EOI = 0;
	xctx->toknum_EOF = -1;
	xctx->toknum_ID  = -2;
	xctx->toknum_NUMBER = -3;
	xctx->toknum_LINENUM = -4;
	xctx->toknum_TMPL = -5;
	xctx->toknum_LINK = -6;
	xctx->toknum_NONE = -7;
	xctx->toknum_ERR = -8;

	xctx->context = uls_alloc_object(uls_context_t); // initial-context
	uls_init_context(xctx->context, gettok, xctx->toknum_NONE);
}

void
ULS_QUALIFIED_METHOD(uls_xcontext_reset)(uls_xcontext_ptr_t xctx)
{
	uls_context_set_line(xctx->context, NULL, -1);
	uls_ctx_set_tag(xctx->context, "", 1);
}

void
ULS_QUALIFIED_METHOD(uls_xcontext_deinit)(uls_xcontext_ptr_t xctx)
{
	uls_xcontext_reset(xctx);

	xctx->commtypes = nilptr;
	xctx->n_commtypes = 0;

	uls_mfree(xctx->prepended_input);
	xctx->len_prepended_input = xctx->lfs_prepended_input = 0;
	xctx->quotetypes = nilptr;

	uls_mfree(xctx->uldfile_buf);
	xctx->uldfile_buflen = 0;

	uls_deinit_context(xctx->context);
	uls_dealloc_object(xctx->context);
	xctx->context = nilptr;
}

int
ULS_QUALIFIED_METHOD(check_rec_boundary_host_order)(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln)
{
	uls_input_ptr_t inp = xctx->context->input;
	uls_callback_type_this(xctx_boundary_checker2) checker2;
	int rc;

	checker2 = uls_ref_callback_this(__check_rec_boundary_host_order);
	rc = __check_rec_boundary_bin(xctx, checker2);
	parm_ln->lptr = inp->rawbuf.buf;

	return rc;
}

int
ULS_QUALIFIED_METHOD(check_rec_boundary_reverse_order)(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln)
{
	uls_input_ptr_t inp = xctx->context->input;
	uls_callback_type_this(xctx_boundary_checker2) checker2;
	int rc;

	checker2 = uls_ref_callback_this(__check_rec_boundary_reverse_order);
	rc = __check_rec_boundary_bin(xctx, checker2);
	parm_ln->lptr = inp->rawbuf.buf;

	return rc;
}

int
ULS_QUALIFIED_METHOD(xcontext_raw_filler)(uls_xcontext_ptr_t xctx)
{
	uls_context_ptr_t ctx = xctx->context;
	uls_input_ptr_t inp = xctx->context->input;
	const char    *ch_ctx = xctx->ch_context;

	_uls_ptrtype_tool(csz_str) ss_dst1 = uls_ptr(ctx->zbuf1);
	_uls_ptrtype_tool(csz_str) ss_dst2 = uls_ptr(ctx->zbuf2);
	uls_lexseg_ptr_t  lexseg;
	const char  *lptr1, *lptr, *lptr_end;
	int   n_segs = 0, k2, offset1, rc;
	char  ch, ch_grp;

	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	int i, n_lfs;
	uls_type_tool(outparam) parms1;

	offset1 = 0;
	lptr1 = lptr = inp->rawbuf_ptr;
	lptr_end = lptr + inp->rawbuf_bytes;

	for ( ; ; ) {
		if (lptr_end < lptr + ULS_LEN_SURPLUS) {
			if ((rc = (int) (lptr-lptr1)) > 0) {
				_uls_tool(csz_append)(ss_dst1, lptr1, rc);
			}

			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);

			if (inp->refill(inp, ULS_LEN_SURPLUS) < 0) {
//				_uls_log(err_log)("%s: I/O error", __func__);
				uls_input_reset_cursor(inp);
				return -1;
			}

			lptr1 = lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;

			if (inp->rawbuf_bytes == 0) {
				ctx->flags |= ULS_CTX_FL_EOF;
				break;
			}
		}

		if ((ch=*lptr) < ULS_SYNTAX_TABLE_SIZE) {
			ch_grp = ch_ctx[ch];
		} else {
			ch_grp = ULS_CH_COMM | ULS_CH_QUOTE;
		}

		if (ch_grp == 0) {
			if ((rc = (int) (lptr-lptr1)) > 0) _uls_tool(csz_append)(ss_dst1, lptr1, rc);
			if (csz_length(ss_dst1) >= ULS_ZBUF1_ROUGHSIZE) break;

			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);

			if ((rc = input_space_proc(ch_ctx, inp, ss_dst1, uls_ptr(parms1))) < 0) {
				_uls_log(err_log)("%s: I/O error", __func__);
				return -1;
			}

			inp->line_num += n_lfs = parms1.n;
			lptr1 = lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;

			if (rc == 0) break;

		} else if ((ch_grp & ULS_CH_COMM) &&
			(cmt = is_commtype_start(xctx, lptr, (int) (lptr_end - lptr))) != nilptr) {
			if ((rc = (int) (lptr - lptr1)) > 0) _uls_tool(csz_append)(ss_dst1, lptr1, rc);
			lptr += cmt->len_start_mark;

			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);

			rc = input_skip_comment(cmt, inp, uls_ptr(parms1));
			n_lfs = parms1.n;

			lptr1 = lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;

			if (rc <= 0) {
				_uls_log(err_log)("[%s:%d] Unterminated comment at EOF!", uls_ctx_get_tag(ctx), inp->line_num);
				return -1;
			}

			n_lfs += cmt->n_lfs;
			for (i=0; i < n_lfs; i++) _uls_tool(csz_putc)(ss_dst1, '\n');
			inp->line_num += n_lfs;

		} else if ((ch_grp & ULS_CH_QUOTE) &&
			(qmt = uls_xcontext_find_quotetype(xctx, lptr, (int) (lptr_end - lptr))) != nilptr) {
			if ((rc = (int) (lptr - lptr1)) > 0) _uls_tool(csz_append)(ss_dst1, lptr1, rc);

			if (n_segs + 1 >= ctx->lexsegs.n) {
				break;
			}

			lptr += qmt->len_start_mark;
			lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), n_segs);
			lexseg->tokdef_vx = qmt->tokdef_vx;

			lexseg->offset1 = offset1;
			lexseg->len1 = csz_length(ss_dst1) - offset1;
			// forcing '\0' before putting literal-string ...
			_uls_tool(csz_add_eos)(ss_dst1);
			offset1 = csz_length(ss_dst1);

			lexseg->offset2 = k2 = csz_length(ss_dst2);

			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);

			rc = input_quote_proc(inp, qmt, ss_dst2, uls_ptr(parms1));
			n_lfs = parms1.n;

			if (rc == ULS_LITPROC_ENDOFQUOTE) {
				lexseg->n_lfs_raw = n_lfs;
				lexseg->len_text = csz_length(ss_dst2) - lexseg->offset2;
				// put '\0' at the end of literal string
				_uls_tool(csz_add_eos)(ss_dst2);
				++n_segs;

			} else if (rc == ULS_LITPROC_DISMISSQUOTE) {
				lexseg->n_lfs_raw = n_lfs;
				csz_truncate(ss_dst2, k2);

			} else {
				_uls_log(err_log)("[%s:%d] Unterminated literal string at EOF!", uls_ctx_get_tag(ctx), inp->line_num);
				return -1;
			}

			lptr1 = lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;
			inp->line_num += n_lfs;

		} else {
			if (ch == '\n') ++inp->line_num;
			++lptr;
		}
	}

	inp->rawbuf_ptr = lptr;
	inp->rawbuf_bytes = (int) (lptr_end - lptr);
	_uls_tool(csz_text)(ss_dst1);

	lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), n_segs);
	uls_reset_lexseg(lexseg, offset1, csz_length(ss_dst1) - offset1, -1, -1, nilptr);

	ctx->n_lexsegs = n_segs;
	lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), 0);

	return lexseg->len1;
}

int
ULS_QUALIFIED_METHOD(xcontext_binfd_filler)(uls_xcontext_ptr_t xctx)
{
	uls_context_ptr_t ctx = xctx->context;
	uls_type_tool(parm_line) parm_ln1;
	int rc;

	do {
		if ((rc = __xcontext_binfd_filler(xctx)) <= 0) {
			return rc;
		}

		rc = xctx->context->record_boundary_checker(xctx, uls_ptr(parm_ln1));
		if (rc < 0) {
			return -1;
		}
		// rc : # of recs
	} while (rc == 0);

	return csz_length(uls_ptr(ctx->zbuf1));
}

int
ULS_QUALIFIED_METHOD(xcontext_txtfd_filler)(uls_xcontext_ptr_t xctx)
{
	uls_context_ptr_t ctx = xctx->context;
	uls_type_tool(outparam) parms;
	int rc, n_recs;

	do {
		if ((rc = __xcontext_txtfd_filler(xctx, uls_ptr(parms))) < 0) {
			return -1;
		} else if (rc == 0) {
			ctx->flags |= ULS_CTX_FL_EOF;
			break;
		}

		n_recs = parms.n;
	} while (n_recs == 0);

	return csz_length(uls_ptr(ctx->zbuf1));
}

void
ULS_QUALIFIED_METHOD(uls_ctx_set_tag)(uls_context_ptr_t ctx, const char *tagstr, int lno)
{
	if (tagstr != NULL) {
		_uls_tool(csz_reset)(uls_ptr(ctx->tag));
		_uls_tool(csz_append)(uls_ptr(ctx->tag), tagstr, -1);
	}

	if (lno > 0) {
		__uls_ctx_set_lineno(ctx, lno);
	}
}

void
ULS_QUALIFIED_METHOD(uls_ctx_inc_lineno)(uls_context_ptr_t ctx, int delta)
{
	int lno;

	if ((lno = ctx->lineno + delta) > 0) {
		uls_ctx_set_tag(ctx, NULL, lno);
	}
}

int
ULS_QUALIFIED_METHOD(uls_xcontext_delete_litstr_analyzer)(uls_xcontext_ptr_t xctx, const char *prefix)
{
	uls_decl_parray_slots_init(slots_qmt, quotetype, xctx->quotetypes);
	uls_quotetype_ptr_t qmt;
	int i, j;

	for (i=0; ; i++) {
		if (i >= xctx->quotetypes->n) {
			return -1;
		}

		qmt = slots_qmt[i];
		if (uls_streql(uls_get_namebuf_value(qmt->start_mark), prefix)) {
			break;
		}
	}

	if (i < xctx->quotetypes->n - 1) {
		qmt = slots_qmt[i];
		uls_destroy_quotetype(qmt);

		for (j=i; j < xctx->quotetypes->n - 1; j++) {
			qmt = slots_qmt[j+1];
			slots_qmt[j] = qmt;
		}
	}

	--xctx->quotetypes->n;
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_xcontext_change_litstr_analyzer)(uls_xcontext_ptr_t xctx,
	const char *prefix, uls_litstr_analyzer_t lit_analyzer, uls_voidptr_t dat)
{
	uls_decl_parray_slots_init(slots_qmt, quotetype, xctx->quotetypes);
	uls_quotetype_ptr_t qmt;
	int i, stat = -1, is_userproc = 1;

	if (lit_analyzer == nilptr) {
		lit_analyzer = uls_ref_callback_this(dfl_lit_analyzer_escape0);
		is_userproc = 0;
	}

	for (i=0; i<xctx->quotetypes->n; i++) {
		qmt = slots_qmt[i];

		if (uls_streql(uls_get_namebuf_value(qmt->start_mark), prefix)) {
			qmt->litstr_analyzer = lit_analyzer;
			qmt->litstr_context = dat;
			if (is_userproc) {
				qmt->flags |= ULS_QSTR_USERPROC;
			} else {
				qmt->flags &= ~ULS_QSTR_USERPROC;
			}
			stat = 0;
			break;
		}
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_context_set_line)(uls_context_ptr_t ctx, const char *line, int len)
{
	if (line == NULL) {
		line = "";
		len = 0;
	} else if (len < 0) {
		len = _uls_tool_(strlen)(line);
	}

	ctx->line = ctx->lptr = line;
	if ((ctx->line_end=line+len) < line)
		_uls_log(err_panic)("%s: invalid param(len=%d)!", __func__, len);
}
