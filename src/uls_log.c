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
  <file> uls_log.c </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, August 2011.
  </author>
*/
#include "uls/uls_lex.h"
#define __ULS_LOG__
#include "uls/uls_log.h"
#include "uls/uls_misc.h"

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fmtproc_coord)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx)
{
	char buf[81];
	int len, n_chars;

	if (uls == nilptr) {
		len = uls_strcpy(buf, "<fault>");
	} else {
		len = __uls_lf_snprintf(buf, sizeof(buf), nilptr, "%s:%04d", __uls_get_tag(uls), __uls_get_lineno(uls));
	}

	n_chars = ustr_num_wchars(buf, len, nilptr);
	return uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(ctx->perfmt), buf, len, n_chars);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fmtproc_tokname)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx)
{
	const char *wrdptr;
	int len, n_chars, t;
	uls_outparam_t parms1;

	if (uls == nilptr) {
		wrdptr = "<fault>";
	} else if ((wrdptr=uls_tok2name(uls, t=__uls_tok(uls))) == NULL) {
		wrdptr = "";
	}

	n_chars = ustr_num_wchars(wrdptr, -1, uls_ptr(parms1));
	len = parms1.len;

	return uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(ctx->perfmt),  wrdptr, len, n_chars);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fmtproc_keyword)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx)
{
	const char *wrdptr;
	int len, n_chars;
	uls_outparam_t parms1;

	if (uls == nilptr) {
		wrdptr = "<fault>";
	} else {
		wrdptr = __uls_lexeme(uls);
	}

	n_chars = ustr_num_wchars(wrdptr, -1, uls_ptr(parms1));
	len = parms1.len;

	return uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(ctx->perfmt),  wrdptr, len, n_chars);
}

void
ULS_QUALIFIED_METHOD(initialize_uls_syserr)(void)
{
	uls_voidptr_t syslog;
	uls_lf_puts_t syslog_puts;

	uls_lf_init_convspec_map(uls_ptr(lf_map_syserr), 0);
	uls_add_default_convspecs(uls_ptr(lf_map_syserr));

	uls_lf_init_convspec_map(uls_ptr(lf_map_logdfl), 0);
	uls_add_default_log_convspecs(uls_ptr(lf_map_logdfl));

	syslog = _uls_stdio_fp(2);
	syslog_puts = uls_lf_puts_file;
	uls_lf_init(uls_ptr(lf_syserr), uls_ptr(lf_map_syserr), syslog, syslog_puts);
}

void
ULS_QUALIFIED_METHOD(finalize_uls_syserr)(void)
{
	uls_lf_deinit(uls_ptr(lf_syserr));

	uls_lf_deinit_convspec_map(uls_ptr(lf_map_logdfl));
	uls_lf_deinit_convspec_map(uls_ptr(lf_map_syserr));
}

int
ULS_QUALIFIED_METHOD(uls_fmtproc_coord)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls;

	uls = (uls_lex_ptr_t) va_arg(ctx->args, uls_lex_ptr_t);
	return __uls_fmtproc_coord(x_dat, puts_proc, uls, ctx);
}

int
ULS_QUALIFIED_METHOD(uls_fmtproc_tokname)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls;

	uls = (uls_lex_ptr_t) va_arg(ctx->args, uls_lex_ptr_t);
	return __uls_fmtproc_tokname(x_dat, puts_proc, uls, ctx);
}

int
ULS_QUALIFIED_METHOD(uls_fmtproc_keyword)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls;

	uls = (uls_lex_ptr_t) va_arg(ctx->args, uls_lex_ptr_t);
	return __uls_fmtproc_keyword(x_dat, puts_proc, uls, ctx);
}

int
ULS_QUALIFIED_METHOD(uls_log_fmtproc_coord)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls = (uls_lex_ptr_t) ctx->g_dat;
	return __uls_fmtproc_coord(x_dat, puts_proc, uls, ctx);
}

int
ULS_QUALIFIED_METHOD(uls_log_fmtproc_tokname)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls = (uls_lex_ptr_t) ctx->g_dat;
	return __uls_fmtproc_tokname(x_dat, puts_proc, uls, ctx);
}

int
ULS_QUALIFIED_METHOD(uls_log_fmtproc_keyword)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx)
{
	uls_lex_t  *uls = (uls_lex_ptr_t) ctx->g_dat;
	return __uls_fmtproc_keyword(x_dat, puts_proc, uls, ctx);
}

