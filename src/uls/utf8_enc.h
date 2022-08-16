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
  <file> utf8_enc.h </file>
  <brief>
    convert the utf16, utf32 streams into utf8 ones.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#ifndef __UTF8_ENC_H__
#define __UTF8_ENC_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define ULS_FL_UTF_INBUF_BUF_ALLOCED 0x01

#define UTF_INPUT_FORMAT_8       0
#define UTF_INPUT_FORMAT_16      1
#define UTF_INPUT_FORMAT_32      2
#define UTF_INPUT_FORMAT_MASK    0x0F
#define UTF_INPUT_FORMAT_REVERSE 0x10

#ifdef _ULS_IMPLDLL
#define UTF_INPUT_BUFSIZ 512
#endif
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DECLARE_STRUCT(utf_inbuf);
ULS_DEFINE_DELEGATE_BEGIN(fill_utf_rawbuf, int)(uls_utf_inbuf_ptr_t inp);
ULS_DEFINE_DELEGATE_END(fill_utf_rawbuf);

ULS_DEFINE_DELEGATE_BEGIN(dec_utf_rawbuf, int)(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz);
ULS_DEFINE_DELEGATE_END(dec_utf_rawbuf);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT_BEGIN(utf_inbuf)
{
	uls_flags_t flags;

	char   *bytesbuf;
	int    bytesbuf_siz; // bytes unit
	char   *wrdptr;    // reference to bytesbuf[]
	int    n_wrds;     // 1, 2, 4 bytes unit
	uls_uch_t wch_buffered;

	int    fd;
	int    is_eof;
	uls_voidptr_t data;

	uls_callback_type_this(fill_utf_rawbuf) fill_rawbuf;
	uls_callback_type_this(dec_utf_rawbuf)  dec_rawbuf;
	int    reverse;
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_UTF8_ENC__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int fill_utf8_buf(uls_utf_inbuf_ptr_t inp);
ULS_DECL_STATIC int dec_utf8_buf(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz);

ULS_DECL_STATIC int fill_utf16_buf(uls_utf_inbuf_ptr_t inp);
ULS_DECL_STATIC int dec_utf16_buf(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz);

ULS_DECL_STATIC int fill_utf32_buf(uls_utf_inbuf_ptr_t inp);
ULS_DECL_STATIC int dec_utf32_buf(uls_utf_inbuf_ptr_t inp, uls_uch_t* out_buf, int out_bufsiz);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_utf_init_inbuf(uls_utf_inbuf_ptr_t inp, char *buf, int bufsiz, int mode);
void utf_deinit_inbuf(uls_utf_inbuf_ptr_t inp);
void uls_utf_reset_inbuf(uls_utf_inbuf_ptr_t inp, int mode);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
int uls_fill_utf8str(uls_uch_t *uchs, int n_uchs,
	char* utf8buf, int siz_utf8buf, int *p_len_utf8buf);
int uls_fill_utf8buf(uls_utf_inbuf_ptr_t inp, char* utf8buf, int len_utf8buf, int siz_utf8buf);

char* uls_enc_utf16str_to_utf8str(uls_uint16 *wstr1, int l_wstr1, uls_outparam_ptr_t parms);
char* uls_enc_utf32str_to_utf8str(uls_uint32 *wstr1, int l_wstr1, uls_outparam_ptr_t parms);

int uls_utf_is_inbuf_empty(uls_utf_inbuf_ptr_t inp);
void uls_utf_set_inbuf(uls_utf_inbuf_ptr_t inp, int fd);
uls_utf_inbuf_ptr_t uls_utf_create_inbuf(char *buf, int bufsiz, int mode);
void uls_utf_destroy_inbuf(uls_utf_inbuf_ptr_t inp);
int uls_utf_drain_inbuf(uls_utf_inbuf_ptr_t inp, uls_uch_t *buf, int bufsiz);

#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __UTF8_ENC_H__
