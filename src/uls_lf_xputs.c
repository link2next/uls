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
  <file> uls_lf_xputs.c </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..).
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2011.
  </author>
*/

#define __ULS_LF_XPUTS__
#include "uls/uls_lf_xputs.h"

#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"

int
ULS_QUALIFIED_METHOD(uls_lf_puts_null)(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	return 0;
}

void
ULS_QUALIFIED_METHOD(__uls_lf_sysputs)(const char *msg)
{
	uls_put_binstr(msg, -1, _uls_stdio_fd(2));
}

int
ULS_QUALIFIED_METHOD(uls_lf_puts_csz)(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	if (wrdptr == NULL) {
		csz_text((csz_str_ptr_t ) x_dat);
		return 0;
	}

	csz_append((csz_str_ptr_t ) x_dat, wrdptr, wrdlen);
	return wrdlen;
}

int
ULS_QUALIFIED_METHOD(uls_lf_puts_str)(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	uls_buf4str_ptr_t lb = (uls_buf4str_ptr_t) x_dat;

	if (wrdptr == NULL) {
		if (lb->bufsiz <= 0) {
			__uls_lf_sysputs("sprintf: buffer overflow");
			return -1;
		}
		*lb->bufptr = '\0';
		return 0;
	}

	if (lb->bufsiz <= wrdlen) { // considering the space for '\0'
		__uls_lf_sysputs("sprintf: buffer overflow");
		return -1;
	}

	uls_memcopy(lb->bufptr, wrdptr, wrdlen);
	lb->bufptr += wrdlen;
	lb->bufsiz -= wrdlen;

	return wrdlen;
}

int
ULS_QUALIFIED_METHOD(uls_lf_puts_file)(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	if (fwrite(wrdptr, sizeof(char), wrdlen, fout) < (size_t) wrdlen) {
		wrdlen = -1;
	}

	return wrdlen;
}

#ifdef __ULS_WINDOWS__
int
uls_lf_aputs_csz(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	csz_str_ptr_t csz = (csz_str_ptr_t) x_dat;
	int alen, rc;
	const char *astr;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		csz_text(csz);
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		rc = alen = buf_csz.outlen;
		csz_append(csz, astr, alen);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return rc;
}

