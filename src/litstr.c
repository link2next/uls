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
 * litstr.c -- utility routines for manipulating literal strings in ULS --
 *     written by Stanley Hong <link2next@gmail.com>, August 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#include "uls/litstr.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

uls_wch_t
ULS_QUALIFIED_METHOD(uls_get_escape_char_initial)(uls_litstr_ptr_t lit)
{
	uls_wch_t prefix_ch = lit->ch_escaped;
	uls_type_tool(outparam) parms1;
	uls_wch_t wch = 0;
	int n;

	if (prefix_ch == 'x') { // hexa-decimal
		n = 2;
	} else if (prefix_ch == 'u') { // unicode
		n = 4;
	} else if (prefix_ch == 'U') { // unicode
		n = 8;
	} else if (prefix_ch >= '0' && prefix_ch < '8') { // octal
		wch = prefix_ch - '0';
		if (prefix_ch == '0') {
			n = 3;
		} else {
			n = 2;
		}
	} else {
		parms1.x1 = prefix_ch;
		uls_get_simple_escape_char(uls_ptr(parms1));
		wch = parms1.x2;
		n = 0;
	}

	lit->len_ch_escaped = n;
	lit->wch = wch;

	return wch;
}

uls_wch_t
ULS_QUALIFIED_METHOD(uls_get_escape_char_cont)(uls_litstr_ptr_t lit)
{
	const char   *lptr, *lptr_end;
	uls_wch_t prefix_ch;
	uls_wch_t wch;
	char ch;

	prefix_ch = lit->ch_escaped;
	wch = lit->wch;
	lptr = lit->lptr;
	lptr_end = lptr + lit->len_ch_escaped;

	if (prefix_ch == 'x' || prefix_ch == 'u' || prefix_ch == 'U') { // hexa-decimal, unicode
		for ( ; lptr < lptr_end; lptr++) {
			if (!_uls_tool_(isxdigit)(ch=*lptr)) {
				break;
			}

			ch = _uls_tool_(isdigit)(ch) ? ch - '0' : 10 + (_uls_tool_(toupper)(ch) - 'A');
			wch = (wch<<4) + ch;
		}
	} else { // octal
		for ( ; lptr < lptr_end; lptr++) {
			if ((ch=*lptr) < '0' || ch >= '8') {
				break;
			}
			wch = (wch<<3) + (ch-'0');
		}
	}

	lit->lptr = lptr;
	lit->wch = wch;

	return wch;
}

uls_wch_t
ULS_QUALIFIED_METHOD(uls_get_escape_char)(uls_litstr_ptr_t lit)
{
	const char  *lptr = lit->lptr;
	uls_wch_t wch;

	lit->ch_escaped = *lptr++;
	lit->lptr = lptr;

	wch = uls_get_escape_char_initial(lit);
	if (lit->len_ch_escaped > 0) {
		wch = uls_get_escape_char_cont(lit);
	}

	return wch;
}

int
ULS_QUALIFIED_METHOD(uls_get_escape_str)(char quote_ch, uls_ptrtype_tool(wrd) wrdx)
{
	char *lptr = wrdx->lptr;
	char *outbuff = wrdx->wrd;
	int siz_outbuff = wrdx->siz;
	uls_litstr_t lit1;
	int rc, escape = 0, k = 0, stat = 0;
	uls_wch_t  wch;
	char ch;

	for ( ; ; ) {
		if (k + ULS_UTF8_CH_MAXLEN > siz_outbuff) {
			stat = -3;
			break;
		}

		ch = *lptr;
		if (escape) {
			lit1.lptr = lptr;
			wch = uls_get_escape_char(uls_ptr(lit1));

			if ((rc = _uls_tool_(encode_utf8)(wch, outbuff + k, ULS_UTF8_CH_MAXLEN)) <= 0) {
				stat = -2;
				break;
			}
			k += rc;
			lptr = (char *) lit1.lptr;

			escape = 0;
		} else {
			if (ch == quote_ch || ch == '\0') {
				if (ch == '\0') {
					if (ch != quote_ch) {
						// an unterminated string is found but return the string...
						stat = -1;
						break;
					}
				} else {
					++lptr;
				}
				stat = k;
				break;
			}

			if (ch == '\\') {
				escape = 1;
			} else {
				outbuff[k++] = ch;
			}
			++lptr;
		}
	}

	outbuff[k] = '\0';
	wrdx->len = k;
	wrdx->lptr = lptr;

	return stat;
}

