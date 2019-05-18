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
  <file> uls_alog.c </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#define __ULS_ALOG__
#include "uls/uls_alog.h"

#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"
#include "uls/uls_util_astr.h"

#include "uls/uls_alex.h"
#include "uls/uls_util_astr.h"

ULS_DECL_STATIC uls_lf_map_t logdfl_convspec_amap;
ULS_DECL_STATIC uls_lf_map_t log_convspec_amap;
ULS_DECL_STATIC uls_lf_ptr_t dfl_syslog_alf;
ULS_DECL_STATIC csz_str_t abuff_csz;

ULS_DECL_STATIC void
__ulslog_aflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
{
	char *wrdptr;
	int wrdlen;

	wrdptr = csz_text(csz);
	wrdlen = csz_length(csz);

	lf_puts(data, wrdptr, wrdlen);
	lf_puts(data, "\n", 1);
	lf_puts(data, NULL, 0);

	csz_reset(csz);
}

int
ulslog_vaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_alf, const char* afmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int len;

	uls_lf_lock(uls_alf);

	delegate.xdat = csz;
	delegate.puts = uls_lf_puts_csz;
	__uls_lf_change_puts(uls_alf, uls_ptr(delegate));
	len = __uls_lf_vxaprintf(uls_alf, afmt, args);
	__uls_lf_change_puts(uls_alf, uls_ptr(delegate));

	uls_lf_unlock(uls_alf);

	return len;
}

int
ulslog_aprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_alf, const char* afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = ulslog_vaprintf(csz, uls_alf, afmt, args);
	va_end(args);

	return len;
}

void
ulslog_aflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
{
	err_syslog_lock();
	__ulslog_aflush(csz, data, lf_puts);
	err_syslog_unlock();
}

void
initialize_uls_alog(void)
{
	uls_lf_puts_t proc_log;

	uls_lf_init_convspec_amap(uls_ptr(logdfl_convspec_amap), 0);
	uls_add_default_convspecs(uls_ptr(logdfl_convspec_amap));

	uls_lf_init_convspec_amap(uls_ptr(log_convspec_amap), 0);
	uls_add_default_log_convspecs(uls_ptr(log_convspec_amap));

#ifdef ULS_WINDOWS
	proc_log = uls_lf_aputs_file;
#else
	proc_log = uls_lf_puts_file;
#endif
	dfl_syslog_alf = uls_alf_create(uls_ptr(logdfl_convspec_amap), _uls_stdio_fp(2), proc_log);

	csz_init(uls_ptr(abuff_csz), 128);
}

void
finalize_uls_alog(void)
{
	csz_deinit(uls_ptr(abuff_csz));
	uls_alf_destroy(dfl_syslog_alf);
	uls_lf_deinit_convspec_amap(uls_ptr(log_convspec_amap));
	uls_lf_deinit_convspec_amap(uls_ptr(logdfl_convspec_amap));
}

ULS_DLL_EXTERN void
err_valog(const char* afmt, va_list args)
{
	ulslog_vaprintf(uls_ptr(abuff_csz), dfl_syslog_alf, afmt, args);
	ulslog_aflush(uls_ptr(abuff_csz), dfl_syslog_alf->x_dat, dfl_syslog_alf->uls_lf_puts);
}

ULS_DLL_EXTERN void
err_alog(const char* afmt, ...)
{
	va_list	args;

	va_start(args, afmt);
	err_valog(afmt, args);
	va_end(args);
}

ULS_DLL_EXTERN void
err_vapanic(const char* afmt, va_list args)
{
	err_valog(afmt, args);
	err_panic("");
}

ULS_DLL_EXTERN void
err_apanic(const char* afmt, ...)
{
	va_list	args;

	va_start(args, afmt);
	err_valog(afmt, args);
	va_end(args);
	err_panic("");
}

