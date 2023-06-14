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
 * uls_lex.h --  setting input-stream in uls object --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_LEX_H__
#define __ULS_LEX_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_core.h"
#include "uls/unget.h"
#include "uls/uls_istream.h"
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#if defined(__ULS_LEX__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __uls_change_stream_hdr(uls_lex_ptr_t uls, uls_istream_ptr_t istr, int flags);
ULS_DECL_STATIC void uls_fd_ungrabber(uls_voidptr_t data);
ULS_DECL_STATIC int __check_fd_dup(int fd, int flags);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int uls_select_isrc_filler(uls_context_ptr_t ctx, uls_istream_ptr_t istr);
uls_gettok_t find_isrc_filler(int fd_type, int fd_subtype, uls_ptrtype_tool(outparam) parms);
int uls_push_isrc_type(uls_lex_ptr_t uls, int fd_type, int fd_subtype);
int uls_set_isrc_type(uls_lex_ptr_t uls, int fd_type, int fd_subtype);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int uls_push_file(uls_lex_ptr_t uls, const char* filepath, int flags);
ULS_DLL_EXTERN int uls_set_file(uls_lex_ptr_t uls, const char* filepath, int flags);

ULS_DLL_EXTERN int uls_push_fd(uls_lex_ptr_t uls, int fd, int flags);
ULS_DLL_EXTERN int uls_set_fd(uls_lex_ptr_t uls, int fd, int flags);

ULS_DLL_EXTERN int uls_push_fp(uls_lex_ptr_t uls, FILE* fp, int flags);
ULS_DLL_EXTERN int uls_set_fp(uls_lex_ptr_t uls, FILE* fp, int flags);

ULS_DLL_EXTERN int uls_push_istream(uls_lex_ptr_t uls, uls_istream_ptr_t istr, uls_tmpl_list_ptr_t tmpl_list, int flags);
ULS_DLL_EXTERN int uls_set_istream(uls_lex_ptr_t uls, uls_istream_ptr_t istr, uls_tmpl_list_ptr_t tmpl_list, int flags);

ULS_DLL_EXTERN int uls_register_ungrabber(uls_lex_ptr_t uls, int at_tail,
	uls_input_ungrabber_t proc, uls_voidptr_t data);

ULS_DLL_EXTERN void uls_push_utf16_line(uls_lex_ptr_t uls, uls_uint16* wline, int wlen);
ULS_DLL_EXTERN void uls_set_utf16_line(uls_lex_ptr_t uls, uls_uint16* wline, int wlen);

ULS_DLL_EXTERN void uls_push_utf32_line(uls_lex_ptr_t uls, uls_uint32* wline, int wlen);
ULS_DLL_EXTERN void uls_set_utf32_line(uls_lex_ptr_t uls, uls_uint32* wline, int wlen);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN int uls_push_istream_2(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const char** tmpl_nams, const char** tmpl_vals, int n_tmpls, int flags);
#endif

ULS_DLL_EXTERN int ulsjava_push_line(uls_lex_ptr_t uls, const void *line, int len, int flags);
ULS_DLL_EXTERN int ulsjava_push_file(uls_lex_ptr_t uls, const void *filepath, int len_filepath, int flags);

ULS_DLL_EXTERN int ulsjava_set_line(uls_lex_ptr_t uls, const void *line, int len, int flags);
ULS_DLL_EXTERN int ulsjava_set_file(uls_lex_ptr_t uls, const void *filepath, int len_filepath, int flags);

ULS_DLL_EXTERN int _uls_MBCS(void);
ULS_DLL_EXTERN int _uls_const_MBCS_UTF8(void);
ULS_DLL_EXTERN int _uls_const_MBCS_MS_MBCS(void);

ULS_DLL_EXTERN void uls_skip_white_spaces(uls_lex_ptr_t uls);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USEDLL
#include "uls/litstr.h"
#include "uls/unget.h"
#ifdef ULS_USE_WSTR
#include "uls/uls_util_wstr.h"
#include "uls/uls_wprint.h"
#include "uls/uls_wlog.h"
#include "uls/uls_wlex.h"
#endif
#endif

#endif // __ULS_LEX_H__
