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
 * input.c -- input manipulating procedures from fd --
 *     written by Stanley Hong <link2next@gmail.com>, May 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_INPUT__
#include "uls/uls_input.h"
#include "uls/uls_misc.h"
#include "uls/utf_file.h"
#include "uls/utf8_enc.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC _ULS_INLINE int
ULS_QUALIFIED_METHOD(__input_space_proc)(const char* ch_ctx, _uls_ptrtype_tool(csz_str) ss_dst, uls_ptrtype_tool(outparam) parms)
{
	const char  *lptr = parms->lptr, *lptr_end = parms->lptr_end;
	int   stat=0, n_spaces, n_spaces_total, n_lfs_total;
	char  ch;

	n_spaces_total = parms->n1;
	n_lfs_total = parms->n2; // '\n'

	for (n_spaces=0; ; ) {
		if ((ch = *lptr) >= ULS_SYNTAX_TABLE_SIZE || ch_ctx[ch] != 0) {
			stat = 1;
			break;
		}

		if (ch == '\n') { // treat as special cases
			if (n_spaces > 0) {
				_uls_tool(csz_putc)(ss_dst, ' ');
				++n_spaces_total;
				n_spaces = 0;
			}
			_uls_tool(csz_putc)(ss_dst, '\n');
			++n_lfs_total;

		} else {
			if (n_spaces >= (ULS_INT_MAX>>1)) {
				// n_spaces reaches ULS_INT_MAX / 2
				_uls_tool(csz_putc)(ss_dst, ' ');
				++n_spaces_total;
				n_spaces = 0; // reset n_spaces to zero
			}
			++n_spaces;
		}

		if (++lptr == lptr_end) {
			break;
		}
	}

	if (n_spaces > 0) {
		_uls_tool(csz_putc)(ss_dst, ' ');
		++n_spaces_total;
//		n_spaces = 0;
	}

	parms->lptr = lptr;
	parms->n1 = n_spaces_total;
	parms->n2 = n_lfs_total;

	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_init_commtype)(uls_commtype_ptr_t cmt)
{
	uls_initial_zerofy_object(cmt);
	uls_init_namebuf(cmt->start_mark, 1 + ULS_COMM_MARK_MAXSIZ);
	uls_init_namebuf(cmt->end_mark, ULS_COMM_MARK_MAXSIZ);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_commtype)(uls_commtype_ptr_t cmt)
{
	uls_deinit_namebuf(cmt->start_mark);
	uls_deinit_namebuf(cmt->end_mark);
}

int
ULS_QUALIFIED_METHOD(input_skip_comment)(uls_commtype_ptr_t cmt, uls_input_ptr_t inp, uls_ptrtype_tool(outparam) parms)
{
	const char  *lptr, *lptr_end, *mark;
	int n_lfs = 0, len_mark;
	int  stat = 1; // --> success

	lptr = inp->rawbuf_ptr;
	lptr_end = lptr + inp->rawbuf_bytes;

	mark = uls_get_namebuf_value(cmt->end_mark);
	len_mark = cmt->len_end_mark;

	for ( ; ; lptr++) {
		if (lptr_end < lptr + len_mark) {
			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);
			if (inp->refill(inp, len_mark) < 0) {
//				_uls_log(err_log)("%s: I/O error", __func__);
				uls_input_reset_cursor(inp);
				lptr = lptr_end = inp->rawbuf.buf;
				stat = -1; // error
				break;
			} else if (inp->rawbuf_bytes < len_mark) {
				uls_input_reset_cursor(inp);
				lptr = lptr_end = inp->rawbuf.buf;
				stat = 0; // comment unterminated!
				break;
			}

			lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;
		}

		if (lptr[0] == mark[0]) {
			if (len_mark == 1 || !_uls_tool_(strncmp)(lptr+1,mark+1, len_mark-1)) {
				lptr += len_mark;
				break;
			}
		}

		if (*lptr == '\n') ++n_lfs;
	}

	inp->rawbuf_ptr = lptr;
	inp->rawbuf_bytes = (int) (lptr_end - lptr);

	parms->n = n_lfs;
	return stat;
}

