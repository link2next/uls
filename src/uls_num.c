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
 * uls_num.c -- recognizing the number tokens from input --
 *     written by Stanley Hong <link2next@gmail.com>, Feb. 2023.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_NUM__
#include "uls/uls_num.h"
#include "uls/uls_prim.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC uls_uint64
ULS_QUALIFIED_METHOD(get_uint64_from_radix_numstr)(int radix, const char *numbuf, int numlen)
{
	int i, n;
	uls_uint64 num = 0;
	char ch;

	for (i=0; i<numlen; i++) {
		ch = numbuf[i];
		n = is_num_radix(ch, radix);

		num = num * radix + n;
	}

	return num;
}

int
ULS_QUALIFIED_METHOD(add_decstr_by_xx)(const char* numstr, int n_digits, int nn, char* outbuf)
{
	int val, ch, carry;
	int i, k, a;

	if (n_digits < 0) n_digits = uls_strlen(numstr);

	if (n_digits == 1 && numstr[0] == '0') {
		k = 0;
		if ((a = nn / 10) != 0) {
			outbuf[k++] = '0' + a;
		}

		a = nn % 10;
		outbuf[k++] = '0' + a;
		outbuf[k] = '\0';
		return k;
	}

	if (nn <= 0) {
		uls_memcopy(outbuf, numstr, n_digits);
		outbuf[n_digits] = '\0';
		return n_digits;
	}

	// copy numstr to outbuf, reversely, binary
	for (i=0, k=n_digits-1; i<n_digits/2; i++, k--) {
		ch = numstr[i];
		outbuf[i] = numstr[k] - '0';
		outbuf[k] = ch - '0';
	}
	if (n_digits % 2 != 0) {
		ch = numstr[i];
		outbuf[i] = ch - '0';
	}

	// the first digit
	a = nn % 10;
	if ((val = a + outbuf[0]) >= 10) {
		carry = 1; val -= 10;
	} else {
		carry = 0;
	}
	outbuf[0] = val;

	// the second digit
	if ((a = nn / 10) != 0) {
		if (n_digits < 2) outbuf[1] = 0;

		if ((val = carry + a  + outbuf[1]) >= 10) {
			carry = 1; val -= 10;
		} else {
			carry = 0;
		}
		outbuf[1] = val;
		i = 2;
	} else {
		i = 1;
	}

	// remainder
	for ( ; i<n_digits; i++) {
		if ((val = carry + outbuf[i]) >= 10) {
			carry = 1;
			val -= 10;
		} else {
			carry = 0;
		}

		outbuf[i] = val;
	}

	if (carry) {
		// carry == 1
		outbuf[i++] = 1;
	}

	k = i;
	outbuf[k] = '\0';

	// convert the binary numbuer outbuf[i] to ASCII chars.
	for (i=0; i<k; i++) {
		ch = '0' + outbuf[i];
		outbuf[i] = ch;
	}
	uls_reverse_bytes(outbuf, k);

	return k;
}

int
ULS_QUALIFIED_METHOD(mul_decstr_by_xx)(const char* numstr, int n_digits, int multiplier, char* outbuf)
{
	// numstr * multiplier --> outbuf
	int i, n1, k, a, b, val, carry = 0;
	char ch, *numstr_tmp = NULL;

	if (n_digits < 0) n_digits = uls_strlen(numstr);

	if ((n_digits == 1 && numstr[0] == '0') || multiplier <= 0) {
		outbuf[0] = '0'; outbuf[1] = '\0';
		return 1;
	}

	if (multiplier == 1) {
		uls_memcopy(outbuf, numstr, n_digits);
		outbuf[n_digits] = '\0';
		return n_digits;
	}

	if (outbuf == numstr) {
		numstr_tmp = (char *) uls_strdup(numstr, n_digits);
		numstr = numstr_tmp;
	}

	if ((a = multiplier % 10) != 0) {
		for (n1=0,i=n_digits-1; i>=0; i--) {
			val = numstr[i] - '0';
			val = carry + val * a;
			carry = val / 10;
			val = val % 10;
			outbuf[n1++] = val;
		}
		if (carry > 0) outbuf[n1++] = carry;
		carry = 0;
	} else {
		for (i=0; i<n_digits; i++) {
			outbuf[i] = 0;
		}
		n1 = n_digits;
	}

	if ((b = multiplier / 10) != 0) {
		outbuf[n1] = 0;
		for (k=1,i=n_digits-1; i>=0; i--,k++) {
			val = numstr[i] - '0';
			val = carry + val * b + outbuf[k];
			carry = val / 10;
			val = val % 10;
			outbuf[k] = val;
		}
		if (carry > 0) outbuf[k++] = carry;
	} else {
		k = n1;
	}

	uls_mfree(numstr_tmp);

	// convert the binary numbuer outbuf[i] to ASCII chars.
	for (i=0; i<k; i++) {
		ch = '0' + outbuf[i];
		outbuf[i] = ch;
	}
	uls_reverse_bytes(outbuf, k);
	outbuf[k] = '\0';
	return k;
}

