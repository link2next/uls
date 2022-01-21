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
 * uls_ostream.c -- the routines that make lexical streams ... --
 *     written by Stanley Hong <link2next@gmail.com>, Aug 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_OSTREAM__
#include "uls/uls_ostream.h"
#include "uls/uls_lex.h"
#include "uls/uld_conf.h"
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_make_pkt__null)(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) outbuf)
{
	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__make_pkt_to_binstr)(uls_wr_packet_ptr_t pkt, char* binstr, int reclen, int txtlen)
{
	uls_int32 hdrbuf[ULS_BIN_RECHDR_NUM_INT32];
	char *outptr = binstr;

	hdrbuf[0] = pkt->pkt.tok_id;
	hdrbuf[1] = txtlen;

	outptr = (char *) _uls_tool_(memcopy)(outptr, hdrbuf, ULS_BIN_RECHDR_SZ);

	if (pkt->pkt.tokstr != NULL) {
		outptr = (char *) _uls_tool_(memcopy)(outptr, pkt->pkt.tokstr, txtlen);
	} else {
		outptr += txtlen;
	}

	*outptr = '\0';

	pkt->reorder_bytes(binstr, reclen);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_make_pkt__bin)(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst)
{
	int  txtlen, reclen;
	char *outptr0;

	txtlen = pkt->pkt.len_tokstr;
	reclen = ULS_BIN_REC_SZ(txtlen);

	outptr0 = _uls_tool(csz_append)(ss_dst, NULL, reclen);
	_uls_tool_(bzero)(outptr0, reclen);
	__make_pkt_to_binstr(pkt, outptr0, reclen, txtlen);

	return reclen;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_make_pkt__bin_lno)(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst)
{
	int  len_lno_buf, txtlen, reclen;
	char  *hdrptr, *outptr;
	char lno_buf[N_MAX_DIGITS_INT+1];

	len_lno_buf = _uls_log_(snprintf)(lno_buf, sizeof(lno_buf), "%X", pkt->lineno);

	if (pkt->pkt.len_tokstr > 0) {
		txtlen = len_lno_buf + 1 + pkt->pkt.len_tokstr; // +1 == the separator ' '
	} else {
		txtlen = len_lno_buf;
	}
	reclen = ULS_BIN_REC_SZ(txtlen);

	hdrptr = _uls_tool(csz_append)(ss_dst, NULL, reclen);
	_uls_tool_(bzero)(hdrptr, reclen);
	outptr = hdrptr + ULS_BIN_RECHDR_SZ;

	// lineno
	outptr = (char *) _uls_tool_(memcopy)(outptr, lno_buf, len_lno_buf);

	// tagstr
	if (pkt->pkt.tokstr != NULL && pkt->pkt.len_tokstr > 0) {
		*outptr++ = ' ';
		_uls_tool_(memcopy)(outptr, pkt->pkt.tokstr, pkt->pkt.len_tokstr);
	}

	pkt->pkt.tokstr = NULL;
	pkt->pkt.len_tokstr = txtlen;
	__make_pkt_to_binstr(pkt, hdrptr, reclen, txtlen);

	return reclen;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_make_pkt__txt)(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst)
{
	char hdrbuf_txt[ULS_TXT_RECHDR_SZ];
	int  len, k0;

	k0 = csz_length(ss_dst);

	len = _uls_log_(snprintf)(hdrbuf_txt, sizeof(hdrbuf_txt), "%5d %5d",
		pkt->pkt.tok_id, pkt->pkt.len_tokstr);
	_uls_tool(csz_append)(ss_dst, hdrbuf_txt, len);

	_uls_tool(csz_putc)(ss_dst, ' ');
	if (pkt->pkt.tokstr != NULL)
		_uls_tool(csz_append)(ss_dst, pkt->pkt.tokstr, pkt->pkt.len_tokstr);

	_uls_tool(csz_putc)(ss_dst, '\n');
	return csz_length(ss_dst) - k0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_make_pkt__txt_lno)(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst)
{
	char hdrbuf_txt[ULS_TXT_RECHDR_SZ];
	char lno_buf[N_MAX_DIGITS_INT+1];
	int  len_lno_buf;
	int  txtlen, len, k0;

	k0 = csz_length(ss_dst);

	len_lno_buf = _uls_log_(snprintf)(lno_buf, sizeof(lno_buf), "%X", pkt->lineno);

	if (pkt->pkt.tokstr != NULL) {
		txtlen = len_lno_buf + 1 + pkt->pkt.len_tokstr;
		// +1 == the separator ' '
	} else {
		txtlen = len_lno_buf;
	}

	len = _uls_log_(snprintf)(hdrbuf_txt, sizeof(hdrbuf_txt), "%5d %5d",
		pkt->pkt.tok_id, txtlen);
	_uls_tool(csz_append)(ss_dst, hdrbuf_txt, len);

	_uls_tool(csz_putc)(ss_dst, ' ');
	_uls_tool(csz_append)(ss_dst, lno_buf, len_lno_buf);

	if (pkt->pkt.tokstr != NULL) {
		_uls_tool(csz_putc)(ss_dst, ' ');
		// ' ' == the separator
		_uls_tool(csz_append)(ss_dst, pkt->pkt.tokstr, pkt->pkt.len_tokstr);
	}
	_uls_tool(csz_putc)(ss_dst, '\n');

	return csz_length(ss_dst) - k0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__init_ostream)(uls_ostream_ptr_t ostr) {
	uls_init_stream_header(uls_ptr(ostr->header));

	ostr->fd = -1;
	uls_init_wr_packet(uls_ptr(ostr->pktbuf));

	_uls_tool(csz_init)(uls_ptr(ostr->out_fd_csz), 1024);
	ostr->make_packet_token = uls_ref_callback_this(uls_make_pkt__null);
	ostr->make_packet_lineno = uls_ref_callback_this(uls_make_pkt__null);
	ostr->rearrange_packet_bytes = uls_ref_callback_this(uls_reorder_bytes_null);
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__deinit_ostream)(uls_ostream_ptr_t ostr)
{
	_uls_tool(csz_deinit)(uls_ptr(ostr->out_fd_csz));
	uls_deinit_wr_packet(uls_ptr(ostr->pktbuf));
	uls_deinit_stream_header(uls_ptr(ostr->header));
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_ostream_ptr_t)
ULS_QUALIFIED_METHOD(__create_ostream)(int fd)
{
	uls_ostream_ptr_t ostr;

	if (fd < 0) return nilptr;

	ostr = uls_alloc_object(uls_ostream_t);
	uls_initial_zerofy_object(ostr);
	__init_ostream(ostr);

	ostr->fd = fd;
	_uls_tool_(version_make)(uls_ptr(ostr->header.filever),
		ULS_VERSION_STREAM_MAJOR, ULS_VERSION_STREAM_MINOR,
		ULS_VERSION_STREAM_DEBUG);

	ostr->header.filetype = ULS_STREAM_ULS;
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_BIG_ENDIAN) {
		ostr->header.subtype = ULS_STREAM_BIN_BE;
	} else { // ULS_LITTLE_ENDIAN
		ostr->header.subtype = ULS_STREAM_BIN_LE;
	}

	ostr->ref_cnt = 1;

	return ostr;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__destroy_ostream)(uls_ostream_ptr_t ostr)
{
	ostr->ref_cnt = 0;
	ostr->fd = -1;
	__deinit_ostream(ostr);
	uls_dealloc_object(ostr);
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__bind_ostream_callbacks)(uls_ostream_ptr_t ostr, int stream_type)
{
	if (stream_type == ULS_STREAM_BIN_LE) {
		ostr->make_packet_token = uls_ref_callback_this(uls_make_pkt__bin);
		ostr->make_packet_lineno = uls_ref_callback_this(uls_make_pkt__bin_lno);
		ostr->rearrange_packet_bytes = uls_ref_callback_this(uls_reorder_bytes_binle);

	} else if (stream_type == ULS_STREAM_BIN_BE) {
		ostr->make_packet_token = uls_ref_callback_this(uls_make_pkt__bin);
		ostr->make_packet_lineno = uls_ref_callback_this(uls_make_pkt__bin_lno);
		ostr->rearrange_packet_bytes = uls_ref_callback_this(uls_reorder_bytes_binbe);

	} else if (stream_type == ULS_STREAM_TXT) {
		ostr->make_packet_token = uls_ref_callback_this(uls_make_pkt__txt);
		ostr->make_packet_lineno = uls_ref_callback_this(uls_make_pkt__txt_lno);
		ostr->rearrange_packet_bytes = uls_ref_callback_this(uls_reorder_bytes_null);

	} else {
		ostr->make_packet_token = uls_ref_callback_this(uls_make_pkt__null);
		ostr->make_packet_lineno = uls_ref_callback_this(uls_make_pkt__null);
		ostr->rearrange_packet_bytes = uls_ref_callback_this(uls_reorder_bytes_null);
	}

	ostr->pktbuf.reorder_bytes = ostr->rearrange_packet_bytes;
	_uls_tool(csz_reset)(uls_ptr(ostr->out_fd_csz));
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(writeline_istr_hdr)(char *buf, int bufsiz, int k, const char *line, int linelen)
{
	int k2;

	if (linelen < 0) {
		linelen = _uls_tool_(strlen)(line);
	}

	k2 = k + linelen + 1; // +1 for '\n'
	if (k2 > bufsiz) {
		_uls_log(err_log)("%s: uls-header size overflow!", __FUNCTION__);
		return -1;
	}

	_uls_tool_(memcopy)(buf + k, line, linelen);
	k += linelen;
	buf[k] = '\n';

	return k2;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(fill_uls_redundant_lines)(char *buff, int buflen, int len1, const char *mesg2)
{
	int i, k, n_bytes, n_lines, n;
	int n_bytes_per_line = 63;
	const char *mesg1 = "RECOMMENDED NOT TO EDIT!";

	k = buflen;
	buff[k++] = '\n';

	n_bytes = len1 - k;
	n = n_bytes_per_line + 1;
	n_lines = n_bytes / n;
	n_bytes %= n;

	n = n_bytes_per_line - 1;
	for (i=0; i<n_lines; i++) {
		buff[k++] = '#';
		_uls_tool_(memset)(buff + k, '_', n);
		if (i == 0) {
			_uls_tool_(memcopy)(buff + k + 16, mesg1, _uls_tool_(strlen)(mesg1));
		} else if (i == 1 && mesg2 != NULL) {
			_uls_tool_(memcopy)(buff + k + 12, mesg2, _uls_tool_(strlen)(mesg2));
		}
		k += n;
		buff[k++] = '\n';
	}

	if (n_bytes > 0) {
		n = n_bytes - 1;
		_uls_tool_(memset)(buff + k, ' ', n);
		k += n;
		buff[k++] = '\n';
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(write_uld_to_ostream)(uls_ref_array_type10(lst_names,nam_tok),
	uls_ptrtype_tool(outparam) parms, int fd_out)
{
	int n_lst_names = lst_names->n;
	char *remap_buf = parms->line;
	int k, len, len1, remap_bufsiz = parms->n1 << ULS_BIN_BLKSIZ_LOG2;

	char linebuff[ULS_LINEBUFF_SIZ__ULD + 1];
	int i, fpos, stat=0;
	uls_nam_tok_ptr_t e;

	for (i=k=0; i<n_lst_names; i++) {
		e = uls_get_array_slot_type10(lst_names, i);

		len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "\t%-16s %d", e->name, e->tok_id);
		if ((k = writeline_istr_hdr(remap_buf, remap_bufsiz, k, linebuff, len)) < 0) {
			stat = -1;
			break;
		}
	}

	parms->len = k;
	len = uls_ceil_log2(k + 3, ULS_BIN_BLKSIZ_LOG2);
	parms->n2 = len >> ULS_BIN_BLKSIZ_LOG2;
	len1 = len - 3;

	if (k < len1) {
		fill_uls_redundant_lines(remap_buf, k, len1, NULL);
	}

	remap_buf[len1] = remap_buf[len1 + 1] = '%';
	remap_buf[len1 + 2] = '\n';

	fpos = ULS_BIN_BLKSIZ;
	if (uls_fd_seek(fd_out, fpos, SEEK_SET) != fpos) {
		return -2;
	}

	if (_uls_tool_(writen)(fd_out, remap_buf, len) < len) {
		stat = -3;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(format_uls_hdrbuf)(char *ulshdr)
{
	const char* magic_code = "#34183847-D64D-C131-D754-577215664901-ULS-STREAM\n";
	int magic_code_len = _uls_tool_(strlen)(magic_code);

	_uls_tool_(memcopy)(ulshdr, magic_code, magic_code_len);

	return magic_code_len;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(do_end_of_uls_hdr)(char *buff, int buflen)
{
	int  len1 = ULS_BIN_HDR_SZ - 3;
	const char *mesg2 = "FIXED HEADER SIZE UPTO 512-bytes";

	if (buflen > len1) {
		_uls_log(err_log)("%s: uls-header size overflow!", __FUNCTION__);
		return -1;
	}

	if (buflen < len1) {
		fill_uls_redundant_lines(buff, buflen, len1, mesg2);
	}

	buff[len1] = buff[len1+1] = '%';
	buff[len1+2] = '\n';

	return 0;
}

int
ULS_QUALIFIED_METHOD(save_istr_hdrbuf)(char *ulshdr, int buflen, int fd)
{
	int stat = 0;

	if (do_end_of_uls_hdr(ulshdr, buflen) < 0) {
		_uls_log(err_log)("%s: failed to format the header", __FUNCTION__);
		stat = -1;

	} else if (uls_fd_seek(fd, 0, SEEK_SET) != 0) {
		_uls_log(err_log)("%s: error to seek file", __FUNCTION__);
		stat = -1;

	} if (uls_fd_write(fd, ulshdr, ULS_BIN_HDR_SZ) < ULS_BIN_HDR_SZ) {
		_uls_log(err_log)("%s: error to write file", __FUNCTION__);
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(write_ostream_header)(uls_ostream_ptr_t ostr,
	uls_ref_array_type10(lst_names,nam_tok))
{
	char verstr[ULS_VERSION_STR_MAXLEN+1];
	char *mode_str, time_buf[ULS_STREAM_CTIME_SIZE+1];
	int  k, subtype;
	uls_type_tool(version) stream_filever;
	uls_type_tool(outparam) parms1;

	char ulshdr[ULS_BIN_HDR_SZ];
	char linebuff[ULS_LINEBUFF_SIZ__ULS+1];
	char *remap_buff;
	int  remap_size;
	int len, rc, n_blocks;
	int fpos;

	if ((subtype = ostr->header.subtype) == ULS_STREAM_BIN_LE) { // LITTLE-ENDIAN
		mode_str = "BIN/LITTLE";
	} else if (subtype == ULS_STREAM_BIN_BE) { // BIG-ENDIAN
		mode_str = "BIN/BIG";
	} else if (subtype == ULS_STREAM_TXT) {
		mode_str = "TEXT/ASCII";
	} else {
		_uls_log(err_panic)("unknown target streaming format for output!");
		mode_str = NULL;
	}

	_uls_tool_(version_make)(uls_ptr(stream_filever),
		ULS_VERSION_STREAM_MAJOR, ULS_VERSION_STREAM_MINOR,
		ULS_VERSION_STREAM_DEBUG);
	_uls_tool_(version_make_string)(uls_ptr(stream_filever), verstr);

	k = format_uls_hdrbuf(ulshdr);

	len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "FILE_VERSION: %s", verstr);
	if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
		return -1;
	}

	len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "SPEC: %s", uls_get_namebuf_value(ostr->header.specname));
	if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
		return -1;
	}

	len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "TYPE: %s", mode_str);
	if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
		return -1;
	}

	_uls_tool_(get_current_time_yyyymmdd_hhmm)(time_buf, sizeof(time_buf));
	len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "CREATION_TIME: %s", time_buf);
	if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
		return -1;
	}

	len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "TAG: %s", uls_get_namebuf_value(ostr->header.subname));
	if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
		return -1;
	}

	if (lst_names->n > 0) {
		remap_size = lst_names->n * (ULS_LINEBUFF_SIZ__ULD + 1) + 3; // +3 for '%' '%' '\n'
		remap_size = uls_ceil_log2(remap_size, ULS_BIN_BLKSIZ_LOG2);
		remap_buff = (char *) uls_malloc_buffer(remap_size);

		parms1.line = remap_buff;
		parms1.n1 = remap_size >> ULS_BIN_BLKSIZ_LOG2;

		rc = write_uld_to_ostream(lst_names, uls_ptr(parms1), ostr->fd);
		uls_mfree(remap_buff);
		if (rc < 0) {
			return -1;
		}

		n_blocks = parms1.n2;

		len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "TOKEN_REMAP: %d %d", lst_names->n, n_blocks);
		if ((k = writeline_istr_hdr(ulshdr, ULS_BIN_HDR_SZ, k, linebuff, len)) < 0) {
			return -1;
		}

	} else {
		n_blocks = 0;
	}

	if (save_istr_hdrbuf(ulshdr, k, ostr->fd) < 0) {
		return -2;
	}

	fpos = (1 + n_blocks) << ULS_BIN_BLKSIZ_LOG2;
	if (uls_fd_seek(ostr->fd, fpos, SEEK_SET) != fpos) {
		return -3;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_bind_ostream)
	(uls_ostream_ptr_t ostr, const char *specname, uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	if (uls != nilptr) {
		uls_set_namebuf_value(ostr->header.specname, uls_get_namebuf_value(uls->ulc_name));
		if (uld_export_extra_names(uls, parms) < 0) {
			return -1;
		}
		// nam_toks = parms.data;
		// parms->n == n_nam_toks
		uls_grab(uls);
		ostr->uls = uls;

	} else {
		parms->data = nilptr;
		parms->n = 0;
		if (specname == NULL) specname = "<unknown>";
		uls_set_namebuf_value(ostr->header.specname, specname);
	}

	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_unbind_ostream)(uls_ostream_ptr_t ostr)
{
	uls_lex_ptr_t uls;

	if ((uls = (uls_lex_ptr_t ) ostr->uls) != nilptr) {
		uls_ungrab(uls);
		ostr->uls = nilptr;
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__flush_uls_stream_buffer)(_uls_ptrtype_tool(csz_str) outbuf, int fd, int force)
{
	int k;

	if ((k=csz_length(outbuf)) > 1024 || force) {
		if (_uls_tool_(writen)(fd, csz_data_ptr(outbuf), k) < k) {
			_uls_log(err_log)("I/O error");
			return -1;
		}

		_uls_tool(csz_reset)(outbuf);
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_finalize_ostream)(uls_ostream_ptr_t ostr)
{
	_uls_ptrtype_tool(csz_str) outbuf = uls_ptr(ostr->out_fd_csz);
	uls_lex_ptr_t uls;

	if ((uls = (uls_lex_ptr_t ) ostr->uls) != nilptr) {
		ostr->pktbuf.pkt.tok_id = uls->xcontext.toknum_EOI;
		ostr->pktbuf.pkt.tokstr = "";
		ostr->pktbuf.pkt.len_tokstr = 0;
		ostr->pktbuf.lineno = -1;

		ostr->make_packet_token(uls_ptr(ostr->pktbuf), outbuf);
	}

	if (__flush_uls_stream_buffer(outbuf, ostr->fd, 1) < 0) {
		_uls_log(err_log)("I/O error");
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_make_packet_linenum)
	(uls_ostream_ptr_t ostr, int lno, const char* tag, int tag_len)
{
	_uls_ptrtype_tool(csz_str) outbuf = uls_ptr(ostr->out_fd_csz);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) ostr->uls;

	uls_assert(uls != nilptr);

	// 'tag' may be NULL
	if (tag != NULL) {
		if (tag_len < 0) tag_len = _uls_tool_(strlen)(tag);
	} else {
		tag_len = 0;
	}

	ostr->pktbuf.pkt.tok_id = uls->xcontext.toknum_LINENUM;
	ostr->pktbuf.pkt.tokstr = tag; // 'tag' may be NULL
	ostr->pktbuf.pkt.len_tokstr = tag_len; // don't care in case that 'tag' is NULL.
	ostr->pktbuf.lineno = lno;

	return ostr->make_packet_lineno(uls_ptr(ostr->pktbuf), outbuf);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_make_packet)
	(uls_ostream_ptr_t ostr, int tokid, const char* tokstr, int l_tokstr)
{
	_uls_ptrtype_tool(csz_str) outbuf = uls_ptr(ostr->out_fd_csz);
	int k0 = csz_length(outbuf);

	ostr->pktbuf.pkt.tok_id = tokid;
	ostr->pktbuf.lineno = -1;
	ostr->pktbuf.pkt.tokstr = tokstr;
	ostr->pktbuf.pkt.len_tokstr = l_tokstr;
	ostr->make_packet_token(uls_ptr(ostr->pktbuf), outbuf);

	return csz_length(outbuf) - k0;
}

void
ULS_QUALIFIED_METHOD(uls_init_wr_packet)(uls_wr_packet_ptr_t pkt)
{
	pkt->reorder_bytes = uls_ref_callback_this(uls_reorder_bytes_null);
	pkt->pkt.tok_id = 0;
	pkt->pkt.tokstr = "";
	pkt->pkt.len_tokstr = 0;
	pkt->lineno = -1;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_wr_packet)(uls_wr_packet_ptr_t pkt)
{
}

int
ULS_QUALIFIED_METHOD(uls_print_tok_eof)(uls_ostream_ptr_t ostr)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) ostr->uls;
	int tokbuf_len, rc;
	uls_type_tool(outbuf) tokbuf;

	_uls_tool(str_init)(uls_ptr(tokbuf), 64);

	tokbuf_len = __uls_make_eoftok_lexeme(uls_ptr(tokbuf), __uls_get_lineno(uls), __uls_get_tag(uls));
	rc = __uls_print_tok(ostr, uls_toknum_eof(uls), tokbuf.buf, tokbuf_len);

	_uls_tool(str_free)(uls_ptr(tokbuf));

	return rc;
}