int
ULS_QUALIFIED_METHOD(canbe_commtype_mark)(char* wrd, uls_ptrtype_tool(outparam) parms)
{
	char *buff = parms->line;
	uls_type_tool(wrd) wrdx;
	int i, n=0;
	char ch;

	wrdx.wrd = buff;
	wrdx.siz = ULS_COMM_MARK_MAXSIZ + 1;
	wrdx.lptr = wrd;
	if ((parms->len=uls_get_escape_str('\0', uls_ptr(wrdx))) < 0) {
		return 0;
	}

	for (i=0; (ch=buff[i]) != '\0'; i++) {
		if (ch == '\n') {
			++n;
		} else if (!_uls_tool_(isgraph)(ch)) {
			return 0;
		}
	}

	parms->n = n;
	return 1;
}

int
ULS_QUALIFIED_METHOD(canbe_quotetype_mark)(const char *ch_ctx, char* wrd, uls_ptrtype_tool(outparam) parms)
{
	char *buff = parms->line;
	uls_type_tool(wrd) wrdx;
	int i, n=0;
	char ch;

	wrdx.wrd = buff;
	wrdx.siz = ULS_QUOTE_MARK_MAXSIZ + 1;
	wrdx.lptr = wrd;
	if ((parms->len=uls_get_escape_str('\0', uls_ptr(wrdx))) < 0)
		return 0;

	for (i=0; (ch=buff[i]) != '\0'; i++) {
		if (ch == '\n') {
			++n;
		} else if (!(_uls_tool_(isgraph)(ch) || ch=='\t')) {
			return 0;
		}
	}

	ch = buff[0];
	if ((i == 1 && ch == '.') || uls_canbe_ch_idfirst(ch_ctx, ch)) {
		return 0;
	}

	parms->n = n;
	return 1;
}

void
ULS_QUALIFIED_METHOD(uls_init_quotetype)(uls_quotetype_ptr_t qmt)
{
	uls_initial_zerofy_object(qmt);
	uls_init_namebuf(qmt->start_mark, ULS_QUOTE_MARK_MAXSIZ);
	uls_init_namebuf(qmt->end_mark, ULS_QUOTE_MARK_MAXSIZ);
	qmt->escmap = nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_quotetype)(uls_quotetype_ptr_t qmt)
{
	uls_dealloc_escmap(qmt->escmap);

	uls_deinit_namebuf(qmt->start_mark);
	uls_deinit_namebuf(qmt->end_mark);
}

ULS_QUALIFIED_RETTYP(uls_quotetype_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_quotetype)(void)
{
	uls_quotetype_ptr_t qmt;

	qmt = uls_alloc_object(uls_quotetype_t);
	uls_init_quotetype(qmt);

	return qmt;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_quotetype)(uls_quotetype_ptr_t qmt)
{
	uls_deinit_quotetype(qmt);
	uls_dealloc_object(qmt);
}

ULS_QUALIFIED_RETTYP(uls_quotetype_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_quotetype_by_tokid)(uls_ref_parray(quotetypes,quotetype),
	int n_quotetypes, int tok_id)
{
	uls_decl_parray_slots_init(slots_qmt, quotetype, quotetypes);
	uls_quotetype_ptr_t qmt;
	int i;

	for (i=0; i<n_quotetypes; i++) {
		qmt = slots_qmt[i];
		if (qmt->tok_id == tok_id) return qmt;
	}

	return nilptr;
}