int
ULS_QUALIFIED_METHOD(div_decstr_by_16)(char* wrd, uls_outparam_ptr_t parms)
{
	char  *wrd_end;
	char *ptr, *outptr;
	int a, len, rem;

	if ((len=parms->len) <= 0 || !uls_isdigit(wrd[0])) {
		_uls_log(err_panic)("%s: invalid format!", wrd);
	}
	wrd_end = wrd + parms->len;

	a = wrd[0] - '0';
	if (len <= 1) {
		wrd[0] = '0';
		parms->len = 1;
		return a; // the remainder of 'wrd'
	}
	a = 10 * a + (wrd[1] - '0');

	outptr = wrd;
	ptr= wrd + 2;

	if (a < 16) {
		if (ptr == wrd_end) {
			wrd[0] = '0';
			parms->len = 1;
			return a;
		}
		rem = a;
	} else {
		*outptr++ = '0' + a / 16;
		rem = a % 16;
	}

	for ( ; ptr < wrd_end; ptr++) {
		a = 10*rem + (*ptr - '0');
		*outptr++ = '0' + a / 16;
		rem = a % 16;
	}

	// the length of new 'wrd'
	parms->len = (int) (outptr - wrd);
	return rem;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(binstr2hexval)(const char* bi_str, int m)
{
	int hex_val = 0;
	int j;

	for (j=0; j<m; j++) {
		hex_val = (hex_val << 1) + (bi_str[j] - '0');
	}

	return hex_val;
}

int
ULS_QUALIFIED_METHOD(uls_bin2hex_str)(const char* numstr, int n_digits, char* outbuf)
{
	// zeros in front of numstr is NOT permitted.
	int i, k=0, n, m;
	const char *lptr = numstr;
	int h_val;

	if (n_digits < 0) n_digits = uls_strlen(numstr);

	n = n_digits / 4; // 4-bits
	m = n_digits % 4; // 4-bits

	if (m > 0) {
		h_val = binstr2hexval(lptr, m);
		lptr += m;
		if (h_val != 0) outbuf[k++] = num2char_radix(h_val);
	}

	for (i=0; i<n; i++) {
		h_val = binstr2hexval(lptr, 4);
		outbuf[k++] = num2char_radix(h_val);
		lptr += 4;
	}

	if (k <= 0) {
		outbuf[k++] = '0';
	}

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_quat2hex_str)(const char* numstr, int n_digits, char* outbuf)
{
	// zeros in front of numstr is NOT permitted.
	int i, k=0, n, m;
	const char *lptr = numstr;
	int h_val;

	if (n_digits < 0) n_digits = uls_strlen(numstr);
	n = n_digits / 2;
	m = n_digits % 2;

	if (m > 0) {
		h_val = *lptr++ - '0';
		outbuf[k++] = num2char_radix(h_val);
	}

	for (i=0; i<n; i++) {
		h_val = 4 *(lptr[0] - '0') + (lptr[1] - '0');
		outbuf[k++] = num2char_radix(h_val);
		lptr += 2;
	}

	if (k <= 0) {
		outbuf[k++] = '0';
	}

	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_hexanum_from_octal)(uls_outparam_ptr_t parms, int a_bits)
{
	// a_bits = { 1, 2, 3 } bits
	unsigned int two_octals = parms->x1;
	int n, b_bits; // a_bits + b_bits == 4
	unsigned int mask, h_val;

	// a, b = {0, 1}, n: #-of-?'s
	// a_bits=1)  ..a bbb  b_bits=3, n=0
	// a_bits=2)  .aa bb?  b_bits=2, n=1
	// a_bits=3)  aaa b??  b_bits=1, n=2

	b_bits = 4 - a_bits;
	n = 3 - b_bits;
	mask = ((1 << 4) - 1) << n; // covers {a*b*}
	h_val = (two_octals & mask) >> n; // hval == {a*b*}

	mask = (1 << n) - 1;
	two_octals &= mask; // the remainder of the right n-bits
	parms->x2 = two_octals;

	return h_val;
}

int
ULS_QUALIFIED_METHOD(uls_oct2hex_str)(const char *numstr, int n_digits, char* outbuf)
{
	const char  *lptr;
	const char  *lptr_end;
	char ch, *outptr;
	unsigned int o_num;
	int  n_octals, a_bits, b_bits, h_val;
	uls_outparam_t parms1;

	if (n_digits < 0) n_digits = uls_strlen(numstr);
	lptr = numstr;
	lptr_end = lptr + n_digits;

	// skip the prefixed zeros
	for ( ; lptr < lptr_end; lptr++) {
		if (*lptr != '0') break;
	}

	n_octals = (int) (lptr_end - lptr); // #-of-octals
	if (n_octals <= 0) {
		*outbuf = '0';
		return 1;
	}

	if (n_octals == 1) {
		*outbuf = *lptr;
		return 1;
	}

	b_bits = (3 * (n_octals % 4)) % 4;
	o_num = *lptr++ - '0';
	outptr = outbuf;

	if (b_bits > 0) { // b_bits = {1, 2, 3}
		a_bits = 4 - b_bits;
		/**
		 * get b bits, the #-of-bits at the left most.
		 * CASE(b_bits==1): in o_num == 000 b??
		 * CASE(b_bits==2): in o_num == 000 bb?
		 * CASE(b_bits==3): in o_num == 000 bbb
		 */
		parms1.x1 = o_num;
		h_val = get_hexanum_from_octal(uls_ptr(parms1), a_bits);
		ch = num2char_radix(h_val);
		o_num = parms1.x2;

		if (ch != '0') *outptr++ = ch;

		if ((a_bits = 3 - b_bits) == 0) {
			o_num = *lptr++ - '0';
			a_bits = 3;
		}
	} else {
		a_bits = 3;
	}

	while (1) {
		/**
		 * get b bits, the #-of-bits at the left most.
		 * CASE(a_bits==1): in o_num == 00a bbb
		 * CASE(a_bits==2): in o_num == 0aa bb?
		 * CASE(a_bits==3): in o_num == aaa b??
		 */
		b_bits = 4 - a_bits;

		parms1.x1 = o_num = (o_num << 3) | (*lptr++ - '0');
		h_val = get_hexanum_from_octal(uls_ptr(parms1), a_bits);
		ch = num2char_radix(h_val);
		o_num = parms1.x2;

		*outptr++ = ch;
		if (lptr >= lptr_end) {
			break;
		}

		if ((a_bits = 3 - b_bits) == 0) {
			o_num = *lptr++ - '0';
			a_bits = 3;
		}
	}

	// return pure # of hexa digits not considering the length of '0x'
	return (int) (outptr - outbuf);
}

ULS_DECL_STATIC char*
ULS_QUALIFIED_METHOD(decstr2hexbin)(uls_outparam_ptr_t parms)
{
	// In case decimal string is "143230",
	// return-value(outptr): pointer of hexa bin: 022F7E
	//    outptr[0] = 0x02, outptr[1] = 0x2F, outptr[2] = 0x7E
	// nibbles: # of hexa digits, nibbles==5
	// Overwrites the input parms->line.

	char* wrd = parms->line;
	int wrdlen = parms->len;
	char *outptr, ch1, ch2;
	int  rem, n_nibbles = 0, is_nonzero;
	uls_outparam_t parms1;

	outptr = wrd + wrdlen;
	do {
		parms1.len = wrdlen;
		rem = div_decstr_by_16(wrd, uls_ptr(parms1));
		wrdlen = parms1.len;
		is_nonzero = (wrd[0] == '0') ? 0 : 1;

		ch2 = (char) rem;
		if (!is_nonzero) {
			*--outptr = ch2;
			n_nibbles += 1; // 1-nibbles added!
			break;
		}

		parms1.len = wrdlen;
		rem = div_decstr_by_16(wrd, uls_ptr(parms1));
		wrdlen = parms1.len;
		is_nonzero = (wrd[0] == '0') ? 0 : 1;

		ch1 = (char) rem;
		*--outptr = (ch1<<4) | ch2;
		n_nibbles += 2; // 2-nibbles added!
	} while (is_nonzero);

	parms->n = n_nibbles;
	return outptr;
}

int
ULS_QUALIFIED_METHOD(uls_dec2hex_str)(char* numstr, int n_digits, char* outptr) // UNUSED
{
	// reutrn the length of a string of hexa-digits without prepending the '0x'.
	// 'outptr' can be 'numstr'
	char ch, *hexbin;
	int  n_nibbles, i;
	uls_outparam_t parms1;

	if (n_digits < 0) n_digits = uls_strlen(numstr);

	parms1.line = numstr;
	parms1.len = n_digits;
	hexbin = decstr2hexbin(uls_ptr(parms1));
	n_nibbles = parms1.n;

	// n_nibbles==3 if hexbin == 4F2.
	if (n_nibbles % 2 != 0) {
		ch = *hexbin++;
		*outptr++ = num2char_radix(ch & 0x0F);
	}

	for (i=0; i < n_nibbles/2; i++) {
		ch = hexbin[i];
		*outptr++ = num2char_radix(ch >> 4);
		*outptr++ = num2char_radix(ch & 0x0F);
	}

	*outptr = '\0';
	return n_nibbles;
}

int
ULS_QUALIFIED_METHOD(uls_cvt_radix2dec_str)(int radix, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf)
{
	// Convert radix numstr numbuf1 with length n1_digits into decimal string 'outbuf'.
	int i, m, n;
	int n2_digits;
	char *decbuf, *decbuf_tmp;

	if (radix >= 10) {
		// n_digits=1: 2
		// n_digits=2: 2+2 + 2
		// n_digits=3: 2+2+2 + 2+2 + 2
		//   a1      a2      a3      a4      a5      a6
		//       2*2     2*3     2*4     2*5     2*6
		// a(n+1) = a(n) + 2*(n+1), a(1) = 2
		// a(n) = a(1) + sigma(2(k+1), 1, n-1) = n(n+1)
		m = n1_digits * (n1_digits + 1);

	} else {
		// b(n+1) = b(n) + n+1, b(1) = 1
		// b(n) = a(n) / 2 = n(n+1)/2
		// n1_digits * (n1_digits + 1) is an even number.
		m = n1_digits * (n1_digits + 1) / 2;
	}

	n = uls_ceil_log2(2*m + 1, 5); // +1 for '\0'

	// required-sizeof(outbuf) == a(n) + 1 (bytes), 1-digits/byte
	str_modify(outbuf, 0, NULL, n);
	decbuf = str_dataptr(outbuf);
	decbuf_tmp = decbuf + m;

	decbuf[0] = '0'; decbuf[1] = '\0';
	n2_digits = 1;

	for (i=0; i < n1_digits; i++) {
		m = is_num_radix(numbuf1[i], radix);

		n2_digits = mul_decstr_by_xx(decbuf, n2_digits, radix, decbuf_tmp);
		n2_digits = add_decstr_by_xx(decbuf_tmp, n2_digits, m, decbuf);
	}

	decbuf[n2_digits] = '\0';
	return n2_digits;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__cvt_radix2hexa_str_generic)(int radix, const char *numbuf, int n_digits,
	uls_outbuf_ptr_t outbuf, int k)
{
	int k0, val, n2_digits;
	uls_outparam_t parms1;
	uls_outbuf_t outbuf1;
	char *numbuf1, *outptr;

	str_init(uls_ptr(outbuf1), 64);
	n2_digits = uls_cvt_radix2dec_str(radix, numbuf, n_digits, uls_ptr(outbuf1));
	numbuf1 = outbuf1.buf;

	k0 = k;
	str_modify(outbuf, k0, NULL, n2_digits + 1); // +1 for '\0'
	outptr = str_dataptr(outbuf);

	// -0 --> -0
	for (parms1.len = n2_digits; ; ) {
		val = div_decstr_by_16(numbuf1, uls_ptr(parms1));
		outptr[k++] = num2char_radix(val);

		if (parms1.len == 1 && numbuf1[0] == '0') { // is_zero(numbuf1)
			break;
		}
	}

	// swap the digits except for '-'
	uls_reverse_bytes(outptr + k0, k - k0);
	str_free(uls_ptr(outbuf1));

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_cvt_radix2hexa_str_generic)(int radix, const char *numbuf, int n_digits, uls_outbuf_ptr_t outbuf, int k)
{
	if (n_digits < 0) n_digits = uls_strlen(numbuf);

	if (radix == 16) {
		// just copy numbuf to outbuf
		k = str_append(outbuf, k, numbuf, n_digits);
	} else {
		k = __cvt_radix2hexa_str_generic(radix, numbuf, n_digits, outbuf, k);
		str_putc(outbuf, k, '\0');
	}

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_cvt_radix_to_hexadecimal_str)(int radix, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k)
{
	int n2_digits;
	char *outptr;

	if (n1_digits < 0) n1_digits = uls_strlen(numbuf1);

	// get the memory for radix=2,4,8
	str_modify(outbuf, k, NULL, n1_digits + 1); // +1 for '\0'

	if (radix == 2) {
		outptr = str_dataptr_k(outbuf, k);
		n2_digits = uls_bin2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 4) {
		outptr = str_dataptr_k(outbuf, k);
		n2_digits = uls_quat2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 8) {
		outptr = str_dataptr_k(outbuf, k);
		n2_digits = uls_oct2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 16) {
		// just copy numbuf to outbuf
		k = str_append(outbuf, k, numbuf1, n1_digits);

	} else {
		k = uls_cvt_radix2hexa_str_generic(radix, numbuf1, n1_digits, outbuf, k);
	}

	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_cvt_radix_simple_num)(int radix1, const char *numbuf1, int numlen1, int radix2, uls_outbuf_ptr_t outbuf, int k)
{
	uls_uint64 num;
	int k0, val;
	char ch;

	if (radix1 == radix2) {
		// just copy numbuf1 to outbuf
		k = str_append(outbuf, k, numbuf1, numlen1);
		return k;
	}

	if ((num = get_uint64_from_radix_numstr(radix1, numbuf1, numlen1)) == 0) {
		str_putc(outbuf, k++, '0');
		str_putc(outbuf, k, '\0');
		return k;
	}

	k0 = k; do {
		val = num % radix2;
		ch = num2char_radix(val);
		str_putc(outbuf, k++, ch);
	} while ((num /= radix2) != 0);

	str_putc(outbuf, k, '\0');

	// swap the digits
	uls_reverse_bytes(str_dataptr_k(outbuf, k0), k - k0);
	return k;
}

int
ULS_QUALIFIED_METHOD(uls_radix2hexadecimal_str)(int radix1, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k)
{
	int radix2 = 16;
	int n_max_digits;
	static int array_of_n_max_digits[8] = {
		-1, 64, 32, 32, 21, 21, 21, 21
	};

	if (n1_digits < 0) n1_digits = uls_strlen(numbuf1);

	if (radix1 <= 8) {
		n_max_digits = array_of_n_max_digits[radix1-1];
	} else if (radix1 <= 16) {
		n_max_digits = 16; /* 64 / 4 */
	} else if (radix1 <= 32) {
		n_max_digits = 12; /* 64 / 5 */
	} else {
		n_max_digits = 10; /* 64 / 6 */
	}

	if (n1_digits <= n_max_digits) {
		// use system (built-in) integer register to convert
		k = uls_cvt_radix_simple_num(radix1, numbuf1, n1_digits, radix2, outbuf, k);
	} else {
		k = uls_cvt_radix_to_hexadecimal_str(radix1, numbuf1, n1_digits, outbuf, k);
	}

	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(skip_prefixed_zeros)(uls_outparam_ptr_t parms)
{
	// skip the zeros after '0x', '0b', '0o'.
	const char *lptr, *lptr0 = parms->lptr;
	uls_wch_t numsep = parms->wch;
	char prev_ch, ch;
	int m = 0;

	prev_ch = (parms->flags & ULS_NUM_FL_PREVCH_IS_SEP) ? numsep : 'z';

	for (lptr=lptr0; ; lptr++, prev_ch=ch) {
		if ((ch=*lptr) != '0') {
			if (ch != numsep || prev_ch == numsep) {
				break;
			}
		} else {
			++m; // # of zeros
		}
	}

	if (m > 0 && prev_ch == numsep ) {
		parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
	} else {
		parms->flags &= ~ULS_NUM_FL_PREVCH_IS_SEP;
	}

//	parms->n1 = (int) (lptr - lptr0);
	parms->lptr = lptr;

	return m;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__skip_radix_numstr)(uls_outparam_ptr_t parms, int radix,
	uls_outbuf_ptr_t numbuf, int k)
{
	const char *lptr, *lptr0 = parms->lptr;
	uls_wch_t numsep = parms->wch;
	char prev_ch, ch;
	int k0 = k;

	lptr = lptr0;
	prev_ch = *lptr;
	ch = uls_toupper(prev_ch);
	str_putc(numbuf, k++, ch);

	for (++lptr; ; lptr++) {
		ch = *lptr;

		if (is_num_radix(ch, radix) < 0) {
			if (ch == numsep) {
				if (prev_ch == numsep) break;
				prev_ch = ch;
			} else {
				break;
			}
		} else {
			prev_ch = ch;
			ch = uls_toupper(ch);
			str_putc(numbuf, k++, ch);
		}
	}

	if (prev_ch == numsep) {
		--lptr;
	}

	parms->len = k - k0;
	parms->lptr = lptr;

	return k;
}

/**
 * input: flags(ULS_NUM_FL_PREVCH_IS_SEP), wch, lptr
 * output: flags(ULS_NUM_FL_PREVCH_IS_SEP), len, lptr
 */
int
ULS_QUALIFIED_METHOD(uls_skip_radix_numstr)(uls_outparam_ptr_t parms, int radix, uls_outbuf_ptr_t numbuf, int k)
{
	// call __skip_radix_numstr after skip_prefixed_zeros
	int len, n_zeros;

	n_zeros = skip_prefixed_zeros(parms);

	if (is_num_radix(*parms->lptr, radix) < 0) {
		if (n_zeros > 0) { // # of zeros
			if (parms->flags & ULS_NUM_FL_PREVCH_IS_SEP) --parms->lptr;
			parms->flags |= ULS_NUM_FL_ZERO;
			parms->flags &= ~ULS_NUM_FL_PREVCH_IS_SEP;
			str_putc(numbuf, k++, '0');
			len = 1;
		} else {
			parms->flags &= ~ULS_NUM_FL_PREVCH_IS_SEP;
			len = 0;
		}

		parms->len = len;
	} else {
		k = __skip_radix_numstr(parms, radix, numbuf, k);
		parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
	}

	str_putc(numbuf, k, '\0');
	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_expo)(uls_outparam_ptr_t parms)
{
	int expo, minus = 0;
	const char *lptr0=parms->lptr, *lptr;
	char  ch;

	lptr = lptr0;
	if (*lptr != 'E' && *lptr != 'e') {
		parms->n = 0;
		return 0;
	}

	if ((ch = *++lptr) == '-') {
		++lptr;
		minus = 1;
	} else if (ch == '+') {
		++lptr;
	}

	lptr0 = lptr;
	for (expo=0; ; lptr++) {
		if (!uls_isdigit(ch=*lptr)) {
			break;
		}
		expo = 10 * expo + (ch - '0');
	}

	if (lptr0 == lptr) {
		parms->n = 0;
		return 0;
	}

	if (minus) expo = -expo;
	parms->n = expo;
	parms->lptr = lptr;

	return expo;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(append_expo)(int num, uls_outbuf_ptr_t numbuf, int k)
{
	char numstr[ULS_MAXSZ_NUMSTR + 1];
	int j;

	if (num == 0) {
		str_putc(numbuf, k, '\0');
		return k;
	}

	str_putc(numbuf, k++, 'E');
	if (num < 0) {
		str_putc(numbuf, k++, '-');
		num = -num;
	}

	numstr[ULS_MAXSZ_NUMSTR] = '\0';
	j = ULS_MAXSZ_NUMSTR;
	do {
		numstr[--j] = "0123456789"[num%10];
	} while ((num /= 10) != 0);

	k = str_append(numbuf, k, numstr + j, ULS_MAXSZ_NUMSTR - j);
	return k;
}

// To suppress the warning of '-Wunused-but-set-variable'
#define __PREVENT_COMPILE_WARN(var1,var2,var3) do { \
	parms1.lptr = var1; parms1.x1 = var2; \
	parms1.lptr_end = var3; } while (0)
/**
 * uls_num2stdfmt: tokbuf, k0(offset-in-tokbuf)
 *  input : flags, wch(number-separator), n1(length-of-prefix), n2(radix), lptr
 *  output : flags, n(exponent), len(#-of-digits), lptr
 *  returns : len_tokbuf
 */
int
ULS_QUALIFIED_METHOD(uls_num2stdfmt)(uls_outparam_ptr_t parms, uls_outbuf_ptr_t numbuf, int k)
{
	const char *lptr, *lptr1, *lptr2;
	int  expo, n1, k0, k1, j, m, n_zeros, has_zero;
	uls_wch_t numsep = parms->wch;
	char ch, ch2, *wrd;
	uls_outparam_t parms1;

	parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
	lptr1 = lptr = parms->lptr;
	str_putc(numbuf, k++, '.');
	k0 = k;

	ch = *lptr;
	if (ch == '.' || (ch == '0' && lptr[1] == '.')) {
		// ".xxxx", "0.xxxx",  ".000xxxx", or "0.000xxxx"
		n1 = 0;
		if (ch == '.') {
			has_zero = 0;
			lptr2 = lptr;
		} else { // ch == '0'
			has_zero = 1;
			lptr2 = ++lptr;
		}

		parms->flags |= ULS_NUM_FL_FLOAT;

		++parms->lptr ; // next to '.'
		parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
		n_zeros = skip_prefixed_zeros(parms);

		if (!uls_isdigit(*parms->lptr)) {
			parms->flags |= ULS_NUM_FL_ZERO;

			if (n_zeros > 0) {
				++lptr2;
			} else if (!has_zero) {
				return 0;
			}

			// 0.000E-3
			check_expo(parms);
			parms->n = 0;
			k = str_append(numbuf, 0, "0.", 2);
			parms->len = 1; // # of digits
			return 2;
		}

		if ((expo = n_zeros) > 0) expo = -expo;
		lptr2 = parms->lptr;

	} else if (uls_isdigit(ch)) {
		parms1.lptr = lptr;
		parms1.wch = numsep;
		k = __skip_radix_numstr(uls_ptr(parms1), 10, numbuf, k);
		n1 = parms1.len;
		lptr2 = lptr = parms1.lptr;
		n_zeros = 0;

		k1 = k;
		if ((ch=*lptr) == '.') {
			parms->flags |= ULS_NUM_FL_FLOAT;
			parms1.lptr = ++lptr; // next to '.'
			parms1.wch = numsep;
			parms1.flags = ULS_NUM_FL_PREVCH_IS_SEP;
			n_zeros = skip_prefixed_zeros(uls_ptr(parms1));
			lptr = parms1.lptr;
			if (*lptr == numsep && (parms1.flags & ULS_NUM_FL_PREVCH_IS_SEP)) --lptr;
		}

		if (!uls_isdigit(ch=*lptr)) {
			// 504100.E-2  5041000E-2
			// find 0's backward '.'
			for (m=0,j=-1; ; j--) {
				if ((ch2 = lptr2[j]) == '0') {
					++m;
				} else if (ch2 != numsep) {
					break;
				}
			}

			k -= m;
			parms->len = n1 - m; // # of digits
			expo = n1;

			// 10.000E-3
			parms1.lptr = lptr;
			// Don't use number separator in exponent part.
			expo += check_expo(uls_ptr(parms1));

			if (lptr != parms1.lptr) {
				lptr = parms1.lptr;
				parms->flags |= ULS_NUM_FL_FLOAT;
			} else if (!(parms->flags & ULS_NUM_FL_FLOAT)) {
				// It's an integer: convert decimal format to hexa-decimal format
				k = k1;
				parms->n = 0;
				parms->lptr = lptr;

				j = (*str_dataptr(numbuf) == '-') ? 1 : 0;
				parms->len = k - k0;
				uls_memmove(str_dataptr_k(numbuf,j), str_dataptr_k(numbuf,k0), parms->len);
				k = j + parms->len;
				str_putc(numbuf, k, '\0');

				return k;
			}

			parms->n = expo;
			parms->lptr = lptr;
			if (n_zeros > 0) ++lptr2;
			k = append_expo(expo, numbuf, k);
			return k;
		}

		for (j=0; j<n_zeros; j++) {
			str_putc(numbuf, k++, '0');
		}

		lptr2 = lptr;
		expo = 0;

	} else {
		return 0;
	}

	parms1.lptr = lptr2;
	parms1.wch = numsep;
	k1 = k;
	k = __skip_radix_numstr(uls_ptr(parms1), 10, numbuf, k);

	wrd = str_dataptr(numbuf);
	if (wrd[k-1] == '0') {
		while (1) {
			if (wrd[--k] != '0') {
				str_putc(numbuf, ++k, '\0');
				break;
			}
		}
	}

	expo += check_expo(uls_ptr(parms1));
	lptr = parms1.lptr;
	parms->len = k - k0; // # of digits
	parms->n = expo += n1;
	parms->lptr = lptr;

	k = append_expo(expo, numbuf, k);
	__PREVENT_COMPILE_WARN(lptr1, n1, lptr2);
	return k;
}

int
ULS_QUALIFIED_METHOD(uls_extract_number)(uls_outparam_ptr_t parms, uls_outbuf_ptr_t tokbuf, int k)
{
	const char *lptr0 = parms->lptr, *srp; // StandardRadixPrefix
	int len_prefix = parms->n1, radix = parms->n2;
	int k1, ch, n_zeros;

	parms->n = 0;

	if (len_prefix  > 0) {
		srp = parms->lptr_end;

		k = str_append(tokbuf, k, srp, -1);
		k1 = k;

		parms->lptr += len_prefix;
		parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
		k = uls_skip_radix_numstr(parms, radix, tokbuf, k);

		if (k > k1) {
			if (*str_dataptr_k(tokbuf, k1) == '0') {
				parms->flags |= ULS_NUM_FL_ZERO;
				str_putc(tokbuf, 0, '0');
				str_putc(tokbuf, 1, '\0');
				k = 1;
			}
		} else {
			parms->lptr = lptr0;
			n_zeros = skip_prefixed_zeros(parms);

			if (n_zeros > 0) {
				parms->flags |= ULS_NUM_FL_ZERO;
				if (*parms->lptr == '.') {
					++parms->lptr;
					parms->flags |= ULS_NUM_FL_FLOAT;
				}
				skip_prefixed_zeros(parms);
				check_expo(parms);

				if (parms->flags & ULS_NUM_FL_FLOAT) {
					str_putc(tokbuf, 0, '0');
					str_putc(tokbuf, 1, '.');
					str_putc(tokbuf, 2, '\0');
					parms->len = 1; // # of digits
					k = 2;
				} else {
					str_putc(tokbuf, 0, '0');
					str_putc(tokbuf, 1, '\0');
					k = 1;
				}
			}
		}

		return k;
	}

	parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
	n_zeros = skip_prefixed_zeros(parms);
	ch = *parms->lptr;

	if (uls_isdigit(ch) || (ch == '.' && uls_isdigit(parms->lptr[1]))) {
		if (ch == '.') {
			if (parms->flags & ULS_NUM_FL_PREVCH_IS_SEP) {
				if (n_zeros > 0) {
					--parms->lptr;
					parms->flags |= ULS_NUM_FL_ZERO;
					str_putc(tokbuf, 0, '0');
					str_putc(tokbuf, 1, '\0');
					parms->len = 1; // # of digits
					k = 1;
				} else {
					parms->lptr = lptr0;
					k = -1;
				}
				return k;
			}
			parms->flags |= ULS_NUM_FL_PREVCH_IS_SEP;
		}
		k = uls_num2stdfmt(parms, tokbuf, k);
	} else {
		if (n_zeros > 0) {
			// set tokbuf to zero
			parms->flags |= ULS_NUM_FL_ZERO;
			if (ch == '.') {
				str_putc(tokbuf, 0, '0');
				str_putc(tokbuf, 1, '.');
				str_putc(tokbuf, 2, '\0');
				k = 2;
				parms->flags |= ULS_NUM_FL_FLOAT;
				parms->len = 1;
				++parms->lptr;
			} else {
				str_putc(tokbuf, 0, '0');
				str_putc(tokbuf, 1, '\0');
				k = 1;
				parms->len = 1;
			}
			check_expo(parms);
		} else {
	 		parms->flags = 0;
	 		parms->len = 0;
			parms->lptr = lptr0;
			k = -1;
		}
	}

	return k;
}

void
ULS_QUALIFIED_METHOD(uls_number)(const char *numstr, int len_numstr, int n_expo, uls_outparam_ptr_t parms)
{
	const char *lptr = numstr;
	int len, radix;

	parms->flags = 0;

	if (*lptr == '-') {
		parms->flags |= ULS_NUM_FL_MINUS;
		++lptr;
	}

	if ((len = uls_find_standard_prefix_radix(lptr, &radix)) > 0) {
		parms->lptr = numstr + len;
		parms->len = parms->n1 = len_numstr - len;
		parms->n2 = 0;
		parms->n = 0;
		parms->x1 = radix;

	} else {
		parms->x1 = 10; // radix
		if (*lptr == '.') {
			parms->flags |= ULS_NUM_FL_FLOAT;
			++lptr;
		}
		len = len_numstr - (int) (lptr - numstr);

		if (*lptr == '0') {
			parms->flags |= ULS_NUM_FL_ZERO;
			parms->flags &= ~ULS_NUM_FL_MINUS;
			if (lptr[1] == '.') {
				parms->flags |= ULS_NUM_FL_FLOAT;
			}
			parms->lptr = numstr + len_numstr; // == null-string
			parms->len = parms->n1 = parms->n2 = parms->n = 0;

		} else if (parms->flags & ULS_NUM_FL_FLOAT) {
			parms->lptr = lptr; // NOT null-terminated string
			parms->len = len;   // #-of-digits

			if (n_expo <= 0) {
				parms->n1 = 0;
				parms->n2 = len;
				parms->n = n_expo;
			} else if (n_expo > 0) {
				if (n_expo <= len) {
					parms->n1 = n_expo;         // len(significand)
					parms->n2 = len - n_expo;   // len(mentissa)
					parms->n = 0;               // exponent
				} else {
					parms->n1 = len;
					parms->n2 = 0;
					parms->n = n_expo - len;
				}
			}
		} else {  // decimal-integer
			parms->lptr = lptr;
			parms->len = parms->n1 = len;
			parms->n2 = 0;
			parms->n = 0;
		}
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__make_decstr_gexpr)(const char *tokstr, int l_tokstr, int n_expo,
	uls_outbuf_ptr_t numbuf, int k)
{
	uls_outparam_t parms1;
	int j, m_expo, n1_digits, n2_digits, l_radstr;
	const char *radstr;

	uls_number(tokstr, l_tokstr, n_expo, uls_ptr(parms1));

	m_expo = parms1.n;
	n1_digits = parms1.n1;
 	n2_digits = parms1.n2;
 	radstr = parms1.lptr;
 	l_radstr = parms1.len;

	if (m_expo > 0) {
		if (m_expo + l_radstr > 6) return 0;
		k = str_append(numbuf, k, radstr, l_radstr);
		for (j=0; j<m_expo; j++) {
			str_putc(numbuf, k++, '0');
		}
		str_putc(numbuf, k++, '.');
	} else if (m_expo < 0) {
		if (m_expo <= -6) return 0;
		str_putc(numbuf, k++, '.');
		for (j=0; j<-m_expo; j++) {
			str_putc(numbuf, k++, '0');
		}
		k = str_append(numbuf, k, radstr, l_radstr);
	} else {
		k = str_append(numbuf, k, radstr, n1_digits);
		str_putc(numbuf, k++, '.');
		k = str_append(numbuf, k, radstr + n1_digits, n2_digits);
	}

	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__make_radixint_str_gexpr)(const char *radstr, int l_radstr, int radix,
	uls_outbuf_ptr_t numbuf, int k)
{
	int n, len = l_radstr;
	char *numstr2;

	// the prefix of (standard) hexadecimal
	str_putc(numbuf, k++, '0');
	str_putc(numbuf, k++, 'x');

	switch (radix) {
	case 2:
		len = uls_bin2hex_str(radstr, len, str_dataptr_k(numbuf, k));
		k += len;
		break;
	case 4:
		len = uls_quat2hex_str(radstr, len, str_dataptr_k(numbuf, k));
		k += len;
		break;
	case 8:
		len = uls_oct2hex_str(radstr, len, str_dataptr_k(numbuf, k));
		k += len;
		break;
	case 16:
		k = str_append(numbuf, k, radstr, len);
		break;
	default: // radix = { 3, 10 }
		n = uls_ceil_log2(len + 1, 3);
		numstr2 = uls_malloc_buffer(n);
		uls_memcopy(numstr2, radstr, len);
		numstr2[len] = '\0';
		k = uls_radix2hexadecimal_str(radix, numstr2, len, numbuf, k);
		uls_mfree(numstr2);
		break;
	}

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_number_gexpr)(const char *tokstr, int l_tokstr, int n_expo,
	uls_outbuf_ptr_t numbuf)
{
	const char *radstr;
	int i, l_prefix, l_radstr, radix, k = 0;

	if ( (l_tokstr == 1 && tokstr[0] == '0') ||
		(l_tokstr == 2 && tokstr[0] == '0' && tokstr[1] == '.')) {
		str_putc(numbuf, k, '\0');
		return 0;
	}

	radstr = tokstr;
	l_radstr = l_tokstr;
	if (*radstr == '-') {
		str_putc(numbuf, k++, '-');
		++radstr;
		--l_radstr;
	}

	if (*radstr == '.') {
		for (i = 1; i < l_radstr; i++) {
			if (radstr[i] == 'E') {
				l_radstr = i;
				break;
			}
		}
		k = __make_decstr_gexpr(radstr, l_radstr, n_expo, numbuf, k);
	} else {
		l_prefix = uls_find_standard_prefix_radix(radstr, &radix);
		if (radix != 10) {
			radstr += l_prefix;
			l_radstr -= l_prefix;
			k = __make_radixint_str_gexpr(radstr, l_radstr, radix, numbuf, k);
		} else {
			k = 0;
		}
	}

	str_putc(numbuf, k, '\0');
	return k;
}

