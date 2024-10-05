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
	uls_wch_t prefix_ch = lit->ch_escape;
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
		n = 2;

	} else {
		parms1.x1 = prefix_ch;
		uls_get_simple_escape_char(uls_ptr(parms1));
		wch = parms1.x2;
		n = 0;
	}

	lit->maxlen_esc_oxudigits = n;
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

	prefix_ch = lit->ch_escape;
	wch = lit->wch;
	lptr = lit->lptr;
	lptr_end = lptr + lit->maxlen_esc_oxudigits;

	if (prefix_ch == 'x' || prefix_ch == 'u' || prefix_ch == 'U') { // hexa-decimal, unicode
		for ( ; lptr < lptr_end; lptr++) {
			ch = *lptr;
			if (!_uls_tool_(isxdigit)(ch)) {
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

	lit->ch_escape = *lptr++;
	lit->lptr = lptr;

	wch = uls_get_escape_char_initial(lit);
	if (lit->maxlen_esc_oxudigits > 0) {
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
						// An unterminated string is found but return the string...
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
	const char *lptr;
	uls_type_tool(wrd) wrdx;
	uls_wch_t wch;
	int rc, n_lfs = 0;

	wrdx.wrd = buff;
	wrdx.siz = ULS_COMM_MARK_MAXSIZ + 1;
	wrdx.lptr = wrd;
	if ((parms->len = uls_get_escape_str('\0', uls_ptr(wrdx))) < 0) {
		return 0;
	}

	for (lptr = buff; (wch = *lptr) != '\0'; lptr += rc) {
		if (wch == '\n') ++n_lfs;
		if ((rc = _uls_tool_(decode_utf8)(lptr, -1, &wch)) <= 0) {
			_uls_log(err_log)("comment: encoding error!");
			return 0;
		}
	}

	parms->n = n_lfs;
	return 1;
}

int
ULS_QUALIFIED_METHOD(canbe_quotetype_mark)(char* wrd, uls_ptrtype_tool(outparam) parms)
{
	char *buff = parms->line;
	const char *lptr;
	uls_type_tool(wrd) wrdx;
	int rc, n_lfs = 0;
	uls_wch_t wch;

	wrdx.wrd = buff;
	wrdx.siz = ULS_QUOTE_MARK_MAXSIZ + 1;
	wrdx.lptr = wrd;
	if ((parms->len = uls_get_escape_str('\0', uls_ptr(wrdx))) < 0)
		return 0;

	for (lptr = buff; (wch = *lptr) != '\0'; lptr += rc) {
		if (wch == '\n') ++n_lfs;
		if ((rc = _uls_tool_(decode_utf8)(lptr, -1, &wch)) <= 0) {
			_uls_log(err_log)("quote-string: encoding error!");
			return 0;
		}
	}

	if ((parms->len == 1 && buff[0] == '.')) {
		return 0;
	}

	parms->n = n_lfs;
	return 1;
}

void
ULS_QUALIFIED_METHOD(uls_init_quotetype)(uls_quotetype_ptr_t qmt)
{
	uls_initial_zerofy_object(qmt);
	uls_init_namebuf(qmt->start_mark, ULS_QUOTE_MARK_MAXSIZ);
	uls_init_namebuf(qmt->end_mark, ULS_QUOTE_MARK_MAXSIZ);
	qmt->escmap = nilptr;

	qmt->eos_wch = '\0';
	uls_init_bytespool(qmt->eos_utf8, ULS_UTF8_CH_MAXLEN + 1, 1);
	qmt->eos_utf8[0] = '\0';

	qmt->escsym_wch = '\\';
	uls_init_bytespool(qmt->escsym_utf8, ULS_UTF8_CH_MAXLEN + 1, 1);
	qmt->escsym_utf8[0] = '\\';
	qmt->escsym_utf8[1] = '\0';
}

void
ULS_QUALIFIED_METHOD(uls_deinit_quotetype)(uls_quotetype_ptr_t qmt)
{
	uls_dealloc_escmap(qmt->escmap);
	uls_deinit_namebuf(qmt->start_mark);
	uls_deinit_namebuf(qmt->end_mark);
	uls_deinit_bytespool(qmt->eos_utf8);
	uls_deinit_bytespool(qmt->escsym_utf8);
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
ULS_QUALIFIED_METHOD(proc_litstr_eoi)(uls_litstr_ptr_t lit, int len, const char *emark, int len_emark, _uls_ptrtype_tool(csz_str) outbuf)
{
	const char *line = lit->lptr;
	int rc = 0;

	if (len == 1) {
		if (len_emark == 1 && line[0] == emark[0]) {
			rc = 1;
		}
	} else if (len == 2) {
		if (len_emark == 1) {
			if (line[0] == emark[0]) {
				rc = 1;
			} else if (line[1] == emark[0]) {
				_uls_tool(csz_putc)(outbuf, *line);
				rc = 2;
			}
		} else if (len_emark == 2) {
			if (line[0] == emark[0] && line[1] == emark[1]) {
				rc = 2;
			}
		}
	}

	if (rc > 0) {
		lit->lptr = line += rc;
		rc = ULS_LITPROC_ENDOFQUOTE;
	} else {
		rc = ULS_LITPROC_ERROR;
	}

	return rc;
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape0)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	const char *lptr = lit->lptr, *lptr_end = lit->lptr_end;
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;

	char buff[ULS_UTF8_CH_MAXLEN];
	const char *emark = uls_get_namebuf_value(qmt->end_mark);
	int j, rc, len_emark = qmt->len_end_mark, len = (int) (lptr_end - lptr);
	uls_wch_t wch;

	if ((rc = len) > ULS_UTF8_CH_MAXLEN) rc = ULS_UTF8_CH_MAXLEN;
	for (j = 0; j < rc; j++) buff[j] = lptr[j];

	if ((rc = _uls_tool_(decode_utf8)(buff, rc, NULL)) > len) {
		_uls_tool(csz_putc)(lit_ctx->ss_dst, *lptr++);
		lit->lptr = lptr;

		if (--len > 0) {
			rc = proc_litstr_eoi(lit, len, emark, len_emark, lit_ctx->ss_dst);
		} else {
			rc = ULS_LITPROC_ENDOFQUOTE;
		}
		return rc;
	}

	while (1) {
		if (_uls_tool_(strncmp)(lptr, emark, len_emark) == 0) {
			lptr += len_emark;
			len = ULS_LITPROC_ENDOFQUOTE;
			break;
		}

		if (len > ULS_UTF8_CH_MAXLEN) len = ULS_UTF8_CH_MAXLEN;
		for (j=0; j<len; j++) buff[j] = lptr[j];

		if ((rc = _uls_tool_(decode_utf8)(buff, j, &wch)) <= 0) {
			if (rc < -ULS_UTF8_CH_MAXLEN)
				return ULS_LITPROC_ERROR;
			len = ULS_UTF8_CH_MAXLEN;
			break;
		}

		if (wch == qmt->escsym_wch) {
			lptr += rc;
			lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape1);
			len = ULS_UTF8_CH_MAXLEN; // for '\r''\n' or \utf-8-bytes
			break;
		}

		if (wch == '\n') {
			if ((qmt->flags & ULS_QSTR_MULTILINE) == 0) {
//				_uls_log(err_log)("%s: unterminated literal-string", __func__);
				len = ULS_LITPROC_ERROR;
				break;
			}
			++lit_ctx->n_lfs;
		}

		for (j=0; j<rc; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, lptr[j]);
		}
		lptr += rc;

		if ((len = (int) (lptr_end - lptr)) < len_emark) {
			len = len_emark;
			break;
		}
	}

	lit->lptr = lptr;
	return len; // # of required bytes at the next stage.
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape1)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	const char *lptr = lit->lptr, *lptr_end = lit->lptr_end;
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;
	uls_escstr_ptr_t escstr;

	uls_type_tool(outparam) parms1;
	char ch, buff[ULS_UTF8_CH_MAXLEN];
	int ind, len = (int) (lptr_end - lptr), len1, j, len_buff, rc;
	uls_wch_t wch;

	if ((ch = *lptr) == '\n') {
		buff[0] = '\n';
		len_buff = 1;
		wch = '\n';

		if (len >= 2 && lptr[1] == '\r') {
			len1 = 2;
		} else {
			len1 = 1;
		}
		++lit_ctx->n_lfs;

	} else if (ch == '\r') {
		if (len >= 2 && lptr[1] == '\n') {
			buff[0] = '\n';
			len_buff = 1;
			wch = '\n';

			len1 = 2;
			++lit_ctx->n_lfs;
		} else {
			buff[0] = '\r';
			len_buff = len1 = 1;
			wch = '\r';
		}

	} else {
		if ((rc = len) > ULS_UTF8_CH_MAXLEN) rc = ULS_UTF8_CH_MAXLEN;
		for (j = 0; j < rc; j++) buff[j] = lptr[j];

		if ((len1 = _uls_tool_(decode_utf8)(buff, rc, &wch)) <= 0) {
			if (len1 < -ULS_UTF8_CH_MAXLEN) {
				rc =  ULS_LITPROC_ERROR;
			} else {
				for (j = 0; (ch = qmt->escsym_utf8[j]) != '\0'; j++) {
					_uls_tool(csz_putc)(lit_ctx->ss_dst, ch);
				}
				_uls_tool(csz_putc)(lit_ctx->ss_dst, *lptr++);

				lit->lptr = lptr;
				if (--len > 0) {
					rc = proc_litstr_eoi(lit, len,
						uls_get_namebuf_value(qmt->end_mark), qmt->len_end_mark, lit_ctx->ss_dst);
				} else {
					rc = ULS_LITPROC_ENDOFQUOTE;
				}
			}

			lit->ch_escape = 0; // NA
			lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
			return rc;
		}
		len_buff = len1;
	}

	lit->ch_escape = wch;
	lit->lptr = lptr += len1;
	lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
	len = qmt->len_end_mark; // # of required bytes at the next stage.

	if (wch != '\0' && wch == qmt->eos_wch && (qmt->flags & ULS_QSTR_EOS)) {
		for (j = 0; (ch = qmt->eos_utf8[j]) != '\0'; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, ch);
		}
		return len;
	}

	if (wch == qmt->escsym_wch && (qmt->flags & ULS_QSTR_ESC)) {
		for (j = 0; (ch = qmt->escsym_utf8[j]) != '\0'; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, ch);
		}
		return len;
	}

	if ((ind = uls_escmap_canbe_escch(wch)) < 0) {
		_uls_tool(csz_puts)(lit_ctx->ss_dst, qmt->escsym_utf8);
		for (j=0; j < len_buff; j++) {
			_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
		}
		return len;
	}

	lit->maxlen_esc_oxudigits = 0;
	lit->map_flags = 0;

	if ((escstr = uls_find_escstr_nosafe(qmt->escmap, ind, wch & 0xFF)) == nilptr) {
		if (qmt->flags & ULS_QSTR_ETC) {
			for (j=0; j < len_buff; j++) {
				_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
			}
		} else {
			for (j = 0; (ch = qmt->escsym_utf8[j]) != '\0'; j++) {
				_uls_tool(csz_putc)(lit_ctx->ss_dst, ch);
			}
			for (j=0; j < len_buff; j++) {
				_uls_tool(csz_putc)(lit_ctx->ss_dst, buff[j]);
			}
		}
	} else {
		parms1.x1 = wch;
		if ((rc = uls_dec_escaped_char(escstr, qmt->escmap, uls_ptr(parms1), lit_ctx->ss_dst)) < 0) {
			lit->maxlen_esc_oxudigits = len = -rc; // max#-of-bytes for 1-wchar
			lit->map_flags = parms1.flags;
			lit->wch = wch = parms1.wch;
			lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape2);
		}
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(__dec_escaped_char_cont)(char quote_ch, uls_litstr_ptr_t lit)
{
	const char *lptr = lit->lptr, *lptr_end;
	int cs_mask, map_flags = lit->map_flags;
	int ch, n, start_hdigit, end_hdigit, stat = 0;
	uls_wch_t wch = lit->wch;

	if ((lptr_end = lptr + lit->maxlen_esc_oxudigits) > lit->lptr_end) {
		lptr_end = lit->lptr_end;
	}

	if (map_flags & ULS_FL_ESCSTR_OCTAL) {
		for ( ; lptr < lptr_end; lptr++) {
			if ((ch=*lptr) < '0' || ch >= '8') {
				break;
			}
			wch = (wch<<3) + (ch-'0');
		}

	} else { // hexa-decimal
		cs_mask = map_flags & (ULS_FL_ESCSTR_HEXA_AF|ULS_FL_ESCSTR_HEXA_af);

		if (cs_mask) {
			if (cs_mask == (ULS_FL_ESCSTR_HEXA_AF|ULS_FL_ESCSTR_HEXA_af)) {
				// case-insensitive but uniform: it depends on the 1st alphabet char
				start_hdigit = end_hdigit = 0;
				for ( ; lptr < lptr_end; lptr++) {
					ch = *lptr;

					if (_uls_tool_(isdigit)(ch)) {
						ch -= '0';
					} else {
						if (start_hdigit > 0) {
							if (ch >= start_hdigit && ch <= end_hdigit) {
								ch = ch - start_hdigit + 10;
							} else {
								break;
							}
						} else {
							if (_uls_tool_(isupper)(ch)) {
								start_hdigit = 'A';
								end_hdigit = 'F';
							} else if (_uls_tool_(islower)(ch)) {
								start_hdigit = 'a';
								end_hdigit = 'f';
							} else {
								break;
							}
						}
					}
					wch = (wch << 4) + ch;
				}
			} else {
				if (map_flags & ULS_FL_ESCSTR_HEXA_AF) {
					start_hdigit = 'A';
					end_hdigit = 'F';
				} else if (map_flags & ULS_FL_ESCSTR_HEXA_af) {
					start_hdigit = 'a';
					end_hdigit = 'f';
				} else {
					// NEVER-REACHED
					start_hdigit = end_hdigit = 0;
				}

				for ( ; lptr < lptr_end; lptr++) {
					ch = *lptr;
					if (_uls_tool_(isdigit)(ch)) {
						ch -= '0';
					} else if (ch >= start_hdigit && ch <= end_hdigit) {
						ch = ch - start_hdigit + 10;
					} else {
						break;
					}
					wch = (wch << 4) + ch;
				}
			}
		} else {
			// case-insensitive
			for ( ; lptr < lptr_end; lptr++) {
				ch = *lptr;

				if (!_uls_tool_(isxdigit)(ch)) {
					break;
				}

				if (_uls_tool_(isdigit)(ch)) {
					ch -= '0';
				} else {
					ch = _uls_tool_(toupper)(ch) - 'A' + 10;
				}
				wch = (wch << 4) + ch;
			}
		}
	}

	if ((n = (int) (lptr - lit->lptr)) != lit->maxlen_esc_oxudigits) {
		if (map_flags & ULS_FL_ESCSTR_FIXED_NDIGITS) {
			_uls_log(err_log)("esc-digits %d < %d in esc-str!", n , lit->maxlen_esc_oxudigits);
			stat = -1;
		}
	}

	lit->lptr = lptr;
	lit->wch = wch;
	return stat;
}

