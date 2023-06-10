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
  <file> uls_aprint.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#define __ULS_APRINT__
#include "uls/uls_aprint.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_fileio.h"

ULS_DECL_STATIC uls_lf_ptr_t str_alf, file_alf, csz_alf;
ULS_DECL_STATIC uls_lf_ptr_t prn_alf;
ULS_DECL_STATIC int asysprn_opened;

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// to stdout, the standard output.
// </brief>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="...">varargs</parm>
// <return># of bytes in abuf</return>
void
initialize_uls_aprint(void)
{
	uls_lf_puts_t proc_str, proc_file, proc_csz;

#ifdef ULS_WINDOWS
	proc_str = uls_lf_aputs_str;
	proc_file = uls_lf_aputs_file;
	proc_csz = uls_lf_aputs_csz;
#else
	proc_str = uls_lf_puts_str;
	proc_file = uls_lf_puts_file;
	proc_csz = uls_lf_puts_csz;
#endif

	str_alf = uls_alf_create(nilptr, nilptr, proc_str);
	file_alf = uls_alf_create(nilptr, _uls_stdio_fp(1), proc_file);
	csz_alf = uls_alf_create(nilptr, nilptr, proc_csz);

	prn_alf = uls_alf_create(nilptr, _uls_stdio_fp(1), proc_file);
	asysprn_opened = 0;
}

void
finalize_uls_aprint(void)
{
	uls_alf_destroy(str_alf);
	uls_alf_destroy(file_alf);
	uls_alf_destroy(csz_alf);
	uls_alf_destroy(prn_alf);
}

ULS_DLL_EXTERN int
uls_sysprn_aopen(uls_voidptr_t data, uls_lf_puts_t proc)
{
	uls_lf_delegate_t delegate;

	if (proc == nilptr && data != nilptr) {
#ifdef ULS_WINDOWS
		proc = uls_lf_aputs_file;
#else
		proc = uls_lf_puts_file;
#endif
	}

	if (asysprn_opened) {
		return -1;
	}

	delegate.xdat = data;
	delegate.puts = proc;

	uls_lf_lock(prn_alf);
	__uls_lf_change_puts(prn_alf, uls_ptr(delegate));
	asysprn_opened = 1;

	return 0;
}

// <brief>
// Closes the output file used by uls_sysprn() or uls_wprint().
// </brief>
// <return>none</return>
ULS_DLL_EXTERN void
uls_sysprn_aclose(void)
{
	uls_lf_delegate_t delegate;

	if (asysprn_opened) {
		delegate.xdat = nilptr;
		delegate.puts = uls_lf_puts_null;

		__uls_lf_change_puts(prn_alf, uls_ptr(delegate));
		asysprn_opened = 0;
		uls_lf_unlock(prn_alf);
  	}
}

// <brief>
// An MS-MBCS string version of the 'uls_vprint()'
// </brief>
// <parm name="afmt">The template for message string</parm>
// <return># of bytes</return>
ULS_DLL_EXTERN int
uls_vasysprn(const char* afmt, va_list args)
{
	return __uls_lf_vxaprintf(prn_alf, afmt, args);
}

