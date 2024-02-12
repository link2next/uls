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
  <file> uls_prim.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_PRIM__
#include "uls/uls_prim.h"

#include <stdlib.h>
#include <string.h>
#ifndef ULS_WINDOWS
#include <unistd.h>
#endif
#endif

ULS_DECL_STATIC _ULS_INLINE int
ULS_QUALIFIED_METHOD(__find_first_1bit)(char ch, int j1, int j2)
{
	int j;

	ch <<= j1;

	for (j=j1; j<=j2; j++) {
		if (ch & BYTE_LEFT_MOST_1) return j;
		ch <<= 1;
	}

	return -1;
}

int
ULS_QUALIFIED_METHOD(uls_lf_number_u)(char *numstr, unsigned int num, int base_shift)
{
	unsigned int  base_mask;
	register int   i;

	i = ULS_MAXSZ_NUMSTR; numstr[ULS_MAXSZ_NUMSTR] = '\0';

	if (num == 0) {
		numstr[--i] = '0';
		return i;
	}

	if (base_shift==ULS_LF_SHIFT_BASE_DEC) {
		do {
			numstr[--i] = "0123456789"[(int) (num%10)];
			num /= 10;
		} while (num!=0);

	} else if (base_shift==ULS_LF_SHIFT_BASE_OCT) {
		do {
			numstr[--i] = '0' + (num % 8);
			num /= 8;
		} while (num!=0);

	} else {
		base_mask = (1 << base_shift) - 1;

		do {

			numstr[--i] = "0123456789ABCDEF"[ (int) (num & base_mask) ];
			num >>= base_shift;
		} while (num!=0);
	}

	return i;
}

int
ULS_QUALIFIED_METHOD(uls_lf_number_lu)(char *numstr, unsigned long num, int base_shift)
{
	unsigned long  base_mask;
	register int   i;

	i = ULS_MAXSZ_NUMSTR; numstr[ULS_MAXSZ_NUMSTR] = '\0';

	if (num == 0) {
		numstr[--i] = '0';
		return i;
	}

	if (base_shift==ULS_LF_SHIFT_BASE_DEC) {
		do {
			numstr[--i] = "0123456789"[(int) (num%10)];
			num /= 10;
		} while (num!=0);

	} else {
		base_mask = (1 << base_shift) - 1;

		do {
			numstr[--i] = "0123456789ABCDEF"[ (int) (num & base_mask) ];
			num >>= base_shift;
		} while (num!=0);
	}

	return i;
}

int
ULS_QUALIFIED_METHOD(uls_lf_number_Lu)(char *numstr, unsigned long long num, int base_shift)
{
	unsigned long long base_mask;
	register int   i;

	i = ULS_MAXSZ_NUMSTR; numstr[ULS_MAXSZ_NUMSTR] = '\0';

	if (num == 0) {
		numstr[--i] = '0';
		return i;
	}

	if (base_shift==ULS_LF_SHIFT_BASE_DEC) {
		do {
			numstr[--i] = "0123456789"[(int) (num % 10)];
			num /= 10;
		} while (num!=0);

	} else {
		base_mask = (1ULL << base_shift) - 1;
		do {
			numstr[--i] = "0123456789ABCDEF"[ (int) (num & base_mask) ];
			num >>= base_shift;
		} while (num!=0);
	}

	return i;
}

#ifndef ULS_DOTNET

void
ULS_QUALIFIED_METHOD(err_log_puts)(const char* mesg, int len)
{
	int rc,  fd = STDERR_FILENO;

	if (len < 0) {
		len = uls_strlen(mesg);
	}

	if (len > 0) {
#ifdef ULS_WINDOWS
		rc = _write(fd, mesg, len);
#else
		rc = write(fd, mesg, len);
#endif
		if (rc < 0) {
			exit(1);
		}
	}
}

int
ULS_QUALIFIED_METHOD(uls_vsnprintf_primitive)(char *buf,  int bufsiz, const char* fmt, va_list args)
{
	int   j, len, ival, minus, buflen, bufsiz1;
	const char  *fmtptr, *wrdptr;
	char ch, numbuf[ULS_MAXSZ_NUMSTR+1];
	unsigned int uval;

	bufsiz1 = bufsiz - 1;
	wrdptr = fmtptr = fmt;

	for (buflen=0; ; ) {
		for ( ; (ch=*fmtptr) != '\0'; fmtptr++) {
			if (ch == '%') break;
		}

		if ((len = (int) (fmtptr - wrdptr)) > 0) {
			if (buflen + len > bufsiz1) break;
			uls_memcopy(buf+buflen, wrdptr, len);
			buflen += len;
		}

		if (ch== '\0') {
			break;
		}

		ch = *++fmtptr;
		switch (ch) {
		case 's':
			wrdptr = (const char *) va_arg(args, const char *);
			if (wrdptr == NULL) wrdptr = "<null>";
			len = uls_strlen(wrdptr);
			break;

		case 'u':
			uval = (unsigned int) va_arg(args, unsigned int);
			j = uls_lf_number_u(numbuf, uval, ULS_LF_SHIFT_BASE_DEC);
			wrdptr = numbuf + j;
			len = ULS_MAXSZ_NUMSTR - j;
			break;

		case 'd':
			ival = (int) va_arg(args, int);
			if (ival < 0) {
				minus = 1;
				uval = -ival;
			} else {
				minus = 0;
				uval = ival;
			}

			j = uls_lf_number_u(numbuf, uval, ULS_LF_SHIFT_BASE_DEC);
			if (minus) {
				numbuf[--j] = '-';
			}
			wrdptr = numbuf + j;
			len = ULS_MAXSZ_NUMSTR - j;
			break;

		case 'X':
		case 'x':
			uval = (unsigned int) va_arg(args, unsigned int);
			j = uls_lf_number_u(numbuf, uval, ULS_LF_SHIFT_BASE_HEX);
			wrdptr = numbuf + j;
			len = ULS_MAXSZ_NUMSTR - j;
			break;

		case 'c':
			ival = (int) va_arg(args, int);
			numbuf[0] = (char) ival; numbuf[1] = '\0';
			wrdptr = numbuf;
			len = 1;
			break;

		case '%':
			wrdptr = "%%";
			len = 1;
			break;

		default:
			err_log_puts("unknown conversion specifier in ...", -1);
			err_log_puts(fmtptr, -1);
			exit(1);
			break;
		}

		if (len > 0) {
			if (buflen + len > bufsiz1) break;
			uls_memcopy(buf+buflen, wrdptr, len);
			buflen += len;
		}

		wrdptr = ++fmtptr;
	}

 	buf[buflen] = '\0';
	return buflen;
}

int
ULS_QUALIFIED_METHOD(uls_snprintf_primitive)(char *buf,  int bufsiz, const char* fmt, ...)
{
	int len;
	va_list args;

	va_start(args, fmt);
	len =  uls_vsnprintf_primitive(buf,  bufsiz, fmt, args);
	va_end(args);

	return len;
}

