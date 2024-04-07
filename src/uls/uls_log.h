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
  <file> uls_log.h </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, August 2011.
  </author>
*/
#ifndef __ULS_LOG_H__
#define __ULS_LOG_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_print.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define ULS_LOG_EMERG     0
#define ULS_LOG_ALERT     1
#define ULS_LOG_CRIT      2
#define ULS_LOG_ERROR     3
#define ULS_LOG_WARN      4
#define ULS_LOG_NOTICE    5
#define ULS_LOG_INFO      6
#define ULS_LOG_DEBUG     7
#define ULS_LOG_N_SYSTEMS 8

#define ULS_LOGLEVEL_FLAG(lvl)  (1<<(lvl))

#define uls_log_lock(log) uls_lock_mutex(uls_ptr((log)->mtx))
#define uls_log_unlock(log) uls_unlock_mutex(uls_ptr((log)->mtx))
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define N_LOGBUF_CHARS    81
#define ULS_MAX_LOGLEVEL  (sizeof(uls_uint32)*8 - 1)
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(log)
{
	uls_flags_t flags;
	uls_mutex_struct_t mtx;

	uls_lex_ptr_t uls;

	void  *log_port;
	uls_lf_puts_t log_puts;
	uls_uint32    log_mask;

	uls_lf_ptr_t  lf;
	uls_voidptr_t shell;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_LOG__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_lf_map_t lf_map_syserr;
ULS_DECL_STATIC uls_lf_map_t lf_map_logdfl;
ULS_DECL_STATIC uls_lf_t lf_syserr;
#endif

#ifdef ULS_DECL_PRIVATE_PROC
ULS_DECL_STATIC int __uls_fmtproc_coord(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx);
ULS_DECL_STATIC int __uls_fmtproc_tokname(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx);
ULS_DECL_STATIC int __uls_fmtproc_keyword(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lex_ptr_t uls, uls_lf_context_ptr_t ctx);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void initialize_uls_syserr(void);
void finalize_uls_syserr(void);

int uls_fmtproc_coord(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);
int uls_fmtproc_tokname(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);
int uls_fmtproc_keyword(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);

int uls_log_fmtproc_coord(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);
int uls_log_fmtproc_tokname(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);
int uls_log_fmtproc_keyword(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t ctx);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void err_syslog_lock(void);
ULS_DLL_EXTERN void err_syslog_unlock(void);

ULS_DLL_EXTERN void err_vlog(const char* fmt, va_list args);
ULS_DLL_EXTERN void err_log(const char* fmt, ...);
ULS_DLL_EXTERN void err_vpanic(const char* fmt, va_list args);
ULS_DLL_EXTERN void err_panic(const char* fmt, ...);

ULS_DLL_EXTERN void err_change_port(uls_voidptr_t data, uls_lf_puts_t proc);
ULS_DLL_EXTERN int uls_init_log(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_deinit_log(uls_log_ptr_t log);

ULS_DLL_EXTERN uls_log_ptr_t uls_create_log(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_destroy_log(uls_log_ptr_t log);

ULS_DLL_EXTERN void uls_log_change(uls_log_ptr_t log, uls_voidptr_t data, uls_lf_puts_t proc);

ULS_DLL_EXTERN void uls_set_loglevel(uls_log_ptr_t log, int lvl);
ULS_DLL_EXTERN int uls_loglevel_isset(uls_log_ptr_t log, int lvl);
ULS_DLL_EXTERN void uls_clear_loglevel(uls_log_ptr_t log, int lvl);

ULS_DLL_EXTERN void uls_vlog(uls_log_ptr_t log, const char* fmt, va_list args);
ULS_DLL_EXTERN void uls_log(uls_log_ptr_t log, const char* fmt, ...);

ULS_DLL_EXTERN void uls_vpanic(uls_log_ptr_t log, const char* fmt, va_list args);
ULS_DLL_EXTERN void uls_panic(uls_log_ptr_t log, const char* fmt, ...);

ULS_DLL_EXTERN void uls_add_default_log_convspecs(uls_lf_map_ptr_t lf_map);
ULS_DLL_EXTERN void uls_add_default_convspecs(uls_lf_map_ptr_t lf_map);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_wlog.h"
#endif

#endif // __ULS_LOG_H__