ULS_QUALIFIED_RETTYP(uls_quotetype_ptr_t)
ULS_QUALIFIED_METHOD(uls_get_litstr__quoteinfo)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t litctx = uls_ptr(lit->context);
	return litctx->qmt;
}

uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_get_litstr__user_data)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t litctx = uls_ptr(lit->context);
	uls_quotetype_ptr_t qmt = litctx->qmt;

	if (qmt == nilptr) {
		_uls_log(err_log)("No quoteinfo binded to litstr_context!");
		return nilptr;
	}

	return qmt->litstr_context;
}

ULS_QUALIFIED_RETTYP(uls_litstr_context_ptr_t)
ULS_QUALIFIED_METHOD(uls_get_litstr__context)(uls_litstr_ptr_t lit)
{
	return uls_ptr(lit->context);
}

void
ULS_QUALIFIED_METHOD(uls_litstr_putc)(uls_litstr_context_ptr_t lit_ctx, char ch)
{
	_uls_tool(csz_putc)(lit_ctx->ss_dst, ch);
}

void
ULS_QUALIFIED_METHOD(uls_litstr_puts)(uls_litstr_context_ptr_t lit_ctx, const char *str, int len)
{
	_uls_tool(csz_append)(lit_ctx->ss_dst, str, len);
}

int
ULS_QUALIFIED_METHOD(nothing_lit_analyzer)(uls_litstr_ptr_t lit)
{
	return ULS_LITPROC_ENDOFQUOTE;
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape0)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	const char *lptr = lit->lptr, *lptr_end = lit->lptr_end;
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;
	uls_escmap_ptr_t escmap = qmt->escmap;

	char buff[ULS_UTF8_CH_MAXLEN];
	int rc, len, j;
	uls_wch_t wch;

	if ((len = (int) (lptr_end - lptr)) < qmt->len_end_mark) {
//		_uls_log(err_log)("%s: unterminated literal-string", __func__);
		return ULS_LITPROC_ERROR;
	}

	if (_uls_tool_(strncmp)(lptr, uls_get_namebuf_value(qmt->end_mark), qmt->len_end_mark) == 0) {
		lit->lptr = lptr += qmt->len_end_mark;
		return ULS_LITPROC_ENDOFQUOTE;
	}

	if (len > ULS_UTF8_CH_MAXLEN) len = ULS_UTF8_CH_MAXLEN;
	for (j=0; j<len; j++) buff[j] = lptr[j];

	rc = _uls_tool_(decode_utf8)(buff, j, &wch);

	if (wch == escmap->esc_sym) {
		lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape1);
		len = ULS_UTF8_CH_MAXLEN;
	} else {
		if (wch == '\n') {
			if ((qmt->flags & ULS_QSTR_MULTILINE) == 0) {
//				_uls_log(err_log)("%s: unterminated literal-string", __func__);
				return ULS_LITPROC_ERROR;
			}
			++lit_ctx->n_lfs;
		}
		for (j=0; j<rc; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, lptr[j]);
		}
		len = qmt->len_end_mark;
	}

	lit->lptr = lptr += rc;
	return len; // # of required bytes at the next stage.
}

