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
  <file> ieee754.h </file>
  <brief>
    An implementation modf() for 'long double'.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, May 2011.
  </author>
*/

#ifndef __ULS_IEEE754_H__
#define __ULS_IEEE754_H__

#include "uls/uls_type.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#if defined(__ULS_IEEE754__)
ULS_DECL_STATIC int get_ieee754_biased_expo(char* buff, int n_bits_expo);
ULS_DECL_STATIC void put_ieee754_biased_expo(int m_expo, char* buff, int n_bits_expo);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_ieee754_bankup_bits(char* srcptr, int start_bit, int end_bit, int n_shift);
int uls_ieee754_longdouble_isspecial(long double x, char* nambuf);
	// ret-val == 1 : x is a special-value with nambuf filled.
	// ret-val == 0 x is a finite value
long double uls_ieee754_modlf(long double x, long double* p_frac);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif //  __ULS_IEEE754_H__
