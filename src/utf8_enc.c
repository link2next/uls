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

char*
ULS_QUALIFIED_METHOD(uls_enc_utf16str_to_utf8str)(uls_uint16 *wstr1, int l_wstr1, uls_outparam_ptr_t parms)
{
	char *utf_chrs, *buff_chrs;
	int i, rc, l_buff_chrs, siz_buff_chrs;
	uls_uch_t uch;

	if (wstr1 == NULL || l_wstr1 <= 0) return NULL;

	siz_buff_chrs = 4 * l_wstr1 + 1;
	buff_chrs = utf_chrs = uls_malloc_buffer(siz_buff_chrs);

	for (i=0; i < l_wstr1; i += rc) {
		if ((rc = uls_decode_utf16(wstr1 + i, l_wstr1 - i, &uch)) <= 0) {
			_uls_log(err_log)("Incorrect UTF-16 format!");
			uls_mfree(buff_chrs);
			return NULL;
		}
		utf_chrs += uls_encode_utf8(uch, utf_chrs);
	}

	l_buff_chrs = (int) (utf_chrs - buff_chrs);
	buff_chrs[l_buff_chrs] = '\0';

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
	int i, rc, l_buff_chrs, siz_buff_chrs;
	uls_uch_t uch;

	if (wstr1 == NULL || l_wstr1 <= 0) return NULL;

	siz_buff_chrs = 4 * l_wstr1 + 1;
	buff_chrs = utf_chrs = uls_malloc_buffer(siz_buff_chrs);

	for (i=0; i < l_wstr1; i++) {
		if (uls_decode_utf32(wstr1[i], &uch) <= 0) {
			_uls_log(err_log)("Incorrect UTF-32 format!");
			uls_mfree(buff_chrs);
			return NULL;
		}
		utf_chrs += rc = uls_encode_utf8(uch, utf_chrs);
	}

	l_buff_chrs = (int) (utf_chrs - buff_chrs);
	buff_chrs[l_buff_chrs] = '\0';

	if (parms != nilptr) {
		parms->line = buff_chrs;
		parms->len = l_buff_chrs;
	}

	return buff_chrs;
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

		rc = uls_encode_utf8(uch, NULL);
		if (len_utf8buf + rc > siz_utf8buf) {
			break;
		}
		rc = uls_encode_utf8(uch, outptr);

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
		if (i + wrdsiz > siz_utf8buf) {
			for ( ; len_utf8buf < siz_utf8buf; len_utf8buf += n_bytes) {
				n_words = uls_utf_drain_inbuf(inp, &uch, 1);
				if (n_words < 0) {
					_uls_log(err_log)("Can't read input!!");
					return -1;
				} else if (n_words < 1) {
					break;
				}

				if ((rc = uls_fill_utf8str(&uch, 1,
					utf8buf + len_utf8buf, siz_utf8buf - len_utf8buf, &n_bytes)) < 0) {
					_uls_log(err_log)("%s: An incorrect uch exists", __func__);
					return -2;
				} else if (rc == 0) {
					inp->wch_buffered = uch;
					break;
				}
			}
			break;
		}

		n_uchbuf = (siz_utf8buf - i) / wrdsiz;
		uchbuf = (uls_uch_t *) (utf8buf + i);

		n_words = uls_utf_drain_inbuf(inp, uchbuf, n_uchbuf);
		if (n_words < 0) {
			_uls_log(err_log)("Can't read input!!");
			return -1;
		} else if (n_words == 0) {
			break;
		}

		if (uls_fill_utf8str(uchbuf, n_words,
			utf8buf + len_utf8buf, siz_utf8buf - len_utf8buf, &n_bytes) < n_words) {
			_uls_log(err_log)("%s: An incorrect uch exists", __func__);
			return -1;
		}

		len_utf8buf += n_bytes;
	}

	return len_utf8buf - len0_utf8buf;
}

void
ULS_QUALIFIED_METHOD(uls_utf_init_inbuf)(uls_utf_inbuf_ptr_t inp,
	char *buf, int bufsiz, int mode)
{
	if (bufsiz <= 0) {
		_uls_log(err_panic)("%s: invalid paramerter 'bufsiz'", __func__);
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
	stat = inp->is_eof < 0 ||
		(inp->is_eof > 0 && inp->n_wrds <= 0 && inp->wch_buffered == 0);
	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_utf_set_inbuf)(uls_utf_inbuf_ptr_t inp, int fd)
{
	inp->fd = fd;
	inp->is_eof = 0;
	inp->n_wrds = 0;
	inp->wch_buffered = 0;
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
	inp->is_eof = 0;
}

int
ULS_QUALIFIED_METHOD(uls_utf_drain_inbuf)(uls_utf_inbuf_ptr_t inp, uls_uch_t *buf, int bufsiz)
{
	uls_uch_t *bufptr = buf;
	int  rc, buflen = 0;

	if (inp->is_eof < 0) return -1;

	if (inp->wch_buffered != 0) {
		*bufptr++ = inp->wch_buffered;
		inp->wch_buffered = 0;
		++buflen;
	}

	for ( ; buflen < bufsiz; buflen += rc, bufptr += rc) {
		if (inp->n_wrds < 4 && inp->fill_rawbuf(inp) < 0) {
			_uls_log(err_log)("%s: I/O error!", __func__);
			buflen = -1;
			break;
		}

		if (inp->n_wrds <= 0) {
			break;
		}

		if ((rc = inp->dec_rawbuf(inp, bufptr, bufsiz - buflen)) <= 0) {
			_uls_log(err_log)("I/O error at EOF, file truncated or encoding error!?");
			buflen = -1;
			break;
		}
	}

	return buflen;
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
	}

	if (rc == 0) inp->is_eof = 1;
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
			if (rc < -ULS_UTF8_CH_MAXLEN) {
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
			if (rc < -ULS_UTF16_CH_MAXLEN) {
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
	int wrdsiz = sizeof(uls_uint32);
	uls_uint32 *codpnts = (uls_uint32 *) inp->wrdptr;
	int i, n_uchs = 0;

	for (i = 0; i < inp->n_wrds && n_uchs < out_bufsiz; i++) {
		out_buf[n_uchs++] = codpnts[i];
	}

	inp->wrdptr += i * wrdsiz;
	inp->n_wrds -= i;

	return n_uchs;
}
