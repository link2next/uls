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

#include "uls/csz_stream.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef _ULS_IMPLDLL
#define round_uup(x) ((int)((x)+0.5))
#define ULS_FLOAT_DFLPREC 6
#endif

#if defined(__ULS_LF_PERCENT_F__)
ULS_DECL_STATIC void reverse_char_array(char* ary, int n);
ULS_DECL_STATIC int unsigned2str(unsigned int n, csz_str_ptr_t ss);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int  uls_lf_digits_to_percent_f(char* numstr, int minus, int n_expo, int n_prec, csz_str_ptr_t ss);
void uls_lf_digits_to_percent_e(char* numstr, int minus, int n_expo, int n_prec, csz_str_ptr_t ss);
void uls_lf_digits_to_percent_g(char* numstr, int minus, int n_expo, int n_prec, csz_str_ptr_t ss);
#endif

ULS_DLL_EXTERN int uls_lf_double2digits(double x, int n_precision, csz_str_ptr_t numstr);
ULS_DLL_EXTERN int uls_lf_longdouble2digits(long double x, int n_precision, csz_str_ptr_t numstr);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_PERCENT_F_H__
