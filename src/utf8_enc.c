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
  <file> utf8_enc.c </file>
  <brief>
    convert the utf16, utf32 streams into utf8 ones.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_UTF8_ENC__
#include "uls/utf8_enc.h"

#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(enc_utf16_to_utf8)(uls_uint16 *codpnts, int n_codpnts, uls_outparam_ptr_t parms)
{
	char* utf8_buf = parms->line;
	uls_uch_t uch;
	int rc;

	if ((rc = uls_decode_utf16(codpnts, n_codpnts, &uch)) <= 0) {
		if (rc < 0) _uls_log(err_log)("Incorrect UTF-16 format!");
		parms->len = 0;
		return rc;
	}

	if ((parms->len = uls_encode_utf8(uch, utf8_buf, parms->n)) <= 0) {
		return -1;
	}

	// rc == 1 or 2
	return rc; // # of consumed words for codpnts[]
}

char*
ULS_QUALIFIED_METHOD(uls_enc_utf16str_to_utf8str)(uls_uint16 *wstr1, int l_wstr1, uls_outparam_ptr_t parms)
{
	char *utf_chrs, *buff_chrs;
	int i, rc, rc2, l_buff_chrs, siz_buff_chrs;
	uls_outparam_t parms1;

	if (wstr1 == NULL || l_wstr1 <= 0) return NULL;
	siz_buff_chrs = 4 * l_wstr1+1;
	buff_chrs = utf_chrs = uls_malloc_buffer(siz_buff_chrs);

	for (i=0; i < l_wstr1; ) {
		parms1.line = utf_chrs;
		parms1.n = siz_buff_chrs - (int) (utf_chrs - buff_chrs);
		rc = enc_utf16_to_utf8(wstr1+i, l_wstr1-i, uls_ptr(parms1));
		rc2 = parms1.len;

		if (rc <= 0) {
			_uls_log(err_log)("Incorrect UTF-16 format!");
			uls_mfree(buff_chrs);
			return NULL;
		}

		i += rc;
		utf_chrs += rc2;
	}

	l_buff_chrs = (int) (utf_chrs - buff_chrs);
	buff_chrs[l_buff_chrs] = '\0';

	if (l_buff_chrs+1 != siz_buff_chrs)
		buff_chrs = (char *) uls_mrealloc(buff_chrs, l_buff_chrs+1);

	if (parms != nilptr) {
		parms->line = buff_chrs;
		parms->len = l_buff_chrs;
	}

	return buff_chrs;
}

