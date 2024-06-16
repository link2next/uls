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
 * set_isrc.c --  setting input-stream in uls object --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_LEX__
#include "uls/uls_lex.h"
#include "uls/utf8_enc.h"
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_change_stream_hdr)(uls_lex_ptr_t uls, uls_istream_ptr_t istr, int flags)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t inp = ctx->input;
	int ipos=0, start_lno;

	if (flags & ULS_WANT_EOFTOK) uls_want_eof(uls);
	else uls_unwant_eof(uls);

	uls_select_isrc_filler(ctx, istr);

	if (xctx->len_prepended_input > 0 && (ctx->flags & ULS_CTX_FL_FILL_RAW)) {
		ipos = uls_init_line_in_input(inp, xctx->prepended_input, xctx->len_prepended_input, ipos);
		start_lno = 1 - xctx->lfs_prepended_input;
	} else {
		start_lno = 1;
	}

	uls_init_line_in_input(inp, istr->firstline, istr->len_firstline, ipos);
	uls_ctx_set_tag(ctx, uls_get_namebuf_value(istr->filepath), -1);
	__uls_ctx_set_lineno(ctx, start_lno);

	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(uls_fd_ungrabber)(uls_voidptr_t data)
{
	uls_ptrtype_tool(outparam) parm = (uls_ptrtype_tool(outparam)) data;
	int fd = parm->n;

	uls_dealloc_object(parm);
	_uls_tool_(fd_close)(fd);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__check_fd_dup)(int fd, int flags)
{
	int fd2;

	if (flags & ULS_DO_DUP) {
		if ((fd2 = uls_fd_dup(fd)) < 0) {
			_uls_log(err_log)("%s: error to dup-fd", __func__);
			return -1;
		}
	} else {
		fd2 = fd;
	}

	return fd2;
}

int
ULS_QUALIFIED_METHOD(uls_select_isrc_filler)(uls_context_ptr_t ctx, uls_istream_ptr_t istr)
{
	uls_ptrtype_tool(utf_inbuf) utf_inp;
	uls_callback_type_this(fill_isource) fill_isrc;
	uls_callback_type_this(ungrab_isource) ungrab_proc;
	int mode, subtype = istr->header.subtype;
	uls_voidptr_t dat;

	if (istr->header.filetype == ULS_STREAM_RAW) {
		fill_isrc = uls_ref_callback_this(uls_fill_fd_isrc_utf8);
		mode = subtype;
		if (istr->header.reverse) mode |= UTF_INPUT_FORMAT_REVERSE;
		utf_inp = _uls_tool_(utf_create_inbuf)(NULL, UTF_INPUT_BUFSIZ, mode);

		_uls_tool_(utf_set_inbuf)(utf_inp, istr->fd);
		utf_inp->data = (uls_voidptr_t) istr;

		dat = (uls_voidptr_t) utf_inp;
		ungrab_proc = uls_ref_callback_this(uls_ungrab_fd_utf);

	} else {
		fill_isrc = nilptr;
		dat = (uls_voidptr_t) istr;
		fill_isrc = uls_ref_callback_this(uls_fill_fd_stream);
		ungrab_proc = uls_ref_callback_this(uls_ungrab_fd_stream);
	}

	uls_input_reset(ctx->input, ULS_INPUT_BUFSIZ, 0);
	uls_input_change_filler(ctx->input, dat, fill_isrc, ungrab_proc);

	return 0;
}

ULS_QUALIFIED_RETTYP(uls_gettok_t)
ULS_QUALIFIED_METHOD(find_isrc_filler)(int fd_type, int fd_subtype, uls_ptrtype_tool(outparam) parms)
{
	uls_callback_type_this(gettok) proc = nilptr;
	uls_callback_type_this(xcontext_filler) fillproc = nilptr;
	uls_callback_type_this(xctx_boundary_checker) subproc2 = nilptr;
	int is_fillproc_dfl = 0;

	if (fd_type == ULS_STREAM_RAW) {
		proc = uls_ref_callback_this(uls_gettok_raw);
		fillproc = uls_ref_callback_this(xcontext_raw_filler);
		subproc2 = uls_ref_callback_this(check_rec_boundary_null);
		is_fillproc_dfl = 1;

	} else if (fd_type == ULS_STREAM_ULS) {
		if (fd_subtype == ULS_STREAM_BIN_LE) {
			if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
				proc = uls_ref_callback_this(uls_gettok_bin);
				fillproc = uls_ref_callback_this(xcontext_binfd_filler);
				subproc2 = uls_ref_callback_this(check_rec_boundary_host_order);
			} else { // ULS_BIG_ENDIAN
				proc = uls_ref_callback_this(uls_gettok_bin);
				fillproc = uls_ref_callback_this(xcontext_binfd_filler);
				subproc2 = uls_ref_callback_this(check_rec_boundary_reverse_order);
			}

		} else if (fd_subtype == ULS_STREAM_BIN_BE) {
			if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
				proc = uls_ref_callback_this(uls_gettok_bin);
				fillproc = uls_ref_callback_this(xcontext_binfd_filler);
				subproc2 = uls_ref_callback_this(check_rec_boundary_reverse_order);
			} else { // ULS_BIG_ENDIAN
				proc = uls_ref_callback_this(uls_gettok_bin);
				fillproc = uls_ref_callback_this(xcontext_binfd_filler);
				subproc2 = uls_ref_callback_this(check_rec_boundary_host_order);
			}

		} else if (fd_subtype == ULS_STREAM_TXT) {
			proc = uls_ref_callback_this(uls_gettok_bin);
			fillproc = uls_ref_callback_this(xcontext_txtfd_filler);
			subproc2 = uls_ref_callback_this(check_rec_boundary_null);
		}
	}

	parms->proc = (uls_voidptr_t) fillproc;
	parms->data = (uls_voidptr_t) subproc2;
	parms->flags = is_fillproc_dfl;

	return proc;
}

