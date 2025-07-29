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
  <file> uls_wprint.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/

#include "uls/uls_lf_swprintf.h"
#include "uls/uls_fileio_wstr.h"
#include "uls/uls_util_wstr.h"
#ifdef __ULS_WINDOWS__
#include "uls/uls_util_astr.h"
#endif
#include "uls/uls_wprint.h"

ULS_DECL_STATIC uls_lf_ptr_t str_wlf, file_wlf, csz_wlf;
ULS_DECL_STATIC uls_lf_ptr_t prn_wlf;
ULS_DECL_STATIC uls_voidptr_t prn_wlf_xdat;
ULS_DECL_STATIC int wsysprn_opened;

// <brief>
// An wide string version of the method 'uls_vprintf()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <return># of chars printed</return>
void
initialize_uls_wprint(void)
{
	str_wlf = uls_wlf_create(nilptr, uls_lf_wputs_str);
	file_wlf = uls_wlf_create(nilptr, uls_lf_puts_aufile);
	csz_wlf = uls_wlf_create(nilptr, uls_lf_wputs_csz);

	prn_wlf = uls_wlf_create(nilptr, uls_lf_puts_file);
	prn_wlf_xdat = nilptr;
	wsysprn_opened = 0;
}

void
finalize_uls_wprint(void)
{
	uls_wlf_destroy(str_wlf);
	uls_wlf_destroy(file_wlf);
	uls_wlf_destroy(csz_wlf);

	prn_wlf_xdat = nilptr;
	uls_wlf_destroy(prn_wlf);
}

int
uls_sysprn_wopen(uls_voidptr_t data, uls_lf_puts_t proc)
{
	uls_lf_delegate_t delegate;

	if (proc == nilptr && data != nilptr) {
		proc = uls_lf_puts_file;
	}

	if (wsysprn_opened) {
		return -1;
	}

	delegate.puts = proc;

	uls_lf_lock(prn_wlf);
	prn_wlf_xdat = data;
	__uls_lf_change_puts(prn_wlf, uls_ptr(delegate));
	wsysprn_opened = 1;

	return 0;
}

// <brief>
// Closes the output file used by uls_sysprn() or uls_wprint().
// </brief>
// <return>none</return>
void
uls_sysprn_wclose(void)
{
	uls_lf_delegate_t delegate;

	if (wsysprn_opened) {
		delegate.puts = uls_lf_puts_null;

		prn_wlf_xdat = nilptr;
		__uls_lf_change_puts(prn_wlf, uls_ptr(delegate));
		wsysprn_opened = 0;
		uls_lf_unlock(prn_wlf);
  	}
}

// <brief>
// An wide string version of the 'uls_vprint()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's</return>
int
uls_vwsysprn(const wchar_t *wfmt, va_list args)
{
	int len;
	len = __uls_lf_vxwprintf(prn_wlf_xdat, prn_wlf, wfmt, args);
	return len;
}

// <brief>
// An wide string version of the 'uls_sysprn()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's</return>
int
uls_wsysprn(const wchar_t *wfmt, ...)
{
	va_list	args;
	int wlen;

	va_start(args, wfmt);
	wlen = uls_vwsysprn(wfmt, args);
	va_end(args);

	return wlen;
}

// <brief>
// Stores a wide string formatted by the string 'wfmt' with 'args' to 'wbuf'.
// </brief>
// <parm name="wbuf">The buffer of wchar_t to store the result.</parm>
// <parm name="wbuf_siz">The size of 'wbuf', # of wchar_t's</parm>
// <parm name="wfmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of wchar_t's int 'wbuf'</return>
int
__uls_lf_vsnwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	uls_buf4wstr_t stdwbuf;
	int len;

	if (wbuf_siz <= 1) {
		if (wbuf_siz == 1) {
			wbuf[0] = L'\0';
		}
		return 0;
	}

	stdwbuf.flags = 0;
	stdwbuf.wbufptr = stdwbuf.wbuf = wbuf;
	stdwbuf.wbufsiz = wbuf_siz;

	len = __uls_lf_vxwprintf(uls_ptr(stdwbuf), uls_lf, wfmt, args);
	if (len > 0) len /= sizeof(wchar_t);
	return len;
}

int
uls_lf_vsnwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int wlen;

	uls_lf_lock(uls_lf);
	wlen = __uls_lf_vsnwprintf(wbuf, wbuf_siz, uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return wlen;
}

int
__uls_lf_snwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = __uls_lf_vsnwprintf(wbuf, wbuf_siz, uls_lf, wfmt, args);
	va_end(args);

	return wlen;
}

int
uls_lf_snwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = uls_lf_vsnwprintf(wbuf, wbuf_siz, uls_lf, wfmt, args);
	va_end(args);

	return wlen;
}

// <brief>
// An wide string version of the method 'uls_snprintf()'
// </brief>
// <parm name="wbuf">The output buffer for the formatted string</parm>
// <parm name="wbufsiz">The size of 'buf', # of wchar_t's</parm>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's</return>

int
__uls_vsnwprintf(wchar_t *wbuf, int wbuf_siz, const wchar_t *wfmt, va_list args)
{
	return __uls_lf_vsnwprintf(wbuf, wbuf_siz, str_wlf, wfmt, args);
}

int
uls_vsnwprintf(wchar_t *wbuf, int wbuf_siz, const wchar_t *wfmt, va_list args)
{
	int wlen;

	uls_lf_lock(str_wlf);
	wlen = __uls_vsnwprintf(wbuf, wbuf_siz, wfmt, args);
	uls_lf_unlock(str_wlf);

	return wlen;
}

