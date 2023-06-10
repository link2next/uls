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
  <file> uls_ostream_wstr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#ifndef __ULS_OSTREAM_WSTR_H__
#define __ULS_OSTREAM_WSTR_H__

#include "uls/uls_lex.h"
#include "uls/uls_ostream.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DLL_EXTERN uls_ostream_ptr_t __uls_create_ostream_wstr(int fd_out, uls_lex_ptr_t uls, int stream_type, const wchar_t* subname);
ULS_DLL_EXTERN uls_ostream_ptr_t uls_create_ostream_wstr(int fd_out, uls_lex_ptr_t uls, const wchar_t* subname);
ULS_DLL_EXTERN uls_ostream_ptr_t uls_create_ostream_file_wstr(const wchar_t* filepath, uls_lex_ptr_t uls, const wchar_t* subname);
ULS_DLL_EXTERN int uls_print_tok_linenum_wstr(uls_ostream_ptr_t ostr, int lno, const wchar_t* tag);
ULS_DLL_EXTERN int uls_print_tok_wstr(uls_ostream_ptr_t ostr, int tokid, const wchar_t* tokstr);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define __uls_create_ostream __uls_create_ostream_wstr
#define uls_create_ostream uls_create_ostream_wstr
#define uls_create_ostream_file uls_create_ostream_file_wstr
#define uls_print_tok_linenum uls_print_tok_linenum_wstr
#define uls_print_tok uls_print_tok_wstr
#endif
#endif

#endif // __ULS_OSTREAM_WSTR_H__
