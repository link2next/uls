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
  <file> ieee754.c </file>
  <brief>
    An implementation modf() for 'long double'.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, May 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_IEEE754__
#include "uls/ieee754.h"
#include "uls/uls_prim.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ieee754_biased_expo)(char* buff, int n_bits_expo)
{
	char  mask, ch, ch_carry;
	int expo, i, j2, n_bits, m;

	ch_carry = buff[0] & 0x7F;

	for (expo=0, i=0; i<n_bits_expo/8; ) {
		++i;
		ch = ch_carry << 1;
		if (buff[i] & 0x80) {
			ch |= 0x01;
		}

		expo = (expo << 8) | ch;
		ch_carry = buff[i] & 0x7F;
	}

	if ((n_bits = n_bits_expo % 8) > 0) {
		j2 = n_bits;
		m = 7 - j2;

		mask = ((1 << n_bits) - 1) << m;
		ch = (buff[i] & mask) >> m;
		expo = (expo << n_bits) | ch;

	} else {
		expo = (expo << 7) | ch_carry;
	}

	return expo;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(put_ieee754_biased_expo)(int m_expo, char* buff, int n_bits_expo)
{
	char  mask, ch;
	int i, n_bits, m;

	i = n_bits_expo / 8;

	if ((n_bits = (1+n_bits_expo) % 8) > 0) {
		m = 8 - n_bits;
		mask = (1 << n_bits) - 1;

		ch = (m_expo & mask) << m;
		mask <<= m;

		buff[i] = (buff[i] & ~mask) | ch;
		--i;
		m_expo >>= n_bits;
	}

	for ( ; i>0; i--, m_expo>>=8) {
		buff[i] = m_expo & 0xFF;
	}

	mask = 0x7F;
	buff[0] = (buff[0] & ~mask) | (m_expo & mask);
}

void
ULS_QUALIFIED_METHOD(uls_ieee754_bankup_bits)(char* srcptr, int start_bit, int end_bit, int n_shift)
{
	int  i1, i2, i1_dash, i2_dash;
	int  j1, j2, j2_dash;
	int  l1, l2, n_bits, n_bytes, m, k;

	char ch_src, ch_dst, ch_carry, mask, mask_carry;
	char *dstptr, *srcptr0=srcptr;

	//
	//  This shifts left by 'n_shift' the bits of range [start_bit,end_bit] from 'srcptr'.
	//  (end_bit + 1) % 8 == 0
	//
	//     A0         A0+1        A0+2        A0+3        A0+4
	//     ------------------------------------------------------------------
	//     |        ###|$$=========|           |      ###$$|**********|
	//     --------|---|--|--------------------|-----|-----|-----------------
	//             P2  P2"P2'                        P1    P1'
	//             i2     i2'                        i1    i1'
	//             j2     j2'                        j1    j1'
	//
	//   P1(i1,j1) == P(start_bit/8,start_bit%8) :: P1'(i1',j1')
	//   P2(i2,j2) :: P2'(i2',j2')
	//
	//   Let length(P1-P1') == l1 (bits) and
	//     length(P2"-P2) == l2 (bits)
	//

	if ((end_bit + 1) % 8 != 0) {
		_uls_log(err_panic)("%s: invalid paramerter 'end_bit'", __FUNCTION__);
	}

	if (n_shift <= 0 || start_bit > end_bit) return;
	if (start_bit < n_shift) n_shift = start_bit;

	i1 = start_bit / 8;
	j1 = start_bit % 8;
	m = start_bit - n_shift;
	i2 = m / 8;
	j2 = m % 8;

	n_bits = end_bit - start_bit + 1;
	n_bytes = n_bits / 8;

	if (j1 > 0) {
		// move (8-j1)bits from (i1,j1) to (i2,i2)
		l1 = 8 - j1;
		ch_src = ((1 << l1) - 1) & srcptr[i1];
		ch_dst = srcptr[i2];

		if (j2 > 0) {
			if ((l2=8-j2) > l1) {
				i2_dash = i2;
				j2_dash = j2 + l1;

				mask = ((1 << l1) - 1) << (l2-l1);
				ch_src <<= l2 - l1;
				srcptr[i2]  = (ch_dst & ~mask) | ch_src;

			} else { // l2 <= l1
				i2_dash = i2 + 1;
				j2_dash = l1 - l2;

				mask = (1 << l2) - 1;
				ch_dst &= ~mask;

				if (l2 < l1) {
					m = l1 - l2;
					ch_dst |=  ch_src >> m;
					srcptr[i2] = ch_dst;

					ch_src <<= 8 - m;
					mask = ((1 << m) - 1) << (8-m);
					srcptr[i2+1] = (srcptr[i2+1] & ~mask) | ch_src;

				} else {
					srcptr[i2] = ch_dst | ch_src;
				}
			}

		} else  {
			i2_dash = i2;
			j2_dash = l1;

			mask = ((1 << l1) - 1) << (8-l1);
			srcptr[i2] = (ch_dst & ~mask) | (ch_src << j1);
		}

		i1_dash = i1 + 1;

	} else {
		i2_dash = i2;
		j2_dash = j2;
		i1_dash = i1;
	}

	if (n_bytes > 0) {
		dstptr = srcptr + i2_dash;
		srcptr += i1_dash;

		if (j2_dash > 0) {
			// copy the bytes left
			//                 (i2',i2'+1  <--  (i1')
			//               (i2'+1,i2'+2) <--  (i1'+1)
			//               (i2'+2,i2'+3) <--  (i1'+2)
			//                  ........   <--  .......
			// (i2'+n_bytes-1,i2'+n_bytes) <-- (i1'+n_bytes-1)

			m = 8 - j2_dash;
			mask =  ((1 << m) - 1) << j2_dash;
			mask_carry = ((1 << j2_dash) - 1) << m;
			ch_carry = dstptr[0] & mask_carry;

			for (k=0; k<n_bytes; k++) {
				ch_src = srcptr[k];
				dstptr[k] = ch_carry | ((ch_src & mask) >> j2_dash);
				ch_carry = ch_src << m;
			}

			dstptr[k] = (dstptr[k] & ~mask_carry) | ch_carry;

		} else {
			uls_memmove(dstptr, srcptr, n_bytes);
		}
	}

	// and clear the bits trailing the shifted bits.
	uls_clear_bits(srcptr0, end_bit - n_shift + 1, end_bit);
}

int
ULS_QUALIFIED_METHOD(uls_ieee754_longdouble_isspecial)(long double x, char* nambuf)
{
	// ret-val == 1 : x is a special-value with nambuf filled.
	// ret-val == 0 x is a finite value
	char  buff[SIZEOF_LONG_DOUBLE];
	int expo_biasd, stat = 0;
	int i, i0, minus;

	uls_memcopy(buff, &x, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));
	uls_arch2be_array(buff, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));

	minus = (buff[0] & 0x80) ? 1 : 0;

	expo_biasd = get_ieee754_biased_expo(buff, _uls_sysinfo_(LDOUBLE_EXPOSIZE_BITS));
	i0 = _uls_sysinfo_(LDOUBLE_MENTISA_STARTBIT);

	if (expo_biasd == (1<<_uls_sysinfo_(LDOUBLE_EXPOSIZE_BITS))-1) {
		if (uls_find_first_1bit(buff,
				i0, _uls_sysinfo_(LDOUBLE_SIZE_BITS)-1, (uls_uint32 *) &i) == 0) {
			if (minus) uls_strcpy(nambuf, "-INF");
			else uls_strcpy(nambuf, "+INF");

		} else if (i == i0) {
			uls_strcpy(nambuf, "QNaN");
		} else { // i > i0
			uls_strcpy(nambuf, "SNaN");
		}
		stat = 1;

	} else if (expo_biasd == 0) {
		if (uls_find_first_1bit(buff,
			i0, _uls_sysinfo_(LDOUBLE_SIZE_BITS)-1, (uls_uint32 *) &i) == 0) {
			if (minus) uls_strcpy(nambuf, "-0");
			else uls_strcpy(nambuf, "+0");
		} else {
			uls_strcpy(nambuf, "PDR");
		}
	} else {
		uls_strcpy(nambuf, "PNR");
	}

	return stat;
}

