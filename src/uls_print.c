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
  <file> uls_print.c </file>
  <brief>
    Vararg routines for printing output to files.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#define __ULS_PRINT__
#include "uls/uls_print.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"

void
initialize_uls_sysprn(void)
{
	dfl_str_lf = uls_lf_create(nilptr, nilptr, uls_lf_puts_str);
	dfl_file_lf = uls_lf_create(nilptr, _uls_stdio_fp(1), uls_lf_puts_file);
	dfl_csz_lf = uls_lf_create(nilptr, nilptr, uls_lf_puts_csz);

	dfl_sysprn_lf = uls_lf_create(nilptr, nilptr, uls_lf_puts_null);
	sysprn_opened = 0;
}

void
finalize_uls_sysprn(void)
{
	uls_lf_destroy(dfl_sysprn_lf);

	uls_lf_destroy(dfl_str_lf);
	uls_lf_destroy(dfl_file_lf);
	uls_lf_destroy(dfl_csz_lf);
}

// <brief>
// Opens a file for output.
// After calls of print() or wprint(), be sure to call uls_sysprn_close().
// </brief>
// <parm name="out_file">The output file path</parm>
// <return>none</return>
int
uls_sysprn_open(uls_voidptr_t data, uls_lf_puts_t proc)
{
	uls_lf_delegate_t delegate;

	if (proc == nilptr) {
		if (data == nilptr) {
			proc = uls_lf_puts_null;
		} else {
			proc = uls_lf_puts_file;
		}
	}

	if (sysprn_opened) {
		return -1;
	}

	delegate.xdat = data;
	delegate.puts = proc;

	uls_lf_lock(dfl_sysprn_lf);
	__uls_lf_change_puts(dfl_sysprn_lf, uls_ptr(delegate));

	sysprn_opened = 1;
	uls_sysprn_set_tabsiz(0);

	return 0;
}

void
uls_sysprn_set_tabsiz(int tabsiz)
{
	char ch_tab;

	if (!sysprn_opened) return;

	if (tabsiz > 0) {
		if (tabsiz > ULS_SYSPRN_TABBUF_SIZE) {
			tabsiz = ULS_SYSPRN_TABBUF_SIZE;
		}
		ch_tab = ' ';
	} else {
		ch_tab = '\t';
		tabsiz = 1;
	}

	sysprn_ntabs = ULS_SYSPRN_TABBUF_SIZE / tabsiz;
	sysprn_tabsiz = tabsiz;
	sysprn_tabbuf_len = sysprn_tabsiz * sysprn_ntabs;

	uls_memset(sysprn_tabbuf, ch_tab, sysprn_tabbuf_len);
}

// <brief>
// Closes the output file used by uls_sysprn() or uls_wprint().
// </brief>
// <return>none</return>
void
uls_sysprn_close(void)
{
	uls_lf_delegate_t delegate;

	if (sysprn_opened) {
		delegate.xdat = nilptr;
		delegate.puts = uls_lf_puts_null;

		__uls_lf_change_puts(dfl_sysprn_lf, uls_ptr(delegate));
		sysprn_opened = 0;
		uls_lf_unlock(dfl_sysprn_lf);
  	}
}

// <brief>
// This will print the formatted string to the standard oupput, by default.
// But you can change the output port by calling
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
uls_vsysprn(const char* fmt, va_list args)
{
	return __uls_lf_vxprintf(dfl_sysprn_lf, fmt, args);
}

int
uls_sysprn(const char* fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = uls_vsysprn(fmt, args);
	va_end(args);

	return len;
}

void
uls_sysprn_puttabs(int n)
{
	uls_voidptr_t sysprn = dfl_sysprn_lf->x_dat;
	uls_lf_puts_t sysprn_puts = dfl_sysprn_lf->uls_lf_puts;
	int i, buf_len = sysprn_tabbuf_len;

	if (n <= 0) return;

	for (i=0; i < n / sysprn_ntabs; i++) {
		sysprn_puts(sysprn, sysprn_tabbuf, buf_len);
	}

	if ((i = n % sysprn_ntabs) > 0) {
		buf_len = i * sysprn_tabsiz;
		sysprn_puts(sysprn, sysprn_tabbuf, buf_len);
	}
}

void
uls_sysprn_tabs(int n_tabs, char *fmt, ...)
{
	va_list args;

	uls_sysprn_puttabs(n_tabs);

	va_start(args, fmt);
	uls_vsysprn(fmt, args);
	va_end(args);
}

// <brief>
// This stores the formatted string to the 'buf'.
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="bufsiz">The size of 'buf'</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled except for '\0'</return>

