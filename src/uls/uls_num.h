
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
  <file> uls_num.h </file>
  <brief>
    Recognizing the number tokens from input.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, May 2011.
  </author>
*/

#ifndef __ULS_NUM_H__
#define __ULS_NUM_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define ULS_NUM_FL_ZERO           0x01
#define ULS_NUM_FL_FLOAT          0x02
#define ULS_NUM_FL_PREVCH_IS_SEP  0x10

#endif // ULS_DECL_PROTECTED_TYPE

#if defined(__ULS_NUM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC uls_uint64 get_uint64_from_radix_numstr(int radix, const char *numbuf, int numlen1);
ULS_DECL_STATIC int uls_cvt_radix_simple_num(int radix1, const char *numbuf1, int numlen1, int radix2, uls_outbuf_ptr_t outbuf, int k);

ULS_DECL_STATIC int get_hexanum_from_octal(uls_outparam_ptr_t parms, int a_bits);
ULS_DECL_STATIC int __skip_radix_numstr(uls_outparam_ptr_t parms, int radix,
	uls_outbuf_ptr_t numbuf, int k);
ULS_DECL_STATIC int binstr2hexval(const char* bi_str, int m);
ULS_DECL_STATIC int skip_prefixed_zeros(uls_outparam_ptr_t parms);
ULS_DECL_STATIC int check_expo(uls_outparam_ptr_t parms);
ULS_DECL_STATIC char* decstr2hexbin(uls_outparam_ptr_t parms);
ULS_DECL_STATIC int append_expo(int num, uls_outbuf_ptr_t numbuf, int k);
ULS_DECL_STATIC int __cvt_radix2hexa_str_generic(int radix, const char *numbuf, int n_digits,
	uls_outbuf_ptr_t outbuf, int k);
ULS_DECL_STATIC int __make_decstr_gexpr(const char *tokstr, int l_tokstr, int n_expo,
	uls_outbuf_ptr_t numbuf, int k);
ULS_DECL_STATIC int __make_radixint_str_gexpr(const char *radstr, int l_radstr, int radix,
	uls_outbuf_ptr_t numbuf, int k);
#endif

#ifdef ULS_DECL_PROTECTED_PROC

/*
 * This function transforms the number format to a standard form
 * The (input) number category of which this function suppots are
 *  decimal, octal, hexadecimal integer, or the scientific floating number.
 * For more details, refer to the tests/dump_toks
 */

int add_decstr_by_xx(const char* numstr, int n_digits, int nn, char* outbuf);
int mul_decstr_by_xx(const char* numstr, int n_digits, int multiplier, char* outbuf);
int div_decstr_by_16(char* wrd, uls_outparam_ptr_t parms);

int skip_radix_numstr(uls_outparam_ptr_t parms, int radix, uls_outbuf_ptr_t numbuf, int k);
int uls_num2stdfmt(uls_outparam_ptr_t parms, uls_outbuf_ptr_t numbuf, int k);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
int uls_oct2hex_str(const char *numbuf, int n_digits, char* outbuf);
int uls_bin2hex_str(const char* numstr, int n_digits, char* outbuf);
int uls_quat2hex_str(const char* numstr, int n_digits, char* outbuf);
int uls_dec2hex_str(char* numstr, int n_digits, char* outptr);

int uls_cvt_radix2dec_str(int radix, const char *numbuf, int n1_digits, uls_outbuf_ptr_t outbuf);
int uls_cvt_radix2hexa_str_generic(int radix, const char *numbuf, int n_digits, uls_outbuf_ptr_t outbuf, int k);
int uls_cvt_radix_to_hexadecimal_str(int radix, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k);
int uls_radix2hexadecimal_str(int radix1, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k);

int uls_extract_number(uls_outparam_ptr_t parms, uls_outbuf_ptr_t tokbuf, int k);
void uls_number(const char *numstr, int len_numstr, int n_expo, uls_outparam_ptr_t parms);
int uls_number_gexpr(const char *tokstr, int l_tokstr, int n_expo, uls_outbuf_ptr_t numbuf);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_NUM_H__

