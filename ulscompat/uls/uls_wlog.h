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
  <file> uls_wlog.h </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_WLOG_H__
#define __ULS_WLOG_H__

#include "uls/uls_wprint.h"
#include "uls/uls_log.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(wlog_shell)
{
	csz_str_t wstr_buff;
};

void initialize_uls_wlog(void);
void finalize_uls_wlog(void);

ULS_DLL_EXTERN int uls_init_wlog(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_deinit_wlog(uls_log_ptr_t log);

ULS_DLL_EXTERN uls_log_ptr_t uls_create_wlog(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_destroy_wlog(uls_log_ptr_t log);

ULS_DLL_EXTERN void err_vwlog(const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN void err_wlog(const wchar_t* wfmt, ...);
ULS_DLL_EXTERN void err_vwpanic(const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN void err_wpanic(const wchar_t* wfmt, ...);

ULS_DLL_EXTERN void uls_vwlog(uls_log_ptr_t log, const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN void uls_wlog(uls_log_ptr_t log, const wchar_t* wfmt, ...);
ULS_DLL_EXTERN void uls_vwpanic(uls_log_ptr_t log, const wchar_t* wfmt, va_list args);
ULS_DLL_EXTERN void uls_wpanic(uls_log_ptr_t log, const wchar_t* wfmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_WLOG_H__