int
ULS_QUALIFIED_METHOD(input_quote_proc)(uls_input_ptr_t inp, uls_quotetype_ptr_t qmt, _uls_ptrtype_tool(csz_str) ss_dst, uls_ptrtype_tool(outparam) parms)
{
	uls_litstr_t lit;
	uls_litstr_context_ptr_t lit_ctx = uls_ptr(lit.context);

	const char *lptr, *lptr_end;
	int   stat, n_bytes_req, n_qmt_lines=qmt->n_lfs;

	lit_ctx->qmt = qmt;
	lit_ctx->litstr_proc = qmt->litstr_analyzer;
	lit_ctx->n_lfs = 0;
	lit_ctx->ss_dst = ss_dst;

	lptr = inp->rawbuf_ptr;
	lptr_end = lptr + inp->rawbuf_bytes;

	n_bytes_req = _uls_tool_(strlen)(uls_get_namebuf_value(qmt->end_mark));

	for ( ; ; ) {
		if (lptr_end < lptr + n_bytes_req) {
			inp->rawbuf_ptr = lptr;
			inp->rawbuf_bytes = (int) (lptr_end - lptr);

			if (inp->refill(inp, n_bytes_req) < 0) {
//				_uls_log(err_log)("%s: I/O error", __func__);
				uls_input_reset_cursor(inp);
				return ULS_LITPROC_ERROR;
			}

			lptr = inp->rawbuf_ptr;
			lptr_end = lptr + inp->rawbuf_bytes;
			// In case of EOF, lptr == lptr_end
		}

		lit.lptr = lptr;
		lit.lptr_end = lptr_end;
		n_bytes_req = lit_ctx->litstr_proc(uls_ptr(lit));
		lptr = lit.lptr;

		if (n_bytes_req <= 0) {
			if (n_bytes_req == ULS_LITPROC_ENDOFQUOTE && (qmt->flags & ULS_QSTR_R_EXCLUSIVE)) {
				lptr -= qmt->len_end_mark;
				n_qmt_lines = qmt->n_left_lfs;
			}
			stat = n_bytes_req;
			break;
		}
	}

	inp->rawbuf_ptr = lptr;
	inp->rawbuf_bytes = (int) (lptr_end - lptr);

	parms->n = n_qmt_lines + lit_ctx->n_lfs;

	return stat;
}

int
ULS_QUALIFIED_METHOD(input_space_proc)(const char* ch_ctx, uls_input_ptr_t inp,
	_uls_ptrtype_tool(csz_str) ss_dst, int len_surplus, uls_ptrtype_tool(outparam) parms0)
{
	int  n_lfs, stat = 0;
	uls_type_tool(outparam) parms;

	parms.lptr = inp->rawbuf_ptr;
	parms.lptr_end = parms.lptr + inp->rawbuf_bytes;
	parms.n1 = parms.n2 = 0;

	do {
		if (parms.lptr_end < parms.lptr + len_surplus) {
			inp->rawbuf_ptr = parms.lptr;
			inp->rawbuf_bytes = (int) (parms.lptr_end - parms.lptr);

			if (inp->refill(inp, len_surplus) < 0) {
//				_uls_log(err_log)("%s: I/O error", __func__);
				return -1;
			}

			parms.lptr = inp->rawbuf_ptr;
			parms.lptr_end = parms.lptr + inp->rawbuf_bytes;

			if (inp->rawbuf_bytes == 0) {
				stat = 0; // EOF
				break;
			}
		}
	} while ((stat = __input_space_proc(ch_ctx, ss_dst, uls_ptr(parms))) == 0);

	parms0->n = n_lfs = parms.n2;

	inp->rawbuf_ptr = parms.lptr;
	inp->rawbuf_bytes = (int) (parms.lptr_end - parms.lptr);

	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_input_change_filler_null)(uls_input_ptr_t inp)
{
	_uls_tool(str_free)(uls_ptr(inp->rawbuf));

	inp->refill = uls_ref_callback_this(uls_input_refill_null);
	inp->rawbuf_ptr = NULL;
	inp->rawbuf_bytes = 0;

	inp->isource.usrc = nilptr;
	inp->isource.usrc_fillbuff = uls_ref_callback_this(uls_fill_null_source);
	inp->isource.usrc_ungrab = uls_ref_callback_this(uls_ungrab_null_source);
	inp->isource.flags = 0;
}