long double
ULS_QUALIFIED_METHOD(uls_ieee754_modlf)(long double x, long double* p_frac)
{
	char  buff[SIZEOF_LONG_DOUBLE];
	char  buff2[SIZEOF_LONG_DOUBLE];
	int m_expo, i0, i;

	uls_memcopy(buff, &x, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));
	uls_arch2be_array(buff, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));

	m_expo = get_ieee754_biased_expo(buff, _uls_sysinfo_(LDOUBLE_EXPOSIZE_BITS)) - _uls_sysinfo_(LDOUBLE_EXPO_BIAS);
	if (m_expo < 0) {
		if (p_frac!=NULL) *p_frac = x;
		return 0.;
	}

	i0 = _uls_sysinfo_(LDOUBLE_MENTISA_STARTBIT) + m_expo;

	if (p_frac != NULL) {
		uls_memcopy(buff2, buff, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));

		if (uls_find_first_1bit(buff2, i0+1, _uls_sysinfo_(LDOUBLE_SIZE_BITS)-1, (uls_uint32 *) &i)==0) {
			*p_frac = 0.;
		} else {
			uls_ieee754_bankup_bits(buff2, i, _uls_sysinfo_(LDOUBLE_SIZE_BITS)-1, i-_uls_sysinfo_(LDOUBLE_MENTISA_STARTBIT));
			put_ieee754_biased_expo(i0-i+_uls_sysinfo_(LDOUBLE_EXPO_BIAS), buff2, _uls_sysinfo_(LDOUBLE_EXPOSIZE_BITS));
			uls_arch2be_array(buff2, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));
			uls_memcopy(p_frac, buff2, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));
		}
	}

	uls_clear_bits(buff, i0+1, _uls_sysinfo_(LDOUBLE_SIZE_BITS)-1);

	uls_arch2be_array(buff, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));
	uls_memcopy(&x, buff, _uls_sysinfo_(LDOUBLE_SIZE_BYTES));

	return x;
}