void
ULS_QUALIFIED_METHOD(err_syslog_lock)(void)
{
	uls_lf_lock(uls_ptr(lf_syserr));
}

void
ULS_QUALIFIED_METHOD(err_syslog_unlock)(void)
{
	uls_lf_unlock(uls_ptr(lf_syserr));
}

void
ULS_QUALIFIED_METHOD(err_vlog)(const char* fmt, va_list args)
{
	uls_voidptr_t syslog = lf_syserr.x_dat;
	uls_lf_puts_t syslog_puts = lf_syserr.uls_lf_puts;

	uls_lf_lock(uls_ptr(lf_syserr));

	__uls_lf_vxprintf(uls_ptr(lf_syserr), fmt, args);
	syslog_puts(syslog, "\n", 1);
	syslog_puts(syslog, nilptr, 0);

	uls_lf_unlock(uls_ptr(lf_syserr));
}

void
ULS_QUALIFIED_METHOD(err_log)(const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	err_vlog(fmt, args);
	va_end(args);
}

void
ULS_QUALIFIED_METHOD(err_vpanic)(const char* fmt, va_list args)
{
	err_vlog(fmt, args);
	uls_appl_exit(1);
}

void
ULS_QUALIFIED_METHOD(err_panic)(const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	err_vlog(fmt, args);
	va_end(args);

	uls_appl_exit(1);
}

void
ULS_QUALIFIED_METHOD(err_change_port)(uls_voidptr_t data, uls_lf_puts_t proc)
{
	uls_lf_delegate_t delegate;

	if (proc == nilptr) {
		if (data == nilptr) {
			proc = uls_lf_puts_null;
		} else {
			proc = uls_lf_puts_file;
		}
	}

	delegate.xdat = data;
	delegate.puts = proc;
	uls_lf_change_puts(uls_ptr(lf_syserr), uls_ptr(delegate));
}

int
ULS_QUALIFIED_METHOD(uls_init_log)(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	if (uls == nilptr) {
		return -1;
	}

	log->flags = ULS_FL_STATIC;
	uls_init_mutex(uls_ptr(log->mtx));

	uls_grab(uls);
	log->uls = uls;

	log->log_port = lf_syserr.x_dat;
	log->log_puts = lf_syserr.uls_lf_puts;
	log->log_mask = 0;

	uls_set_loglevel(log, ULS_LOG_WARN);

	if (lf_map == nilptr) lf_map = uls_ptr(lf_map_logdfl);

	if ((log->lf = uls_lf_create(lf_map, log->log_port, log->log_puts)) == nilptr) {
		uls_lf_destroy_convspec_map(lf_map);
		return -1;
	}

	log->shell = nilptr;
	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_log)(uls_log_ptr_t log)
{
	uls_lex_ptr_t uls = log->uls;

	uls_lf_destroy(log->lf);

	log->flags &= ULS_FL_STATIC;

	log->uls = nilptr;
	uls_ungrab(uls);

	log->log_port = nilptr;
	log->log_puts = uls_lf_puts_null;
	log->shell = nilptr;

	uls_deinit_mutex(uls_ptr(log->mtx));
}

ULS_QUALIFIED_RETTYP(uls_log_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_log)(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls)
{
	uls_log_ptr_t log;

	log = uls_alloc_object(uls_log_t);

	if (uls_init_log(log, lf_map, uls) < 0) {
		uls_dealloc_object(log);
		return nilptr;
	}

	log->flags &= ~ULS_FL_STATIC;

	return log;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_log)(uls_log_ptr_t log)
{
	uls_deinit_log(log);

	if ((log->flags & ULS_FL_STATIC) == 0) {
		uls_dealloc_object(log);
	}
}

void
ULS_QUALIFIED_METHOD(uls_log_change)(uls_log_ptr_t log, uls_voidptr_t data, uls_lf_puts_t proc)
{
	uls_lf_delegate_t delegate;

	if (proc == nilptr) {
		if (data == nilptr) {
			proc = uls_lf_puts_null;
		} else {
			proc = uls_lf_puts_file;
		}
	}

	uls_log_lock(log);
	log->log_port = data;
	log->log_puts = proc;
	uls_log_unlock(log);

	delegate.xdat = data;
	delegate.puts = proc;
	uls_lf_change_puts(log->lf, uls_ptr(delegate));
}