// <brief>
// A method that emits MS-MBCS string message after formatting the string 'afmt' with 'args'
// </brief>
// <parm name="log">the log object created by uls_create_log()</parm>
// <parm name="afmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
// <return>none</return>
ULS_DLL_EXTERN void
uls_valog(uls_log_ptr_t log, const char* afmt, va_list args)
{
	uls_alog_shell_ptr_t alog_shell = (uls_alog_shell_ptr_t) log->shell;
	uls_voidptr_t old_gdat;
	int len;

	if (log == nilptr || log->uls == nilptr) {
		err_valog(afmt, args);
		return;
	}

	uls_log_lock(log);
	old_gdat = uls_lf_change_gdat(log->lf, log->uls);

	len = ulslog_aprintf(uls_ptr(alog_shell->astr_buff), log->lf, "[ULS] [%s:%d] ",
		uls_get_tag2_astr(log->uls, NULL), uls_get_lineno(log->uls));
	len += ulslog_vaprintf(uls_ptr(alog_shell->astr_buff), log->lf, afmt, args);

	ulslog_aflush(uls_ptr(alog_shell->astr_buff), log->log_port, log->log_puts);

	uls_lf_change_gdat(log->lf, old_gdat);
	uls_log_unlock(log);
}

// <brief>
// An MS-MBCS string version of the method 'uls_log()'
// </brief>
// <parm name="log">the log object created by uls_create_log()</parm>
// <parm name="afmt">The template for message string</parm>
// <parm name="...">varargs</parm>
// <return>none</return>
ULS_DLL_EXTERN void
uls_alog(uls_log_ptr_t log, const char* afmt, ...)
{
	va_list	args;

	va_start(args, afmt);
	uls_valog(log, afmt, args);
	va_end(args);
}

ULS_DLL_EXTERN void
uls_vapanic(uls_log_ptr_t log, const char* afmt, va_list args)
{
	uls_valog(log, afmt, args);
	err_panic("");
}

// <brief>
// An MS-MBCS string version of the method 'uls_panic()'
// </brief>
// <parm name="afmt">The template for message string</parm>
// <parm name="...">varargs</parm>
// <return>none</return>
ULS_DLL_EXTERN void
uls_apanic(uls_log_ptr_t log, const char* afmt, ...)
{
	va_list	args;

	va_start(args, afmt);
	uls_valog(log, afmt, args);
	va_end(args);
	err_panic("");
}

ULS_DLL_EXTERN int
uls_init_alog(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	uls_alog_shell_ptr_t alog_shell;
	uls_alf_shell_ptr_t alf_shell;

	if (lf_map == nilptr) lf_map = uls_ptr(log_convspec_amap);

	if (uls_init_log(log, lf_map, uls) < 0) {
		return -1;
	}
	alf_shell = uls_alloc_object(uls_alf_shell_t);
	alf_shell_init(alf_shell, log->lf);

	log->shell = alog_shell = uls_alloc_object(uls_alog_shell_t);
	csz_init(uls_ptr(alog_shell->astr_buff), 128);

	return 0;
}

ULS_DLL_EXTERN void
uls_deinit_alog(uls_log_ptr_t log)
{
	uls_lf_ptr_t uls_alf = log->lf;
	uls_alf_shell_ptr_t alf_shell = (uls_alf_shell_ptr_t ) uls_alf->shell;
	uls_alog_shell_ptr_t alog_shell = (uls_alog_shell_ptr_t) log->shell;

	csz_deinit(uls_ptr(alog_shell->astr_buff));
	uls_dealloc_object(log->shell);
	log->shell = nilptr;

	alf_shell_deinit(alf_shell);
	uls_dealloc_object(alf_shell);
	uls_alf->shell = nilptr;

	uls_deinit_log(log);
}

ULS_DLL_EXTERN uls_log_ptr_t
uls_create_alog(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	uls_log_ptr_t log;

	log = uls_alloc_object(uls_log_t);

	if (uls_init_alog(log, lf_map, uls) < 0) {
		uls_dealloc_object(log);
		return nilptr;
	}

	log->flags &= ~ULS_FL_STATIC;

	return log;
}

ULS_DLL_EXTERN void
uls_destroy_alog(uls_log_ptr_t log)
{
	uls_deinit_alog(log);

	if ((log->flags & ULS_FL_STATIC) == 0) {
		uls_dealloc_object(log);
	}
}