int
ULS_QUALIFIED_METHOD(err_vlog_primitive)(const char* fmt, va_list args)
{
	char buf[256];
	int len;

	len = uls_vsnprintf_primitive(buf, sizeof(buf), fmt, args);
	if (len < sizeof(buf)) {
		buf[len++] = '\n';
	}

	err_log_puts(buf, len);

	return len;
}

void
err_log_primitive(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	err_vlog_primitive(fmt, args);
	va_end(args);
}

void
err_panic_primitive(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	err_vlog_primitive(fmt, args);
	va_end(args);

	exit(1);
}

#endif // ULS_DOTNET

int
ULS_QUALIFIED_METHOD(uls_isgraph)(int c)
{
	return c>=0x21 && c<=0x7E;
}

int
ULS_QUALIFIED_METHOD(uls_isprint)(int c)
{
	return c>=0x20 && c<=0x7E;
}

int
ULS_QUALIFIED_METHOD(uls_iscntrl)(int c)
{
	return c<0x20 || c>0x7E;
}

int
ULS_QUALIFIED_METHOD(uls_isspace)(int c)
{
	return c==' ' || (c>=0x9 && c<=0xD);
}

int
ULS_QUALIFIED_METHOD(uls_isdigit)(int c)
{
	return _IS_CH_DIGIT(c);
}

int
ULS_QUALIFIED_METHOD(uls_islower)(int c)
{
	return _IS_CH_LOWER(c);
}

int
ULS_QUALIFIED_METHOD(uls_isupper)(int c)
{
	return _IS_CH_UPPER(c);
}

int
ULS_QUALIFIED_METHOD(uls_isalpha)(int c)
{
	return _IS_CH_LOWER(c) || _IS_CH_UPPER(c);
}

int
ULS_QUALIFIED_METHOD(uls_isalnum)(int c)
{
	return _IS_CH_LOWER(c) || _IS_CH_UPPER(c) || _IS_CH_DIGIT(c);
}

int
ULS_QUALIFIED_METHOD(uls_isxdigit)(int c)
{
	return _IS_CH_DIGIT(c) || (c>='a' && c<='f') || (c>='A' && c<='F');
}

char
ULS_QUALIFIED_METHOD(uls_toupper)(int c)
{
	return _IS_CH_LOWER(c) ? 'A'+(c-'a') : c;
}

char
ULS_QUALIFIED_METHOD(uls_tolower)(int c)
{
	return _IS_CH_UPPER(c) ? 'a'+(c-'A') : c;
}

int
ULS_QUALIFIED_METHOD(is_octal_char)(char ch)
{
	if (ch >= '0' && ch < '8') return 1;
	return 0; // including '\0'
}

