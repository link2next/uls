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
  <file> uls_lf_percent_f.c </file>
  <brief>
    The routines for '%f', '%e', ... of uls_lf_*printf*().
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, May 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_LF_PERCENT_F__
#include "uls/uls_lf_percent_f.h"
#include "uls/ieee754.h"
#include <math.h>
#endif

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(reverse_char_array)(char* ary, int n)
{
	int i, j;
	char ch;

	// swap numstr[0,.., n-1]
	for (i=0; i<n/2; i++) {
		// swap numstr[i], numstr[n-i]
		j = n-i-1;

		ch = ary[i];
		ary[i] = ary[j];
		ary[j] = ch;
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(unsigned2str)(unsigned int n, csz_str_ptr_t ss)
{
	int i0, len;

	if (n==0) {
		csz_putc(ss, '0');
		return 1;
	}

	i0 = csz_length(ss);
	for ( ; n!=0; ) {
		csz_putc(ss, '0' + (n % 10));
		n /= 10;
	}

	len = csz_length(ss) - i0;
	reverse_char_array(csz_text(ss) + i0, len);

	return len;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__pad_zeros)(int n_zeros, unsigned int flags, csz_str_ptr_t ss)
{
	char ch;
	int j;

	if (n_zeros <= 0) return;

	if (flags & ULS_LF_ZEROPAD) {
		ch = '0';
	} else if (flags & ULS_LF_SPECIAL_CHAR) {
		ch = ' ';
	} else {
		ch = '\0';
	}

	if (ch != '\0') {
		for (j = 0; j < n_zeros; j++) csz_putc(ss, ch);
	}
}

int
ULS_QUALIFIED_METHOD(uls_lf_digits_to_percent_f)
	(char* numstr, int minus, int n_expo, int n_prec, unsigned int flags, csz_str_ptr_t ss)
{
	char *ptr, ch;
	int j, n;

	if (numstr[0] == '0' && numstr[1] == '\0') {
		csz_putc(ss, '0');
		csz_putc(ss, '.');
		__pad_zeros(n_prec, flags, ss);
		return csz_length(ss);
	}

	if (minus) {
		csz_putc(ss, '-');
	}

	ptr = numstr;
	if (n_expo > 0) {
		for (n=0; n < n_expo; n++) {
			if ((ch=*ptr) == '\0') {
				csz_putc(ss, '0');
			} else {
				csz_putc(ss, ch);
				++ptr;
			}
		}
		csz_putc(ss, '.');
		j = 0;

	} else {
		csz_putc(ss, '0');
		csz_putc(ss, '.');
		for (j = n = 0; n < -n_expo && j < n_prec; n++, j++) {
			csz_putc(ss, '0');
		}
	}

	for ( ; (ch=*ptr) != '\0' && j < n_prec; ptr++, j++) {
		csz_putc(ss, ch);
	}

	__pad_zeros(n_prec - j, flags, ss);
	return csz_length(ss);
}

void
ULS_QUALIFIED_METHOD(uls_lf_digits_to_percent_e)
	(char* numstr, int minus, int n_expo, int n_prec, csz_str_ptr_t ss)
{
	char *ptr;
	int j = 0;

	if (numstr[0] == '\0' || numstr[0]=='0' || n_expo + n_prec <= 0) {
		csz_putc(ss, '0');
		csz_putc(ss, '.');
		return;
	}

	// minus/plus
	if (minus) {
		csz_putc(ss, '-');
	}

	csz_putc(ss, numstr[0]);
	csz_putc(ss, '.');
	--n_expo;

	ptr = numstr + 1;
	for ( ; *ptr != '\0' && j < n_prec; ptr++, j++) { // n_expo+n_prec
		csz_putc(ss, *ptr);
	}

	if (n_expo != 0) {
		csz_putc(ss, 'E');

		if (n_expo < 0) {
			csz_putc(ss, '-');
			n_expo = -n_expo;
		} else {
			csz_putc(ss, '+');
		}

		unsigned2str(n_expo, ss);
	}
}

void
ULS_QUALIFIED_METHOD(uls_lf_digits_to_percent_g)
	(char* numstr, int minus, int n_expo, int n_prec, unsigned int flags, csz_str_ptr_t ss)
{
	int len;

	if ((len=n_expo) < 0) len = -n_expo;

	if (n_expo < -4 || len >= n_prec) {
		uls_lf_digits_to_percent_e(numstr, minus, n_expo, n_prec, ss);
	} else {
		uls_lf_digits_to_percent_f(numstr, minus, n_expo, n_prec, flags, ss);
	}
}

int
ULS_QUALIFIED_METHOD(uls_lf_double2digits)(double x, int n_precision, csz_str_ptr_t numstr)
{
	double frac, x_int;
	double frac2;
	int  n_expo;
	int  a, j, n, n_first_zeros;
	char *numbuf;

	if (n_precision < 0) n_precision = ULS_FLOAT_DFLPREC;

	if (x == 0.) {
		csz_putc(numstr, '0');
		return 1;
	}

	if (x < 0.) {
		x = -x;
	}

	frac = modf(x, uls_ptr(x_int));

	j = 0; // for the 'n_precision'

	if (frac == 0.) {
		for (n_first_zeros=0; x_int>.1; ) {
			frac2 = modf(x_int/10., uls_ptr(x_int));

			if ((a=round_uup(10. * frac2)) != 0) {
				csz_putc(numstr, '0' + a);
				break;
			}
			++n_first_zeros;
		}

		for ( ; x_int>.1; ) {
			frac2 = modf(x_int/10., uls_ptr(x_int));
			csz_putc(numstr, '0' + round_uup(frac2 * 10.));
		}

		n = csz_length(numstr);
		reverse_char_array(csz_text(numstr), n);
		n_expo = n + n_first_zeros; // 10^n_expo

	} else {
		for ( ; x_int>.1; ) {
			frac2 = modf(x_int/10., uls_ptr(x_int));
			csz_putc(numstr, '0' + round_uup(frac2 * 10.));
		}

		n = csz_length(numstr);
		reverse_char_array(csz_text(numstr), n);

		if (n == 0) {
			n_first_zeros = 0;
			for ( ; frac != 0.; j++) {
				frac = modf(frac*10., uls_ptr(x_int));
				if (x_int > 0.1) {
					csz_putc(numstr, '0' + (int) round_uup(x_int));
					++j;
					break;
				}
				++n_first_zeros;
			}
			n_expo = -n_first_zeros;

		} else {
			n_expo = n;
		}

		for ( ; j < n_precision && frac!=0.; j++) {
			frac = modf(frac*10., uls_ptr(x_int));
			csz_putc(numstr, '0' + (int) round_uup(x_int));
		}
	}

	numbuf = (char *) csz_text(numstr);
	for (j = csz_length(numstr) - 1; j >= 0; j--) {
		if (numbuf[j] != '0') {
			break;
		}
	}
	++j;
	csz_truncate(numstr, j);

	return n_expo;
}

int
ULS_QUALIFIED_METHOD(uls_lf_longdouble2digits)(long double x, int n_precision, csz_str_ptr_t numstr)
{
	long double frac, x_int;
	long double frac2;
	int  n_expo;
	int  a, j, n, n_first_zeros;

	if (n_precision < 0) n_precision = ULS_FLOAT_DFLPREC;

	if (x == 0.) {
		csz_putc(numstr, '0');
		return 1;
	}

	if (x < 0.) {
		x = -x;
	}

	frac = modfl(x, uls_ptr(x_int));
	j = 0;

	if (frac == 0.) {
		for (n_first_zeros=0; x_int>.1; ) {
			frac2 = modfl(x_int/10., uls_ptr(x_int));
			if ((a=round_uup(10. * frac2)) != 0) {
				csz_putc(numstr, '0' + a);
				break;
			}
			++n_first_zeros;
		}

		for ( ; x_int>.1; ) {
			frac2 = modfl(x_int/10., uls_ptr(x_int));
			csz_putc(numstr, '0' + round_uup(frac2 * 10.));
		}

		n = csz_length(numstr);
		reverse_char_array(csz_text(numstr), n);
		n_expo = n + n_first_zeros; // 10^n_expo

	} else {
		for ( ; x_int>.1; ) {
			frac2 = modfl(x_int/10., uls_ptr(x_int));
			csz_putc(numstr, '0' + round_uup(frac2 * 10.));
		}

		n = csz_length(numstr);
		reverse_char_array(csz_text(numstr), n);

		if (n == 0) {
			n_first_zeros = 0;
			for ( ; frac != 0.; j++) {
				frac = modfl(frac * 10., uls_ptr(x_int));
				if (x_int > 0.1) {
					csz_putc(numstr, '0' + (int) round_uup(x_int));
					++j;
					break;
				}
				++n_first_zeros;
			}
			n_expo = -n_first_zeros;

		} else {
			n_expo = n;
		}

		for ( ; j < n_precision && frac!=0.; j++) {
			frac = modfl(frac * 10., uls_ptr(x_int));
			csz_putc(numstr, '0' + (int) round_uup(x_int));
		}
	}

	return n_expo;
}

