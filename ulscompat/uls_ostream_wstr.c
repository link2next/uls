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
  <file> uls_ostream_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#include "uls/uls_lex.h"
#include "uls/uls_util.h"
#include "uls/uls_auw.h"
#include "uls/uls_ostream_wstr.h"
#include "uls/uls_wlog.h"

uls_ostream_ptr_t
__uls_create_ostream_wstr(int fd_out, uls_lex_ptr_t uls, int stream_type, const wchar_t* subname)
{
	char *ustr;
	csz_str_t csz;
	uls_ostream_ptr_t hdr;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(subname, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		hdr = nilptr;
	} else {
		hdr = __uls_create_ostream(fd_out, uls, stream_type, ustr);
	}

	csz_deinit(uls_ptr(csz));
	return hdr;
}

uls_ostream_ptr_t
uls_create_ostream_wstr(int fd_out, uls_lex_ptr_t uls, const wchar_t* subname)
{
	char *ustr;
	csz_str_t csz;
	uls_ostream_ptr_t hdr;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(subname, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_create_ostream(fd_out, uls, ustr);
	}

	csz_deinit(uls_ptr(csz));
	return hdr;
}

uls_ostream_ptr_t
uls_create_ostream_file_wstr(const wchar_t* filepath, uls_lex_ptr_t uls, const wchar_t* subname)
{
	uls_ostream_ptr_t ostr=nilptr;
	const wchar_t *wrdlst[2] = { filepath, subname };
	char *ustr[2];
	csz_str_t csz[2];
	int  i;

	for (i=0; ; i++) {
		if (i >= 2) {
			ostr = uls_create_ostream_file(ustr[0], uls, ustr[1]);
			break;
		}

		if (wrdlst[i] == NULL) {
			ustr[i] = NULL;

		} else {
			csz_init(csz + i, -1);

			if ((ustr[i] = uls_wstr2ustr(wrdlst[i], -1, csz + i)) == NULL) {
				err_wlog(L"encoding error!");
				csz_deinit(csz + i);
				break;
			}
		}
	}

	for (--i; i>=0; i--) {
		if (ustr[i] != NULL) {
			csz_deinit(csz + i);
		}
	}

	return ostr;
}

int
uls_print_tok_wstr(uls_ostream_ptr_t ostr, int tokid, const wchar_t* tokstr)
{
	char *ustr;
	int  ulen, rc;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(tokstr, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		rc = -1;
	} else {
		ulen = csz_length(uls_ptr(csz));
		rc = __uls_print_tok(ostr, tokid, ustr, ulen);
	}

	csz_deinit(uls_ptr(csz));
	return rc;
}

int
uls_print_tok_linenum_wstr(uls_ostream_ptr_t ostr, int lno, const wchar_t* tag)
{
	char *ustr;
	int  ulen, rc;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(tag, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		rc = -1;
	} else {
		ulen = csz_length(uls_ptr(csz));
		rc = __uls_print_tok_linenum(ostr, lno, ustr, ulen);
	}

	csz_deinit(uls_ptr(csz));
	return rc;
}
