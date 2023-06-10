
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
#define ULS_NUM_FL_MINUS    0x10
#define ULS_NUM_FL_ZERO     0x20
#define ULS_NUM_FL_FLOAT    0x40

#endif // ULS_DECL_PROTECTED_TYPE

#if defined(__ULS_NUM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC uls_uint64 get_uint64_from_radix_numstr(int radix, const char *numbuf, int numlen1);
ULS_DECL_STATIC int uls_cvt_radix_simple_num(int radix1, const char *numbuf1, int numlen1, int radix2, uls_outbuf_ptr_t outbuf, int k);

ULS_DECL_STATIC int get_hexanum_from_octal(_uls_tool_ptrtype_(outparam) parms, int a_bits);
ULS_DECL_STATIC int __skip_radix_number(_uls_tool_ptrtype_(outparam) parms, int radix, uls_uch_t numsep, 
	_uls_tool_ptrtype_(outbuf) numbuf, int k);
ULS_DECL_STATIC int binstr2hexval(const char* bi_str, int m);
ULS_DECL_STATIC int skip_prefixed_zeros(_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int check_expo(_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC char* decstr2hexbin(_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int append_expo(int num, uls_outbuf_ptr_t numbuf, int k);
#endif

#ifdef ULS_DECL_PROTECTED_PROC

/*
 * This function transforms the number format to a standard form
 * The (input) number category of which this function suppots are
 *  decimal, octal, hexadecimal integer, or the scientific floating number.
 * For examples,
        '0' -----> 0
        '-0' -----> 0
        '-0.' -----> 0.
        '-0.314' -----> -.314
        '-.314' -----> -.314
        '314' -----> 314
        '-314' -----> -314
        '314.15' -----> .31415 E 3
        '-5041.7094' -----> -.50417094 E 4
        '314.15E-108' -----> .31415 E -105
        '-0.31415e303' -----> -.31415 E 303
        '.0050417094' -----> .50417094 E -2
        '0.031415e+1103' -----> .31415 E 1102
        '314.000' -----> .314 E 3
        '-3140000.000' -----> -.314 E 7
        '-50.0' -----> -.5 E 2
        '0.000' -----> 0.
        '-.000' -----> 0.
        '.000' -----> 0.
        '0xAF6' -----> 0xAF6
        '0x00AF6' -----> 0xAF6
        '-0x0AF6' -----> -0xAF6
        '0775' -----> 0x1FD
        '0000775' -----> 0x1FD
        '-000775' -----> -0x1FD
        '0' -----> 0
        '0.' -----> 0.
        '.0' -----> 0.
        '.' -----> NOT Number!
        '-0' -----> 0
        '-0.' -----> 0.
        '-.0' -----> 0.
        '-.' -----> NOT Number!
        '1.' -----> .1 E 1
        '.1' -----> .1
        '-1.' -----> -.1 E 1
        '-.1' -----> -.1
        '0.0' -----> 0.
        '1.0' -----> .1 E 1
        '0.1' -----> .1
        '1.1' -----> .11 E 1
        '-0.0' -----> 0.
        '-1.0' -----> -.1 E 1
        '-0.1' -----> -.1
        '-1.1' -----> -.11 E 1
 */

int add_decstr_by_xx(const char* numstr, int n_digits, int nn, char* outbuf);
int mul_decstr_by_xx(const char* numstr, int n_digits, int multiplier, char* outbuf);
int div_decstr_by_16(char* wrd, _uls_tool_ptrtype_(outparam) parms);
int __uls_cvt_radix2hexa_str_generic(int radix, const char *numbuf, int n_digits, uls_outbuf_ptr_t outbuf, int k);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
int uls_oct2hex_str(const char *numbuf, int n_digits, char* outbuf);
int uls_bin2hex_str(const char* numstr, int n_digits, char* outbuf);
int uls_quaternary2hex_str(const char* numstr, int n_digits, char* outbuf);
int uls_dec2hex_str(char* wrd, int wrdlen, char* outptr);

int uls_cvt_radix2dec_str(int radix, const char *numbuf, int n1_digits, uls_outbuf_ptr_t outbuf);
int uls_cvt_radix2hexa_str_generic(int radix, const char *numbuf, int n_digits, uls_outbuf_ptr_t outbuf, int k);
int uls_cvt_radix_to_hexadecimal_str(int radix, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k);
int uls_radix2hexadecimal_str(int radix1, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k);

int skip_radix_number(uls_outparam_ptr_t parms, int radix, uls_outbuf_ptr_t numbuf, int k);
int num2stdfmt_0(_uls_tool_ptrtype_(outparam) parms, _uls_tool_ptrtype_(outbuf) numbuf, int k);
int num2stdfmt(_uls_tool_ptrtype_(outparam) parms, _uls_tool_ptrtype_(outbuf) numbuf, int k);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_NUM_H__