int
ULS_QUALIFIED_METHOD(dfl_lit_analyzer_escape2)(uls_litstr_ptr_t lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit->context);
	uls_quotetype_ptr_t qmt = lit_ctx->qmt;
	char buff[ULS_UTF8_CH_MAXLEN];
	int i, rc, map_flags = lit->map_flags;

	if (__dec_escaped_char_cont('\0', lit) < 0) {
		return ULS_LITPROC_ERROR;
	}

	if (map_flags & ULS_FL_ESCSTR_UNICODE) {
		if ((rc = _uls_tool_(encode_utf8)(lit->wch, buff, ULS_UTF8_CH_MAXLEN)) <= 0) {
			_uls_log(err_log)("Unknown unicode 0x%08x in the literal string!", lit->wch);
			return ULS_LITPROC_ERROR;
		}
	} else { // hex, oct
		if (lit->wch > 0xFF) {
			_uls_log(err_log)("InternalError: Overflow of hex-byte 0x%X in the literal string!", lit->wch);
		}
		buff[0] = (char) (lit->wch & 0xFF);
		rc = 1;
	}

	for (i = 0; i < rc; i++) {
		_uls_tool(csz_add_ch)(lit_ctx->ss_dst, buff[i]);
	}

	lit_ctx->litstr_proc = uls_ref_callback_this(dfl_lit_analyzer_escape0);
	return qmt->len_end_mark;
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_parse_escmap)(char *line, uls_quotetype_ptr_t qmt, uls_escmap_pool_ptr_t escmap_pool2)
{
	uls_escmap_ptr_t esc_map;
	uls_type_tool(outparam) parms1;
	int do_dup;

	parms1.line = line;
	esc_map = uls_parse_escmap_feature(uls_ptr(parms1));
	if (esc_map == nilptr) return nilptr;

	do_dup = parms1.x1;
	if ((line = parms1.line) != NULL) {
		line = _uls_tool(skip_blanks)(line);
		if (*line != '\0') do_dup = 1;
	}

	if (parms1.flags & ULS_ESCMAP_MODERN_EOS) {
		qmt->flags |= ULS_QSTR_EOS;
	}

	if (parms1.flags & ULS_ESCMAP_MODERN_ESC) {
		qmt->flags |= ULS_QSTR_ESC;
	}

	if (parms1.flags & ULS_ESCMAP_MODERN_ETC) {
		qmt->flags |= ULS_QSTR_ETC;
	}

	if (do_dup) {
		esc_map = uls_dup_escmap(esc_map, escmap_pool2, parms1.flags);
	}

	// A trailed mapping esc-ch --> utf8-str in form of ch:str
	if (line != NULL) {
		if (uls_parse_escmap_mapping(esc_map, escmap_pool2, line) < 0) {
			uls_dealloc_escmap(esc_map);
			esc_map = nilptr;
		}
	}

	return esc_map;
}
