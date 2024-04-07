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
  <file> uls_lf_percent_f.h </file>
  <brief>
    The routines for '%f', '%e', ... of uls_lf_*printf*().
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, May 2011.
  </author>
*/

#ifndef __ULS_PERCENT_F_H__
#define __ULS_PERCENT_F_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
#define ULS_LF_LEFT_JUSTIFIED     0x0001  /* left justified */
#define ULS_LF_ZEROPAD            0x0002  /* pad with zero */
#define ULS_LF_PLUS_PREFIX        0x0004  /* show plus */
#define ULS_LF_MINUS_PREFIX       0x0008  /* show minus */

#define ULS_LF_SPECIAL_CHAR       0x0010  /* 0x */
#define ULS_LF_DYNAMIC_WIDTH      0x0020
#define ULS_LF_DYNAMIC_PRECISION  0x0040

#define ULS_LF_BIN_PREFIX         0x0100
#define ULS_LF_HEX_PREFIX         0x0200
#define ULS_LF_PERCENT_E          0x0400
#define ULS_LF_PERCENT_G          0x0800
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#ifdef _ULS_IMPLDLL
#define round_uup(x) ((int)((x)+0.5))
#define ULS_FLOAT_DFLPREC 6
#endif
#endif

#if defined(__ULS_LF_PERCENT_F__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC void reverse_char_array(char* ary, int n);
ULS_DECL_STATIC int unsigned2str(unsigned int n, csz_str_ptr_t ss);
ULS_DECL_STATIC void __pad_zeros(int n_prec, unsigned int flags, csz_str_ptr_t ss);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int  uls_lf_digits_to_percent_f(char* numstr, int minus, int n_expo, int n_prec, unsigned int flags, csz_str_ptr_t ss);
void uls_lf_digits_to_percent_e(char* numstr, int minus, int n_expo, int n_prec, csz_str_ptr_t ss);
void uls_lf_digits_to_percent_g(char* numstr, int minus, int n_expo, int n_prec, unsigned int flags, csz_str_ptr_t ss);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int uls_lf_double2digits(double x, int n_precision, csz_str_ptr_t numstr);
ULS_DLL_EXTERN int uls_lf_longdouble2digits(long double x, int n_precision, csz_str_ptr_t numstr);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_PERCENT_F_H__