int
uls_lf_aputs_str(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	uls_buf4str_ptr_t alb = (uls_buf4str_ptr_t) x_dat;
	const char *astr;
	int alen, rc;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		if (alb->bufsiz <= 0) {
			return -1;
		}
		*alb->bufptr = '\0';
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if (wrdlen >= alb->bufsiz ||
		(astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		rc = alen = buf_csz.outlen;
		uls_memcopy(alb->bufptr, astr, alen);
		alb->bufptr += alen;
		alb->bufsiz -= alen;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}

int
uls_lf_aputs_file(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;
	char ch, buff_crlf[2] = { '\r', '\n' };
	int alen, i1, i, rc, n;
	const char *astr;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		auw_deinit_outparam(uls_ptr(buf_csz));
		return -1;
	}

	wrdptr = astr;
	wrdlen = alen = buf_csz.outlen;

	for (n = i1 = i = 0; ; i++) {
		if (i >= wrdlen) {
			if ((rc = i - i1) > 0 && fwrite(wrdptr + i1, sizeof(char), rc, fout) != rc) {
				wrdlen = -1;
			}
			n += rc;
			break;
		}

		if ((ch = wrdptr[i]) == '\n') {
			if ((rc = i - i1) > 0 && fwrite(wrdptr + i1, sizeof(char), rc, fout) != rc) {
				wrdlen = -1;
				break;
			}

			if (fwrite(buff_crlf, sizeof(char), 2, fout) != 2) {
				wrdlen = -1;
				break;
			}

			i1 += rc + 1; // +1 == '\n'
			n += rc + 2;  // +2 == '\r' '\n'
		}
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return wrdlen;
}

int
fmtproc_as(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_t* lf_ctx)
{
	int ulen, rc;
	const char *astr, *ustr;
	auw_outparam_t buf_csz;

	astr = (char *) va_arg(lf_ctx->args, char *);
	if (astr == NULL) astr = "<NULL>";

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(astr, -1, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		ulen = buf_csz.outlen;
		rc = ustr_num_wchars(ustr, ulen, nilptr);
		rc = uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), ustr, ulen, rc);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}

#endif // __ULS_WINDOWS__

int
ULS_QUALIFIED_METHOD(uls_lf_puts_aufile)(uls_voidptr_t x_dat, const char *wrdstr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;
	int i1, i, m, rc, k = 0, l_buff_crlf;
	char ch, ch2, buff_crlf[4];

#ifdef __ULS_WINDOWS__
	auw_outparam_t buf_csz;
	const char *astr;
	int alen;
#endif

	if (wrdstr == NULL) {
		fflush(fout);
		return 0;
	}

#ifdef __ULS_WINDOWS__
	buff_crlf[0] = '\r';
	buff_crlf[1] = '\n';
	buff_crlf[2] = '\0';
	l_buff_crlf = 2;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(wrdstr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		auw_deinit_outparam(uls_ptr(buf_csz));
		return -1;
	}

	wrdstr = astr;
	wrdlen = alen = buf_csz.outlen;
#else
	buff_crlf[0] = '\n';
	buff_crlf[1] = '\0';
	l_buff_crlf = 1;
#endif

	for (i1 = i = 0; ; ) {
		if (i >= wrdlen) {
			if ((rc = i - i1) > 0 && fwrite(wrdstr + i1, sizeof(char), rc, fout) < rc) {
				k = -1;
			} else {
				k += rc;
			}
			break;
		}

		if ((ch = wrdstr[i]) == '\n' || ch == '\r') {
			if (i + 1 < wrdlen) {
				ch2 = wrdstr[i + 1];
			} else {
				ch2 = '\0';
			}
		} else {
			ch2 = '\0'; // NA
		}

		if ((ch == '\r' && ch2 == '\n') || (ch == '\n' && ch2 == '\r')) {
			m = 2;
		} else if (ch == '\n') {
			m = 1;
		} else {
			m = 0;
		}

		if (m > 0) {
			if ((rc = i - i1) > 0 && fwrite(wrdstr + i1, sizeof(char), rc, fout) < rc) {
				k = -1;
				break;
			}
			i = i1 += rc + m;
			k += rc;

			if (fwrite(buff_crlf, sizeof(char), l_buff_crlf, fout) < l_buff_crlf) {
				k = -1;
				break;
			}
			k += l_buff_crlf;
		} else {
			++i;
		}
	}

#ifdef __ULS_WINDOWS__
	auw_deinit_outparam(uls_ptr(buf_csz));
#endif
	return k;
}

int
ULS_QUALIFIED_METHOD(uls_lf_puts_cons)(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	return uls_lf_puts_aufile(x_dat, wrdptr, wrdlen);
}

#ifndef ULS_DOTNET
int
uls_lf_wputs_csz(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	csz_str_ptr_t csz = (csz_str_ptr_t) x_dat;
	csz_str_t csz_wstr;
	wchar_t *wstr;
	int wlen, rc;

	if (wrdptr == NULL) {
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL) {
		rc = -1;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		rc = wlen * sizeof(wchar_t);
		csz_append(csz, (uls_voidptr_t) wstr, rc);
	}

	csz_deinit(uls_ptr(csz_wstr));

	return rc;
}

int
uls_lf_wputs_str(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	uls_buf4wstr_ptr_t wlb = (uls_buf4wstr_ptr_t) x_dat;
	csz_str_t csz_wstr;
	wchar_t *wstr;
	int wlen, rc;

	if (wrdptr == NULL) {
		if (wlb->wbufsiz <= 0) {
			return -1;
		}
		*wlb->wbufptr = L'\0';
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL ||
		(wlen = auw_csz_wlen(uls_ptr(csz_wstr))) >= wlb->wbufsiz) {
		rc = -1;
	} else {
		rc = wlen * sizeof(wchar_t);
		uls_memcopy(wlb->wbufptr, wstr, rc);
		wlb->wbufptr += wlen;
		wlb->wbufsiz -= wlen;
	}

	csz_deinit(uls_ptr(csz_wstr));

	return rc;
}

int
uls_lf_wputs_file(uls_voidptr_t x_dat, const char *wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;
	csz_str_t csz_wstr;
	wchar_t *wstr;
	int wlen, rc;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL) {
		rc = -1;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		rc = wlen * sizeof(wchar_t);
		if (fwrite(wstr, sizeof(wchar_t), wlen, fout) < (size_t) wlen) {
			rc = -1;
		}
	}

	csz_deinit(uls_ptr(csz_wstr));
	return rc;
}
#endif // ~ULS_DOTNET