int
ULS_QUALIFIED_METHOD(uls_init_line_in_input)(uls_input_ptr_t inp, const char* line, int n_bytes, int ipos)
{
	int ipos2;

	if (n_bytes <= 0 || line == NULL) return ipos;

	_uls_tool(str_modify)(uls_ptr(inp->rawbuf), ipos, line, n_bytes);
	inp->rawbuf_ptr = inp->rawbuf.buf;

	if ((ipos2=ipos + n_bytes) > inp->rawbuf_bytes)
		inp->rawbuf_bytes = ipos2;

	return ipos2;
}

void
ULS_QUALIFIED_METHOD(uls_input_reset_cursor)(uls_input_ptr_t inp)
{
	inp->rawbuf_ptr = inp->rawbuf.buf;
	inp->rawbuf_bytes = 0;
}

void
ULS_QUALIFIED_METHOD(uls_input_reset)(uls_input_ptr_t inp, int bufsiz, int flags)
{
	int m;

	if (bufsiz > 0) {
		m = MAX(ULS_TXT_RECHDR_SZ,ULS_BIN_RECHDR_SZ);
		if (bufsiz < m) bufsiz = m;
		if ((bufsiz = uls_roundup(bufsiz, ULS_FDBUF_INITSIZE)) > inp->rawbuf.siz) {
			_uls_tool(str_free)(uls_ptr(inp->rawbuf));
			_uls_tool(str_init)(uls_ptr(inp->rawbuf), bufsiz);
		}
	} else if (bufsiz == 0) {
		_uls_tool(str_free)(uls_ptr(inp->rawbuf));
	}

	uls_input_reset_cursor(inp);

	if (flags >= 0) {
		if (flags & ULS_INP_FL_REFILL_NULL) {
			inp->refill = uls_ref_callback_this(uls_input_refill_null);
		} else {
			inp->refill = uls_ref_callback_this(uls_input_refill_buffer);
		}
	}
}

void
ULS_QUALIFIED_METHOD(uls_input_change_filler)(uls_input_ptr_t inp,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	if (fill_rawbuf == nilptr)
		fill_rawbuf = uls_ref_callback_this(uls_fill_null_source);

	if (ungrab_proc == nilptr)
		ungrab_proc = uls_ref_callback_this(uls_ungrab_null_source);

	inp->isource.usrc = isrc;
	inp->isource.usrc_fillbuff = fill_rawbuf;
	inp->isource.usrc_ungrab = ungrab_proc;
	inp->isource.flags = 0;
}

void
ULS_QUALIFIED_METHOD(uls_init_isource)(uls_source_ptr_t isrc)
{
	uls_initial_zerofy_object(isrc);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_isource)(uls_source_ptr_t isrc)
{
	uls_initial_zerofy_object(isrc);
}

void
ULS_QUALIFIED_METHOD(uls_init_input)(uls_input_ptr_t inp)
{
	uls_initial_zerofy_object(inp);
	uls_init_isource(uls_ptr(inp->isource));
	inp->refill = uls_ref_callback_this(uls_input_refill_null);

	uls_input_change_filler(inp, nilptr, nilptr, nilptr);
	inp->line_num = 1;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_input)(uls_input_ptr_t inp)
{
	uls_input_reset(inp, 0, ULS_INP_FL_REFILL_NULL);
	uls_input_change_filler(inp, nilptr, nilptr, nilptr);
	uls_deinit_isource(uls_ptr(inp->isource));
}

ULS_QUALIFIED_RETTYP(uls_input_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_input)(void)
{
	uls_input_ptr_t inp;

	inp = uls_alloc_object(uls_input_t);
	uls_init_input(inp);

	return inp;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_input)(uls_input_ptr_t inp)
{
	uls_deinit_input(inp);
	uls_dealloc_object(inp);
}

int
ULS_QUALIFIED_METHOD(uls_input_read)(uls_source_ptr_t isrc, char *buf, int buflen0, int bufsiz)
{
	// assume: buflen0 < bufsiz
	int buflen = buflen0;
	int rc;

	while (1) {
		if (isrc->flags & ULS_ISRC_FL_ERR) return -1;
		if (isrc->flags & ULS_ISRC_FL_EOF) break;

		rc = isrc->usrc_fillbuff(isrc, buf, buflen, bufsiz);
		if (rc < 0) {
			isrc->flags |= ULS_ISRC_FL_ERR | ULS_ISRC_FL_EOF;
			return -1;
		}

		if ((buflen += rc) >= bufsiz) break;
		if (rc == 0) _uls_tool_(msleep)(3);
	}

	return buflen - buflen0;
}