// <brief>
// A method that emits a message after formatting the string 'fmt' with 'args'
// </brief>
// <parm name="fmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
// <return>none</return>
void
ULS_QUALIFIED_METHOD(uls_vlog)(uls_log_ptr_t log, const char* fmt, va_list args)
{
	char numbuf[N_LOGBUF_CHARS];
	uls_voidptr_t old_gdat;
	int len;

	if (log->uls == nilptr) {
		err_vlog(fmt, args);
		return;
	}

	uls_log_lock(log);
	old_gdat = uls_lf_change_gdat(log->lf, log->uls);

	len = uls_snprintf(numbuf, N_LOGBUF_CHARS, "[ULS] [%s:%d] ",
		__uls_get_tag(log->uls), __uls_get_lineno(log->uls));
	log->log_puts(log->log_port, numbuf, len);

	uls_lf_vxprintf(log->lf, fmt, args);

	log->log_puts(log->log_port, "\n", 1);
	log->log_puts(log->log_port, nilptr, 0);

	uls_lf_change_gdat(log->lf, old_gdat);
	uls_log_unlock(log);
}

// <brief>
// Logs a formatted message
// You can use %t %w to print the current token, its location.
// No need to append '\n' to the end of line 'fmt'
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return>void</return>
void
ULS_QUALIFIED_METHOD(uls_log)(uls_log_ptr_t log, const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	uls_vlog(log, fmt, args);
	va_end(args);
}

void
ULS_QUALIFIED_METHOD(uls_vpanic)(uls_log_ptr_t log, const char* fmt, va_list args)
{
	uls_vlog(log, fmt, args);
	uls_appl_exit(1);
}

// <brief>
// Logs a formatted message and the program will die.
// No need to append '\n' to the end of line 'fmt'
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return>none</return>
void
ULS_QUALIFIED_METHOD(uls_panic)(uls_log_ptr_t log, const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	uls_vlog(log, fmt, args);
	va_end(args);

	uls_appl_exit(1);
}

void
ULS_QUALIFIED_METHOD(uls_set_loglevel)(uls_log_ptr_t log, int lvl)
{
	if (lvl > ULS_MAX_LOGLEVEL || lvl < 0) {
		err_log("verbose level %d dosn't exist", lvl);
		return;
	}

	if (lvl < ULS_LOG_N_SYSTEMS) {
		if (lvl >= ULS_LOG_ERROR) {
			log->log_mask &= ~((1<<ULS_LOG_N_SYSTEMS)-1);
			log->log_mask |= (1 << (lvl+1)) - 1;
		}
	} else {
		log->log_mask |= (1 << lvl);
	}
}

int
ULS_QUALIFIED_METHOD(uls_loglevel_isset)(uls_log_ptr_t log, int lvl)
{
	if (lvl > ULS_MAX_LOGLEVEL || lvl < 0) {
		return 0;
	}

	return (log->log_mask & ULS_LOGLEVEL_FLAG(lvl)) ? 1 : 0;
}

void
ULS_QUALIFIED_METHOD(uls_clear_loglevel)(uls_log_ptr_t log, int lvl)
{
	if (lvl > ULS_MAX_LOGLEVEL || lvl < 0) {
		err_log("verbose level %d dosn't exist", lvl);
		return;
	}

	if (!uls_loglevel_isset(log, lvl))
		return;

	if (lvl < ULS_LOG_N_SYSTEMS) {
		if (--lvl >= ULS_LOG_ERROR) {
			uls_set_loglevel(log, lvl);
		}
	} else {
		log->log_mask &= ~ULS_LOGLEVEL_FLAG(lvl);
	}
}

void
ULS_QUALIFIED_METHOD(uls_add_default_log_convspecs)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_register_convspec(lf_map, "w", uls_log_fmtproc_coord);
	uls_lf_register_convspec(lf_map, "t", uls_log_fmtproc_tokname);
	uls_lf_register_convspec(lf_map, "k", uls_log_fmtproc_keyword);
}

void
ULS_QUALIFIED_METHOD(uls_add_default_convspecs)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_register_convspec(lf_map, "w", uls_fmtproc_coord);
	uls_lf_register_convspec(lf_map, "t", uls_fmtproc_tokname);
	uls_lf_register_convspec(lf_map, "k", uls_fmtproc_keyword);
}