int
ULS_QUALIFIED_METHOD(is_hexa_char)(char ch)
{
	int stat;

	if (uls_isdigit(ch)) stat = 1;
	else if (ch >= 'A' && ch <= 'F') stat = 2;
	else if (ch >= 'a' && ch <= 'f') stat = 3;
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(is_num_radix)(uls_wch_t ch, int radix)
{
	int num;

	if (uls_isdigit(ch)) {
		num = ch - '0';
	} else if (ch >= 'A' && ch <= 'Z') {
		num = 10 + (ch - 'A');
	} else if (ch >= 'a' && ch <= 'z') {
		num = 10 + (ch - 'a');
	} else {
		return -1;
	}

	if (num >= radix) num = -1;
	return num;
}

char
ULS_QUALIFIED_METHOD(num2char_radix)(int val)
{
	char ch;

	if (val < 10) {
		ch ='0' + val;
	} else if (val < 36) {
		ch = val - 10 + 'A';
	} else {
		ch = '^';
	}

	return ch;
}

char
ULS_QUALIFIED_METHOD(read_hexa_char)(char* ptr)
{
	unsigned n, ch_val = 0;
	char ch;

	if ((ch=*ptr) == '0') {
		if ((ch=*++ptr)=='x') ++ptr;
	}

	for ( ; (ch=*ptr)!='\0'; ptr++) {
		if (uls_isalpha(ch)) {
			if ((ch=uls_tolower(ch)) > 'f') break;
			n = 10 + ch - 'a';
		} else if (uls_isdigit(ch)) {
			n = ch - '0';
		} else {
			break;
		}

		ch_val = 16 *ch_val + n;
	}

	return (char) ch_val;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_standard_number_prefix)(int radix)
{
	const char *prefix;

	switch (radix) {
	case 2:
		prefix = "0b";
		break;
	case 3:
		prefix = "0t";
		break;
	case 4:
		prefix = "0q";
		break;
	case 8:
		prefix = "0o";
		break;
	case 16:
		prefix = "0x";
		break;
	default:
		prefix = NULL;
		break;
	}

	return prefix;
}

int
ULS_QUALIFIED_METHOD(uls_find_standard_prefix_radix)(const char *line, int *ptr_radix)
{
	int ch, len_prefix = 0, radix;

	if ((ch = line[0]) == '0') {
		ch = line[1];
		if (ch == 'x') { // hexa-decimal: 0x
			len_prefix = 2;
			radix = 16;
		} else if (ch == 'o') { // octal, 0o
			len_prefix = 2;
			radix = 8;
		} else if (ch == 'b') { // binary, 0b
			len_prefix = 2;
			radix = 2;
		} else if (ch == 't') { // 0t
			len_prefix = 2;
			radix = 3;
		} else if (ch == 'q') { // 0q
			len_prefix = 2;
			radix = 4;
		} else {
			len_prefix = 0;
			radix = 10;
		}
	} else {
		len_prefix = 0;
		radix = 10;
	}

	*ptr_radix = radix;
	return len_prefix;
}

int
ULS_QUALIFIED_METHOD(is_pure_integer)(const char* lptr, uls_outparam_ptr_t parms)
{
	int minus=0, pure=1, num = 0, n_bytes;
	const char* lptr0;
	char ch;

	if (*lptr == '-') {
		minus = 1;
		++lptr;
	}

	for (lptr0 = lptr; (ch=*lptr) !='\0'; lptr++) {
		if (!uls_isdigit(ch)) {
			pure = 0;
			break;
		}
		num = num * 10 + (ch - '0');
	}

	if ((n_bytes = (int) (lptr - lptr0)) > 0) {
		if (minus) {
			num = -num;
			++n_bytes;
		}
		if (parms != nilptr) {
			parms->len = n_bytes;
			parms->n = num;
		}
		if (!pure) n_bytes = -n_bytes;
	}

	return n_bytes;
}

int
ULS_QUALIFIED_METHOD(is_pure_word)(const char* lptr, int must_id)
{
	int len, stat=0;
	const char* lptr0 = lptr;
	char ch;

	for ( ; (ch=*lptr) !='\0'; lptr++) {
		if (!(uls_isalnum(ch) || ch == '_')) {
			stat = -1;
		}
	}

	if ((len = (int) (lptr - lptr0)) == 0 || stat < 0) {
		return -len;
	}

	if (len > 0 && must_id) {
		if (uls_isdigit(*lptr0)) {
			len = -len;
		}
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(uls_atoi)(const char *str)
{
	int i, minus, n = 0;
	char ch;

	if (str[0] == '-') {
		minus = 1;
		i = 1;
	} else {
		minus = 0;
		i = 0;
	}

	for ( ; ; i++) {
		if ((ch=str[i]) > '9' || ch < '0')
			break;
		n = ch - '0' + n * 10;
	}

	if (minus) n = -n;
	return n;
}

void
ULS_QUALIFIED_METHOD(uls_get_xrange)(const char* wrd, uls_uint32* ptr_x1, uls_uint32* ptr_x2)
{
	uls_uint32 val1, val2;
	uls_outparam_t parms;

	parms.lptr = wrd;
	val1 = uls_skip_atox(uls_ptr(parms));
	wrd = parms.lptr;

	if (*wrd == '-') {
		parms.lptr = ++wrd;
		val2 = uls_skip_atox(uls_ptr(parms));
		wrd = parms.lptr;
	} else {
		val2 = val1;
	}

	*ptr_x1 = val1;
	*ptr_x2 = val2;
}

int
ULS_QUALIFIED_METHOD(get_range_aton)(uls_outparam_ptr_t parms)
{
	unsigned int i1, i2;
	uls_outparam_t parms1;
	const char  *lptr;

	if (*(lptr = parms->lptr) == '\0')
		return 0;

	if (lptr[1] == '-') {
		i1 = lptr[0];
		i2 = lptr[2];
		lptr += 3;

	} else if (lptr[0] == '0' && lptr[1] == 'x') {
		parms1.lptr = lptr + 2;
		i1 = uls_skip_atox(uls_ptr(parms1));
		lptr = parms1.lptr;

		if (*lptr == '-') {
			parms1.lptr = ++lptr;
			i2 = uls_skip_atox(uls_ptr(parms1));
			lptr = parms1.lptr;
		} else {
			i2 = i1;
		}

	} else {
		i1 = i2 = lptr[0];
		++lptr;
	}

	if (i2 < i1) {
		return 0;
	}

	parms->x1 = i1;
	parms->x2 = i2;
	parms->lptr = lptr;

	return 1;
}

int
ULS_QUALIFIED_METHOD(uls_index_range)(uls_outparam_ptr_t parms, int i2_limit)
{
	int i1 = parms->n1;
	int i2p1 = parms->n2;

	if (i1 < 0) i1 = 0;

	if (i2p1 < 0) {
		i2p1 = i2_limit;
	} else if (i2p1 > i2_limit) {
		i2p1 = i2_limit;
	}

	if (i1 >= i2p1) {
		i2p1 = i1;
	}

	// i1 <= i2p1
	parms->n1 = i1;
	parms->n2 = i2p1;

	return i2p1 - i1; // may be zero
}

int
ULS_QUALIFIED_METHOD(uls_range_of_bits)(uls_uint32 n)
{
	int  i, i0 = -1;

	for (i=0; i<32; i++) {
		if (n & 0x01) i0 = i;
		n >>= 1;
	}

	return i0 + 1;
}

void
ULS_QUALIFIED_METHOD(uls_clear_bits)(char* srcptr, uls_uint32 start_bit, uls_uint32 end_bit)
{
	uls_uint32  i1, j1, i2, j2, j;
	char  *dstptr;

	//
	//  This clear all the bits in [start_bit,end_bit] from 'dstptr'.
	//
	//     A0         A0+1        A0+2        A0+3        A0+4
	//     -------------------------------------------------
	//     |           |       #####|**********|###        |
	//     ------------|-------|----|----------|---|-------|-
	//                         P1                 P2
	//                         i1                 i2
	//                         j1                 j2
	//
	//   P1(i1,j1) == P(start_bit/8,start_bit%8)
	//   P2(i2,j2)
	//

	if (start_bit > end_bit) return;

	i1 = start_bit / 8;
	j1 = start_bit % 8;
	i2 = end_bit / 8;
	j2 = end_bit % 8;

	if (j1 > 0) {
		dstptr = srcptr + i1;

		if (i1 == i2) {
			for (j=j1; j<=j2; j++) {
				uls_clear_bit(j, dstptr);
			}
			return;
		}

		for (j=j1; j<8; j++) {
			uls_clear_bit(j, dstptr);
		}

		++i1;
		j1 = 0;
	}

	if (j2 == 7) {
		uls_bzero(srcptr + i1, i2 - i1 + 1);
	} else {
		uls_bzero(srcptr + i1, i2 - i1);
		dstptr = srcptr + i2;
		for (j=0; j<=j2; j++) {
			uls_clear_bit(j, dstptr);
		}
	}
}

int
ULS_QUALIFIED_METHOD(uls_find_first_1bit)(char* srcptr,
	uls_uint32 start_bit, uls_uint32 end_bit, uls_uint32* found_bit)
{
	char  *dstptr;
	uls_uint32 i1, i2, i;
	int  j1, j2, j;

	//
	//  This find the first 1-bit in the range of [start_bit,end_bit] from 'srcptr'.
	//
	//     A0         A0+1        A0+2        A0+3        A0+4
	//     -------------------------------------------------
	//     |           |       #####|**********|###        |
	//     ------------|-------|----|----------|---|-------|-
	//                         P1                 P2
	//                         i1                 i2
	//                         j1                 j2
	//
	//   P1(i1,j1) == P(start_bit/8,start_bit%8)
	//   P2(i2,j2)
	//

	if (start_bit > end_bit) return 0; // NOT FOUND

	i1 = start_bit / 8;
	j1 = start_bit % 8;
	i2 = end_bit / 8;
	j2 = end_bit % 8;

	if (j1 > 0) {
		dstptr = srcptr + i1;

		if (i1 == i2) {
			if ((j=__find_first_1bit(*dstptr, j1, j2)) >= 0) {
				if (found_bit != NULL) *found_bit = i1*8 + j;
				return 1;
			}

			return 0; // NOT FOUND
		}

		if ((j=__find_first_1bit(*dstptr, j1, 7))>=0) {
			if (found_bit != NULL) *found_bit = i1*8 + j;
			return 1; // FOUND
		}

		++i1;
		j1 = 0;
	}

	for (i=i1; i < (uls_uint32) ((j2 < 7) ? i2 : i2 + 1); i++) {
		if (srcptr[i] != 0) {
			j = __find_first_1bit(srcptr[i], 0, 7);
			if (found_bit != NULL) *found_bit = i*8 + j;
			return 1; // FOUND
		}
	}

	if (j2 < 7) {
		dstptr = srcptr + i2;
		if ((j=__find_first_1bit(*dstptr, 0, j2))>=0) {
			if (found_bit != NULL) *found_bit = i2*8 + j;
			return 1; // FOUND
		}
	}

	return 0; // NOT FOUND
}

int
ULS_QUALIFIED_METHOD(uls_host_byteorder)(void)
{
	unsigned int ui;
	char *ptr;
	int stat;

	ui = 1;
	ptr = (char *) &ui;
	stat = (ptr[0] == 1) ?  ULS_LITTLE_ENDIAN : ULS_BIG_ENDIAN;

	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_reverse_bytes)(char* ary, int n)
{
	char ch;
	int i, j;

	// swap numstr[0,.., n-1]
	for (i=0, j=n-1; i<n/2; i++, j--) {
		// swap numstr[i], numstr[n-i]
		ch = ary[i];
		ary[i] = ary[j];
		ary[j] = ch;
	}
}

int
ULS_QUALIFIED_METHOD(uls_check_longdouble_fmt)(int endian)
{
	long double temp_lf;
	char *ptr = (char *) &temp_lf;
	int f_siz=SIZEOF_LONG_DOUBLE;
	int j, ret_typ=-1, n_bits=0;

	uls_bzero(ptr, f_siz);
	temp_lf = -1.0;

	if (endian == ULS_LITTLE_ENDIAN) {
		for (j=0; j<f_siz; j++) {
			if (ptr[j] == 0xBF) {
				n_bits = (j + 1) << 3;
				break;
			}
		}
	} else if (endian == ULS_BIG_ENDIAN) {
		for (j=f_siz-1; j>=0; j--) {
			if (ptr[j] == 0xBF) {
				n_bits = (f_siz - j) << 3;
				break;
			}
		}
	}

	if (n_bits == 64) {
		ret_typ = ULS_IEEE754_BINARY64;
	} else if (n_bits == 80) {
		ret_typ = ULS_IEEE754_BINARY80;
	} else if (n_bits == 128) {
		ret_typ = ULS_IEEE754_BINARY128;
	} else {
#ifdef ULS_DOTNET
		ret_typ = ULS_IEEE754_BINARY64;
#endif
	}

	return ret_typ;
}

int
ULS_QUALIFIED_METHOD(uls_strcmp)(const char* str1, const char* str2)
{
	const char *ptr1=str1, *ptr2=str2;
	char ch1, ch2;
	int stat = 0;

	for ( ; ; ptr1++,ptr2++) {
		ch1 = *ptr1;
		ch2 = *ptr2;

		if (ch1 != ch2) {
			stat = (int) ch1 - (int) ch2;
			break;
		}

		if (ch1 == '\0') break;
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_strncmp)(const char* str1, const char* str2, int n)
{
	const char *ptr1=str1, *ptr2=str2;
	char ch1, ch2;
	int i, stat = 0;

	for (i=0; i<n; i++, ptr1++, ptr2++) {
		ch1 = *ptr1;
		ch2 = *ptr2;

		if (ch1 != ch2) {
			stat = (int) ch1 - (int) ch2;
			break;
		}

		if (ch1 == '\0') break;
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_strcasecmp)(const char* str1, const char* str2)
{
	char  ch1, ch2;
	int  i, stat = 0;

	for (i=0; ; i++) {
		ch1 = str1[i];
		ch2 = str2[i];

		if (uls_isalpha(ch1)) ch1 = uls_toupper(ch1);
		if (uls_isalpha(ch2)) ch2 = uls_toupper(ch2);

		if (ch1 != ch2) {
			stat = (int) ch1 - ch2;
			break;
		}

		if (ch1 == '\0') break;
	}

	return stat;
}

char*
ULS_QUALIFIED_METHOD(uls_strchr)(const char* lptr, char ch0)
{
	char ch;
	int i;

	if (ch0 >= 0x80) {
		return NULL;
	}

	for (i=0; (ch=lptr[i]) != '\0'; i++) {
		if (ch == ch0) return (char *) (lptr + i);
	}

	if (ch0 == '\0') return (char *) (lptr + i);

	return NULL;
}

char*
ULS_QUALIFIED_METHOD(uls_strchr_r)(const char* lptr, char ch)
{
	int i, len = uls_strlen(lptr);

	if (ch >= 0x80) {
		return NULL;
	}

	for (i=len-1; i >= 0; i--) {
		if (lptr[i] == ch) return (char *) (lptr + i);
	}

	return NULL;
}

const char*
ULS_QUALIFIED_METHOD(uls_strstr)(const char *str, const char* substr)
{
	int i, l_str, l_substr;
	char ch0;

	if ((ch0 = *substr) == '\0') {
		return str + uls_strlen(str);
	}

	l_str = uls_strlen(str);
	l_substr = uls_strlen(substr);

	for (i=0; i<=l_str-l_substr; i++) {
		if (str[i] != ch0) continue;
		if (uls_strncmp(str+i, substr, l_substr) == 0) {
			return str + i;
		}
	}

	return NULL;
}

int
ULS_QUALIFIED_METHOD(uls_str_tolower)(const char* src, char *dst, int len)
{
	int i;
	char ch;

	if (len < 0) {
		for(i=0; (ch=src[i])!='\0'; i++ )
			dst[i] = (char) uls_tolower(ch);
	} else {
		for(i=0; i<len && (ch=src[i])!='\0'; i++)
			dst[i] = (char) uls_tolower(ch);
	}

	dst[i] = '\0';
	return i;
}

int
ULS_QUALIFIED_METHOD(uls_str_toupper)(const char* src, char *dst, int len)
{
	int i;
	char ch;

	if (len < 0) {
		for(i=0; (ch=src[i])!='\0'; i++)
			dst[i] = (char) uls_toupper(ch);
	} else {
		for(i=0; i<len && (ch=src[i])!='\0'; i++)
			dst[i] = (char) uls_toupper(ch);
	}

	dst[i] = '\0';
	return i;
}

int
ULS_QUALIFIED_METHOD(uls_memcmp)(const void *src1, const void *src2, int n)
{
	int rc;

	if (n <= 0) return 0;
	rc = memcmp(src1, src2, n);
	if (rc < 0) rc = -1;
	else if (rc > 0) rc = 1;

	return rc;
}

unsigned int
ULS_QUALIFIED_METHOD(uls_skip_atou)(uls_outparam_ptr_t parms)
{
	const char   *lptr = parms->lptr;
	unsigned int n = 0;
	char ch;

	for ( ; uls_isdigit(ch=*lptr); lptr++) {
		n = n*10 + (ch - '0');
	}

	parms->lptr = lptr;
	return n;
}

unsigned int
ULS_QUALIFIED_METHOD(uls_skip_atox)(uls_outparam_ptr_t parms)
{
	const char   *lptr = parms->lptr;
	unsigned int  n = 0;
	char ch;

	for ( ; ; lptr++) {
		if ((ch=*lptr) >= '0' && ch <= '9') {
			n = (n << 4) + (ch - '0');
		} else if (uls_isalpha(ch) && (ch=uls_toupper(ch)) >= 'A' && ch <= 'F') {
			n = (n << 4) + 10 + (ch - 'A');
		} else {
			break;
		}
	}

	parms->lptr = lptr;
	return n;
}

char*
ULS_QUALIFIED_METHOD(split_clause)(uls_outparam_ptr_t parms)
{
	char *str;

	str = skip_blanks(parms->line);
	parms->line = split_litstr(str, ' ');

	return str;
}

char*
ULS_QUALIFIED_METHOD(split_litstr)(char *str, char qch)
{
	char   *ptr, *ptr1;
	char   ch, esc_ch=0;

	for (ptr1=ptr=str; ; ptr++) {
		if (!uls_isprint(ch=*ptr)) { // ctrl-ch or '\0'
			if (ch != '\0') ++ptr;
			break;
		}

		if (esc_ch) {
			*ptr1++ = ch;
			esc_ch = 0;
		} else if (ch == '\\') {
			esc_ch = 1;
		} else if (ch == qch) {
			++ptr;
			break;
		} else {
			*ptr1++ = ch;
		}
	}

	*ptr1 = '\0';
	return ptr;
}

int
ULS_QUALIFIED_METHOD(uls_fp_getline)(FILE* fp, char* buf, int buf_siz)
{
	int len;
	char ch;

	if (fgets(buf, buf_siz, fp) == NULL) {
		if (ferror(fp)) {
			return -2;
		}
		if (feof(fp)) {
			return -1;
		}
	}

	if ((len=uls_strlen(buf)) + 1 ==  buf_siz) {
		return -3;
	}

	if (len > 0 && ((ch=buf[len-1]) == '\n' || ch == '\r')) {
		buf[--len] = '\0';
	}

	if (len > 0 && ((ch=buf[len-1]) == '\n' || ch == '\r')) {
		buf[--len] = '\0';
	}

	return len;
}

void
ULS_QUALIFIED_METHOD(uls_appl_exit)(int exit_code)
{
	exit(exit_code);
}

void
ULS_QUALIFIED_METHOD(uls_init_nambuf)(uls_nambuf_ptr_t arg, int siz)
{
	// siz >= 0
	arg->buf_siz = siz + 1;
	arg->str = uls_malloc_buffer(arg->buf_siz);
	arg->str[0] = '\0';
	arg->len = 0;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_nambuf)(uls_nambuf_ptr_t arg)
{
	uls_mfree(arg->str);
	arg->buf_siz = arg->len = 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_nambuf)(uls_nambuf_ptr_t arg, const char *name, int name_len)
{
	int len, siz = arg->buf_siz - 1;

	if (name_len < 0) {
		name_len = uls_strlen(name);
	}

	if (name_len > siz) {
		name_len = siz;
	}

	arg->len = len = uls_strncpy(arg->str, name, name_len);
	return len;
}

int
ULS_QUALIFIED_METHOD(uls_set_nambuf_raw)(char *argbuf, int argbuf_siz, const char *name, int name_len)
{
	int len;

	if (name_len < 0) {
		name_len = uls_strlen(name);
	}

	if (name_len >= argbuf_siz) {
		name_len = argbuf_siz - 1;
	}

	len = uls_strncpy(argbuf, name, name_len);
	return len;
}

void
ULS_QUALIFIED_METHOD(uls_memset)(void *dstbuf, char ch, int n)
{
	char *buf, *ptr = (char *) dstbuf;
	int i;

	if (n > ULS_MEMSET_CHUNKSIZE) {
		for (i=0; i<ULS_MEMSET_CHUNKSIZE; i++) {
			*ptr++ = ch;
		}

		buf = (char *) dstbuf;
		for (i=1; i<n/ULS_MEMSET_CHUNKSIZE; i++) {
			ptr = (char *) uls_memcopy(ptr, buf, ULS_MEMSET_CHUNKSIZE);
		}

		n %= ULS_MEMSET_CHUNKSIZE;
	}

	for (i=0; i<n; i++) {
		*ptr++ = ch;
	}
}

void
ULS_QUALIFIED_METHOD(uls_bzero)(void *dstbuf, int n)
{
	uls_memset(dstbuf, 0x00, n);
}

void*
ULS_QUALIFIED_METHOD(uls_malloc)(unsigned int n_bytes)
{
	void *ptr;

	if (n_bytes == 0) {
		ptr = NULL;
	} else {
		if ((ptr = malloc(n_bytes))==NULL) {
			_uls_log_primitive(err_panic)("malloc error, need some scheduling, ...");
		}
	}

	return ptr;
}

void*
ULS_QUALIFIED_METHOD(uls_malloc_clear)(unsigned int n_bytes)
{
	void *ptr;

	if ((ptr = uls_malloc(n_bytes)) != NULL) {
		uls_bzero(ptr, n_bytes);
	}

	return ptr;
}

void*
ULS_QUALIFIED_METHOD(uls_mrealloc)(void *ptr, unsigned int n_bytes)
{
	if (n_bytes == 0) {
		if (ptr!=NULL) free(ptr);
		return NULL;
	}

	if (ptr == NULL) {
		ptr = malloc(n_bytes);
	} else {
		ptr = realloc(ptr, n_bytes);
	}

	if (ptr == NULL) {
		_uls_log_primitive(err_panic)("malloc error");
	}

	return ptr;
}

void*
ULS_QUALIFIED_METHOD(__uls_mfree)(void *ptr)
{
	if (ptr != NULL) free(ptr);
	return NULL;
}

char*
ULS_QUALIFIED_METHOD(uls_strdup)(const char* str, int len)
{
	char *ptr;

	if (len < 0)
		len = uls_strlen(str);

	ptr = uls_malloc_buffer(len + 1);
	uls_memcopy(ptr, str, len);
	ptr[len] = '\0';

	return ptr;
}

void*
ULS_QUALIFIED_METHOD(uls_memcopy)(void *dst, const void* src, int n)
{
	void *ptr;

	if (n > 0) {
		memcpy(dst, src, n);
		ptr = (char *) dst + n;
	} else {
		ptr = dst;
	}

	return ptr;
}

void*
ULS_QUALIFIED_METHOD(uls_memmove)(void *dst, const void* src, int n)
{
	void *ptr;

	if (n > 0) {
		memmove(dst, src, n);
		ptr = (char *) dst + n;
	} else {
		ptr = dst;
	}

	return ptr;
}

int
ULS_QUALIFIED_METHOD(uls_strlen)(const char* str)
{
	const char* ptr;

	for (ptr=str; *ptr != '\0'; ptr++)
		/* NOTHING */;

	return (int) (ptr - str);
}

int
ULS_QUALIFIED_METHOD(uls_strcpy)(char* bufptr, const char* str)
{
	const char *ptr;
	char ch;

	for (ptr=str; (ch=*ptr) != '\0'; ptr++) {
		*bufptr++ = ch;
	}

	*bufptr = '\0';
	return (int) (ptr - str);
}

int
ULS_QUALIFIED_METHOD(uls_strncpy)(char* bufptr, const char* ptr, int n)
{
	char ch;
	int i;

	if (n < 0) n = uls_strlen(ptr);

	for (i=0; i<n; i++, ptr++) {
		if ((ch=*ptr) == '\0') break;
		*bufptr++ = ch;
	}

	*bufptr = '\0';
	return i;
}

char*
ULS_QUALIFIED_METHOD(skip_blanks)(const char* lptr)
{
	register int ch;

	for ( ; (ch=*lptr)==' ' || ch=='\t'; lptr++)
		/* nothing */;
	return (char *) lptr;
}

int
ULS_QUALIFIED_METHOD(str_trim_end)(char* str, int len)
{
	register int   i;

	if (len < 0) len = uls_strlen(str);

	for (i=len-1; i>=0; i--) {
		if (!uls_isspace(str[i])) break;
	}
	str[++i] = '\0';
	return i;
}

char*
ULS_QUALIFIED_METHOD(_uls_splitstr)(uls_wrd_ptr_t uw)
{
	char      *str = uw->lptr;
	char      *ptr, *ptr0;
	register int   ch;
	int len;

	ptr0 = ptr = skip_blanks(str);

	for (len=0; (ch=*ptr) != '\0'; ptr++) {
		if (ch==' ' || ch=='\t') {
			len = (int) (ptr - ptr0);
			*ptr++ = '\0';
			break;
		}
	}

	uw->lptr = ptr;
	uw->wrd = ptr0;
	uw->len = len;

	return ptr0;
}

int
ULS_QUALIFIED_METHOD(_uls_explode_str)(uls_wrd_ptr_t uw, char delim_ch, int dupstr, uls_arglst_ptr_t arglst)
{
	char  *lptr=uw->lptr, *lptr1, ch;
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	uls_argstr_ptr_t arg;
	int   i, k, len;

	for (k = 0; k < arglst->args.n_alloc; ) {
		if (delim_ch == ' ') {
			for ( ; (ch=*lptr) == ' ' || ch == '\t'; lptr++)
				/* NOTHING */;
		}

		for (lptr1=lptr; ; lptr++) {
			if ((ch=*lptr) == '\0') {
				len = (int) (lptr - lptr1);
				if (lptr != lptr1) {
					if ((arg=al[k]) == nilptr) {
						al[k] = arg =  uls_create_argstr();
					}
					uls_set_argstr(arg, lptr1, len);
					++k;
				}
				goto end_1;
			}

			if (ch == delim_ch || (delim_ch == ' ' && ch == '\t')) {
				len = (int) (lptr - lptr1);
				*lptr++ = '\0';

				if ((arg=al[k]) == nilptr) {
					al[k] = arg =  uls_create_argstr();
				}
				uls_set_argstr(arg, lptr1, len);
				++k;
				break;
			}
		}
	}

 end_1:
 	if (dupstr) {
 		for (i=0; i<k; i++) {
 			arg = al[i];
			uls_copy_argstr(arg, arg->str, arg->len);
 		}
 	}

	uw->lptr = lptr;
	arglst->args.n = k;
	return k;
}

char*
ULS_QUALIFIED_METHOD(_uls_filename)(uls_outparam_ptr_t parms)
{
	const char *filepath = parms->lptr;
	const char *ptr, *ptr0;
	const char *filename;
	int  len_filename, len, i;
	char  ch;

	if (filepath == NULL) return NULL;
	for (ptr0=NULL,ptr=filepath; (ch=*ptr)!='\0'; ptr++) {
		if (ch == ULS_FILEPATH_DELIM) ptr0 = ptr;
	}

	if (ptr0 != NULL) {
		filename = ptr0 + 1;
	} else {
		filename = filepath;
	}

	len_filename = len = uls_strlen(filename);
	for (i=len_filename-1; i >= 0; i--) {
		if (filename[i] == '.') {
			len = i;
			break;
		}
	}

	parms->line = (char *) filename;
	parms->len = len;

	return (char *) filename;
}

void
ULS_QUALIFIED_METHOD(uls_init_argstr)(uls_argstr_ptr_t arg, int siz)
{
	if (siz >= 0) {
		arg->buf_siz = siz + 1;
		arg->buf = uls_malloc_buffer(arg->buf_siz);
		arg->buf[0] = '\0';
		arg->str = arg->buf;
		arg->len = 0;

	} else {
		arg->str = arg->buf = NULL;
		arg->buf_siz = 0;
		arg->len = -1;
	}
}

void
ULS_QUALIFIED_METHOD(uls_deinit_argstr)(uls_argstr_ptr_t arg)
{
	uls_mfree(arg->buf);
	arg->buf_siz = 0;

	arg->str = arg->buf = NULL;
	arg->len = -1;
}

ULS_QUALIFIED_RETTYP(uls_argstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_argstr)(void)
{
	uls_argstr_ptr_t arg;

	arg = uls_alloc_object(uls_argstr_t);
	uls_init_argstr(arg, -1);

	return arg;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_argstr)(uls_argstr_ptr_t arg)
{
	uls_deinit_argstr(arg);
	uls_dealloc_object(arg);
}

void
ULS_QUALIFIED_METHOD(uls_set_argstr)(uls_argstr_ptr_t arg, char *name, int name_len)
{
	arg->str = name;

	if (name != NULL) {
		if (name_len < 0) {
			arg->len = uls_strlen(name);
		} else {
			arg->len = name_len;
		}
	} else {
		arg->len = -1;
	}
}

char*
ULS_QUALIFIED_METHOD(uls_copy_argstr)(uls_argstr_ptr_t arg, const char *name, int name_len)
{
	char *str;

	if (name != NULL) {
		if (name_len < 0) name_len = uls_strlen(name);

		if (name_len >= arg->buf_siz) {
			arg->buf_siz = name_len + 1;
			arg->buf = (char *) uls_mrealloc(arg->buf, arg->buf_siz);
		}

		uls_strncpy(arg->buf, name, name_len);
		arg->str = str = arg->buf;
		arg->len = name_len;

	} else {
		uls_deinit_argstr(arg);
		str = NULL;
	}

	return str;
}

void
ULS_QUALIFIED_METHOD(uls_init_arglst)(uls_arglst_ptr_t arglst, int siz)
{
	uls_init_parray(uls_ptr(arglst->args), argstr, siz);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_arglst)(uls_arglst_ptr_t arglst)
{
	uls_reset_arglst(arglst);
	uls_deinit_parray(uls_ptr(arglst->args));
}

void
ULS_QUALIFIED_METHOD(uls_reset_arglst)(uls_arglst_ptr_t arglst)
{
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	uls_argstr_ptr_t arg;
	int i;

	for (i=0; i<arglst->args.n; i++) {
		if ((arg = al[i]) != nilptr) {
			uls_destroy_argstr(arg);
			al[i] = nilptr;
		}
	}

	arglst->args.n = 0;
}

void
ULS_QUALIFIED_METHOD(uls_resize_arglst)(uls_arglst_ptr_t arglst, int n1_alloc)
{
	uls_decl_parray_slots(al, argstr);
	uls_argstr_ptr_t arg;
	int i;

	al = uls_parray_slots(uls_ptr(arglst->args));
	for (i=n1_alloc; i<arglst->args.n; i++) {
		if ((arg = al[i]) != nilptr) {
			uls_destroy_argstr(arg);
			al[i] = nilptr;
		}
	}

	uls_resize_parray(uls_ptr(arglst->args), argstr, n1_alloc);

	if (arglst->args.n > arglst->args.n_alloc) {
		arglst->args.n = arglst->args.n_alloc;
	}
}

int
ULS_QUALIFIED_METHOD(uls_append_arglst)(uls_arglst_ptr_t arglst, uls_argstr_ptr_t arg)
{
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	int k;

	if ((k=arglst->args.n) >= arglst->args.n_alloc) {
		uls_resize_arglst(arglst, k + 16);
		al = uls_parray_slots(uls_ptr(arglst->args));
	}

	al[k] = arg;
	++arglst->args.n;

	return 0;
}

#ifndef ULS_DOTNET
void*
ULS_QUALIFIED_METHOD(uls_zalloc)(unsigned int n_bytes)
{
	void *ptr;

	if ((ptr = uls_malloc(n_bytes)) != NULL) {
		uls_bzero(ptr, n_bytes);
	}

	return ptr;
}

char*
ULS_QUALIFIED_METHOD(uls_splitstr)(char** p_str)
{
	uls_wrd_t wrdx;
	char *wrd;

	wrdx.lptr = *p_str;
	wrd = _uls_splitstr(uls_ptr(wrdx));
	*p_str = wrdx.lptr;

	return wrd;
}

int
ULS_QUALIFIED_METHOD(uls_explode_str)(char **ptr_line, char delim_ch, char** args, int n_args)
{
	uls_wrd_t wrdx;
	uls_arglst_t wrdlst;
	uls_argstr_ptr_t argstr;
	int n_wrd, i;
	uls_decl_parray_slots(al_wrds, argstr);

	wrdx.lptr = *ptr_line;

	uls_init_arglst(uls_ptr(wrdlst), n_args);

	n_wrd = _uls_explode_str(uls_ptr(wrdx), delim_ch, 0, uls_ptr(wrdlst));
	al_wrds = uls_parray_slots(uls_ptr(wrdlst.args));
	for (i=0; i<n_wrd; i++) {
		argstr = al_wrds[i];
		args[i] = argstr->str;
	}

	*ptr_line = wrdx.lptr;

	uls_deinit_arglst(uls_ptr(wrdlst));

	return n_wrd;
}

char*
ULS_QUALIFIED_METHOD(uls_filename)(const char *filepath, int* len_fname)
{
	uls_outparam_t parms;
	char *fname;

	parms.lptr = filepath;
	fname = _uls_filename(uls_ptr(parms));

	if (len_fname != NULL) {
		*len_fname = parms.len;
	}

	return fname;
}

#endif // ULS_DOTNET

int
ULS_QUALIFIED_METHOD(uls_encode_utf8)(uls_wch_t wch, char* utf8buf, int siz_utf8buf)
{
	char tmpbuf[ULS_UTF8_CH_MAXLEN];
	int i, rc;

	if (wch <= 0x7F) {
		tmpbuf[0] = wch;
		rc = 1;

	} else if (wch <= 0x07FF) {
		tmpbuf[0] = 0xC0 | (wch >> 6);
		tmpbuf[1] = 0x80 | (0x3F & wch);
		rc = 2;

	} else if (wch <= 0xFFFF) {
		tmpbuf[0] = 0xE0 | (wch >> 12);
		tmpbuf[1] = 0x80 | (0x3F & (wch >> 6));
		tmpbuf[2] = 0x80 | (0x3F & wch);
		rc = 3;

	} else if (wch <= 0x1FFFFF) {
		tmpbuf[0] = 0xF0 | (wch >> 18);
		tmpbuf[1] = 0x80 | (0x3F & (wch >> 12));
		tmpbuf[2] = 0x80 | (0x3F & (wch >> 6));
		tmpbuf[3] = 0x80 | (0x3F & wch);
		rc = 4;

	} else {
		return -(ULS_UTF8_CH_MAXLEN + 1);
	}

	if (siz_utf8buf >= 0 && siz_utf8buf < rc) {
		if (utf8buf != NULL) rc = -rc;
	} else if (utf8buf != NULL) {
		for (i=0; i<rc; i++) utf8buf[i] = tmpbuf[i];
	}

	return rc;
}

int
ULS_QUALIFIED_METHOD(uls_decode_utf8)(const char *utf8buf, int len_utf8buf, uls_wch_t *p_wch)
{
	const char *bufptr = utf8buf;
	char ch, ch_mask, ch_ary[3] = { 0x20, 0x10, 0x08 };
	uls_wch_t val;
	int  n, i;

	if (len_utf8buf < 0) {
		for (i=0; i<ULS_UTF8_CH_MAXLEN; i++) {
			if (utf8buf[i] == '\0') break;
		}
		if ((len_utf8buf = i) == 0) {
			len_utf8buf = 1; // including '\0'
		}
	} else if (len_utf8buf == 0) {
		return -(ULS_UTF8_CH_MAXLEN + 1);
	}

	ch = *bufptr;
	if ((ch & 0xC0) != 0xC0) {
		if (p_wch != NULL) *p_wch = ch;
		return 1;
	}

	for (n=0; ; n++) {
		if (n >= 3) return -(ULS_UTF8_CH_MAXLEN + 2);
		if ((ch_ary[n] & ch) == 0) {
			++n;
			break;
		}
	}
	// n-bytes followed additionally
	i = 1 + n;
	if (p_wch == NULL) return i;
	if (i > len_utf8buf) return -i;

	ch_mask = (1 << (6-n)) - 1;
	val = ch & ch_mask;

	ch_mask = (1 << 6) - 1;
	for (i=0; i<n; i++) {
		ch = *++bufptr;
		if ((ch & 0xC0) != 0x80) {
			return -(ULS_UTF8_CH_MAXLEN + 3);
		}
		val = (val << 6) | (ch & ch_mask);
	}

	*p_wch = val;
	return 1 + n;
}

int
ULS_QUALIFIED_METHOD(uls_encode_utf16)(uls_wch_t wch, uls_uint16 *utf16buf, int siz_utf16buf)
{
	uls_wch_t wch2;
	uls_uint16 tmpbuf[ULS_UTF16_CH_MAXLEN];
	int i, rc;

	if (wch <= 0xFFFF) {
		tmpbuf[0] = (uls_uint16) wch;
		rc = 1;
	} else if (wch <= ULS_UTF16_CODEPOINT_END) {
		wch2 = wch - 0x10000;
		tmpbuf[0] = 0xD800 + (wch2 >> 10);
		tmpbuf[1] = 0xDC00 + (wch2 & 0x3FF);
		rc = 2;
	} else {
		return -(ULS_UTF16_CH_MAXLEN + 1);
	}

	if (siz_utf16buf >= 0 && siz_utf16buf < rc) {
		if (utf16buf != NULL) rc = -rc;
	} else if (utf16buf != NULL) {
		for (i=0; i<rc; i++) utf16buf[i] = tmpbuf[i];
	}

	return rc;
}

int
ULS_QUALIFIED_METHOD(uls_decode_utf16)(const uls_uint16 *utf16buf, int len_utf16buf, uls_wch_t *p_wch)
{
	uls_wch_t wch;
	int i, rc;

	if (len_utf16buf < 0) {
		for (i=0; i<ULS_UTF16_CH_MAXLEN; i++) {
			if (utf16buf[i] == 0) break;
		}
		if ((len_utf16buf = i) == 0) {
			len_utf16buf = 1; // including '\0'
		}
	} else if (len_utf16buf == 0) {
		return -(ULS_UTF16_CH_MAXLEN + 1);
	}

	if (utf16buf[0] >= ULS_UTF16_CODEPOINT_RSVD_START && utf16buf[0] <= ULS_UTF16_CODEPOINT_RSVD_END) {
		rc = 2;
		if (len_utf16buf >= 2) {
			if (utf16buf[1] < 0xDC00 || utf16buf[1] > 0xDFFF)
				return -(ULS_UTF16_CH_MAXLEN + 2);
			wch  = (utf16buf[0] - 0xD800) << 10;
			wch |= (utf16buf[1] - 0xDC00);
			wch += 0x10000;
		} else {
			wch = 0;
			if (p_wch != NULL) rc = -rc;
		}
	} else {
		wch = utf16buf[0];
		rc = 1;
	}

	if (p_wch != NULL) *p_wch = wch;
	return rc;
}

int
ULS_QUALIFIED_METHOD(uls_encode_utf32)(uls_wch_t wch, uls_uint32 *buf)
{
	if (wch > ULS_UTF32_CODEPOINT_END) {
		return -1;
	}

	buf[0] = wch;

	return 1;
}

int
ULS_QUALIFIED_METHOD(uls_decode_utf32)(uls_uint32 buf, uls_wch_t *p_wch)
{
	uls_wch_t wch;

	wch = (uls_wch_t) buf;
	if (wch > ULS_UTF32_CODEPOINT_END) {
		return -1;
	}

	if (p_wch != NULL) *p_wch = wch;
	return 1;
}

int
ULS_QUALIFIED_METHOD(ustr_num_wchars)(const char *ustr, int len, uls_outparam_ptr_t parms)
{
	const char *cptr = ustr, *cptr_end;
	int n, rc, wlen = 0, stat = 0;

	if (len < 0) {
		for ( ; *cptr != '\0'; cptr += rc) {
			if ((rc = uls_decode_utf8(cptr, -1, NULL)) <= 0) {
				if (rc < -ULS_UTF8_CH_MAXLEN) stat = -1;
				break;
			}
			++wlen;
		}
	} else {
		cptr_end = ustr + len;
		for ( ; cptr < cptr_end; cptr += rc) {
			n = (int) (cptr_end - cptr);
			if ((rc = uls_decode_utf8(cptr, n, NULL)) <= 0) {
				if (rc < -ULS_UTF8_CH_MAXLEN) stat = -1;
				break;
			} else if (rc > n) {
				break;
			}
			++wlen;
		}
	}

	len = (int) (cptr - ustr);
	if (parms != nilptr) {
		parms->n = wlen;
		parms->len = len;
	}

	if (stat < 0) wlen = -1;
	return wlen; // # of wchars
}

#if defined(HAVE_PTHREAD)
void
ULS_QUALIFIED_METHOD(uls_init_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(uls_ptr(attr));

	if (pthread_mutexattr_settype(uls_ptr(attr), PTHREAD_MUTEX_RECURSIVE) != 0) {
		_uls_log_static(err_panic)("error: fail to make the mutex recursive-one");
	}

	a_mtx->mtx_pthr = uls_alloc_object(pthread_mutex_t);
	if (pthread_mutex_init(a_mtx->mtx_pthr, uls_ptr(attr)) != 0) {
		_uls_log_static(err_panic)("error: mutex init");
	}

	pthread_mutexattr_destroy(uls_ptr(attr));
}

void
ULS_QUALIFIED_METHOD(uls_deinit_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;

	if (pthread_mutex_destroy(a_mtx->mtx_pthr) != 0) {
		_uls_log_static(err_panic)("error: mutex destroy");
	}
	uls_dealloc_object(a_mtx->mtx_pthr);
}

void
ULS_QUALIFIED_METHOD(uls_lock_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;

	if (pthread_mutex_lock(a_mtx->mtx_pthr) != 0) {
		_uls_log_static(err_panic)("error to pthread_mutex_lock");
	}
}

void
ULS_QUALIFIED_METHOD(uls_unlock_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;

	if (pthread_mutex_unlock(a_mtx->mtx_pthr) != 0) {
		_uls_log_static(err_panic)("error to pthread_mutex_unlock");
	}
}

#elif defined(ULS_WINDOWS)
void
ULS_QUALIFIED_METHOD(uls_init_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;

	a_mtx->hndl = CreateMutex(NULL, FALSE, NULL);
	if (a_mtx->hndl == INVALID_HANDLE_VALUE) {
		_uls_log_static(err_panic)("error: mutex init");
	}
}

void
ULS_QUALIFIED_METHOD(uls_deinit_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;

	CloseHandle(a_mtx->hndl);
	a_mtx->hndl = INVALID_HANDLE_VALUE;
}

void
ULS_QUALIFIED_METHOD(uls_lock_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(a_mtx->hndl, INFINITE);

	if (dwWaitResult != WAIT_OBJECT_0) {
		_uls_log_static(err_panic)("error: mutex lock");
	}
}

void
ULS_QUALIFIED_METHOD(uls_unlock_mutex)(uls_mutex_t mtx)
{
	uls_mutex_struct_ptr_t  a_mtx = (uls_mutex_struct_ptr_t) mtx;
	ReleaseMutex(a_mtx->hndl);
}
#else
void
ULS_QUALIFIED_METHOD(uls_init_mutex)(uls_mutex_t mtx)
{
}

void
ULS_QUALIFIED_METHOD(uls_deinit_mutex)(uls_mutex_t mtx)
{
}

void
ULS_QUALIFIED_METHOD(uls_lock_mutex)(uls_mutex_t mtx)
{
}

void
ULS_QUALIFIED_METHOD(uls_unlock_mutex)(uls_mutex_t mtx)
{
}
#endif

void
ULS_QUALIFIED_METHOD(uls_sys_lock)(void)
{
	uls_lock_mutex(uls_ptr(uls_global_mtx));
}

void
ULS_QUALIFIED_METHOD(uls_sys_unlock)(void)
{
	uls_unlock_mutex(uls_ptr(uls_global_mtx));
}

int
ULS_QUALIFIED_METHOD(initialize_primitives)(void)
{
	uls_init_mutex(uls_ptr(uls_global_mtx));
	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_primitives)(void)
{
	uls_deinit_mutex(uls_ptr(uls_global_mtx));
}

void
ULS_QUALIFIED_METHOD(uls_msleep)(int msecs)
{
#ifdef ULS_WINDOWS
	Sleep(msecs);
#else
	usleep(msecs * 1000);
#endif
}