int
__uls_snwprintf(wchar_t *wbuf, int wbuf_siz, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = __uls_vsnwprintf(wbuf, wbuf_siz, wfmt, args);
	va_end(args);

	return wlen;
}

int
uls_snwprintf(wchar_t *wbuf, int wbuf_siz, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = uls_vsnwprintf(wbuf, wbuf_siz, wfmt, args);
	va_end(args);

	return wlen;
}

// <brief>
// An wide string version of the method 'uls_fvprintf()'
// </brief>
// <parm name="fp">The pointer of FILE to be wrriten</parm>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's written</return>
int
__uls_lf_vzwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int len;
	len = __uls_lf_vxwprintf(csz, uls_lf, wfmt, args);
	if (len > 0) len /= sizeof(wchar_t);
	return len;
}

int
uls_lf_vzwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int wlen;

	uls_lf_lock(uls_lf);
	wlen = __uls_lf_vzwprintf(csz, uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return wlen;
}

int
__uls_lf_zwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = __uls_lf_vzwprintf(csz, uls_lf, wfmt, args);
	va_end(args);

	return wlen;
}

int
uls_lf_zwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = uls_lf_vzwprintf(csz, uls_lf, wfmt, args);
	va_end(args);

	return wlen;
}

// <brief>
// An wide string version of the method 'uls_fvprintf()'
// </brief>
// <parm name="fp">The pointer of FILE to be wrriten</parm>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's written</return>
int
__uls_vzwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, va_list args)
{
	int len;
	len = __uls_lf_vzwprintf(csz, csz_wlf, wfmt, args);
	return len;
}

int
uls_vzwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, va_list args)
{
	int wlen;

	uls_lf_lock(csz_wlf);
	wlen = __uls_vzwprintf(csz, wfmt, args);
	uls_lf_unlock(csz_wlf);

	return wlen;
}

int
__uls_zwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = __uls_vzwprintf(csz, wfmt, args);
	va_end(args);

	return wlen;
}

int
uls_zwprintf(csz_str_ptr_t csz, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = uls_vzwprintf(csz, wfmt, args);
	va_end(args);

	return wlen;
}

// <brief>
// An wide string version of the method 'uls_fvprintf()'
// </brief>
// <parm name="fp">The pointer of FILE to be wrriten</parm>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's written</return>
int
__uls_lf_vfwprintf(FILE *fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int len;
	len = __uls_lf_vxwprintf(fp, uls_lf, wfmt, args);
	return len;
}

int
uls_lf_vfwprintf(FILE *fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vfwprintf(fp, uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_fwprintf(FILE *fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = __uls_lf_vfwprintf(fp, uls_lf, wfmt, args);
	va_end(args);

	return len;
}

int
uls_lf_fwprintf(FILE *fp, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = uls_lf_vfwprintf(fp, uls_lf, wfmt, args);
	va_end(args);

	return len;
}

// <brief>
// An wide string version of the method 'uls_fprintf()'
// </brief>
// <parm name="fp">The pointer of FILE to be wrriten</parm>
// <parm name="wfmt">The template for message string</parm>
// <return># of wchar_t's written</return>
int
__uls_vfwprintf(FILE *fp, const wchar_t *wfmt, va_list args)
{
	int aulen;
	aulen = __uls_lf_vfwprintf(fp, file_wlf, wfmt, args);
	return aulen;
}

int
uls_vfwprintf(FILE *fp, const wchar_t *wfmt, va_list args)
{
	int aulen;

	uls_lf_lock(file_wlf);
	aulen = __uls_vfwprintf(fp, wfmt, args);
	uls_lf_unlock(file_wlf);

	return aulen;
}

int
__uls_fwprintf(FILE *fp, const wchar_t *wfmt, ...)
{
	va_list args;
	int aulen;

	va_start(args, wfmt);
	aulen = __uls_vfwprintf(fp, wfmt, args);
	va_end(args);

	return aulen;
}

int
uls_fwprintf(FILE *fp, const wchar_t *wfmt, ...)
{
	va_list args;
	int aulen;

	va_start(args, wfmt);
	aulen = uls_vfwprintf(fp, wfmt, args);
	va_end(args);

	return aulen;
}

// <brief>
// An wide string version of the method 'uls_vprintf()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <return># of chars printed</return>

int
__uls_lf_vwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	return __uls_lf_vfwprintf(_uls_stdio_fp(1), uls_lf, wfmt, args);
}

int
uls_lf_vwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vwprintf(uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_wprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = __uls_lf_vwprintf(uls_lf, wfmt, args);
	va_end(args);

	return len;
}

int
uls_lf_wprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = uls_lf_vwprintf(uls_lf, wfmt, args);
	va_end(args);

	return len;
}

// <brief>
// An wide string version of the method 'uls_printf()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <return># of chars printed</return>

int
__uls_vwprintf(const wchar_t *wfmt, va_list args)
{
	return __uls_vfwprintf(_uls_stdio_fp(1), wfmt, args);
}

int
uls_vwprintf(const wchar_t *wfmt, va_list args)
{
	int aulen;

	uls_lf_lock(file_wlf);
	aulen = __uls_vwprintf(wfmt, args);
	uls_lf_unlock(file_wlf);

	return aulen;
}

int
__uls_wprintf(const wchar_t *wfmt, ...)
{
	va_list args;
	int aulen;

	va_start(args, wfmt);
	aulen = __uls_vwprintf(wfmt, args);
	va_end(args);

	return aulen;
}

int
uls_wprintf(const wchar_t *wfmt, ...)
{
	va_list args;
	int aulen;

	va_start(args, wfmt);
	aulen = uls_vwprintf(wfmt, args);
	va_end(args);

	return aulen;
}
