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
  <file> uls_alog.h </file>
  <brief>
    A part of logging framework of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/

#ifndef __ULS_ALOG_H__
#define __ULS_ALOG_H__
#include "uls/uls_aprint.h"
#include "uls/uls_log.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(alog_shell)
{
	csz_str_t astr_buff;
};

void initialize_uls_alog(void);
void finalize_uls_alog(void);

ULS_DLL_EXTERN int uls_init_alog(uls_log_ptr_t log, uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_deinit_alog(uls_log_ptr_t log);

ULS_DLL_EXTERN uls_log_ptr_t uls_create_alog(uls_lf_map_ptr_t lf_map, uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_destroy_alog(uls_log_ptr_t log);

ULS_DLL_EXTERN void err_valog(const char* afmt, va_list args);
ULS_DLL_EXTERN void err_alog(const char* afmt, ...);
ULS_DLL_EXTERN void err_vapanic(const char* afmt, va_list args);
ULS_DLL_EXTERN void err_apanic(const char* afmt, ...);

ULS_DLL_EXTERN void uls_valog(uls_log_ptr_t log, const char* afmt, va_list args);
ULS_DLL_EXTERN void uls_alog(uls_log_ptr_t log, const char* afmt, ...);
ULS_DLL_EXTERN void uls_vapanic(uls_log_ptr_t log, const char* afmt, va_list args);
ULS_DLL_EXTERN void uls_apanic(uls_log_ptr_t log, const char* afmt, ...);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_init_log uls_init_alog
#define uls_deinit_log uls_deinit_alog

#define uls_create_log uls_create_alog
#define uls_destroy_log uls_destroy_alog

#define err_vlog err_valog
#define err_log err_alog
#define err_vpanic err_vapanic
#define err_panic err_apanic

#define uls_vlog uls_valog
#define uls_log uls_alog
#define uls_vpanic uls_vapanic
#define uls_panic uls_apanic
#endif
#endif

#endif // __ULS_ALOG_H__
