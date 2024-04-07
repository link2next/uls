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
  <file> uls_wlog.c </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#define __ULS_WLOG__
#include "uls/uls_wlog.h"

#include "uls/uls_auw.h"
#include "uls/uls_fileio.h"

#include "uls/uls_wlex.h"
#include "uls/uls_util.h"
#include "uls/uls_util_wstr.h"

ULS_DECL_STATIC uls_lf_map_t logdfl_convspec_wmap;
ULS_DECL_STATIC uls_lf_map_t log_convspec_wmap;
ULS_DECL_STATIC uls_lf_ptr_t dfl_syslog_wlf;
ULS_DECL_STATIC csz_str_t wbuff_csz;


ULS_DECL_STATIC void
__ulslog_wflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
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
ulslog_vwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_wlf, const wchar_t *wfmt, va_list args)
{
	uls_lf_delegate_t delegate;
	int wlen;

	uls_lf_lock(uls_wlf);

	delegate.xdat = csz;
	delegate.puts = uls_lf_puts_csz;
	__uls_lf_change_puts(uls_wlf, uls_ptr(delegate));
	wlen = __uls_lf_vxwprintf(uls_wlf, wfmt, args);
	__uls_lf_change_puts(uls_wlf, uls_ptr(delegate));

	uls_lf_unlock(uls_wlf);

	return wlen;
}

int
ulslog_wprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_wlf, const wchar_t *wfmt, ...)
{
	va_list args;
	int wlen;

	va_start(args, wfmt);
	wlen = ulslog_vwprintf(csz, uls_wlf, wfmt, args);
	va_end(args);

	return wlen;
}

void
ulslog_wflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
{
	err_syslog_lock();
	__ulslog_wflush(csz, data, lf_puts);
	err_syslog_unlock();
}

void
initialize_uls_wlog(void)
{
	uls_lf_puts_t proc_log;

	uls_lf_init_convspec_wmap(uls_ptr(logdfl_convspec_wmap), 0);
	uls_add_default_convspecs(uls_ptr(logdfl_convspec_wmap));

	uls_lf_init_convspec_wmap(uls_ptr(log_convspec_wmap), 0);
	uls_add_default_log_convspecs(uls_ptr(log_convspec_wmap));

	proc_log = uls_lf_puts_file;
	dfl_syslog_wlf = uls_wlf_create(uls_ptr(logdfl_convspec_wmap), _uls_stdio_fp(2), proc_log);

	csz_init(uls_ptr(wbuff_csz), 128 * sizeof(wchar_t));
}

void
finalize_uls_wlog(void)
{
	csz_deinit(uls_ptr(wbuff_csz));
	uls_wlf_destroy(dfl_syslog_wlf);
	uls_lf_deinit_convspec_wmap(uls_ptr(log_convspec_wmap));
	uls_lf_deinit_convspec_wmap(uls_ptr(logdfl_convspec_wmap));
}

void
err_vwlog(const wchar_t* wfmt, va_list args)
{
	ulslog_vwprintf(uls_ptr(wbuff_csz), dfl_syslog_wlf, wfmt, args);
	ulslog_wflush(uls_ptr(wbuff_csz), dfl_syslog_wlf->x_dat, dfl_syslog_wlf->uls_lf_puts);
}

void
err_wlog(const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	err_vwlog(wfmt, args);
	va_end(args);
}

void
err_vwpanic(const wchar_t* wfmt, va_list args)
{
	err_vwlog(wfmt, args);
	err_panic("");
}

void
err_wpanic(const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	err_vwlog(wfmt, args);
	va_end(args);
	err_panic("");
}

// <brief>
// A method that emits wide string message after formatting the string 'wfmt' with 'args'
// </brief>
// <parm name="log">the log object created by uls_create_log()</parm>
// <parm name="wfmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
// <return>none</return>
void
uls_vwlog(uls_log_ptr_t log, const wchar_t* wfmt, va_list args)
{
	uls_wlog_shell_ptr_t wlog_shell = (uls_wlog_shell_ptr_t) log->shell;
	uls_voidptr_t old_gdat;
	int len;

	if (log == nilptr || log->uls == nilptr) {
		err_vwlog(wfmt, args);
		return;
	}

	uls_log_lock(log);
	old_gdat = uls_lf_change_gdat(log->lf, log->uls);

	len = ulslog_wprintf(uls_ptr(wlog_shell->wstr_buff), log->lf, L"[ULS] [%s:%d] ",
		uls_get_tag2_wstr(log->uls, NULL), uls_get_lineno(log->uls));
	len += ulslog_vwprintf(uls_ptr(wlog_shell->wstr_buff), log->lf, wfmt, args);

	ulslog_wflush(uls_ptr(wlog_shell->wstr_buff), log->log_port, log->log_puts);

	uls_lf_change_gdat(log->lf, old_gdat);
	uls_log_unlock(log);
}

// <brief>
// An wide string version of the method 'uls_log()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <parm name="...">varargs</parm>
// <return>none</return>
void
uls_wlog(uls_log_ptr_t log, const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	uls_vwlog(log, wfmt, args);
	va_end(args);
}

void
uls_vwpanic(uls_log_ptr_t log, const wchar_t* wfmt, va_list args)
{
	uls_vwlog(log, wfmt, args);
	err_panic("");
}

// <brief>
// An wide string version of the method 'uls_panic()'
// </brief>
// <parm name="wfmt">The template for message string</parm>
// <parm name="...">varargs</parm>
// <return>none</return>
void
uls_wpanic(uls_log_ptr_t log, const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	uls_vwlog(log, wfmt, args);
	va_end(args);
	err_panic("");
}

int
uls_init_wlog(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	uls_wlog_shell_ptr_t wlog_shell;
	uls_wlf_shell_ptr_t wlf_shell;

	if (lf_map == nilptr) lf_map = uls_ptr(log_convspec_wmap);

	if (uls_init_log(log, lf_map, uls) < 0) {
		return -1;
	}
	wlf_shell = uls_alloc_object(uls_wlf_shell_t);
	wlf_shell_init(wlf_shell, log->lf);

	log->shell = wlog_shell = uls_alloc_object(uls_wlog_shell_t);
	csz_init(uls_ptr(wlog_shell->wstr_buff), 128*sizeof(wchar_t));

	return 0;
}

void
uls_deinit_wlog(uls_log_ptr_t log)
{
	uls_lf_ptr_t uls_wlf = log->lf;
	uls_wlf_shell_ptr_t wlf_shell = (uls_wlf_shell_ptr_t) uls_wlf->shell;
	uls_wlog_shell_ptr_t wlog_shell = (uls_wlog_shell_ptr_t) log->shell;

	csz_deinit(uls_ptr(wlog_shell->wstr_buff));
	uls_dealloc_object(log->shell);
	log->shell = nilptr;

	wlf_shell_deinit(wlf_shell);
	uls_dealloc_object(wlf_shell);
	uls_wlf->shell = nilptr;

	uls_deinit_log(log);
}

uls_log_ptr_t
uls_create_wlog(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	uls_log_ptr_t log;

	log = uls_alloc_object(uls_log_t);

	if (uls_init_wlog(log, lf_map, uls) < 0) {
		uls_dealloc_object(log);
		return nilptr;
	}

	log->flags &= ~ULS_FL_STATIC;

	return log;
}

void
uls_destroy_wlog(uls_log_ptr_t log)
{
	uls_deinit_wlog(log);

	if ((log->flags & ULS_FL_STATIC) == 0) {
		uls_dealloc_object(log);
	}
}

