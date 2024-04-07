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
 *     written by Stanley Hong <link2next@gmail.com>, May 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_NUM__
#include "uls/uls_num.h"
#include "uls/uls_prim.h"
#include "uls/uls_log.h"
#endif

#define num2char_radix(val) (((val) >= 10) ?  (val) - 10 + 'A' : (val) + '0')

ULS_DECL_STATIC uls_uint64
ULS_QUALIFIED_METHOD(get_uint64_from_radix_numstr)(int radix, const char *numbuf, int numlen1)
{
	int i, n;
	uls_uint64 num = 0;
	char ch;

	for (i=0; i<numlen1; i++) {
		ch = numbuf[i];

		n = is_num_radix(ch, radix);
		num = num * radix + n;
	}

	return num;
}

int
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
	uls_reverse_bytes(outbuf->buf + k0, k - k0);

	return k;
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

	if (numstr_tmp != NULL) {
		uls_mfree(numstr_tmp);
	}

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
	int i, k=0, n, m;
	const char *lptr = numstr;
	int h_val;

	if (n_digits < 0) n_digits = uls_strlen(numstr);

	n = n_digits / 4; // 4-bits
	m = n_digits % 4; // 4-bits

	if (m > 0) {
		h_val = binstr2hexval(lptr, m); lptr += m;
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
ULS_QUALIFIED_METHOD(uls_quaternary2hex_str)(const char* numstr, int n_digits, char* outbuf)
{
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
ULS_QUALIFIED_METHOD(uls_oct2hex_str)(const char *numbuf, int n_digits, char* outbuf)
{
	const char  *lptr;
	const char  *lptr_end;
	char ch, *outptr;
	unsigned int o_num;
	int  n_octals, a_bits, b_bits, h_val;
	uls_outparam_t parms1;

	if (n_digits < 0) n_digits = uls_strlen(numbuf);

	lptr = numbuf;
	lptr_end = lptr + n_digits;

	for ( ; lptr < lptr_end; lptr++) {
		if (*lptr != '0') break;
	}

	n_octals = (int) (lptr_end - lptr);
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

	return (int) (outptr - outbuf);
}

ULS_DECL_STATIC char*
ULS_QUALIFIED_METHOD(decstr2hexbin)(uls_outparam_ptr_t parms)
{
	// In case decimal string is "143230",
	// return-value(outptr): pointer of hexa bin: 0x022F7E
	//    outptr[0] = 0x02, outptr[1] = 0x2F, outptr[2] = 0x7E
	// nibbles: # of hexa digits, nibbles==5

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
ULS_QUALIFIED_METHOD(uls_dec2hex_str)(char* wrd, int wrdlen, char* outptr)
{
	char ch, *hexbin;
	int  n_nibbles, i;
	uls_outparam_t parms1;

	if (wrdlen < 0) wrdlen = uls_strlen(wrd);

	parms1.line = wrd;
	parms1.len = wrdlen;
	hexbin = decstr2hexbin(uls_ptr(parms1));
	n_nibbles = parms1.n;

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
	int i, m;
	int n2_digits;
	char *decbuf, *decbuf_tmp;

	if (radix >= 10) {
		m = n1_digits * (n1_digits + 1);

	} else {
		m = n1_digits * (n1_digits + 1) / 2;
	}

	// required-sizeof(outbuf) == a(n) + 1 (bytes), 1-digits/byte
	str_modify(outbuf, 0, NULL, 2*m + 1); // +1 for '\0'
	decbuf = outbuf->buf; decbuf_tmp = decbuf + m;

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

int
ULS_QUALIFIED_METHOD(__uls_cvt_radix2hexa_str_generic)(int radix, const char *numbuf, int n_digits, uls_outbuf_ptr_t outbuf, int k)
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
	outptr = outbuf->buf;

	// -0 --> -0
	for (parms1.len = n2_digits; ; ) {
		val = div_decstr_by_16(numbuf1, uls_ptr(parms1));
		outptr[k++] = num2char_radix(val);

		if (parms1.len == 1 && numbuf1[0] == '0') { // is_zero(numbuf1)
			break;
		}
	}
	outptr[k] = '\0';

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
		k = __uls_cvt_radix2hexa_str_generic(radix, numbuf, n_digits, outbuf, k);
	}

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_cvt_radix_to_hexadecimal_str)(int radix, const char *numbuf1, int n1_digits, uls_outbuf_ptr_t outbuf, int k)
{
	int n2_digits;
	char *outptr;

	if (n1_digits < 0) n1_digits = uls_strlen(numbuf1);

	str_modify(outbuf, k, NULL, n1_digits + 1); // +1 for '\0'

	if (radix == 2) {
		outptr = outbuf->buf + k;
		n2_digits = uls_bin2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 4) {
		outptr = outbuf->buf + k;
		n2_digits = uls_quaternary2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 8) {
		outptr = outbuf->buf + k;
		n2_digits = uls_oct2hex_str(numbuf1, n1_digits, outptr);
		outptr[n2_digits] = '\0';
		k += n2_digits;

	} else if (radix == 16) {
		k = str_append(outbuf, k, numbuf1, n1_digits);

	} else {
		k = uls_cvt_radix2hexa_str_generic(radix, numbuf1, n1_digits, outbuf, k);
	}

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
		k = uls_cvt_radix_simple_num(radix1, numbuf1, radix2, n1_digits, outbuf, k);
	} else {
		k = uls_cvt_radix_to_hexadecimal_str(radix1, numbuf1, n1_digits, outbuf, k);
	}

	return k;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(skip_prefixed_zeros)(uls_outparam_ptr_t parms)
{
	// skip the zeros after '0x', '0b', '0o'.
	const char *lptr0 = parms->lptr, *lptr;
	uls_wch_t numsep = parms->wch;
	char prev_ch, ch;
	int m = 0;

	prev_ch = parms->flags ? numsep : 'z';

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
		parms->flags = 1;
	} else {
		parms->flags = 0;
	}

	parms->n1 = (int) (lptr - lptr0);
	parms->lptr = lptr;

	return m;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__skip_radix_number)(uls_outparam_ptr_t parms, int radix, uls_wch_t numsep,
	uls_outbuf_ptr_t numbuf, int k)
{
	const char *lptr = parms->lptr, *lptr1;
	char prev_ch, ch;
	int k1 = k;

	lptr1 = lptr;
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

	parms->lptr_end = lptr1;
	parms->n1 = (int) (lptr - lptr1);

	parms->len = k - k1;
	parms->lptr = lptr;

	return k;
}

int
ULS_QUALIFIED_METHOD(skip_radix_number)(uls_outparam_ptr_t parms, int radix, uls_outbuf_ptr_t numbuf, int k)
{
	const char *lptr0 = parms->lptr, *lptr1, *lptr;
	uls_wch_t numsep = parms->wch;
	int n1, len, n_zeros, is_prev_numsep;
	uls_outparam_t parms1;

	parms1.lptr = lptr0;
	parms1.wch = parms->wch; // numsep
	parms1.flags = parms->flags; // is prev_ch numsep?

	n_zeros = skip_prefixed_zeros(uls_ptr(parms1));
	is_prev_numsep = parms1.flags;
	lptr = parms1.lptr;

	if (is_num_radix(*lptr, radix) < 0) {
		parms->lptr_end = lptr1 = lptr0;

		if (n_zeros > 0) { // # of zeros
			if (is_prev_numsep) --lptr;

			parms->lptr = lptr;
			parms->flags = 0; // zero
			str_putc(numbuf, k++, '0');
			len = 1;
		} else {
			parms->flags = -1; // NA
			len = 0;
		}

		n1 = (int) (lptr - lptr1);
		parms->n1 = n1;
		parms->len = len;

	} else {
		parms->lptr = lptr;
		k = __skip_radix_number(parms, radix, numsep, numbuf, k);
		parms->flags = 1; // is_nonzero == true
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

#define ZERO_RET(lptr0) do { \
	parms->flags |= ULS_NUM_FL_ZERO; \
	parms->flags &= ~ULS_NUM_FL_MINUS; \
	parms->lptr_end = lptr0; parms->n1 = 1; \
	parms->lptr = lptr0 + 1; \
	str_putc(numbuf, 0, '0'); str_putc(numbuf, 1, '\0'); parms->len = 1; \
} while (0)

int
ULS_QUALIFIED_METHOD(num2stdfmt_0)(uls_outparam_ptr_t parms, uls_outbuf_ptr_t numbuf, int k)
{
	// process only integer, 0(zero), octal, binary, hexadecimal
	const char *lptr=parms->lptr, *lptr0, *lptr1;
	uls_wch_t numsep = parms->wch;
	int len_prefix = parms->n1, radix1 = parms->n2;
	int k1, n1, len, is_prev_numsep;
	char *numstr2;
	uls_outparam_t parms1;

	parms->n2 = parms->n = 0; // unused

	if (len_prefix > 1) {
		is_prev_numsep = 1;
	} else {
		is_prev_numsep = 0;
	}

	lptr0 = lptr;
	lptr += len_prefix;

	k1 = k;
	parms1.wch = numsep; parms1.flags = is_prev_numsep;
	parms1.lptr = lptr;
	k = skip_radix_number(uls_ptr(parms1), radix1, numbuf, k1);

	lptr = parms1.lptr;
	if (parms1.flags == 0) {
		lptr0 = lptr - 1;
		ZERO_RET(lptr0);
		return 1;
	}

	if (parms1.flags < 0) { // '0x' isn't number prefix.
		if (len_prefix == 1) {
			parms->flags |= ULS_NUM_FL_ZERO;
			parms->flags &= ~ULS_NUM_FL_MINUS;

			parms1.lptr = lptr;
			check_expo(uls_ptr(parms1));
			if (lptr != parms1.lptr) {
				parms->flags |= ULS_NUM_FL_FLOAT;
				lptr = parms1.lptr;
				str_append(numbuf, 0, ".0", 2);
				parms->len = 1; // # of digits
				parms->lptr = lptr;
				k = 2;
			} else {
				lptr0 = lptr - 1;
				ZERO_RET(lptr0);
				k = 1;
			}
		} else {
			ZERO_RET(lptr0);
			k = 1;
		}
		return k;
	}

	lptr1 = parms1.lptr_end;
	n1 = parms1.n1;
	len = parms1.len;

	if (radix1 != 16) {
		switch (radix1) {
		case 2:
			len = uls_bin2hex_str(numbuf->buf + k1, len, numbuf->buf + k1);
			k = k1 + len; str_putc(numbuf, k, '\0');
			break;
		case 4:
			len = uls_quaternary2hex_str(numbuf->buf + k1, len, numbuf->buf + k1);
			k = k1 + len; str_putc(numbuf, k, '\0');
			break;
		case 8:
			len = uls_oct2hex_str(numbuf->buf + k1, len, numbuf->buf + k1);
			k = k1 + len; str_putc(numbuf, k, '\0');
			break;
		default:
			numstr2 = uls_strdup(numbuf->buf + k1, len);
			k = uls_radix2hexadecimal_str(radix1, numstr2, len, numbuf, k1);
			uls_mfree(numstr2);
			break;
		}
	}

	parms->lptr_end = lptr1;
	parms->n1 = n1;
	parms->len = len;
	parms->lptr = lptr;

	return k;
}
#undef ZERO_RET

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
#define __FORCE2USE(var1,var2,var3,var4) do { \
	parms1.lptr_end = var1; parms1.x1 = var2; \
	parms1.lptr_end = var3; parms1.x2 = var4; } while (0)

int
ULS_QUALIFIED_METHOD(num2stdfmt)(uls_outparam_ptr_t parms, uls_outbuf_ptr_t numbuf, int k)
{
	const char *lptr=parms->lptr, *lptr1, *lptr2;
	int  expo, n1, n2, k0, k1, j, m, n_zeros, has_zero;
	uls_wch_t numsep = parms->wch;
	char ch, ch2;
	uls_outparam_t parms1;

	ch = *lptr;
	lptr1 = lptr;
	str_putc(numbuf, k++, '.');
	k0 = k;

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

		parms1.lptr = ++lptr; // next to '.'
		parms1.wch = numsep; parms1.flags = 1;
		n_zeros = skip_prefixed_zeros(uls_ptr(parms1));
		lptr = parms1.lptr;

		if (!uls_isdigit(*lptr)) {
			if (n_zeros > 0) {
				++lptr2;
			} else if (!has_zero) {
				__FORCE2USE(lptr1, n1, lptr2, n1);
				return 0;
			}

			// 0.000E-3
			parms1.lptr = lptr;
			check_expo(uls_ptr(parms1));
			lptr = parms1.lptr;

			parms->flags |= ULS_NUM_FL_ZERO;
			parms->flags &= ~ULS_NUM_FL_MINUS;
			parms->n = 0;
			parms->lptr = lptr;
			k = str_append(numbuf, 0, ".0", 2);
			parms->len = 1; // # of digits

			__FORCE2USE(lptr1, n1, lptr2, n1);
			return 2;
		}

		if ((expo = n_zeros) > 0) expo = -expo;
		lptr2 = lptr;

	} else if (uls_isdigit(ch)) {
		parms1.lptr = lptr;
		k = __skip_radix_number(uls_ptr(parms1), 10, numsep, numbuf, k);
		n1 = parms1.len;
		lptr2 = lptr = parms1.lptr;
		n_zeros = 0;

		k1 = k;
		if ((ch=*lptr) == '.') {
			parms->flags |= ULS_NUM_FL_FLOAT;
			parms1.lptr = ++lptr; // next to '.'
			parms1.wch = numsep; parms1.flags = 1;
			n_zeros = skip_prefixed_zeros(uls_ptr(parms1));
			lptr = parms1.lptr;
			if (*lptr == numsep && parms1.flags) --lptr;
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
			} else if ((parms->flags & ULS_NUM_FL_FLOAT) == 0) {
				// It's an integer: convert decimal format to hexa-decimal format
				k = k1;
				parms->n = 0;
				parms->lptr = lptr;

				j = (numbuf->buf[0] == '-') ? 1 : 0;
				parms->len = _uls_tool_(dec2hex_str)(numbuf->buf + k0, k - k0, numbuf->buf + j);
				k = j + parms->len;

				__FORCE2USE(lptr1, n1, lptr1, n1);
				str_putc(numbuf, k, '\0');
				return k;
			}

			parms->n = expo;
			parms->lptr = lptr;
			if (n_zeros > 0) ++lptr2;

			__FORCE2USE(lptr1, n1, lptr2, n1);
			k = append_expo(expo, numbuf, k);
			return k;
		}

		for (j=0; j<n_zeros; j++) {
			str_putc(numbuf, k++, '0');
		}

		lptr2 = lptr;
		expo = 0;

	} else {
		__FORCE2USE(lptr1, 0, lptr1, 0);
		return 0;
	}

	parms1.lptr = lptr2;
	k1 = k;
	k = __skip_radix_number(uls_ptr(parms1), 10, numsep, numbuf, k);
	n2 = parms1.len;

	if (numbuf->buf[k - 1] == '0') {
		for (--k; ; ) {
			if (numbuf->buf[--k] != '0') {
				str_putc(numbuf, ++k, '\0');
				n2 = k - k1;
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
	__FORCE2USE(lptr1, n1, lptr2, n2);
	return k;
}
#undef __FORCE2USE
