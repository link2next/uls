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
 * uls_dump.h -- Dumping uls token setting. --
 *     written by Stanley J. Hong <link2next@gmail.com>, June 2018.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_DUMP_H__
#define __ULS_DUMP_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls_lex.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void ulc_dump_tokdef_sorted(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_char_context(uls_lex_ptr_t uls);

ULS_DLL_EXTERN void uls_dump_quote(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_1char(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_2char(uls_lex_ptr_t uls);

ULS_DLL_EXTERN void uls_dump_utf8(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_char_tokmap(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_tokdef_rsvd(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_tokdef_vx(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_tokdef_names(uls_lex_ptr_t uls);

ULS_DLL_EXTERN void dump_fd_tower(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_dump_kwtable(uls_kwtable_ptr_t tbl);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_DUMP_H__