int
ULS_QUALIFIED_METHOD(uls_print_tok_eoi)(uls_ostream_ptr_t ostr)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) ostr->uls;
	return uls_print_tok(ostr, uls_toknum_eoi(uls), NULL);
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_ostream_ptr_t)
ULS_QUALIFIED_METHOD(__uls_create_ostream)
	(int fd_out, uls_lex_ptr_t uls, int stream_type, const char* subname)
{
	uls_ostream_ptr_t ostr;
	uls_ref_array_type10(lst_names, nam_tok);
	uls_type_tool(outparam) parms;
	int  rc;

	if (uls == nilptr || fd_out < 0) {
		_uls_log(err_log)("%s: Invalid parameter 'uls_lex_ptr_t' or 'fd'!");
		return nilptr;
	}

	if ((ostr = __create_ostream(fd_out)) == nilptr) {
		_uls_log(err_log)("%s: malloc error", __FUNCTION__);
		return nilptr;
	}

	__bind_ostream_callbacks(ostr, stream_type);

	if (stream_type == ULS_STREAM_BIN_LE) { // LITTLE-ENDIAN
	} else if (stream_type == ULS_STREAM_BIN_BE) { // BIG-ENDIAN
	} else if (stream_type == ULS_STREAM_TXT) {
	} else {
		_uls_log(err_log)("unknown target streaming format!");
		__destroy_ostream(ostr);
		return nilptr;
	}

	ostr->header.filetype = ULS_STREAM_ULS;
	ostr->header.subtype = stream_type;

	if (subname != NULL) {
		uls_set_namebuf_value(ostr->header.subname, subname);
	}

	if (__uls_bind_ostream(ostr, nilptr, uls, uls_ptr(parms)) < 0) {
		_uls_log(err_log)("%s: binding error!", __FUNCTION__);
		__destroy_ostream(ostr);
		return nilptr;
	}

	lst_names = uls_cast_array_type10(nam_tok) parms.data;
	rc = write_ostream_header(ostr, lst_names);
	uld_unexport_extra_names(lst_names);

	if (rc < 0) {
		_uls_log(err_log)("%s: can't bind!", __FUNCTION__);
 		__uls_unbind_ostream(ostr);
 		__destroy_ostream(ostr);
 		return nilptr;
 	}

	return ostr;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_ostream_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_ostream)(int fd_out, uls_lex_ptr_t uls, const char* subname)
{
	int stream_type;

	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_BIG_ENDIAN) {
		stream_type = ULS_STREAM_BIN_BE;
	} else {
		stream_type = ULS_STREAM_BIN_LE;
	}

	return __uls_create_ostream(fd_out, uls, stream_type, subname);
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_ostream_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_ostream_file)(const char* filepath, uls_lex_ptr_t uls, const char* subname)
{
	uls_ostream_ptr_t ostr;
	int fd;

	if ((fd = _uls_tool_(fd_open)(filepath, ULS_FIO_CREAT|ULS_FIO_WRITE)) < 0) {
		_uls_log(err_log)("can't create file '%s'!", filepath);
		return nilptr;
	}

	if ((ostr = uls_create_ostream(fd, uls, subname)) == nilptr) {
		_uls_log(err_log)("fail to create out-stream for '%s'.", filepath);
		_uls_tool_(fd_close)(fd);
		return nilptr;
	}

	ostr->flags |= ULS_STREAM_FDCLOSE;
	return ostr;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_destroy_ostream)(uls_ostream_ptr_t ostr)
{
	if (ostr == nilptr || ostr->ref_cnt <= 0) {
		_uls_log(err_log)("%s: called for invalid object! ref-cnt invalid", __FUNCTION__);
		return -1;
	}

	if (--ostr->ref_cnt > 0) return ostr->ref_cnt;

	if (uls_print_tok_eof(ostr) < 0 || uls_print_tok_eoi(ostr) < 0) {
		return -1;
	}

	if ((ostr->flags & ULS_STREAM_ERR) == 0)
		__uls_finalize_ostream(ostr);

	if (ostr->flags & ULS_STREAM_FDCLOSE) {
		_uls_tool_(fd_close)(ostr->fd);
		ostr->flags &= ~ULS_STREAM_FDCLOSE;
	}

	ostr->fd = -1;

	if (ostr->uls != nilptr) {
		uls_ungrab(ostr->uls);
		ostr->uls = nilptr;
	}

	__destroy_ostream(ostr);

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_print_tok)(uls_ostream_ptr_t ostr, int tokid, const char* tokstr, int l_tokstr)
{
	int rc;

	if ((rc = __uls_make_packet(ostr, tokid, tokstr, l_tokstr)) < 0) {
		return -1;
	}

	if (__flush_uls_stream_buffer(uls_ptr(ostr->out_fd_csz), ostr->fd, 0) < 0) {
		return -1;
	}

	return rc;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_print_tok)(uls_ostream_ptr_t ostr, int tokid, const char* tokstr)
{
	int l_tokstr;

	if (tokstr == NULL) {
		tokstr = "";
		l_tokstr = 0;
	} else {
		l_tokstr = _uls_tool_(strlen)(tokstr);
	}

	return __uls_print_tok(ostr, tokid, tokstr, l_tokstr);
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_print_tok_linenum)(uls_ostream_ptr_t ostr, int lno, const char* tag, int tag_len)
{
	int rc;

	if ((rc = __uls_make_packet_linenum(ostr, lno, tag, tag_len)) < 0) {
		return -1;
	}

	if (__flush_uls_stream_buffer(uls_ptr(ostr->out_fd_csz), ostr->fd, 0) < 0) {
		return -1;
	}

	return rc;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_print_tok_linenum)(uls_ostream_ptr_t ostr, int lno, const char* tag)
{
	int tag_len;

	if (tag == NULL) {
		tag_len = 0;
	} else {
		tag_len = _uls_tool_(strlen)(tag);
	}

	return __uls_print_tok_linenum(ostr, lno, tag, tag_len);
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_start_stream)(uls_ostream_ptr_t ostr, int flags)
{
	_uls_ptrtype_tool(csz_str) outbuf;
	_uls_type_tool(csz_str) tag_buf;
	int  lno=-1, tok_id, stat=0, fd, numbering;
	uls_lex_ptr_t uls;

	numbering = flags & ULS_LINE_NUMBERING;
	if ((uls=ostr->uls) == nilptr) {
		_uls_log(err_log)("%s: The 'uls' improper, binding error!", __FUNCTION__);
		return -1;
	}

	fd = ostr->fd;
	outbuf = uls_ptr(ostr->out_fd_csz);
	_uls_tool(csz_reset)(outbuf);

	_uls_tool(csz_init)(uls_ptr(tag_buf), 128);

	// Let TOK_LINENUM be printed in the first line
	//	by making uls->tag != tag_buf intentionally.
	if (__uls_get_taglen(uls) == 0) _uls_tool(csz_append)(uls_ptr(tag_buf), "?", 1);
	else _uls_tool(csz_append)(uls_ptr(tag_buf), "", 0);

	uls_want_eof(uls);

	while (1) {
		if ((tok_id = uls_get_tok(uls)) == uls->xcontext.toknum_ERR) {
			stat = -2;
			break;
		} else if (tok_id == uls->xcontext.toknum_EOI) {
			if (__flush_uls_stream_buffer(outbuf, fd, 1) < 0) {
				stat = -1;
			}
			uls_pop(uls);
			break;
		}

		if (!uls_is_context_initial(uls) && numbering) {
			if (__uls_get_taglen(uls) != csz_length(uls_ptr(tag_buf)) ||
				!uls_streql(_uls_tool(csz_text)(uls_ptr(tag_buf)), __uls_get_tag(uls)) ) {
				lno = __uls_get_lineno(uls);
				_uls_tool(csz_reset)(uls_ptr(tag_buf));
				_uls_tool(csz_append)(uls_ptr(tag_buf), __uls_get_tag(uls), __uls_get_taglen(uls));
				__uls_make_packet_linenum(ostr, lno, _uls_tool(csz_text)(uls_ptr(tag_buf)), csz_length(uls_ptr(tag_buf)));

			} else if (lno != __uls_get_lineno(uls)) {
				lno = __uls_get_lineno(uls);
				__uls_make_packet_linenum(ostr, lno, NULL, -1);
			}
		}

		__uls_make_packet(ostr, __uls_tok(uls), __uls_lexeme(uls), __uls_lexeme_len(uls));
		if (__flush_uls_stream_buffer(outbuf, fd, 0) < 0) {
			stat = -1;
			break;
		}
	}

	_uls_tool(csz_reset)(outbuf);
	_uls_tool(csz_deinit)(uls_ptr(tag_buf));

	return stat;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(_uls_const_LINE_NUMBERING)(void)
{
	return ULS_LINE_NUMBERING;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_ostream_ptr_t)
ULS_QUALIFIED_METHOD(ulsjava_create_ostream_file)(const void *filepath, int len_filepath, uls_lex_ptr_t uls,
	const void *subname, int len_subname)
{
	const char *ustr1 = _uls_tool_(strdup)((const char *)filepath, len_filepath);
	const char *ustr2 = _uls_tool_(strdup)((const char *)subname, len_subname);
	uls_ostream_ptr_t ostr;

	ostr = uls_create_ostream_file(ustr1, uls, ustr2);
	uls_mfree(ustr1);
	uls_mfree(ustr2);

	return ostr;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(ulsjava_print_tok)(uls_ostream_ptr_t ostr, int tokid, const void *tokstr, int len_tokstr)
{
	const char *ustr = _uls_tool_(strdup)((const char *)tokstr, len_tokstr);
	int rc;

	rc = uls_print_tok(ostr, tokid, ustr);
	uls_mfree(ustr);

	return rc;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(ulsjava_print_tok_linenum)(uls_ostream_ptr_t ostr, int lno, const void *tag, int len_tag)
{
	const char *ustr = _uls_tool_(strdup)((const char *)tag, len_tag);
	int rc;

	rc = uls_print_tok_linenum(ostr, lno, ustr);
	uls_mfree(ustr);

	return rc;
}