// <brief>
// An MS-MBCS string version of the 'uls_sysprn()'
// </brief>
// <parm name="afmt">The template for message string</parm>
// <return># of bytes</return>
ULS_DLL_EXTERN int
uls_asysprn(const char* afmt, ...)
{
	va_list	args;
	int alen;

	va_start(args, afmt);
	alen = uls_vasysprn(afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// </brief>
// <parm name="abuf">The buffer to store the result.</parm>
// <parm name="buf_siz">The size of 'abuf', # of bytes</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_lf_vsnaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	uls_buf4str_t stdbuf;
	uls_lf_delegate_t delegate;
	int alen;

	if (abuf_siz <= 1) {
		if (abuf_siz==1) {
			abuf[0] = '\0';
		}
		return 0;
	}

	stdbuf.flags = 0;
	stdbuf.bufptr = stdbuf.buf = abuf;
	stdbuf.bufsiz = abuf_siz;

	delegate.xdat = uls_ptr(stdbuf);
	delegate.puts = uls_lf_aputs_str;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	alen = __uls_lf_vxaprintf(uls_lf, afmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_vsnaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(uls_lf);
	alen = __uls_lf_vsnaprintf(abuf, abuf_siz, uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_lf_snaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_lf_vsnaprintf(abuf, abuf_siz, uls_lf, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_snaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_lf_vsnaprintf(abuf, abuf_siz, uls_lf, afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' from varargs.
// </brief>
// <parm name="abuf">The buffer to store the result.</parm>
// <parm name="buf_siz">The size of 'abuf', # of bytes</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="...">varargs</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_vsnaprintf(char* abuf, int abuf_siz, const char *afmt, va_list args)
{
	return __uls_lf_vsnaprintf(abuf, abuf_siz, str_alf, afmt, args);
}

ULS_DLL_EXTERN int
uls_vsnaprintf(char* abuf, int abuf_siz, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(str_alf);
	alen = __uls_vsnaprintf(abuf, abuf_siz, afmt, args);
	uls_lf_unlock(str_alf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_snaprintf(char* abuf, int abuf_siz, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_vsnaprintf(abuf, abuf_siz, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_snaprintf(char* abuf, int abuf_siz, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_vsnaprintf(abuf, abuf_siz, afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// </brief>
// <parm name="abuf">The buffer to store the result.</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_lf_vzaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int alen;

	delegate.xdat = csz;
	delegate.puts = uls_lf_aputs_csz;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	alen = __uls_lf_vxaprintf(uls_lf, afmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_vzaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(uls_lf);
	alen = __uls_lf_vzaprintf(csz, uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_lf_zaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_lf_vzaprintf(csz, uls_lf, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_zaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_lf_vzaprintf(csz, uls_lf, afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// </brief>
// <parm name="abuf">The buffer to store the result.</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_vzaprintf(csz_str_t* csz, const char *afmt, va_list args)
{
	return __uls_lf_vxaprintf_generic(csz, csz_alf, afmt, args);
}


ULS_DLL_EXTERN int
uls_vzaprintf(csz_str_t* csz, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(csz_alf);
	alen = __uls_vzaprintf(csz, afmt, args);
	uls_lf_unlock(csz_alf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_zaprintf(csz_str_ptr_t csz, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_vzaprintf(csz, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_zaprintf(csz_str_ptr_t csz, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_vzaprintf(csz, afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// to file-pointer fp.
// </brief>
// <parm name="fp">the file poiner indicating output file</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_lf_vfaprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int alen;

	delegate.xdat = fp;
	delegate.puts = uls_lf_aputs_file;
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	alen = __uls_lf_vxaprintf(uls_lf, afmt, args);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_vfaprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(uls_lf);
	alen = __uls_lf_vfaprintf(fp, uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_lf_faprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_lf_vfaprintf(fp, uls_lf, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_faprintf(FILE* fp, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_lf_vfaprintf(fp, uls_lf, afmt, args);
	va_end(args);

	return alen;
}


// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// to file-pointer fp.
// </brief>
// <parm name="fp">the file poiner indicating output file</parm>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_vfaprintf(FILE* fp, const char *afmt, va_list args)
{
	return __uls_lf_vxaprintf_generic(fp, file_alf, afmt, args);
}

ULS_DLL_EXTERN int
uls_vfaprintf(FILE* fp, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(file_alf);
	alen = __uls_vfaprintf(fp, afmt, args);
	uls_lf_unlock(file_alf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_faprintf(FILE* fp, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_vfaprintf(fp, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_faprintf(FILE* fp, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_vfaprintf(fp, afmt, args);
	va_end(args);

	return alen;
}

// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// to stdout, the standard output.
// </brief>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="args">The list of args</parm>
// <return># of bytes in abuf</return>

ULS_DLL_EXTERN int
__uls_lf_vaprintf(uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	return __uls_lf_vfaprintf(_uls_stdio_fp(1), uls_lf, afmt, args);
}

ULS_DLL_EXTERN int
uls_lf_vaprintf(uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(uls_lf);
	alen = __uls_lf_vaprintf(uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_lf_aprintf(uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_lf_vaprintf(uls_lf, afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_lf_aprintf(uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_lf_vaprintf(uls_lf, afmt, args);
	va_end(args);

	return alen;
}


// <brief>
// Stores a MS-MBCS string formatted by the string 'afmt' with 'args' to 'abuf'.
// to stdout, the standard output.
// </brief>
// <parm name="afmt">The format string, template for printing</parm>
// <parm name="...">varargs</parm>
// <return># of bytes in abuf</return>
ULS_DLL_EXTERN int
__uls_vaprintf(const char *afmt, va_list args)
{
	return __uls_vfaprintf(_uls_stdio_fp(1), afmt, args);
}

ULS_DLL_EXTERN int
uls_vaprintf(const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(file_alf);
	alen = __uls_vaprintf(afmt, args);
	uls_lf_unlock(file_alf);

	return alen;
}

ULS_DLL_EXTERN int
__uls_aprintf(const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = __uls_vaprintf(afmt, args);
	va_end(args);

	return alen;
}

ULS_DLL_EXTERN int
uls_aprintf(const char *afmt, ...)
{
	va_list args;
	int alen;

	va_start(args, afmt);
	alen = uls_vaprintf(afmt, args);
	va_end(args);

	return alen;
}