int
ULS_QUALIFIED_METHOD(uls_push_isrc_type)(uls_lex_ptr_t uls, int fd_type, int fd_subtype)
{
	uls_context_ptr_t ctx;
	uls_xcontext_filler_t fill_isrc;
	uls_xctx_boundary_checker_t fill_subproc2;
	uls_gettok_t  proc;
	uls_type_tool(outparam) parms1;

	if ((proc=find_isrc_filler(fd_type, fd_subtype, uls_ptr(parms1))) == nilptr) {
		_uls_log(err_log)("%s: unknown fd-type %d", __func__, fd_type);
		return -1;
	}

	fill_isrc = (uls_xcontext_filler_t) parms1.proc;
	fill_subproc2 = (uls_xctx_boundary_checker_t) parms1.data;

	ctx = uls_push(uls);
	ctx->gettok = proc;

	ctx->fill_proc = fill_isrc;
	if (parms1.flags) {
		ctx->flags |= ULS_CTX_FL_FILL_RAW;
	} else {
		ctx->flags &= ~ULS_CTX_FL_FILL_RAW;
	}

	ctx->record_boundary_checker = fill_subproc2;

	__uls_ctx_set_lineno(ctx, 1);
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_isrc_type)(uls_lex_ptr_t uls, int fd_type, int fd_subtype)
{
	uls_pop(uls);
	return uls_push_isrc_type(uls, fd_type, fd_subtype);
}