int
ULS_QUALIFIED_METHOD(uls_input_refill_null)(uls_input_ptr_t inp, int n_bytes)
{
	inp->isource.flags |= ULS_ISRC_FL_EOF;
	return inp->rawbuf_bytes;
}

int
ULS_QUALIFIED_METHOD(uls_input_refill_buffer)(uls_input_ptr_t inp, int n_bytes)
{
	int n, rc;

	if (n_bytes <= inp->rawbuf_bytes) return inp->rawbuf_bytes;
	uls_regulate_rawbuf(inp);

	if ((n = inp->rawbuf_bytes + ULS_UTF8_CH_MAXLEN) > inp->rawbuf.siz) {
		if (uls_resize_rawbuf(inp, n - inp->rawbuf.siz) < 0) {
			return -1;
		}
	}

	for ( ; ; ) {
		if ((rc = uls_input_read(uls_ptr(inp->isource),
			inp->rawbuf.buf, inp->rawbuf_bytes, inp->rawbuf.siz)) < 0) {
			return -1;
		}

		if (rc == 0 || (inp->rawbuf_bytes += rc) >= n_bytes) {
			break;
		}
		_uls_tool_(msleep)(15);
	}

	return inp->rawbuf_bytes;
}

void
ULS_QUALIFIED_METHOD(uls_regulate_rawbuf)(uls_input_ptr_t inp)
{
	int n_bytes = inp->rawbuf_bytes;

	if (inp->rawbuf.buf < inp->rawbuf_ptr) {
		if (n_bytes > 0) {
			_uls_tool_(memmove)(inp->rawbuf.buf, inp->rawbuf_ptr, n_bytes);
		}
		inp->rawbuf_ptr = inp->rawbuf.buf;
	}
}

int
ULS_QUALIFIED_METHOD(uls_resize_rawbuf)(uls_input_ptr_t inp, int delta)
{
	int m;

	if (delta < 0) {
		m = inp->rawbuf.siz - inp->rawbuf_bytes;
		if (m < -delta) delta = -m;
	}

	if (delta != 0) {
		m = inp->rawbuf.siz + delta;
		if (delta > 0) {
			m = uls_roundup(m, ULS_FDBUF_INITSIZE);
		} else {
			m = uls_roundup(m, sizeof(uls_uch_t));
		}

		inp->rawbuf_ptr = inp->rawbuf.buf = (char *) _uls_tool_(mrealloc)(inp->rawbuf.buf, m);
		inp->rawbuf.siz = m;
	}

	return inp->rawbuf.siz;
}

void
ULS_QUALIFIED_METHOD(uls_ungrab_linecheck)(uls_source_ptr_t isrc)
{
	uls_ptrtype_tool(outparam) parm = (uls_ptrtype_tool(outparam)) isrc->usrc;
	uls_mfree(parm->lptr);
	uls_dealloc_object(parm);
}

int
ULS_QUALIFIED_METHOD(uls_fill_null_source)(uls_source_ptr_t isrc, char* buf, int buflen, int bufsiz)
{
	isrc->flags |= ULS_ISRC_FL_EOF;
	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_ungrab_null_source)(uls_source_ptr_t isrc)
{
}

int
ULS_QUALIFIED_METHOD(uls_fill_fd_source_utf8)(uls_source_ptr_t isrc, char* buf, int buflen, int bufsiz)
{
	const uls_ptrtype_tool(utf_inbuf) inp = (const uls_ptrtype_tool(utf_inbuf)) isrc->usrc;
	int rc;

	rc = _uls_tool_(fill_utf8buf)(inp, buf, buflen, bufsiz);
	if (rc == 0) {
		if (inp->is_eof > 0) {
			isrc->flags |= ULS_ISRC_FL_EOF;
		} else if (inp->is_eof < 0) {
			rc = -1;
		}
	}

	return rc;
}
