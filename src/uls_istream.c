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
 * uls_istream.c -- the routines that make lexical streams ... --
 *     written by Stanley Hong <link2next@gmail.com>, Aug 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_ISTREAM__
#include "uls/uls_istream.h"
#include "uls/uls_sysprops.h"
#include "uls/utf8_enc.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"

#include <sys/types.h>
#endif

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__init_istream)(uls_istream_ptr_t istr)
{
	uls_init_namebuf(istr->filepath, ULS_FILEPATH_MAX);
	uls_init_stream_header(uls_ptr(istr->header));

	istr->fd = -1;
	istr->start_off = -1;

	istr->firstline = (char *) uls_malloc_buffer(ULS_MAGICCODE_SIZE + 1);
	_uls_tool_(init_tempfile)(uls_ptr(istr->uld_file));
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(__create_istream)(int fd)
{
	uls_istream_ptr_t istr;

	istr = uls_alloc_object(uls_istream_t);
	uls_initial_zerofy_object(istr);
	__init_istream(istr);

	istr->fd = fd;

	_uls_tool_(version_make)(uls_ptr(istr->header.filever),
		ULS_VERSION_STREAM_MAJOR, ULS_VERSION_STREAM_MINOR,
		ULS_VERSION_STREAM_DEBUG);

	istr->ref_cnt = 1;

	return istr;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__destroy_istream)(uls_istream_ptr_t istr)
{
	istr->ref_cnt = 0;
	istr->fd = -1;
	uls_mfree(istr->firstline);

	_uls_tool_(deinit_tempfile)(uls_ptr(istr->uld_file));
	uls_deinit_stream_header(uls_ptr(istr->header));

	uls_deinit_namebuf(istr->filepath);
	uls_dealloc_object(istr);
}

int
ULS_QUALIFIED_METHOD(check_istr_compatibility)(uls_istream_ptr_t istr, uls_lex_ptr_t uls)
{
	const char *specname;
	int ftype;

	if ((ftype = istr->header.filetype) == ULS_STREAM_RAW) {
		return 1;
	}

	specname = uls_get_namebuf_value(istr->header.specname);
	if (!uls_check_stream_ver(uls_ptr(istr->header), uls)) {
		_uls_log(err_log)("%s: Unsupported version: ", specname);
		return 0;
	}

	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(make_tokpkt_seqence)(uls_lex_ptr_t uls, const char* line, uls_tmpl_pool_ptr_t tmpls_pool)
{
	_uls_ptrtype_tool(csz_str) str_pool = uls_ptr(tmpls_pool->str_pool);
	const char *lxm;
	int  n=0, tok, k, len;

	len = _uls_tool_(strlen)(line);

	if (uls_push_line(uls, line, len, 0) < 0) {
		_uls_log(err_log)("%s: can't push string line", __FUNCTION__);
		return -1;
	}

	uls_want_eof(uls);

	while (1) {
		uls_gettok_raw(uls);
		if ((tok = __uls_tok(uls)) == uls->xcontext.toknum_EOF) {
			break;
		}

		lxm = __uls_lexeme(uls);
		len = __uls_lexeme_len(uls);

		k = csz_length(str_pool);
		_uls_tool(csz_append)(str_pool, lxm, len);
		_uls_tool(csz_add_eos)(str_pool);

		add_rd_packet_to_tmpls_pool(tok, len, (const char *)(uls_uint64)k, tmpls_pool);
		++n;
	}

	uls_pop(uls); // pop EOF!
	return n;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(make_eoif_lexeme_bin)(uls_context_ptr_t ctx, int tok_id, const char *txt, int txtlen)
{
	ctx->tok = tok_id;
	_uls_tool(str_append)(uls_ptr(ctx->tokbuf), 0, txt, txtlen);

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = txtlen;
	ctx->s_val_uchars = _uls_tool(ustr_num_chars)(ctx->s_val, txtlen, nilptr);
}

int
ULS_QUALIFIED_METHOD(uls_check_stream_ver)(uls_stream_header_ptr_t hdr, uls_lex_ptr_t uls)
{
	int stat;

	if (hdr->filetype == ULS_STREAM_RAW ||
		uls_spec_compatible(uls, uls_get_namebuf_value(hdr->specname), uls_ptr(hdr->filever))) {
		stat = 1;
	} else {
		stat = 0;
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(get_rawfile_subtype)(char *buff, int n_bytes, uls_ptrtype_tool(outparam) parms)
{
	int mode = -1, byte_order = -1, reverse = 0, fpos = 0;

	if (n_bytes >= 4) {
		if (buff[0] == 0 && buff[1] == 0 && buff[2] == 0xFE && buff[3] == 0xFF) {
			mode = UTF_INPUT_FORMAT_32;
			byte_order = ULS_BIG_ENDIAN;
			fpos = 4;
		} else if (buff[0] == 0xFF && buff[1] == 0xFE && buff[2] == 0 && buff[3] == 0) {
			mode = UTF_INPUT_FORMAT_32;
			byte_order = ULS_LITTLE_ENDIAN;
			fpos = 4;
		}
	}

	if (mode < 0 && n_bytes >= 3) {
		if (buff[0] == 0xEF && buff[1] == 0xBB && buff[2] == 0xBF) {
			mode = UTF_INPUT_FORMAT_8;
			fpos = 3;
		}
	}

	if (mode < 0 && n_bytes >= 2) {
		if (buff[0] == 0xFE && buff[1] == 0xFF) {
			mode = UTF_INPUT_FORMAT_16;
			byte_order = ULS_BIG_ENDIAN;
			fpos = 2;
		} else if (buff[0] == 0xFF && buff[1] == 0xFE) {
			mode = UTF_INPUT_FORMAT_16;
			byte_order = ULS_LITTLE_ENDIAN;
			fpos = 2;
		}
	}

	if (fpos > 0) {
		if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
			if (byte_order == ULS_BIG_ENDIAN) reverse = 1;
		} else {
			if (byte_order == ULS_LITTLE_ENDIAN) reverse = 1;
		}
	}

	if (mode < 0) {
		mode = UTF_INPUT_FORMAT_8;
	}

	parms->n1 = mode; // n1:subtype
	parms->n2 = reverse; // n2:reverse

	return fpos;
}

void
ULS_QUALIFIED_METHOD(uls_ungrab_fd_utf)(uls_source_ptr_t isrc)
{
	uls_ptrtype_tool(utf_inbuf) inp = (uls_ptrtype_tool(utf_inbuf)) isrc->usrc;
	uls_istream_ptr_t istr = (uls_istream_ptr_t) inp->data;

	uls_destroy_istream(istr);
	_uls_tool_(utf_destroy_inbuf)(inp);
}

ULS_QUALIFIED_RETTYP(uls_tmpl_pool_ptr_t)
ULS_QUALIFIED_METHOD(uls_import_tmpls)(uls_tmpl_list_ptr_t tmpl_list, uls_lex_ptr_t uls)
{
	uls_decl_array_slots_type10(tmpls1, tmpl);
	uls_tmpl_ptr_t tmpl1;

	uls_ref_array_type10(tmpls2, tmplvar);
	uls_tmplvar_ptr_t tmpl2;
	int n_tmpls;

	uls_tmpl_pool_ptr_t tmpls_pool;
	uls_rd_packet_ptr_t pkt;
	_uls_ptrtype_tool(csz_str) str_pool;

	char *str_ary;
	int i, m, k;
	const char* line;

	if (tmpl_list == nilptr) {
		tmpls1 = nilptr;
		n_tmpls = 0;
	} else {
		tmpls1 = uls_array_slots_type10(uls_ptr(tmpl_list->tmpls));
		n_tmpls = tmpl_list->tmpls.n;
	}

	tmpls_pool = uls_create_tmpl_pool(n_tmpls, 64);

	str_pool = uls_ptr(tmpls_pool->str_pool);
	_uls_tool(csz_add_eos)(str_pool);

	tmpls2 = uls_ptr(tmpls_pool->tmplvars);

	for (i=0; i<n_tmpls; i++) {
		tmpl1 = uls_get_array_slot(tmpls1, i);
		tmpl2 = uls_get_array_slot_type10(tmpls2, i);

		k = csz_length(str_pool);
		_uls_tool(csz_append)(str_pool, tmpl1->name, -1);
		_uls_tool(csz_add_eos)(str_pool);
		tmpl1->idx_name = k;

		if (tmpl1->sval != NULL) {
			k = csz_length(str_pool);
			_uls_tool(csz_append)(str_pool, tmpl1->sval, -1);
			_uls_tool(csz_add_eos)(str_pool);
			tmpl1->idx_sval = k;
		} else {
			tmpl1->idx_sval = 0;
		}

		if ((line=tmpl1->sval) == NULL) {
			tmpl2->n_pkt_ary = -1;
			continue;
		}

		if ((m = make_tokpkt_seqence(uls, line, tmpls_pool)) < 0) {
			uls_destroy_tmpl_pool(tmpls_pool);
			return nilptr;
		}

		tmpl2->n_pkt_ary = m;
	}

	str_ary = csz_data_ptr(uls_ptr(tmpls_pool->str_pool));

	for (i=0; i<tmpls_pool->pkt_ary.n; i++) {
		pkt = uls_get_array_slot_type10(uls_ptr(tmpls_pool->pkt_ary), i);
		k = (int) (uls_uint64) pkt->tokstr;
		pkt->tokstr = str_ary + k;
	}

	for (k=i=0; i<n_tmpls; i++) {
		tmpl2 = uls_get_array_slot_type10(tmpls2, i);
		tmpl1 = uls_get_array_slot(tmpls1, i);

		m = tmpl1->idx_name;
		tmpl2->name = str_ary + m;

		if ((m = tmpl1->idx_sval) > 0) {
			tmpl2->sval = str_ary + m;
		} else {
			tmpl2->sval = NULL;
		}

		if ((m = tmpl2->n_pkt_ary) < 0) {
			tmpl2->i0_pkt_ary = -1;
			continue;
		}

		tmpl2->i0_pkt_ary =  k;
		k += m;
	}

	return tmpls_pool;
}

int
ULS_QUALIFIED_METHOD(uls_bind_tmpls)(uls_istream_ptr_t istr, uls_tmpl_list_ptr_t tmpl_list)
{
	uls_lex_ptr_t  uls;
	uls_context_ptr_t ctx;
	uls_tmpl_pool_ptr_t tmpls_pool;

	if ((uls=istr->uls) == nilptr) {
		_uls_log(err_log)("No uls-object of uls_stream bound!");
		return -1;
	}

	ctx = uls->xcontext.context;

	if (istr->header.filetype == ULS_STREAM_RAW || tmpl_list->tmpls.n <= 0) {
		return 0;
	}

	// filetype == ULS_STREAM_BIN_BE, ULS_STREAM_BIN_LE
	if ((tmpls_pool = uls_import_tmpls(tmpl_list, uls)) == nilptr) {
		_uls_log(err_log)("%s: can't analyze the tmpls array!", __FUNCTION__);
		return -1;
	}

	ctx = uls->xcontext.context;
	uls_destroy_tmpl_pool(ctx->tmpls_pool);
	ctx->tmpls_pool = tmpls_pool;

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_fill_fd_stream)(uls_source_ptr_t isrc, char* buf, int buflen, int bufsiz)
{
	uls_istream_ptr_t istr = (uls_istream_ptr_t) isrc->usrc;
	int rc;

	rc = _uls_tool_(readn)(istr->fd, buf + buflen, bufsiz - buflen);
	if (rc < 0) {
		isrc->flags |= ULS_ISRC_FL_ERR;
	} else if (rc == 0) {
		isrc->flags |= ULS_ISRC_FL_EOF;
	}

	return rc;
}

void
ULS_QUALIFIED_METHOD(uls_ungrab_fd_stream)(uls_source_ptr_t isrc)
{
	uls_istream_ptr_t istr = (uls_istream_ptr_t) isrc->usrc;
	uls_destroy_istream(istr);
}

int
ULS_QUALIFIED_METHOD(uls_gettok_bin)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	const char     *lptr, *pckptr;
	int    tok_id, rc, txtlen;
	uls_uint32  *hdrbuf;
	uls_type_tool(outparam) parms;

 next_loop:
	lptr = ctx->lptr;

	if (lptr >= ctx->line_end) {
		if ((rc=uls_clear_and_fillbuff(uls)) < 0) {
			return 0;
		} else if (rc > 0) {
			goto next_loop; // normal case
		}

		uls->tokdef_vx = set_err_tok(uls, "NO-EOI");
		return 0;
	}

	pckptr = lptr;
	ctx->lptr += ULS_RDPKT_SIZE;

	hdrbuf = (uls_uint32 *) pckptr;
	tok_id = hdrbuf[0];
	txtlen = hdrbuf[1];

	pckptr += 2*sizeof(uls_uint32);
	lptr = *((char **) pckptr);

	if (tok_id == uls->xcontext.toknum_LINENUM) {
		parms.lptr = lptr;
		rc = _uls_tool_(skip_atox)(uls_ptr(parms));
		lptr = parms.lptr;

		if (*lptr == '\0') lptr = NULL;
		else ++lptr; // skip the ' '

		uls_context_set_tag(ctx, lptr, rc);
		goto next_loop;
	}

	if (tok_id == uls->xcontext.toknum_EOF) {
		make_eoif_lexeme_bin(ctx, tok_id, lptr, txtlen);
		uls->tokdef_vx = slots_rsv[EOF_TOK_IDX];
		return 1;
	}

	if (tok_id == uls->xcontext.toknum_EOI) {
		return 1;
	}

	ctx->flags |= ULS_CTX_FL_EXTERN_TOKBUF;

	ctx->tok = tok_id;
	ctx->s_val = lptr;
	ctx->s_val_len = txtlen;
	ctx->s_val_uchars = _uls_tool(ustr_num_chars)(ctx->s_val, txtlen, nilptr);

	uls->tokdef_vx = uls_find_tokdef_vx_force(uls, tok_id);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_readline_buffer)(char* buf, int bufsiz)
{
	int  i;

	for (i=0; i < bufsiz; i++) {
		if (buf[i] == '\n') {
			break;
		}
	}

	return i;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(parse_uls_hdr)(char* line, int fd_in, uls_istream_ptr_t istr)
{
	uls_type_tool(wrd) wrdx;
	char *wrd, filepath_buf[ULS_TEMP_FILEPATH_MAXSIZ + 5];

	int fsubtype, fpos, uld_n_lines, i_lines, len2;
	char *remap_buff, *bufptr2;
	int remap_size, remap_n_blocks = 0;
	FILE *fp_out;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (uls_streql(wrd, "FILE_VERSION:")) {
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

		if (_uls_tool_(version_pars_str)(wrd, uls_ptr(istr->header.filever)) < 0) {
			_uls_log(err_log)("Unsupported version: %s", wrd);
			return -1;
		}

	} else if (uls_streql(wrd, "SPEC:")) {
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
		uls_set_namebuf_value(istr->header.specname, wrd);

	} else if (uls_streql(wrd, "TYPE:")) {
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

		if (uls_streql(wrd, "BIN/LITTLE")) {
			fsubtype = ULS_STREAM_BIN_LE;
		} else if (uls_streql(wrd, "BIN/BIG")) {
			fsubtype = ULS_STREAM_BIN_BE;
		} else if (uls_streql(wrd, "TEXT/ASCII")) {
			fsubtype = ULS_STREAM_TXT;
		} else {
			_uls_log(err_log)("Unsupported file-type: %s", wrd);
			return -1;
		}

		istr->header.subtype = fsubtype;

	} else if (uls_streql(wrd, "CREATION_TIME:")) {
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
		uls_set_namebuf_value(istr->header.ctime, wrd);

	} else if (uls_streql(wrd, "TAG:")) {
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
		uls_set_namebuf_value(istr->header.subname, wrd);

	} else if (uls_streql(wrd, "TOKEN_REMAP:")) {
		// #-of-lines for uld
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
		if ((uld_n_lines = _uls_tool_(atoi)(wrd)) < 0) {
			_uls_log(err_log)("uld_n_lines: incorrect format!!");
			return -1;
		}

		// #-of-blocks: ULS_BIN_BLKSIZ
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
		if ((remap_n_blocks = _uls_tool_(atoi)(wrd)) < 0) {
			_uls_log(err_log)("remap_n_blocks: incorrect format!!");
			return -1;
		}

		fpos = (1 + remap_n_blocks) << ULS_BIN_BLKSIZ_LOG2;
		istr->start_off = fpos;

		remap_size = remap_n_blocks << ULS_BIN_BLKSIZ_LOG2;
		remap_buff = (char *) uls_malloc_buffer(remap_size + 1);
		if (uls_fd_read(fd_in, remap_buff, remap_size) < remap_size) {
			return -1;
		}
		remap_buff[remap_size] = '\0';

		if ((fp_out = _uls_tool_(fopen_tempfile)(uls_ptr(istr->uld_file))) == NULL) {
			_uls_log(err_log)("Error to reading istream");
			uls_mfree(remap_buff);
			return -1;
		}

		_uls_log_(fprintf)(fp_out, "#@%s\n", uls_get_namebuf_value(istr->header.specname));
		bufptr2 = remap_buff;
		for (i_lines=0; i_lines < uld_n_lines; i_lines++) {
			len2 = uls_readline_buffer(bufptr2, remap_size);
			if (len2 == 2 && bufptr2[0] == '%' && bufptr2[1] == '%') {
				break;
			}
			bufptr2[len2] = '\0';

			_uls_log_(fprintf)(fp_out, "%s\n", bufptr2);
			 bufptr2 += len2 + 1;
		}

		uls_mfree(remap_buff);

		if (istr->uls == nilptr) {
			_uls_log_(snprintf)(filepath_buf, ULS_TEMP_FILEPATH_MAXSIZ,
				"%s.uld", uls_get_namebuf_value(istr->uld_file.filepath));
			_uls_tool_(close_tempfile)(uls_ptr(istr->uld_file), filepath_buf);

//			err_log("Reading uld-file for '%s'...", filepath_buf);
			istr->uls = uls_create(filepath_buf);

			if (_uls_tool_(unlink)(filepath_buf) < 0) {
				_uls_log(err_log)("failed to delete temporary uld-file '%s'", filepath_buf);
			}
		}

	} else {
		_uls_log(err_log)("%s: unknown attribute in uls-header.", wrd);
		return -1;
	}

	return 0;
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream)(int fd)
{
	char linebuff[ULS_LINEBUFF_SIZ__ULS+1], *lptr;
	const char *magic_code = "#34183847-D64D-C131-D754-577215664901-ULS-STREAM\n";
	const char *spec_name;
	int  magic_code_len = _uls_tool_(strlen)(magic_code);
	int  len, fpos;
	uls_istream_ptr_t istr;
	uls_type_tool(outparam) parms;

	char ulshdr[ULS_BIN_HDR_SZ + 1], *bufptr;
	int bufsiz;

	if (fd < 0) {
		_uls_log(err_log)("%s: invalid parameter!", __FUNCTION__);
		return nilptr;
	}

	if ((istr = __create_istream(fd)) == nilptr) {
		_uls_log(err_log)("%s: malloc error", __FUNCTION__);
		return nilptr;
	}

	linebuff[magic_code_len] = '\0';

	if ((len=uls_fd_read(fd, linebuff, magic_code_len)) < 0) { // fd may be negative
		_uls_log(err_log)("I/O Error: readline");
		__destroy_istream(istr);
		return nilptr;

	} else if (len < magic_code_len || !uls_streql(linebuff, magic_code)) { // including EOF(len==0)
		_uls_tool_(memcopy)(istr->firstline, linebuff, len);
		istr->firstline[len] = '\0';
		istr->len_firstline = len;

		// in order to compare it with the UTF-BOM
		fpos = get_rawfile_subtype(istr->firstline, istr->len_firstline, uls_ptr(parms));
		istr->header.subtype = parms.n1;
		istr->header.reverse = parms.n2;

		if (uls_fd_seek(istr->fd, fpos, SEEK_SET) == fpos) {
			// Invalidate the firstline[] read in the file.
			istr->firstline[0] = '\0';
			istr->len_firstline = 0;
		}
		return istr;
	}

	bufptr = ulshdr + magic_code_len;
	bufsiz = ULS_BIN_HDR_SZ - magic_code_len;

	if (uls_fd_read(fd, bufptr, bufsiz) < bufsiz) {
		__destroy_istream(istr);
		return nilptr;
	}
	ulshdr[ULS_BIN_HDR_SZ] = '\0';

	istr->header.filetype = ULS_STREAM_ULS;
	istr->header.subtype = -1;
	istr->header.reverse = 0;

	istr->start_off = ULS_BIN_BLKSIZ;

	for ( ; ; bufptr += len + 1, bufsiz -= len + 1) {
		len = uls_readline_buffer(bufptr, bufsiz);
		if (len == 2 && bufptr[0] == '%' && bufptr[1] == '%') {
			break;
		}
		bufptr[len] = '\0';

		if ((len > 0 && bufptr[0] == '#') || *(lptr = _uls_tool(skip_blanks)(bufptr)) == '\0') {
			continue;
		}

		if (parse_uls_hdr(lptr, fd, istr) < 0) {
			__destroy_istream(istr);
			istr = nilptr;
			break;
		}
	}

	if (istr->uls == nilptr) {
		spec_name = uls_get_namebuf_value(istr->header.specname);
//		err_log("Searching for '%s'...", spec_name);
		istr->uls = uls_create(spec_name);
	}

	return istr;
}

void
ULS_QUALIFIED_METHOD(uls_set_istream_tag)(uls_istream_ptr_t istr, const char* tag)
{
	if (tag != NULL) {
		uls_set_namebuf_value(istr->filepath, tag);
	}
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream_file)(const char* fpath)
{
	uls_istream_ptr_t istr;
	int fd;

	if (fpath == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __FUNCTION__);
		return nilptr;
	}

	if ((fd = _uls_tool_(fd_open)(fpath, ULS_FIO_READ)) < 0) {
		_uls_log(err_log)("%s: can't open '%s'!", __FUNCTION__, fpath);
		return nilptr;
	}

	if ((istr = uls_open_istream(fd)) == nilptr) {
		_uls_log(err_log)("%s: can't conjecture the type of file %s!", __FUNCTION__, fpath);
		_uls_tool_(fd_close)(fd);
		return nilptr;
	}

	istr->flags |= ULS_STREAM_FDCLOSE | ULS_STREAM_REWINDABLE;
	uls_set_namebuf_value(istr->filepath, fpath);

	return istr;
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream_fp)(FILE *fp)
{
	uls_istream_ptr_t istr;
	int fd;

	if (fp == NULL || fp == __uls_tool_(stdio_fp)(1) || fp == __uls_tool_(stdio_fp)(2)) {
		_uls_log(err_log)("%s: invalid parameter!", __FUNCTION__);
		return nilptr;
	}

	fd = uls_fp_fileno(fp);

	if ((istr = uls_open_istream(fd)) == nilptr) {
		_uls_log(err_log)("%s: can't conjecture the type of file!", __FUNCTION__);
		return nilptr;
	}

	istr->flags |= ULS_STREAM_REWINDABLE;

	return istr;
}

#ifdef ULS_FDF_SUPPORT
ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream_filter)(fdf_t* fdf, int fd)
{
	uls_istream_ptr_t istr;

	if (fdf == nilptr || fd < 0 || (fd = fdf_open(fdf, fd)) < 0)
		return nilptr;

	if ((istr = uls_open_istream(fd)) == nilptr) {
		fdf_close(fdf);
		return nilptr;
	}

	istr->fdf = fdf;

	// set istr->filepath after calling this func
	return istr;
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream_filter_file)(fdf_t* fdf, const char* fpath)
{
	uls_istream_ptr_t istr;
	int  fd;

	if ((fd = _uls_tool_(fd_open)(fpath, ULS_FIO_READ)) < 0) return nilptr;

	if ((istr=uls_open_istream_filter(fdf, fd)) == nilptr) {
		_uls_tool_(fd_close)(fd);
		return nilptr;
	}

	istr->flags |= ULS_STREAM_FDCLOSE | ULS_STREAM_REWINDABLE;
	uls_set_namebuf_value(istr->filepath, fpath);

	return istr;
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_istream_filter_fp)(fdf_t* fdf, FILE *fp)
{
	uls_istream_ptr_t istr;
	int  fd;

	if (fp == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __FUNCTION__);
		return nilptr;
	}

	fd = uls_fp_fileno(fp);

	if ((istr=uls_open_istream_filter(fdf, fd)) == nilptr) {
		_uls_tool_(fd_close)(fd);
		return nilptr;
	}

	istr->flags |= ULS_STREAM_REWINDABLE;
	return istr;
}

#endif

int
ULS_QUALIFIED_METHOD(uls_rewind_istream)(uls_istream_ptr_t istr)
{
	int fpos = istr->start_off;

	if ((istr->flags & ULS_STREAM_REWINDABLE) == 0) {
		return -1;
	}

	if (uls_fd_seek(istr->fd, fpos, SEEK_SET) != fpos) {
		return -1;
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_istream)(uls_istream_ptr_t istr)
{
	if (istr == nilptr || istr->ref_cnt <= 0) {
		_uls_log(err_log)("%s: called for invalid object!", __FUNCTION__);
		if (istr != nilptr) {
			_uls_log(err_log)("%s: ref_cnt(istr)=%d", __FUNCTION__, istr->ref_cnt);
		}
		return;
	}

	if (--istr->ref_cnt > 0) return;

	if (istr->uls != nilptr) {
		uls_ungrab(istr->uls);
		istr->uls = nilptr;
	}

	if (istr->flags & ULS_STREAM_FDCLOSE) {
		_uls_tool_(fd_close)(istr->fd);
		istr->flags &= ~ULS_STREAM_FDCLOSE;
	}

	istr->fd = -1;
#ifdef ULS_FDF_SUPPORT
	if (istr->fdf != nilptr) {
		if (fdf_close(istr->fdf) < 0)
			_uls_log(err_log)("%s: fail to reap filter procs.", __FUNCTION__);
		istr->fdf = nilptr;
	}
#endif

	__destroy_istream(istr);
}

int
ULS_QUALIFIED_METHOD(uls_bind_istream)(uls_istream_ptr_t istr, uls_lex_ptr_t uls)
{
	if (istr->uls == uls) {
		return 0;
	}

	if (uls != nilptr) {
		if (!check_istr_compatibility(istr, uls)) {
			return -1;
		}
		uls_grab(uls);
	}

	if (istr->uls != nilptr) {
		uls_ungrab(istr->uls);
	}

	istr->uls = uls;
	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_read_tok)(uls_istream_ptr_t istr, uls_ptrtype_tool(outparam) parms)
{
	uls_lex_ptr_t uls;
	int t;

	if ((uls=istr->uls) == nilptr) {
		_uls_log(err_log)("No uls-object of uls_stream bound!");
		return -1;
	}

	t = uls_get_tok(uls);

	if (parms != nilptr) {
		parms->lptr = __uls_lexeme(uls);
		parms->n = t;
	}

	return __uls_lexeme_len(uls);
}

int
ULS_QUALIFIED_METHOD(_uls_get_raw_input_subtype)(FILE* fp)
{
	char linebuff[8];
	int rc, subtype;
	uls_type_tool(outparam) parms;

	rc = (int) fread(linebuff, sizeof(char), sizeof(linebuff)-1, fp);
	linebuff[rc] = '\0';

	get_rawfile_subtype(linebuff, rc, uls_ptr(parms));
	subtype = parms.n1;
	rewind(fp);

	return subtype;
}

int
ULS_QUALIFIED_METHOD(_uls_const_TMPLS_DUP)(void)
{
	return ULS_TMPLS_DUP;
}

ULS_QUALIFIED_RETTYP(uls_istream_ptr_t)
ULS_QUALIFIED_METHOD(ulsjava_open_istream_file)(const void *filepath, int len_filepath)
{
	const char *ustr = _uls_tool_(strdup)((const char *)filepath, len_filepath);
	uls_istream_ptr_t istr;

	istr = uls_open_istream_file(ustr);
	uls_mfree(ustr);

	return istr;
}