char*
ULS_QUALIFIED_METHOD(uls_enc_utf32str_to_utf8str)(uls_uint32 *wstr1, int l_wstr1, uls_outparam_ptr_t parms)
{
	char *utf_chrs, *buff_chrs;
	int i, j, rc2, l_buff_chrs, siz_buff_chrs;
	uls_uch_t uch;

	if (wstr1 == NULL || l_wstr1 <= 0) return NULL;
	siz_buff_chrs = ULS_UTF8_CH_MAXLEN * l_wstr1 + 1;
	buff_chrs = utf_chrs = uls_malloc_buffer(siz_buff_chrs);

	for (i=j=0; i < l_wstr1; i++,j+=rc2) {
		if (uls_decode_utf32(wstr1[i], &uch) < 0) {
			_uls_log(err_log)("Incorrect UTF-32 format!");
			uls_mfree(buff_chrs);
			return NULL;
		}

		rc2 = uls_encode_utf8(uch, utf_chrs, siz_buff_chrs - j);
		utf_chrs += rc2;
	}

	l_buff_chrs = (int) (utf_chrs - buff_chrs);
	buff_chrs[l_buff_chrs] = '\0';

	if (l_buff_chrs+1 != siz_buff_chrs)
		buff_chrs = (char *) uls_mrealloc(buff_chrs, l_buff_chrs+1);

	if (parms != nilptr) {
		parms->line = buff_chrs;
		parms->len = l_buff_chrs;
	}

	return buff_chrs;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fill_utf8_buf)(uls_utf_inbuf_ptr_t inp)
{
	int rc, n_wrds_req;
	char *codpnts;

	if (inp->n_wrds > 0 && inp->bytesbuf != inp->wrdptr) {
		uls_memmove(inp->bytesbuf, inp->wrdptr, inp->n_wrds);
	}

	inp->wrdptr = inp->bytesbuf;
	n_wrds_req = inp->bytesbuf_siz - inp->n_wrds;

	if (inp->is_eof != 0 || n_wrds_req == 0) {
		return inp->n_wrds;
	}

	codpnts = inp->bytesbuf + inp->n_wrds;
	if ((rc = uls_readn(inp->fd, codpnts, n_wrds_req)) < 0) {
		_uls_log(err_log)("IO error or segmented utf16-char at EOF!");
		inp->is_eof = -1;
		return -1;
	} else if (rc == 0) {
		inp->is_eof = 1;
	}

	inp->n_wrds += rc;
	return inp->n_wrds;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(dec_utf8_buf)(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz)
{
	char *codpnts = inp->wrdptr;
	uls_uch_t *out_bufptr = out_buf, uch;
	int n_codpnts = inp->n_wrds, n_uchs = 0;
	int  i, rc;

	for (i=0; i < n_codpnts && n_uchs < out_bufsiz; i += rc) {
		if ((rc = uls_decode_utf8(codpnts+i, n_codpnts-i, &uch)) <= 0) {
			if (inp->is_eof != 0) {
				_uls_log(err_log)("Incorrect utf-8 format!");
				inp->is_eof = -1;
				return -1;
			}
			break;
		}
		*out_bufptr++ = uch;
		++n_uchs;
	}

	inp->wrdptr += i;
	inp->n_wrds -= i;

	return n_uchs;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fill_utf16_buf)(uls_utf_inbuf_ptr_t inp)
{
	int wrdsiz = sizeof(uls_uint16);
	int i, rc, n_wrds_req;
	uls_uint16 *codpnts;

	if (inp->n_wrds > 0 && inp->bytesbuf != inp->wrdptr) {
		uls_memmove(inp->bytesbuf, inp->wrdptr, inp->n_wrds * wrdsiz);
	}
	inp->wrdptr = inp->bytesbuf;

	n_wrds_req = inp->bytesbuf_siz - inp->n_wrds * wrdsiz;
	n_wrds_req /= wrdsiz;
	if (inp->is_eof != 0 || n_wrds_req == 0) {
		return inp->n_wrds;
	}

	codpnts = (uls_uint16 *) (inp->bytesbuf + inp->n_wrds * wrdsiz);
	if ((rc = uls_readn(inp->fd, codpnts, n_wrds_req*wrdsiz)) == 0) {
		inp->is_eof = 1;
	} else if (rc < 0 || rc % wrdsiz != 0) { // 2-bytes ~ utf16
		_uls_log(err_log)("IO error or segmented utf16-char at EOF!");
		inp->is_eof = -1;
		return -1;
	}

	rc /= wrdsiz;
	if (inp->reverse) {
		for (i=0; i<rc; i++) {
			uls_reverse_bytes((char *) (codpnts+i), wrdsiz);
		}
	}

	inp->n_wrds += rc;
	return inp->n_wrds;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(dec_utf16_buf)(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz)
{
	int wrdsiz = sizeof(uls_uint16);
	uls_uint16 *codpnts = (uls_uint16 *) inp->wrdptr;
	uls_uch_t *out_bufptr = out_buf, uch;
	int n_codpnts = inp->n_wrds, n_uchs = 0;
	int  i, rc;

	for (i=0; i < n_codpnts && n_uchs < out_bufsiz; i += rc) {
		if ((rc = uls_decode_utf16(codpnts+i, n_codpnts-i, &uch)) <= 0) {
			if (inp->is_eof != 0) {
				_uls_log(err_log)("Incorrect utf-16 format!");
				inp->is_eof = -1;
				return -1;
			}
			break;
		}

		*out_bufptr++ = uch;
		++n_uchs;
	}

	inp->wrdptr += i * wrdsiz;
	inp->n_wrds -= i;

	return n_uchs;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fill_utf32_buf)(uls_utf_inbuf_ptr_t inp)
{
	int wrdsiz = sizeof(uls_uint32);
	int i, rc, n_wrds_req;
	uls_uint32 *codpnts;

	if (inp->n_wrds > 0 && inp->bytesbuf != inp->wrdptr) {
		uls_memmove(inp->bytesbuf, inp->wrdptr, inp->n_wrds * wrdsiz);
	}
	inp->wrdptr = inp->bytesbuf;

	n_wrds_req = inp->bytesbuf_siz - inp->n_wrds * wrdsiz;
	n_wrds_req /= wrdsiz;
	if (inp->is_eof != 0 || n_wrds_req == 0) {
		return inp->n_wrds;
	}

	codpnts = (uls_uint32 *) (inp->bytesbuf + inp->n_wrds * wrdsiz);
	if ((rc = uls_readn(inp->fd, codpnts, n_wrds_req*wrdsiz)) == 0) {
		inp->is_eof = 1;
	} else if (rc < 0 || rc % wrdsiz != 0) {
		_uls_log(err_log)("IO error or segmented utf32-char at EOF!");
		inp->is_eof = -1;
		return -1;
	}

	rc /= wrdsiz;
	if (inp->reverse) {
		for (i=0; i<rc; i++) {
			uls_reverse_bytes((char *) (codpnts+i), wrdsiz);
		}
	}

	inp->n_wrds += rc;
	return inp->n_wrds;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(dec_utf32_buf)(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz)
{
	int  n_uchs;

	if (inp->wrdptr != (char *) out_buf) {
		_uls_log(err_log)("%s: protocol mismatched!");
		return -1;
	}

	n_uchs = inp->n_wrds;
	inp->n_wrds = 0;

	return n_uchs;
}

void
ULS_QUALIFIED_METHOD(uls_utf_reset_inbuf)(uls_utf_inbuf_ptr_t inp, int mode)
{
	int enc_fmt = mode & UTF_INPUT_FORMAT_MASK;
	int reverse = mode & UTF_INPUT_FORMAT_REVERSE ? 1 : 0;

	if (enc_fmt == UTF_INPUT_FORMAT_8) {
		inp->fill_rawbuf = uls_ref_callback_this(fill_utf8_buf);
		inp->dec_rawbuf = uls_ref_callback_this(dec_utf8_buf);

	} else if (enc_fmt == UTF_INPUT_FORMAT_16) {
		inp->fill_rawbuf = uls_ref_callback_this(fill_utf16_buf);
		inp->dec_rawbuf = uls_ref_callback_this(dec_utf16_buf);

	} else if (enc_fmt == UTF_INPUT_FORMAT_32) {
		inp->fill_rawbuf = uls_ref_callback_this(fill_utf32_buf);
		inp->dec_rawbuf = uls_ref_callback_this(dec_utf32_buf);

	} else {
		inp->fill_rawbuf = nilptr;
		inp->dec_rawbuf = nilptr;
	}

	inp->reverse = reverse;
	inp->wrdptr = inp->bytesbuf;
	inp->n_wrds = 0;

	inp->fd = -1;
	inp->is_eof = -2;
}

void
ULS_QUALIFIED_METHOD(uls_utf_set_inbuf)(uls_utf_inbuf_ptr_t inp, int fd)
{
	inp->fd = fd;
	inp->is_eof = 0;
}

void
ULS_QUALIFIED_METHOD(uls_utf_init_inbuf)(uls_utf_inbuf_ptr_t inp,
	char *buf, int bufsiz, int mode)
{
	if (bufsiz <= 0) {
		_uls_log(err_panic)("%s: invalid paramerter 'bufsiz'", __FUNCTION__);
	}

	uls_initial_zerofy_object(inp);
	inp->flags = ULS_FL_STATIC;

	if (buf == NULL) {
		bufsiz = uls_roundup(bufsiz, UTF_INPUT_BUFSIZ);
		inp->bytesbuf = uls_malloc_buffer(bufsiz);
		inp->flags |= ULS_FL_UTF_INBUF_BUF_ALLOCED;
	} else {
		inp->bytesbuf = buf;
	}

	inp->bytesbuf_siz = bufsiz;

	uls_utf_reset_inbuf(inp, mode);
}

void
ULS_QUALIFIED_METHOD(utf_deinit_inbuf)(uls_utf_inbuf_ptr_t inp)
{
	if (inp->bytesbuf != NULL) {
		if (inp->flags & ULS_FL_UTF_INBUF_BUF_ALLOCED) {
			uls_mfree(inp->bytesbuf);
		}
		inp->bytesbuf = NULL;
		inp->bytesbuf_siz = 0;
	}
}

ULS_QUALIFIED_RETTYP(uls_utf_inbuf_ptr_t)
ULS_QUALIFIED_METHOD(uls_utf_create_inbuf)(char *buf, int bufsiz, int mode)
{
	uls_utf_inbuf_ptr_t inp;

	inp = uls_alloc_object(uls_utf_inbuf_t);

	uls_utf_init_inbuf(inp, buf, bufsiz, mode);
	inp->flags &= ~ULS_FL_STATIC;

	return inp;
}

void
ULS_QUALIFIED_METHOD(uls_utf_destroy_inbuf)(uls_utf_inbuf_ptr_t inp)
{
	utf_deinit_inbuf(inp);

	if (inp->flags & ULS_FL_STATIC) {
		inp->flags = ULS_FL_STATIC;
	} else {
		uls_dealloc_object(inp);
	}
}

int
ULS_QUALIFIED_METHOD(uls_utf_is_inbuf_empty)(uls_utf_inbuf_ptr_t inp)
{
	int stat;
	stat = inp->is_eof < 0 || (inp->is_eof > 0 && inp->n_wrds <= 0);
	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_utf_drain_inbuf)(uls_utf_inbuf_ptr_t inp, uls_uch_t *buf, int bufsiz)
{
	int  buflen, rc;

	for (buflen=0; buflen < bufsiz; buflen += rc) {
		inp->fill_rawbuf(inp);
		if (uls_utf_is_inbuf_empty(inp)) {
			break;
		}

		if ((rc = inp->dec_rawbuf(inp, buf + buflen, bufsiz - buflen)) < 0) {
			_uls_log(err_log)("I/O error!");
			return -1;
		} else if (rc == 0) {
			break;
		}
	}

	return buflen;
}

int
ULS_QUALIFIED_METHOD(uls_fill_utf8str)(uls_uch_t *uchs, int n_uchs,
	char* utf8buf, int siz_utf8buf, int *p_len_utf8buf)
{
	char *outptr = utf8buf;
	int rc, i, len_utf8buf = 0;
	uls_uch_t uch;

	for (i = 0; i < n_uchs; i++) {
		uch = uchs[i];

		rc = uls_encode_utf8(uch, outptr, siz_utf8buf - len_utf8buf);
		if (rc < 0) {
			_uls_log(err_log)("%s: incorrect uch!", __FUNCTION__);
			return -1;
		} else if (rc == 0) {
			break;
		}

		outptr += rc;
		len_utf8buf += rc;
	}

	if (p_len_utf8buf != NULL) {
		*p_len_utf8buf = len_utf8buf;
	}

	return i;
}

int
ULS_QUALIFIED_METHOD(uls_fill_utf8buf)(uls_utf_inbuf_ptr_t inp, char* utf8buf, int len0_utf8buf, int siz_utf8buf)
{
	int len_utf8buf = len0_utf8buf;
	int wrdsiz = sizeof(uls_uint32);
	int i, rc, n_words, n_bytes, n_uchbuf;
	uls_uch_t *uchbuf, uch;

	while (1) {
		i = uls_roundup(len_utf8buf, wrdsiz);
		if (i + ULS_UTF8_CH_MAXLEN > siz_utf8buf) {
			for ( ; len_utf8buf < siz_utf8buf; len_utf8buf += n_bytes) {
				n_words = uls_utf_drain_inbuf(inp, &uch, 1);
				if (n_words < 0) {
					_uls_log(err_log)("Can't read input!!");
					return -1;
				} else if (n_words < 1) {
					break;
				}

				if ((rc=uls_fill_utf8str(&uch, 1,
					utf8buf + len_utf8buf, siz_utf8buf - len_utf8buf, &n_bytes)) < 0) {
					_uls_log(err_log)("%s: An incorrect uch exists", __FUNCTION__);
					return -2;
				} else if (rc == 0) {
					break;
				}
			}
			break;
		}

		n_uchbuf = (siz_utf8buf  - i) / wrdsiz;
		uchbuf = (uls_uch_t *) (utf8buf + i);

		n_words = uls_utf_drain_inbuf(inp, uchbuf, n_uchbuf);
		if (n_words < 0) {
			_uls_log(err_log)("Can't read input!!");
			return -1;
		} else if (n_words == 0) {
			break;
		}

		if (uls_fill_utf8str(uchbuf, n_words,
			utf8buf + len_utf8buf, siz_utf8buf - len_utf8buf, &n_bytes) < 0) {
			_uls_log(err_log)("%s: An incorrect uch exists", __FUNCTION__);
			return -1;
		}

		len_utf8buf += n_bytes;
	}

	return len_utf8buf - len0_utf8buf;
}

int
ULS_QUALIFIED_METHOD(uls_enc_utf16file_to_8)(int fd, int fd_out, int reverse)
{
	uls_uch_t *outbuf;
	uls_utf_inbuf_t inbuff;
	int i, rc, outbuf_len, outbuf_siz, mode, stat=0;
	int len_utf8_buf, siz_utf8_buf;
	char *utf8_buf;

	mode = UTF_INPUT_FORMAT_16;
	if (reverse) mode |= UTF_INPUT_FORMAT_REVERSE;
	uls_utf_init_inbuf(uls_ptr(inbuff), NULL, UTF_INPUT_BUFSIZ, mode);

	outbuf_siz = inbuff.bytesbuf_siz;
	outbuf = (uls_uch_t *) uls_malloc_buffer(outbuf_siz * sizeof(uls_uch_t));

	siz_utf8_buf = outbuf_siz * ULS_UTF8_CH_MAXLEN;
	utf8_buf = (char *) uls_malloc_buffer(siz_utf8_buf);

	uls_utf_set_inbuf(uls_ptr(inbuff), fd);

	while (!uls_utf_is_inbuf_empty(uls_ptr(inbuff))) {
		if ((outbuf_len = uls_utf_drain_inbuf(uls_ptr(inbuff), outbuf, outbuf_siz)) <= 0) {
			stat = outbuf_len;
			break;
		}

		for (len_utf8_buf = 0, i=0; i < outbuf_len; i++) {
			if ((rc = uls_encode_utf8(outbuf[i], utf8_buf + len_utf8_buf, siz_utf8_buf - len_utf8_buf)) <= 0) {
				_uls_log(err_log)("invalid uch = 0x%x!", outbuf[i]);
				stat = -1;
				break;
			}
			len_utf8_buf += rc;
		}

		if (stat < 0 || uls_writen(fd_out, utf8_buf, len_utf8_buf) < len_utf8_buf) {
			_uls_log(err_log)("failed to write fd!");
			stat = -2;
			break;
		}
	}

	uls_mfree(utf8_buf);
	uls_mfree(outbuf);

	utf_deinit_inbuf(uls_ptr(inbuff));

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_enc_utf8file_to_16)(int fd, int fd_out, int reverse)
{
	uls_uch_t *outbuf;
	uls_utf_inbuf_t inbuff;
	int i, n, rc, outbuf_len, outbuf_siz, stat=0;
	int len_utf16_buf, siz_utf16_buf;
	uls_uint16 *utf16_buf;

	uls_utf_init_inbuf(uls_ptr(inbuff), NULL, UTF_INPUT_BUFSIZ, UTF_INPUT_FORMAT_8);

	outbuf_siz = inbuff.bytesbuf_siz;
	outbuf = (uls_uch_t *) uls_malloc_buffer(outbuf_siz * sizeof(uls_uch_t));

	siz_utf16_buf = outbuf_siz * 4;
	utf16_buf = (uls_uint16 *) uls_malloc_buffer(siz_utf16_buf);

	uls_utf_set_inbuf(uls_ptr(inbuff), fd);

	while (!uls_utf_is_inbuf_empty(uls_ptr(inbuff))) {
		if ((outbuf_len = uls_utf_drain_inbuf(uls_ptr(inbuff), outbuf, outbuf_siz)) <= 0) {
			stat = outbuf_len;
			break;
		}

		for (len_utf16_buf = 0, i=0; i < outbuf_len; i++) {
			if ((rc = uls_encode_utf16(outbuf[i], utf16_buf + len_utf16_buf)) <= 0) {
				_uls_log(err_log)("invalid uch = 0x%x!", outbuf[i]);
				stat = -1;
				break;
			}
			len_utf16_buf += rc;
		}

		if (stat < 0) {
			break;
		}

		if (reverse) {
			for (i=0; i<len_utf16_buf; i++) {
				uls_reverse_bytes((char *) (utf16_buf+i), sizeof(uls_uint16));
			}
		}

		n = len_utf16_buf * sizeof(uls_uint16);
		if (uls_writen(fd_out, utf16_buf, n) < n) {
			_uls_log(err_log)("failed to write fd!");
			stat = -2;
			break;
		}
	}

	uls_mfree(utf16_buf);
	uls_mfree(outbuf);

	utf_deinit_inbuf(uls_ptr(inbuff));
	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_enc_utf32file_to_8)(int fd, int fd_out, int reverse)
{
	uls_uch_t *outbuf;
	int i, n, rc, outbuf_len, outbuf_siz, wrdsiz=sizeof(uls_uch_t), stat=0;
	int len_utf8_buf, siz_utf8_buf;
	char *utf8_buf;

	outbuf_siz = UTF_INPUT_BUFSIZ;
	outbuf = (uls_uch_t *) uls_malloc_buffer(outbuf_siz * wrdsiz);

	siz_utf8_buf = outbuf_siz * ULS_UTF8_CH_MAXLEN;
	utf8_buf = (char *) uls_malloc_buffer(siz_utf8_buf);

	while (1) {
		n = outbuf_siz * sizeof(uls_uch_t);
		if ((rc = uls_readn(fd, outbuf, n)) == 0) {
			break;
		} else if (rc < 0 || rc % wrdsiz != 0) {
			_uls_log(err_log)("IO error or segmented utf32 at EOF!");
			stat = -1;
		}

		outbuf_len = rc / wrdsiz;

		if (reverse) {
			for (i=0; i<outbuf_len; i++) {
				uls_reverse_bytes((char *) (outbuf+i), wrdsiz);
			}
		}

		for (len_utf8_buf = 0, i=0; i < outbuf_len; i++) {
			if ((rc = uls_encode_utf8(outbuf[i], utf8_buf + len_utf8_buf, siz_utf8_buf - len_utf8_buf)) <= 0) {
				_uls_log(err_log)("invalid uch = 0x%x!", outbuf[i]);
				stat = -1;
				break;
			}
			len_utf8_buf += rc;
		}

		if (stat < 0 || uls_writen(fd_out, utf8_buf, len_utf8_buf) < len_utf8_buf) {
			_uls_log(err_log)("failed to write fd!");
			stat = -2;
			break;
		}
	}

	uls_mfree(utf8_buf);
	uls_mfree(outbuf);

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_enc_utf8file_to_32)(int fd, int fd_out, int reverse)
{
	uls_uch_t *outbuf;
	uls_utf_inbuf_t inbuff;
	int i, n, outbuf_len, outbuf_siz, wrdsiz=sizeof(uls_uch_t), stat=0;

	uls_utf_init_inbuf(uls_ptr(inbuff), NULL, UTF_INPUT_BUFSIZ, UTF_INPUT_FORMAT_8);

	outbuf_siz = inbuff.bytesbuf_siz;
	outbuf = (uls_uch_t *) uls_malloc_buffer(outbuf_siz * wrdsiz);
	uls_utf_set_inbuf(uls_ptr(inbuff), fd);

	while (!uls_utf_is_inbuf_empty(uls_ptr(inbuff))) {
		if ((outbuf_len = uls_utf_drain_inbuf(uls_ptr(inbuff), outbuf, outbuf_siz)) <= 0) {
			stat = outbuf_len;
			break;
		}

		if (reverse) {
			for (i=0; i<outbuf_len; i++) {
				uls_reverse_bytes((char *) (outbuf+i), wrdsiz);
			}
		}

		n = outbuf_len * wrdsiz;
		if (uls_writen(fd_out, outbuf, n) < n) {
			_uls_log(err_log)("failed to write fd!");
			stat = -2;
			break;
		}
	}

	uls_mfree(outbuf);
	utf_deinit_inbuf(uls_ptr(inbuff));

	return stat;
}
