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
  <file> uls_ostream_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#include "uls/uls_ostream_astr.h"
#include "uls/uls_alog.h"

uls_ostream_ptr_t
__uls_create_ostream_astr(int fd_out, uls_lex_ptr_t uls, int stream_type, const char *subname)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	uls_ostream_ptr_t hdr;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(subname, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		hdr = nilptr;
	} else {
		hdr = __uls_create_ostream(fd_out, uls, stream_type, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return hdr;
}

uls_ostream_ptr_t
uls_create_ostream_astr(int fd_out, uls_lex_ptr_t uls, const char *subname)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	uls_ostream_ptr_t hdr;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(subname, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_create_ostream(fd_out, uls, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return hdr;
}

uls_ostream_ptr_t
uls_create_ostream_file_astr(const char *filepath, uls_lex_ptr_t uls, const char *subname)
{
	uls_ostream_ptr_t ostr=nilptr;
	const char *wrdlst[2] = { filepath, subname };
	const char *ustr[2];
	auw_outparam_t buf_csz[2];
	int i;

	for (i=0; ; i++) {
		if (i >= 2) {
			ostr = uls_create_ostream_file(ustr[0], uls, ustr[1]);
			break;
		}

		if (wrdlst[i] == NULL) {
			ustr[i] = NULL;
		} else {
			auw_init_outparam(buf_csz + i);

			if ((ustr[i] = uls_astr2ustr_ptr(wrdlst[i], -1, buf_csz + i)) == NULL) {
				err_alog("encoding error!");
				auw_deinit_outparam(buf_csz + i);
				break;
			}
		}
	}

	for (--i; i>=0; i--) {
		if (ustr[i] != NULL) {
			auw_deinit_outparam(buf_csz + i);
		}
	}

	return ostr;
}

int
__uls_print_tok_astr(uls_ostream_ptr_t ostr, int tokid, const char *tokstr, int l_tokstr)
{
	const char *ustr;
	int ulen, rc;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(tokstr, l_tokstr, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		rc = -1;
	} else {
		ulen = buf_csz.outlen;
		rc = __uls_print_tok(ostr, tokid, ustr, ulen);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}

int
__uls_print_tok_linenum_astr(uls_ostream_ptr_t ostr, int lno, const char *atag, int atag_len)
{
	const char *ustr;
	int ulen, rc;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(atag, atag_len, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		rc = -1;
	} else {
		ulen = buf_csz.outlen;
		rc = __uls_print_tok_linenum(ostr, lno, ustr, ulen);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}