int
ULS_QUALIFIED_METHOD(__uls_analyze_esc_ch)(uls_litstr_ptr_t lit, uls_escmap_ptr_t escmap, _uls_ptrtype_tool(csz_str) outbuf)
{
	uls_wch_t prefix_ch = lit->ch_escaped;
	uls_type_tool(outparam) parms1;
	uls_wch_t wch;
	int n, len, map_flags = 0;

	parms1.x1 = prefix_ch;
	if ((len = uls_dec_escaped_char(escmap, uls_ptr(parms1), outbuf)) < 0) {
		map_flags = parms1.flags;
		wch = parms1.wch; // initial-wch
		n = -len; // maximul #-of-bytes of unicode
	} else {
		wch = 0; // N/A
		n = 0;
	}

	lit->len_ch_escaped = n;
	lit->wch = wch;

	return map_flags;
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape1)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	const char *lptr = lit->lptr, *lptr_end = lit->lptr_end;
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;
	uls_escmap_ptr_t escmap = qmt->escmap;

	char buff[ULS_UTF8_CH_MAXLEN];
	int len, len1, j, map_flags;
	uls_wch_t wch;

	if ((len = (int) (lptr_end - lptr)) < 1) {
//		_uls_log(err_log)("%s: unterminated literal-string", __func__);
		return ULS_LITPROC_ERROR;
	}

	if (len > ULS_UTF8_CH_MAXLEN) len = ULS_UTF8_CH_MAXLEN;
	for (j=0; j<len; j++) buff[j] = lptr[j];

	len1 = _uls_tool_(decode_utf8)(buff, j, &wch);

	lptr += len1;
	if (wch == '\n') {
		++lit_ctx->n_lfs;
	}

	if (uls_escmap_canbe_escch(wch) < 0) {
		_uls_tool(csz_putc)(lit_ctx->ss_dst, escmap->esc_sym);
		for (j=0; j<len1; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
		}

		lit->lptr = lptr;
		lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
		return qmt->len_end_mark; // # of required bytes at the next stage.
	}

	lit->ch_escaped = wch;
	map_flags = __uls_analyze_esc_ch(lit, escmap, lit_ctx->ss_dst);

	if (lit->len_ch_escaped > 0) {
		lit->map_flags = map_flags;
		len = lit->len_ch_escaped;
		lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape2);

	} else {
		if ((len = _uls_tool_(encode_utf8)(lit->wch, buff, ULS_UTF8_CH_MAXLEN)) <= 0) {
//			_uls_log(err_log)("%s: encoding error!", __func__);
			return ULS_LITPROC_ERROR;
		}

		for (j=0; j<len; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
		}

		lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
		len = qmt->len_end_mark;
	}

	lit->lptr = lptr;

	return len; // # of required bytes at the next stage.
}

uls_wch_t
ULS_QUALIFIED_METHOD(__dec_escaped_char_cont)(char quote_ch, uls_litstr_ptr_t lit)
{
	const char *lptr = lit->lptr, *lptr_end;
	uls_wch_t wch = lit->wch;
	int map_flags = lit->map_flags;;
	char ch;

	if ((lptr_end = lptr + lit->len_ch_escaped) > lit->lptr_end) {
		lptr_end = lit->lptr_end;
	}

	if (map_flags & ULS_FL_ESCSTR_HEXA) {
		for ( ; lptr < lptr_end; lptr++) {
			if (!_uls_tool_(isxdigit)(ch=*lptr)) {
				break;
			}

			ch = _uls_tool_(isdigit)(ch) ? ch - '0' : 10 + (_uls_tool_(toupper)(ch) - 'A');
			wch = (wch<<4) + ch;
		}
	} else { // octal
		for ( ; lptr < lptr_end; lptr++) {
			if ((ch=*lptr) < '0' || ch >= '8') {
				break;
			}
			wch = (wch<<3) + (ch-'0');
		}
	}

	lit->lptr = lptr;
	lit->wch = wch;

	return wch;
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape2)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;

	char buff[ULS_UTF8_CH_MAXLEN];
	uls_wch_t wch;
	int rc, len, j, map_flags = lit->map_flags;

	wch = __dec_escaped_char_cont('\0', lit);

	if (map_flags & ULS_FL_ESCSTR_MAPUNICODE) {
		if ((rc = _uls_tool_(encode_utf8)(wch, buff, ULS_UTF8_CH_MAXLEN)) <= 0) {
			return ULS_LITPROC_ERROR;
		}
		for (j=0; j<rc; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
		}

	} else if (map_flags & ULS_FL_ESCSTR_MAPHEXA) {
		_uls_tool(csz_putc)(lit_ctx->ss_dst, (char) wch);

	} else {
		_uls_log(err_log)("%s: unknown esc-ch mapping!", __func__);
		return -1;
	}

	lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
	len = qmt->len_end_mark;

	return len; // # of required bytes at the next stage.
}