int
ULS_QUALIFIED_METHOD(uls_push_istream)(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	uls_tmpl_list_ptr_t tmpl_list, int flags)
{
	int stat = 0;

	if (istr == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	if (uls_push_isrc_type(uls, istr->header.filetype, istr->header.subtype) < 0) {
		return -2;
	}

	uls_set_tag(uls, uls_get_namebuf_value(istr->filepath), -1);

	if (__uls_change_stream_hdr(uls, istr, flags) < 0) {
		stat = -3; goto end_1;
	}

	if (uls_bind_istream(istr, uls) < 0) {
		_uls_log(err_log)("%s: not compatible uls-file.", __func__);
		stat = -4; goto end_1;
	}

	if (uls_bind_tmpls(istr, tmpl_list) < 0) {
		_uls_log(err_log)("can't put stream %s on the stack of %s.",
			uls_get_namebuf_value(istr->filepath), uls_get_namebuf_value(uls->ulc_name));
		stat = -5; goto end_1;
	}

	if (uls_fillbuff_and_reset(uls) < 0) {
		stat = -6; goto end_1;
	}

	uls_istr_grab(istr);
	return 0;

 end_1:
 	uls_pop(uls);
	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_set_istream)(uls_lex_ptr_t uls, uls_istream_ptr_t istr, uls_tmpl_list_ptr_t tmpl_list, int flags)
{
	if (istr == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_istream(uls, istr, tmpl_list, flags);
}

#ifndef ULS_DOTNET
int
ULS_QUALIFIED_METHOD(uls_push_istream_2)(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const char** tmpl_nams, const char** tmpl_vals, int n_tmpls, int flags)
{
	uls_tmpl_list_t tmpl_list;
	int i, stat = 0;

	if (istr == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	if (uls_push_isrc_type(uls, istr->header.filetype, istr->header.subtype) < 0)
		return -1;

	if (__uls_change_stream_hdr(uls, istr, flags) < 0) {
		stat = -2; goto end_2;
	}

	uls_set_tag(uls, uls_get_namebuf_value(istr->filepath), -1);

	if (tmpl_nams != nilptr) {
		uls_init_tmpls(uls_ptr(tmpl_list), n_tmpls, ULS_TMPLS_DUP);
		for (i=0; i<n_tmpls; i++) {
			if (uls_add_tmpl(uls_ptr(tmpl_list), tmpl_nams[i], tmpl_vals[i]) < 0) {
				stat = -3; goto end_1;
			}
		}
	} else {
		uls_init_tmpls(uls_ptr(tmpl_list), 0, ULS_TMPLS_DUP);
	}

	if (uls_bind_istream(istr, uls) < 0) {
		_uls_log(err_log)("%s: not compatible uls-file.", __func__);
		stat = -4; goto end_1;
	}

	if (uls_bind_tmpls(istr, uls_ptr(tmpl_list)) < 0) {
		_uls_log(err_log)("can't put stream %s on the stack of %s.",
			uls_get_namebuf_value(istr->filepath), uls_get_namebuf_value(uls->ulc_name));
		stat = -5; goto end_1;
	}

	if (uls_fillbuff_and_reset(uls) < 0) {
		stat = -6; goto end_1;
	}

	uls_deinit_tmpls(uls_ptr(tmpl_list));
	uls_istr_grab(istr);

	return 0;

 end_1:
	uls_deinit_tmpls(uls_ptr(tmpl_list));
 end_2:
 	uls_pop(uls);
	return stat;
}
#endif

int
ULS_QUALIFIED_METHOD(uls_push_file)(uls_lex_ptr_t uls, const char* filepath, int flags)
{
	uls_istream_ptr_t istr;

	if (filepath == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	if ((istr = uls_open_istream_file(filepath)) == nilptr) {
		_uls_log(err_log)("fail to read the header of %s.", filepath);
		return -1;
	}

	if (uls_push_istream(uls, istr, nilptr, flags) < 0) {
		__destroy_istream(istr);
		return -1;
	}

	uls_istr_ungrab(istr);
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_file)(uls_lex_ptr_t uls, const char* filepath, int flags)
{
	if (filepath == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_file(uls, filepath, flags);
}

int
ULS_QUALIFIED_METHOD(uls_push_fp)(uls_lex_ptr_t uls, FILE* fp, int flags)
{
	uls_istream_ptr_t istr;

	if (fp == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	if ((istr = uls_open_istream_fp(fp)) == nilptr) {
		_uls_log(err_log)("fail to read the header of fp.");
		return -1;
	}

	if (uls_push_istream(uls, istr, nilptr, flags) < 0) {
		__destroy_istream(istr);
		return -1;
	}

	uls_istr_ungrab(istr);
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_fp)(uls_lex_ptr_t uls, FILE* fp, int flags)
{
	if (fp == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_fp(uls, fp, flags);
}

int
ULS_QUALIFIED_METHOD(uls_push_fd)(uls_lex_ptr_t uls, int fd, int flags)
{
	uls_istream_ptr_t istr;
	uls_ptrtype_tool(outparam) parm;
	int fd2;

	if ((fd2=__check_fd_dup(fd, flags)) < 0) {
		_uls_log(err_log)("%s: invalid parameter fd=%d", __func__);
		return -1;
	}

	if ((istr = uls_open_istream(fd)) == nilptr) {
		_uls_log(err_log)("fail to read the header of fd.");
		if (fd2 != fd) _uls_tool_(fd_close)(fd2);
		return -1;
	}

	if (uls_push_istream(uls, istr, nilptr, flags) < 0) {
		__destroy_istream(istr);
		if (fd2 != fd) _uls_tool_(fd_close)(fd2);
		return -1;
	}

	if (fd2 != fd) {
		parm = uls_alloc_object(uls_type_tool(outparam));
		parm->n = fd2;
		uls_register_ungrabber(uls, 0,
			uls_ref_callback_this(uls_fd_ungrabber), (uls_voidptr_t) parm);
	}

	uls_istr_ungrab(istr);
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_fd)(uls_lex_ptr_t uls, int fd, int flags)
{
	uls_pop(uls);
	return uls_push_fd(uls, fd, flags);
}

int
ULS_QUALIFIED_METHOD(uls_register_ungrabber)(uls_lex_ptr_t uls, int at_tail, uls_input_ungrabber_t proc, uls_voidptr_t data)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_userdata_ptr_t ud0, ud, ud_prev;

	if (proc == nilptr) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	ud0 = uls_alloc_object(uls_userdata_t);
	ud0->proc = proc;
	ud0->data = data;

	if (at_tail) {
		ud0->inner = nilptr;

		for (ud_prev=nilptr,ud=ctx->user_data; ud != nilptr; ud = ud->inner) {
			ud_prev = ud;
		}

		if (ud_prev != nilptr) {
			ud_prev->inner = ud0;
		} else {
			ctx->user_data = ud0;
		}

	} else {
		ud0->inner = ctx->user_data;
		ctx->user_data = ud0;
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_push_utf16_line)(uls_lex_ptr_t uls, uls_uint16* wline, int wlen)
{
	char  *line;
	int len;
	uls_type_tool(outparam) parms;

	if (wline == NULL || wlen < 0) {
		_uls_log(err_log)("%s: fail to set utf16 string", __func__);
		return;
	} else if (wlen == 0) {
		uls_push_line(uls, "", 0, 0);
		return;
	}

	if ((line = _uls_tool_(enc_utf16str_to_utf8str)(wline, wlen, uls_ptr(parms))) == NULL) {
		_uls_log(err_log)("%s: fail to set utf16 string", __func__);
		return;
	}

	len = parms.len;
	uls_push_line(uls, line, len, ULS_MEMFREE_LINE);
}

void
ULS_QUALIFIED_METHOD(uls_set_utf16_line)(uls_lex_ptr_t uls, uls_uint16* wline, int wlen)
{
	char  *line;
	int len;
	uls_type_tool(outparam) parms;

	if (wline == NULL || wlen < 0) {
		_uls_log(err_log)("%s: fail to set utf16 string", __func__);
		return;
	} else if (wlen == 0) {
		uls_set_line(uls, "", 0, 0);
		return;
	}

	if ((line = _uls_tool_(enc_utf16str_to_utf8str)(wline, wlen, uls_ptr(parms))) == NULL) {
		_uls_log(err_log)("%s: fail to set utf16 string", __func__);
		return;
	}

	len = parms.len;
	uls_set_line(uls, line, len, ULS_MEMFREE_LINE);
}

void
ULS_QUALIFIED_METHOD(uls_push_utf32_line)(uls_lex_ptr_t uls, uls_uint32* wline, int wlen)
{
	char  *line;
	int len;
	uls_type_tool(outparam) parms;

	if (wline == NULL || wlen < 0) {
		_uls_log(err_log)("%s: fail to set utf32 string", __func__);
		return;
	} else if (wlen == 0) {
		uls_push_line(uls, "", 0, 0);
		return;
	}

	if ((line = _uls_tool_(enc_utf32str_to_utf8str)(wline, wlen, uls_ptr(parms))) == NULL) {
		_uls_log(err_log)("%s: fail to set utf32 string", __func__);
		return;
	}

	len = parms.len;
	uls_push_line(uls, line, len, ULS_MEMFREE_LINE);
}

void
ULS_QUALIFIED_METHOD(uls_set_utf32_line)(uls_lex_ptr_t uls, uls_uint32* wline, int wlen)
{
	char  *line;
	int len;
	uls_type_tool(outparam) parms;

	if (wline == NULL || wlen < 0) {
		_uls_log(err_log)("%s: fail to set utf32 string", __func__);
		return;
	} else if (wlen == 0) {
		uls_set_line(uls, "", 0, 0);
		return;
	}

	if ((line = _uls_tool_(enc_utf32str_to_utf8str)(wline, wlen, uls_ptr(parms))) == NULL) {
		_uls_log(err_log)("%s: fail to set utf32 string", __func__);
		return;
	}

	len = parms.len;
	uls_set_line(uls, line, len, ULS_MEMFREE_LINE);
}

int
ULS_QUALIFIED_METHOD(ulsjava_push_line)(uls_lex_ptr_t uls, const void *line, int len_line, int flags)
{
	int rc;
	rc = uls_push_line(uls, (const char*) line, len_line, flags|ULS_DO_DUP);
	return rc;
}

int
ULS_QUALIFIED_METHOD(ulsjava_push_file)(uls_lex_ptr_t uls, const void *filepath, int len_filepath, int flags)
{
	const char *ustr = _uls_tool_(strdup)((const char*)filepath, len_filepath);
	int rc;

	rc = uls_push_file(uls, ustr, flags);
	uls_mfree(ustr);

	return rc;
}

int
ULS_QUALIFIED_METHOD(ulsjava_set_line)(uls_lex_ptr_t uls, const void *line, int len_line, int flags)
{
	int rc;
	rc = uls_set_line(uls, (const char*) line, len_line, flags|ULS_DO_DUP);
	return rc;
}

int
ULS_QUALIFIED_METHOD(ulsjava_set_file)(uls_lex_ptr_t uls, const void *filepath, int len_filepath, int flags)
{
	const char *ustr = _uls_tool_(strdup)((const char*)filepath, len_filepath);
	int rc;

	rc = uls_set_file(uls, ustr, flags);
	uls_mfree(ustr);

	return rc;
}

void
ULS_QUALIFIED_METHOD(uls_skip_white_spaces)(uls_lex_ptr_t uls)
{
	skip_white_spaces(uls);
}