int
__uls_lf_vsnprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	uls_buf4str_t stdbuf;

	uls_lf_delegate_t delegate;
	int len;

	if (bufsiz <= 1) {
		if (bufsiz==1) {
			buf[0] = '\0';
		}
		return 0;
	}

	stdbuf.flags = 0;
	stdbuf.bufptr = stdbuf.buf = buf;
	stdbuf.bufsiz = bufsiz - 1;

	if (uls_lf == nilptr) uls_lf = dfl_str_lf;

	delegate.xdat = uls_ptr(stdbuf);
	delegate.puts = uls_lf_puts_str;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return len;
}

int
uls_lf_vsnprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vsnprintf(buf, bufsiz, uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_snprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vsnprintf(buf, bufsiz, uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
uls_lf_snprintf(char* buf, int bufsiz, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vsnprintf(buf, bufsiz, uls_lf, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'buf'.
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="bufsiz">The size of 'buf'</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled except for '\0'</return>

int
__uls_vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args)
{
	return __uls_lf_vsnprintf(buf, bufsiz, dfl_str_lf, fmt, args);
}

int
uls_vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(dfl_str_lf);
	len = __uls_vsnprintf(buf, bufsiz, fmt, args);
	uls_lf_unlock(dfl_str_lf);

	return len;
}

int
__uls_snprintf(char* buf, int bufsiz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_vsnprintf(buf, bufsiz, fmt, args);
	va_end(args);

	return len;
}

int
uls_snprintf(char* buf, int bufsiz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_vsnprintf(buf, bufsiz, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'buf' with its size unknown.
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled</return>
int
__uls_lf_vzprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int len;

	delegate.xdat = csz;
	delegate.puts = uls_lf_puts_csz;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return len;
}

int
uls_lf_vzprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vzprintf(csz, uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_zprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vzprintf(csz, uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
uls_lf_zprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vzprintf(csz, uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
__uls_vzprintf(csz_str_ptr_t csz, const char *fmt, va_list args)
{
	return __uls_lf_vzprintf(csz, dfl_csz_lf, fmt, args);
}

int
uls_vzprintf(csz_str_ptr_t csz, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(dfl_csz_lf);
	len = __uls_vzprintf(csz, fmt, args);
	uls_lf_unlock(dfl_csz_lf);

	return len;
}

int
__uls_zprintf(csz_str_ptr_t csz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_vzprintf(csz, fmt, args);
	va_end(args);

	return len;
}

int
uls_zprintf(csz_str_ptr_t csz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_vzprintf(csz, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'fp', FILE*.
// </brief>
// <parm name="fp">The pointer of FILE to be wrriten</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars written</return>
int
__uls_lf_vfprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int len;

	delegate.xdat = fp;
	delegate.puts = uls_lf_puts_file;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return len;
}

int
uls_lf_vfprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vfprintf(fp, uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_fprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vfprintf(fp, uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
uls_lf_fprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vfprintf(fp, uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
__uls_vfprintf(FILE* fp, const char *fmt, va_list args)
{
	return uls_lf_vxprintf_generic(fp, dfl_file_lf, fmt, args);
}

int
uls_vfprintf(FILE* fp, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(dfl_file_lf);
	len =  __uls_vfprintf(fp, fmt, args);
	uls_lf_unlock(dfl_file_lf);

	return len;
}

int
__uls_fprintf(FILE* fp, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_vfprintf(fp, fmt, args);
	va_end(args);

	return len;
}

int
uls_fprintf(FILE* fp, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_vfprintf(fp, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This prints the formatted string to the 'stdout'.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
__uls_lf_vprintf(uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	return __uls_lf_vfprintf(_uls_stdio_fp(1), uls_lf, fmt, args);
}

int
uls_lf_vprintf(uls_lf_ptr_t uls_lf, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vprintf(uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_printf(uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vprintf(uls_lf, fmt, args);
	va_end(args);

	return len;
}

int
uls_lf_printf(uls_lf_ptr_t uls_lf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vprintf(uls_lf, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This prints the formatted string to the 'stdout'.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
__uls_vprintf(const char *fmt, va_list args)
{
	return __uls_vfprintf(_uls_stdio_fp(1), fmt, args);
}

int
uls_vprintf(const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(dfl_file_lf);
	len = __uls_vprintf(fmt, args);
	uls_lf_unlock(dfl_file_lf);

	return len;
}

int
__uls_printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_vprintf(fmt, args);
	va_end(args);

	return len;
}

int
uls_printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_vprintf(fmt, args);
	va_end(args);

	return len;
}
