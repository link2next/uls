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
  <file> uls_lf_sprintf.c </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..).
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2011.
  </author>
*/

#define __ULS_LF_SPRINTF__
#include "uls/uls_lf_sprintf.h"
#include "uls/uls_lf_percent_f.h"
#include "uls/ieee754.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_lf_sysputs)(const char* msg)
{
	uls_put_binstr(msg, -1, _uls_stdio_fd(2));
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__puts_proc_str)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, const char* str, int len)
{
	int nn;

	if (len > 0) {
		nn = puts_proc(x_dat, str, len);
	} else {
		nn = 0;
	}

	return nn;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_d)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	int idx, minus;
	int num_d, len;

	num_d = (int) va_arg(lf_ctx->args, int);
	if (num_d < 0) {
		num_d = -num_d;
		minus = 1;
	} else {
		minus = 0;
	}

	idx = uls_lf_number_u(buf, num_d, ULS_LF_SHIFT_BASE_DEC);
	if (minus) lf_ctx->perfmt.flags |= ULS_LF_MINUS_PREFIX;

	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_u)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned int num_u;
	int idx, len;

	num_u = (unsigned int) va_arg(lf_ctx->args, unsigned int);
	idx = uls_lf_number_u(buf, num_u, ULS_LF_SHIFT_BASE_DEC);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lld)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	long long num_Ld;
	int idx, minus, len;

	num_Ld = (long long) va_arg(lf_ctx->args, long long);
	if (num_Ld < 0) {
		num_Ld = -num_Ld;
		minus = 1;
	} else {
		minus = 0;
	}

	idx = uls_lf_number_Lu(buf, num_Ld, ULS_LF_SHIFT_BASE_DEC);
	if (minus) lf_ctx->perfmt.flags |= ULS_LF_MINUS_PREFIX;

	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_llu)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long long num_Lu;
	int idx, len;

	num_Lu = (unsigned long long) va_arg(lf_ctx->args, unsigned long long);
	idx = uls_lf_number_Lu(buf, num_Lu, ULS_LF_SHIFT_BASE_DEC);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_LX)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long long num_Lu;
	int idx, len;

	num_Lu = (unsigned long long) va_arg(lf_ctx->args, unsigned long long);
	idx = uls_lf_number_Lu(buf, num_Lu, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_Lx)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long long num_Lu;
	int idx, len, i;

	num_Lu = (unsigned long long) va_arg(lf_ctx->args, unsigned long long);
	idx = uls_lf_number_Lu(buf, num_Lu, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	for (i=0; i<len; i++)
		wrdptr[i] = uls_tolower(wrdptr[i]);

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_X)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned int num_u;
	int idx, len;

	num_u = (unsigned int) va_arg(lf_ctx->args, unsigned int);

	idx = uls_lf_number_u(buf, num_u, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_x)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned int num_u;
	int idx, len, i;

	num_u = (unsigned int) va_arg(lf_ctx->args, unsigned int);

	idx = uls_lf_number_u(buf, num_u, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	for (i=0; i<len; i++)
		wrdptr[i] = uls_tolower(wrdptr[i]);

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_o)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned int num_u;
	int idx, len;

	num_u = (unsigned int) va_arg(lf_ctx->args, unsigned int);

	idx = uls_lf_number_u(buf, num_u, ULS_LF_SHIFT_BASE_OCT);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_p)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long num_lu;
	int idx, len;

	num_lu = (unsigned long) va_arg(lf_ctx->args, unsigned long);

	idx = uls_lf_number_lu(buf, num_lu, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	lf_ctx->perfmt.flags |= ULS_LF_ZEROPAD;
	lf_ctx->perfmt.flags &= ~ULS_LF_LEFT_JUSTIFIED;

	lf_ctx->perfmt.width = 2 * sizeof(uls_voidptr_t);
	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
		lf_ctx->perfmt.width += 2; // +2 for '0x'
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_ld)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	long num_ld;
	int idx, minus, len;

	num_ld = (long) va_arg(lf_ctx->args, long);
	if (num_ld < 0) {
		num_ld = -num_ld;
		minus = 1;
	} else {
		minus = 0;
	}

	idx = uls_lf_number_lu(buf, num_ld, ULS_LF_SHIFT_BASE_DEC);
	if (minus) lf_ctx->perfmt.flags |= ULS_LF_MINUS_PREFIX;

	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lu)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long num_lu;
	int idx, len;

	num_lu = (unsigned long) va_arg(lf_ctx->args, unsigned long);
	idx = uls_lf_number_lu(buf, num_lu, ULS_LF_SHIFT_BASE_DEC);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lX)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long num_lu;
	int idx, len;

	num_lu = (unsigned long) va_arg(lf_ctx->args, unsigned long);
	idx = uls_lf_number_lu(buf, num_lu, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lx)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	char buf[ULS_MAXSZ_NUMSTR+1];
	char *wrdptr;
	unsigned long num_lu;
	int idx, len, i;

	num_lu = (unsigned long) va_arg(lf_ctx->args, unsigned long);
	idx = uls_lf_number_lu(buf, num_lu, ULS_LF_SHIFT_BASE_HEX);
	wrdptr = buf + idx;
	len = ULS_MAXSZ_NUMSTR - idx;

	for (i=0; i<len; i++)
		wrdptr[i] = uls_tolower(wrdptr[i]);

	if (lf_ctx->perfmt.flags & ULS_LF_SPECIAL_CHAR) {
		lf_ctx->perfmt.flags |= ULS_LF_HEX_PREFIX;
	}

	return uls_lf_fill_numstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__fmtproc_f_e_g)
	(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx, double num_f)
{
	uls_lf_convflag_ptr_t perfmt = uls_ptr(lf_ctx->perfmt);
	char  *wrdptr;
	int    minus, n_expo, len;
	int    numbuf_len1, numbuf_len2;
	char   *numstr;

	csz_str_ptr_t numbuf1 = lf_ctx->numbuf1;
	csz_str_ptr_t numbuf2 = lf_ctx->numbuf2;

	numbuf_len1 = csz_length(numbuf1);
	numbuf_len2 = csz_length(numbuf2);

	if (num_f < 0.) {
		minus = 1;
		num_f = -num_f;
	} else {
		minus = 0;
	}

	n_expo = uls_lf_double2digits(num_f, perfmt->precision, numbuf1);
	numstr = (char *) csz_text(numbuf1) + numbuf_len1;

	if ((perfmt->flags & (ULS_LF_PERCENT_E | ULS_LF_PERCENT_G))==0) {
		uls_lf_digits_to_percent_f(numstr, minus, n_expo, perfmt->precision, numbuf2);
	} else if (perfmt->flags & ULS_LF_PERCENT_E) {
		uls_lf_digits_to_percent_e(numstr, minus, n_expo, perfmt->precision, numbuf2);
	} else if (perfmt->flags & ULS_LF_PERCENT_G) {
		uls_lf_digits_to_percent_g(numstr, minus, n_expo, perfmt->precision, numbuf2);
	}

	wrdptr = (char *) csz_text(numbuf2) + numbuf_len2;
	len = csz_length(numbuf2) - numbuf_len2;

	len = uls_lf_fill_numstr(x_dat, puts_proc, perfmt, wrdptr, len);

	csz_truncate(numbuf1, numbuf_len1);
	csz_truncate(numbuf2, numbuf_len2);

	return len;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_f)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	double num_f;

	num_f = (double) va_arg(lf_ctx->args, double);
	return __fmtproc_f_e_g(x_dat, puts_proc, lf_ctx, num_f);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_e)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	double num_f;

	num_f = (double) va_arg(lf_ctx->args, double);
	lf_ctx->perfmt.flags |= ULS_LF_PERCENT_E;
	return __fmtproc_f_e_g(x_dat, puts_proc, lf_ctx, num_f);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_g)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	double num_f;

	num_f = (double) va_arg(lf_ctx->args, double);
	lf_ctx->perfmt.flags |= ULS_LF_PERCENT_G;
	return __fmtproc_f_e_g(x_dat, puts_proc, lf_ctx, num_f);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__fmtproc_lf_le_lg)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx, long double num_lf)
{
	uls_lf_convflag_ptr_t perfmt = uls_ptr(lf_ctx->perfmt);
	char  *wrdptr;
	int  minus, n_expo, len;
	int  numbuf_len1, numbuf_len2;
	char   *numstr;

	csz_str_ptr_t numbuf1 = lf_ctx->numbuf1;
	csz_str_ptr_t numbuf2 = lf_ctx->numbuf2;

	numbuf_len1 = csz_length(numbuf1);
	numbuf_len2 = csz_length(numbuf2);

	if (num_lf < 0.) {
		minus = 1;
		num_lf = -num_lf;
	} else {
		minus = 0;
	}

	n_expo = uls_lf_longdouble2digits(num_lf, perfmt->precision, numbuf1);
	numstr = (char *) csz_text(numbuf1) + numbuf_len1;

	if ((perfmt->flags & (ULS_LF_PERCENT_E | ULS_LF_PERCENT_G))==0) {
		uls_lf_digits_to_percent_f(numstr, minus, n_expo, perfmt->precision, numbuf2);
	} else if (perfmt->flags & ULS_LF_PERCENT_E) {
		uls_lf_digits_to_percent_e(numstr, minus, n_expo, perfmt->precision, numbuf2);
	} else if (perfmt->flags & ULS_LF_PERCENT_G) {
		uls_lf_digits_to_percent_g(numstr, minus, n_expo, perfmt->precision, numbuf2);
	}

	wrdptr = (char *) csz_text(numbuf2) + numbuf_len2;
	len = csz_length(numbuf2) - numbuf_len2;

	len = uls_lf_fill_numstr(x_dat, puts_proc, perfmt, wrdptr, len);

	csz_truncate(numbuf1, numbuf_len1);
	csz_truncate(numbuf2, numbuf_len2);

	return len;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lf)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	long double num_lf;

	num_lf = (long double) va_arg(lf_ctx->args, long double);
	return __fmtproc_lf_le_lg(x_dat, puts_proc, lf_ctx, num_lf);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_le)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	long double num_lf;

	num_lf = (long double) va_arg(lf_ctx->args, long double);
	lf_ctx->perfmt.flags |= ULS_LF_PERCENT_E;

	return __fmtproc_lf_le_lg(x_dat, puts_proc, lf_ctx, num_lf);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_lg)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	long double num_lf;

	num_lf = (long double) va_arg(lf_ctx->args, long double);
	lf_ctx->perfmt.flags |= ULS_LF_PERCENT_G;

	return __fmtproc_lf_le_lg(x_dat, puts_proc, lf_ctx, num_lf);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(fmtproc_null)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	return 0;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_lf_name2proc_ptr_t)
ULS_QUALIFIED_METHOD(uls_lf_bi_search)(const char* keyw, int len, uls_lf_name2proc_ptr_t ary, int n_ary)
{
	uls_lf_name2proc_ptr_t e;
	int   low, high, mid, cond;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e = ary + mid;

		if ((cond=uls_strncmp(e->name, keyw, len)) < 0) {
			low = mid + 1;
		} else if (cond > 0) {
			high = mid - 1;
		} else {
			return e;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_lf_convspec_t)
ULS_QUALIFIED_METHOD(__find_convspec)(const char** p_fmtptr, uls_lf_map_ptr_t map, uls_lf_context_ptr_t lf_ctx)
{
	const char *fmtptr = *p_fmtptr;
	uls_lf_convspec_table_ptr_t tbl;
	uls_lf_name2proc_ptr_t e, proc_map;
	int  ch, i, len;

	proc_map = map->linear.proc_tab;
	for (i=0; i<map->linear.n_used; i++) {
		if ((e = proc_map + i)->proc==nilptr) continue;
		if (uls_strncmp(e->name, fmtptr, e->l_name)==0) {
			*p_fmtptr = fmtptr + e->l_name;
			lf_ctx->u_dat = e->user_data;
			return e->proc;
		}
	}

	for (len=0; len < ULS_LF_PERCENT_NAMESIZ; len++) {
		if ((ch=fmtptr[len]) == '\0' || !uls_isgraph(ch)) {
			break;
		}
	}

	if (len == 0) {
		lf_ctx->u_dat = nilptr;
		return nilptr;
	}

	for (i=len; i>0; i--) {
		tbl = map->sorted + uls_get_percproc_table_ind(i);

		e = uls_lf_bi_search(fmtptr, i, tbl->proc_tab, tbl->n_used);
		if (e != nilptr) { // found
			*p_fmtptr = fmtptr + i;
			lf_ctx->u_dat = e->user_data;
			return e->proc;
		}
	}

	lf_ctx->u_dat = nilptr;
	return nilptr;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_lf_name2proc_ptr_t)
ULS_QUALIFIED_METHOD(__realloc_convspec_table)(uls_lf_convspec_table_ptr_t tbl, int n)
{
	n += tbl->n_used;

	if (n > tbl->n_allocd) {
		n = uls_ceil_log2(n, 4);
		tbl->proc_tab = (uls_lf_name2proc_ptr_t) uls_mrealloc(
			tbl->proc_tab, n * sizeof(uls_lf_name2proc_t));
		tbl->n_allocd = n;
	}

	return tbl->proc_tab;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__add_convspec_sorted)(uls_lf_convspec_table_ptr_t tbl,
	const char* percent_name, uls_lf_convspec_t proc, uls_voidptr_t user_data)
{
	uls_lf_name2proc_ptr_t proc_map = tbl->proc_tab;
	int j, rc, len;

	len = uls_strlen(percent_name);

	for (j=0; ; j++) {
		if (j >= tbl->n_used) {
			proc_map = __realloc_convspec_table(tbl, 1);
			++tbl->n_used;
			break;
		}

		if ((rc=uls_strcmp(proc_map[j].name, percent_name)) == 0) {
//			__uls_lf_sysputs("overriding pecent convspec ...");
			break;

		} else if (rc > 0) {
			proc_map = __realloc_convspec_table(tbl, 1);
			uls_memmove(proc_map + j+1, proc_map + j, (tbl->n_used - j) *sizeof(uls_lf_name2proc_t) );
			++tbl->n_used;
			break;
		}
	}

	uls_strcpy(proc_map[j].name, percent_name);
	proc_map[j].l_name = len;
	proc_map[j].proc = proc;
	proc_map[j].user_data = user_data;

	return j;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__replace_convspec_linear)(uls_lf_convspec_table_ptr_t tbl,
	const char* percent_name, uls_lf_convspec_t proc, uls_voidptr_t user_data)
{
	uls_lf_name2proc_ptr_t e, proc_map;
	int i;

	proc_map = tbl->proc_tab;
	for (i=0; i<tbl->n_used; i++) {
		if ((e = proc_map + i)->proc==nilptr) continue;

		if (uls_strcmp(e->name, percent_name)==0) {
			e->proc = proc;
			e->user_data = user_data;
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(load_default_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_convspec_table_ptr_t tbl;

	tbl = uls_ptr(lf_map->linear);
	__add_convspec_linear(tbl, "s", fmtproc_s, ULS_LF_PERCPROC_s);
	__add_convspec_linear(tbl, "c", fmtproc_c, ULS_LF_PERCPROC_c);

	__add_convspec_linear(tbl, "d", fmtproc_d, ULS_LF_PERCPROC_d);
	__add_convspec_linear(tbl, "u", fmtproc_u, ULS_LF_PERCPROC_u);
	__add_convspec_linear(tbl, "f", fmtproc_f, ULS_LF_PERCPROC_f);

	__add_convspec_linear(tbl, "ld", fmtproc_ld, ULS_LF_PERCPROC_ld);
	__add_convspec_linear(tbl, "lu", fmtproc_lu, ULS_LF_PERCPROC_lu);
	__add_convspec_linear(tbl, "lf", fmtproc_lf, ULS_LF_PERCPROC_lf);

	__add_convspec_linear(tbl, "lld", fmtproc_lld, ULS_LF_PERCPROC_lld);
	__add_convspec_linear(tbl, "llu", fmtproc_llu, ULS_LF_PERCPROC_llu);

	__add_convspec_linear(tbl, "Ld", fmtproc_lld, ULS_LF_PERCPROC_Ld);
	__add_convspec_linear(tbl, "Lu", fmtproc_llu, ULS_LF_PERCPROC_Lu);

	__add_convspec_linear(tbl, "p", fmtproc_p, ULS_LF_PERCPROC_p);

	tbl = lf_map->sorted + uls_get_percproc_table_ind(1);
	__add_convspec_sorted(tbl, "x", fmtproc_x, nilptr);
	__add_convspec_sorted(tbl, "X", fmtproc_X, nilptr);
	__add_convspec_sorted(tbl, "o", fmtproc_o, nilptr);
	__add_convspec_sorted(tbl, "e", fmtproc_e, nilptr);
	__add_convspec_sorted(tbl, "g", fmtproc_g, nilptr);

	tbl = lf_map->sorted + uls_get_percproc_table_ind(2);
	__add_convspec_sorted(tbl, "lx", fmtproc_lx, nilptr);
	__add_convspec_sorted(tbl, "lX", fmtproc_lX, nilptr);
	__add_convspec_sorted(tbl, "Lx", fmtproc_Lx, nilptr);
	__add_convspec_sorted(tbl, "LX", fmtproc_LX, nilptr);
	__add_convspec_sorted(tbl, "le", fmtproc_le, nilptr);
	__add_convspec_sorted(tbl, "lg", fmtproc_lg, nilptr);

	tbl = lf_map->sorted + uls_get_percproc_table_ind(3);
	__add_convspec_sorted(tbl, "llx", fmtproc_Lx, nilptr);
	__add_convspec_sorted(tbl, "llX", fmtproc_LX, nilptr);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_lf_skip_atou)(const char ** p_ptr)
{
	const char   *ptr = *p_ptr;
	char   ch;
	int     n;

	for (n=0; uls_isdigit(ch=*ptr); ptr++) // ch >= '0 && ch <= '9'
		n = n*10 + (ch - '0');
	*p_ptr = ptr;
	return n;
}

int
ULS_QUALIFIED_METHOD(fmtproc_s)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	const char  *wrdptr;
	int rc, len, n_chars;
	uls_outparam_t parms1;

	wrdptr = (char* ) va_arg(lf_ctx->args, char *);
	if (wrdptr == NULL) wrdptr = "<null>";

	n_chars = ustr_num_chars(wrdptr, -1, uls_ptr(parms1));
	len = parms1.len;

	rc = uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), wrdptr, len, n_chars);

	return rc;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(fmtproc_ws)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	int rc, ulen, wlen;
	wchar_t *wstr;
	char *ustr;
	csz_str_t csz;

	wstr = (wchar_t* ) va_arg(lf_ctx->args, wchar_t *);
	if (wstr==NULL) wstr = L"<null>";
	wlen = uls_wcslen(wstr);

	csz_init(uls_ptr(csz), (wlen+1)*ULS_UTF8_CH_MAXLEN);

	if ((ustr=uls_wstr2ustr(wstr, wlen, uls_ptr(csz))) == NULL) {
		rc = -1;
	} else {
		ulen = csz_length(uls_ptr(csz));
		rc = ustr_num_chars(ustr, ulen, nilptr);
		rc = uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), ustr, ulen, rc);
	}

	csz_deinit(uls_ptr(csz));

	return rc;
}

int
ULS_QUALIFIED_METHOD(fmtproc_c)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx)
{
	uls_uint32 mch;
	char buff[4];
	char *ustr;
	int n, ulen;
	csz_str_t csz;
#ifdef ULS_WINDOWS
	int i;
#else
	wchar_t wch;
#endif

	mch = (uls_uint32) va_arg(lf_ctx->args, unsigned int);
	csz_init(uls_ptr(csz), sizeof(wchar_t)+1);

#ifdef ULS_WINDOWS
	for (i=0; ; i++,mch<<=8) {
		if (i >= 4) {
			buff[0] = 0x00;
			n = 1;
			break;
		}
		if ((mch & 0xFF000000) != 0) {
			for (n=0; i<4; i++,n++,mch<<=8) {
				buff[n] = (char) ((mch & 0xFF000000) >> 24);

			}
			break;
		}
	}

	if (n > 1) {
		if ((ustr=uls_astr2ustr(buff, n, uls_ptr(csz))) == NULL) {
			n = -1; goto end_1;
		}
		ulen = csz_length(uls_ptr(csz));
	} else {
		ustr = buff;
		ulen = 1;
	}
#else
	if (mch & 0xFFFFFF00) {
		wch = (wchar_t) mch;
		if ((ustr=uls_wstr2ustr(uls_ptr(wch), 1, uls_ptr(csz))) == NULL) {
			n = -1; goto end_1;
		}
		ulen = csz_length(uls_ptr(csz));
	} else {
		buff[0] = (char) (mch & 0xFF);
		ustr = buff;
		ulen = 1;
	}
#endif

	n = puts_proc(x_dat, ustr, ulen);

 end_1:
	csz_deinit(uls_ptr(csz));
	return n;
}

void
ULS_QUALIFIED_METHOD(__add_convspec_linear)(uls_lf_convspec_table_ptr_t tbl,
	char* percent_name, uls_lf_convspec_t proc, int tbl_ind)
{
	uls_lf_name2proc_ptr_t e, proc_map;
	int j;

	if (tbl_ind >= tbl->n_used) {
		proc_map = __realloc_convspec_table(tbl, tbl_ind+1-tbl->n_used);
		for (j=tbl->n_used; j<=tbl_ind; j++) {
			proc_map[j].name[0] = '\0';
			proc_map[j].proc = nilptr;
		}
		tbl->n_used = tbl_ind + 1;
	} else {
		proc_map = tbl->proc_tab;
	}

	// No check for duplicate elements.
	e = proc_map + tbl_ind;
	if (e->proc != nilptr)
		__uls_lf_sysputs("overriding pecent convspec ...");

	uls_strcpy(e->name, percent_name);
	e->l_name = uls_strlen(percent_name);
	e->proc = proc;
	e->user_data = nilptr;
}

ULS_QUALIFIED_RETTYP(uls_lf_convspec_t)
ULS_QUALIFIED_METHOD(get_default_convspec)(const char* percent_name, int len)
{
	uls_lf_convspec_table_ptr_t tbl;
	uls_lf_convspec_t proc;
	uls_lf_name2proc_ptr_t e;

	if (len < 0) len = uls_strlen(percent_name);
	tbl = dfl_convspec_map.sorted + uls_get_percproc_table_ind(len);

	if ((e = uls_lf_bi_search(percent_name, len, tbl->proc_tab, tbl->n_used)) == nilptr) {
		proc = fmtproc_null;
	} else {
		proc = e->proc; // default action
	}

	return proc;
}

const char*
ULS_QUALIFIED_METHOD(uls_pars_perfmt)(uls_lf_convflag_ptr_t p, const char* fmt)
{
	uls_lf_convflag_t nilspec;
	char  ch;

	if (p==nilptr)
		p = uls_ptr(nilspec);

	p->flags = 0;
	p->width = -1;
	p->precision = ULS_LF_DFL_FLOAT_PRECISION;

	/* process flags */
	while (1) {
		ch = *fmt;
		if (ch == '-') p->flags |= ULS_LF_LEFT_JUSTIFIED;
		else if (ch == '+') p->flags |= ULS_LF_PLUS_PREFIX;
		else if (ch == '#') p->flags |= ULS_LF_SPECIAL_CHAR;
		else if (ch == '0') p->flags |= ULS_LF_ZEROPAD;
		else break;
		++fmt;
	}

	/* get field width */
	if (ch == '*') {
		p->flags |= ULS_LF_DYNAMIC_WIDTH;
		++fmt;
	} else if (uls_isdigit(ch)) {
		p->width = __uls_lf_skip_atou(uls_ptr(fmt));
	}

	if (*fmt=='.') {
		if ((ch=*++fmt) == '*') {
			p->flags |= ULS_LF_DYNAMIC_PRECISION;
			++fmt;
		} else if (uls_isdigit(ch)) {
			p->precision = __uls_lf_skip_atou(uls_ptr(fmt));
		}
	}

	if (p->flags & ULS_LF_LEFT_JUSTIFIED) p->flags &= ~ULS_LF_ZEROPAD;

	return fmt;
}

void
ULS_QUALIFIED_METHOD(initialize_uls_lf)(void)
{
	uls_lf_init_convspec_map(uls_ptr(dfl_convspec_map), 0);
}

void
ULS_QUALIFIED_METHOD(finalize_uls_lf)(void)
{
	uls_lf_deinit_convspec_map(uls_ptr(dfl_convspec_map));
}

int
ULS_QUALIFIED_METHOD(uls_lf_puts_prefix)(char* str, int flags)
{
	int   k = 0;

	if (flags & ULS_LF_MINUS_PREFIX) {
		str[k++] = '-';
	} else if (flags & ULS_LF_PLUS_PREFIX) {
		str[k++] = '+';
	}

	if (flags & ULS_LF_HEX_PREFIX) {
		str[k++] = '0'; str[k++] = 'x';
	} else  if (flags & ULS_LF_BIN_PREFIX) {
		str[k++] = '0'; str[k++] = 'b';
	}

	str[k] = '\0';

	return k;
}

int
ULS_QUALIFIED_METHOD(uls_lf_fill_ch)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, char ch_fill, int n)
{
	char fillstr[ULS_LF_N_FILLCHS];
	int i, m, nn, rc;

	if (n==0) return 0;

	for (i=0; i<ULS_LF_N_FILLCHS; i++)
		fillstr[i] = ch_fill;

	for (nn=i=0; i < n / ULS_LF_N_FILLCHS; i++) {
		rc = puts_proc(x_dat, fillstr, ULS_LF_N_FILLCHS);
		if (rc < 0) return -1;
		nn += rc;
	}

	if ((m=n % ULS_LF_N_FILLCHS) != 0) {
		rc = puts_proc(x_dat, fillstr, m);
		if (rc < 0) return -1;
		nn += rc;
	}

	return nn;
}

int
ULS_QUALIFIED_METHOD(uls_lf_fill_numstr)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc,
	uls_lf_convflag_ptr_t p, const char* numstr, int l_numstr)
{
	int     width, len1, len2, len3, nn, rc;
	const char *ptr1, *ptr2;
	char    prefix[16];

	len1 = uls_lf_puts_prefix(prefix, p->flags);
	len2 = l_numstr;
	len3 = len1 + len2; /* prefix + number_str */

	if ((width=p->width)<0) width = len3;
	else if (len3 > width) width = len3;

	ptr1 = prefix;
	ptr2 = numstr;

	if (p->flags & ULS_LF_LEFT_JUSTIFIED) {
		nn = __puts_proc_str(x_dat, puts_proc, ptr1, len1);
		if (nn < 0) return -1;

		rc = __puts_proc_str(x_dat, puts_proc, ptr2, len2);
		if (rc < 0) return -1;
		nn += rc;

		rc = uls_lf_fill_ch(x_dat, puts_proc, ' ', width - len3);
		if (rc < 0) return -1;
		nn += rc;

	} else {
		if (p->flags & ULS_LF_ZEROPAD) {
			nn = __puts_proc_str(x_dat, puts_proc, ptr1, len1);
			if (nn < 0) return -1;

			rc = uls_lf_fill_ch(x_dat, puts_proc, '0', width - len3);
			if (rc < 0) return -1;
			nn += rc;

			rc = __puts_proc_str(x_dat, puts_proc, ptr2, len2);
			if (rc < 0) return -1;
			nn += rc;

		} else {
			nn = uls_lf_fill_ch(x_dat, puts_proc, ' ', width - len3);
			if (nn < 0) return -1;

			rc = __puts_proc_str(x_dat, puts_proc, ptr1, len1);
			if (rc < 0) return -1;
			nn += rc;

			rc = __puts_proc_str(x_dat, puts_proc, ptr2, len2);
			if (rc < 0) return -1;
			nn += rc;
		}
	}

	return nn;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_fill_mbstr)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc,
	uls_lf_convflag_ptr_t p, const char* numstr, int l_numstr, int lw_numstr)
{
	int width=p->width, len2, nn, rc;

	if (lw_numstr > width) width = lw_numstr;
	len2 = width - lw_numstr;

	if (p->flags & ULS_LF_LEFT_JUSTIFIED) {
		nn = __puts_proc_str(x_dat, puts_proc, numstr, l_numstr);
		if (nn < 0) return -1;

		rc = uls_lf_fill_ch(x_dat, puts_proc, ' ', len2);
		if (rc < 0) return -1;
		nn += rc;

	} else {
		if (p->flags & ULS_LF_ZEROPAD) {
			nn = uls_lf_fill_ch(x_dat, puts_proc, '0', len2);
			if (nn < 0) return -1;

			rc = __puts_proc_str(x_dat, puts_proc, numstr, l_numstr);
			if (rc < 0) return -1;
			nn += rc;

		} else {
			nn = uls_lf_fill_ch(x_dat, puts_proc, ' ', len2);
			if (nn < 0) return -1;

			rc = __puts_proc_str(x_dat, puts_proc, numstr, l_numstr);
			if (rc < 0) return -1;
			nn += rc;
		}
	}

	return nn;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_lf_map_ptr_t)
ULS_QUALIFIED_METHOD(uls_lf_get_default)(void)
{
	return uls_ptr(dfl_convspec_map);
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_puts_csz)(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	if (wrdptr == NULL) {
		csz_text((csz_str_ptr_t ) x_dat);
		return 0;
	}

	csz_append((csz_str_ptr_t ) x_dat, wrdptr, wrdlen);
	return wrdlen;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_puts_str)(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	uls_buf4str_ptr_t lb = (uls_buf4str_ptr_t) x_dat;

	if (wrdptr == NULL) {
		if (lb->bufsiz <= 0) {
			__uls_lf_sysputs("sprintf: buffer overflow");
			return -1;
		}
		*lb->bufptr = '\0';
		return 0;
	}

	if (lb->bufsiz <= wrdlen) { // considering the space for '\0'
		__uls_lf_sysputs("sprintf: buffer overflow");
		return -1;
	}

	uls_memcopy(lb->bufptr, wrdptr, wrdlen);
	lb->bufptr += wrdlen;
	lb->bufsiz -= wrdlen;

	return wrdlen;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_puts_file)(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	if (fwrite(wrdptr, sizeof(char), wrdlen, fout) < (size_t) wrdlen) {
		wrdlen = -1;
	}

	return wrdlen;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_puts_null)(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	return 0;
}

// <brief>
// Changes the associated procedure with 'percent_name', a converion specification.
// </brief>
// <parm name="lf_map">uls_lf_map</parm>
// <parm name="percent_name">A converion specification without the percent character itself.</parm>
// <parm name="proc">The user defined procedure for processing '%percent_name'</parm>
// <return>0 if it suceeds, otherwise -1</return>
ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_register_convspec)(uls_lf_map_ptr_t lf_map, const char* percent_name, uls_lf_convspec_t proc)
{
	uls_voidptr_t user_data=nilptr;
	uls_lf_convspec_table_ptr_t tbl;
	int len, ch;

	if (lf_map == nilptr) lf_map = uls_ptr(dfl_convspec_map);

	for (len=0; (ch=percent_name[len])!='\0'; len++) {
		if (!uls_isgraph(ch)) {
			__uls_lf_sysputs("uls_lf_register_convspec: The percent-name contains illegal char");
			return -1;
		}
	}

	if (len > ULS_LF_PERCENT_NAMESIZ || len <= 0) {
		__uls_lf_sysputs("Too long percent func ...");
		return -1;
	}

	if (proc == nilptr) proc = get_default_convspec(percent_name, len);

	uls_lf_map_lock(lf_map);

	if (__replace_convspec_linear(uls_ptr(lf_map->linear), percent_name, proc, user_data) == 0) {
		tbl = lf_map->sorted + uls_get_percproc_table_ind(len);
		__add_convspec_sorted(tbl, percent_name, proc, user_data);
	}

	uls_lf_map_unlock(lf_map);

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_init_convspec_map)(uls_lf_map_ptr_t lf_map, int flags)
{
	uls_initial_zerofy_object(lf_map);

	lf_map->flags = ULS_FL_STATIC;
	uls_init_mutex(uls_ptr(lf_map->mtx));

	if (!(flags & ULS_LF_NO_DEFAULT)) {
		load_default_convspec_map(lf_map);
		uls_lf_register_convspec(lf_map, "ls", fmtproc_ws);
		uls_lf_register_convspec(lf_map, "ws", fmtproc_ws);
		uls_lf_register_convspec(lf_map, "S", fmtproc_ws);
	}

	lf_map->ref_cnt = 1;
	return 0;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_lf_map_ptr_t)
ULS_QUALIFIED_METHOD(uls_lf_create_convspec_map)(int flags)
{
	uls_lf_map_ptr_t lf_map;

	lf_map = uls_alloc_object(uls_lf_map_t);
	if (uls_lf_init_convspec_map(lf_map, flags) < 0) {
		uls_dealloc_object(lf_map);
		return nilptr;
	}

	lf_map->flags &= ~ULS_FL_STATIC;
	return lf_map;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(__lf_deinit_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_convspec_table_ptr_t tbl;
	int i;

	lf_map->flags &= ULS_FL_STATIC;

	for (i=0; i < ULS_LF_PERCENT_NAMESIZ; i++) {
		tbl = lf_map->sorted + i;
		uls_mfree(tbl->proc_tab);
		tbl->n_used = tbl->n_allocd = 0;
	}

	tbl = uls_ptr(lf_map->linear);
	uls_mfree(tbl->proc_tab);
	tbl->n_used = tbl->n_allocd = 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_deinit_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	int rc;

	if (lf_map == nilptr) {
		return -1;
	}

	uls_lf_map_lock(lf_map);

	if (lf_map->ref_cnt <= 0) {
		uls_lf_map_unlock(lf_map);
		return -1;
	}

	if ((rc=--lf_map->ref_cnt) > 0) {
		uls_lf_map_unlock(lf_map);
		return rc;
	}

	__lf_deinit_convspec_map(lf_map);

	uls_lf_map_unlock(lf_map);
	uls_deinit_mutex(uls_ptr(lf_map->mtx));

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_destroy_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	int rc;

	if ((rc=uls_lf_deinit_convspec_map(lf_map)) != 0) {
		return rc;
	}

	if (!(lf_map->flags & ULS_FL_STATIC)) {
		uls_mfree(lf_map);
	}

	return 0;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_lf_grab_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_map_lock(lf_map);
	++lf_map->ref_cnt;
	uls_lf_map_unlock(lf_map);
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_lf_ungrab_convspec_map)(uls_lf_map_ptr_t lf_map)
{
	uls_lf_destroy_convspec_map(lf_map);
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_init)(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t lf_map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_lf->x_dat = x_dat;

	if (puts_proc == nilptr) {
		uls_lf->uls_lf_puts = uls_lf_puts_null;
	} else {
		uls_lf->uls_lf_puts = puts_proc;
	}

	uls_init_mutex(uls_ptr(uls_lf->mtx));

	if (lf_map == nilptr) lf_map = uls_ptr(dfl_convspec_map);
	uls_lf_grab_convspec_map(lf_map);

	uls_lf->convspec_map = lf_map;

	uls_lf->g_dat = nilptr;
	csz_init(uls_ptr(uls_lf->wbuf1), ULS_MAXSZ_NUMSTR);
	csz_init(uls_ptr(uls_lf->wbuf2), ULS_MAXSZ_NUMSTR);

	uls_lf->shell = nilptr;

	return 0;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_lf_deinit)(uls_lf_ptr_t uls_lf)
{
	csz_deinit(uls_ptr(uls_lf->wbuf1));
	csz_deinit(uls_ptr(uls_lf->wbuf2));

	uls_lf_destroy_convspec_map(uls_lf->convspec_map);
	uls_deinit_mutex(uls_ptr(uls_lf->mtx));
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_lf_ptr_t)
ULS_QUALIFIED_METHOD(uls_lf_create)(uls_lf_map_ptr_t lf_map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_lf_ptr_t uls_lf;

	uls_lf = uls_alloc_object(uls_lf_t);
	uls_lf_init(uls_lf, lf_map, x_dat, puts_proc);

	return uls_lf;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_lf_destroy)(uls_lf_ptr_t uls_lf)
{
	uls_lf_deinit(uls_lf);
	uls_mfree(uls_lf);
}

// <brief>
// You can use this method to change the default output interface for logging.
// </brief>
// <parm name="uls_lf">uls-lf context</parm>
// <parm name="puts_proc">A newly output interface</parm>
// <return>none</return>
ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(__uls_lf_change_gdat)(uls_lf_ptr_t uls_lf, uls_voidptr_t gdat)
{
	uls_voidptr_t old_gdat;

	old_gdat = uls_lf->g_dat;
	uls_lf->g_dat = gdat;

	return old_gdat;
}

ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_lf_change_gdat)(uls_lf_ptr_t uls_lf, uls_voidptr_t gdat)
{
	uls_voidptr_t old_gdat;

	uls_lf_lock(uls_lf);
	old_gdat = __uls_lf_change_gdat(uls_lf, gdat);
	uls_lf_unlock(uls_lf);

	return old_gdat;
}

// <brief>
// You can use this method to change the default output interface for logging.
// </brief>
// <parm name="uls_lf">uls-lf context</parm>
// <parm name="puts_proc">A newly output interface</parm>
// <return>none</return>
ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(__uls_lf_change_xdat)(uls_lf_ptr_t uls_lf, uls_voidptr_t xdat)
{
	uls_voidptr_t old_xdat;

	old_xdat = uls_lf->x_dat;

	if (old_xdat != nilptr)
		uls_lf->uls_lf_puts(old_xdat, nilptr, 0);

	uls_lf->x_dat = xdat;

	return old_xdat;
}

ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_lf_change_xdat)(uls_lf_ptr_t uls_lf, uls_voidptr_t xdat)
{
	uls_voidptr_t old_xdat;

	uls_lf_lock(uls_lf);
	old_xdat = __uls_lf_change_xdat(uls_lf, xdat);
	uls_lf_unlock(uls_lf);

	return old_xdat;
}

// <brief>
// You can use this method to change the default output interface for logging.
// </brief>
// <parm name="uls_lf">uls-lf context</parm>
// <parm name="puts_proc">A newly output interface</parm>
// <return>none</return>
ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(__uls_lf_change_puts)(uls_lf_ptr_t uls_lf, uls_lf_delegate_ptr_t delegate)
{
	uls_lf_puts_t old_proc;

	delegate->xdat = __uls_lf_change_xdat(uls_lf, delegate->xdat);

	old_proc = uls_lf->uls_lf_puts;

	if (delegate->puts == nilptr) {
		uls_lf->uls_lf_puts = uls_lf_puts_null;
	} else {
		uls_lf->uls_lf_puts = delegate->puts;
	}

	delegate->puts = old_proc;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_lf_change_puts)(uls_lf_ptr_t uls_lf, uls_lf_delegate_ptr_t delegate)
{
	uls_lf_lock(uls_lf);
	__uls_lf_change_puts(uls_lf, delegate);
	uls_lf_unlock(uls_lf);
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_lf_vxprintf)(uls_lf_ptr_t uls_lf, const char* fmt, va_list args)
{
	uls_voidptr_t x_dat = uls_lf->x_dat;
	uls_lf_puts_t     puts_proc = uls_lf->uls_lf_puts;
	uls_lf_convspec_t proc;
	int               buflen = 0, len, zbuf_len1, zbuf_len2;
	const char *fmtptr, *wrdptr;
	uls_lf_context_t  lf_ctx;
	char     ch;

	lf_ctx.g_dat = uls_lf->g_dat;
#ifdef ULS_WINDOWS
	lf_ctx.args = args;
#else
	va_copy(lf_ctx.args, args);
#endif
	zbuf_len1 = csz_length(uls_ptr(uls_lf->wbuf1));
	zbuf_len2 = csz_length(uls_ptr(uls_lf->wbuf2));

	lf_ctx.numbuf1 = uls_ptr(uls_lf->wbuf1);
	lf_ctx.numbuf2 = uls_ptr(uls_lf->wbuf2);

	wrdptr = fmtptr = fmt;
	for ( ; ; ) {
		for ( ; (ch=*fmtptr) != '\0' && ch != '%'; fmtptr += len) {
			if ((len = uls_decode_utf8(fmtptr, -1, NULL)) <= 0) {
				return -2;
			}
		}

		if ((len = (int) (fmtptr - wrdptr)) > 0) {
			if ((len = puts_proc(x_dat, wrdptr, len)) < 0) {
				buflen = -1; break;
			}
			buflen += len;
		}

		if (ch=='\0') {
			if ((len = puts_proc(x_dat, NULL, 0)) < 0) {
				buflen = -1; break;
			}

			buflen += len;
			break;
		}

		wrdptr = fmtptr++;
		if (*fmtptr == '%') {
			wrdptr = fmtptr++;
			continue;
		}

		fmtptr = uls_pars_perfmt(uls_ptr(lf_ctx.perfmt), fmtptr);

		if ((proc=__find_convspec(uls_ptr(fmtptr), uls_lf->convspec_map, uls_ptr(lf_ctx))) == nilptr) {
			fmtptr = wrdptr + 1;
			if ((proc=__find_convspec(uls_ptr(fmtptr), uls_lf->convspec_map, uls_ptr(lf_ctx))) == nilptr) {
				continue;
			}
		}

		wrdptr = fmtptr;

		if (lf_ctx.perfmt.flags & ULS_LF_DYNAMIC_WIDTH) {
			lf_ctx.perfmt.width = (int) va_arg(lf_ctx.args, int);
			if (lf_ctx.perfmt.width < 0) {
				lf_ctx.perfmt.flags |= ULS_LF_LEFT_JUSTIFIED;
				lf_ctx.perfmt.width = -lf_ctx.perfmt.width;
			}
		}

		if (lf_ctx.perfmt.flags & ULS_LF_DYNAMIC_PRECISION) {
			lf_ctx.perfmt.precision = (int) va_arg(lf_ctx.args, int);
			if (lf_ctx.perfmt.precision < 0) {
				lf_ctx.perfmt.precision = ULS_LF_DFL_FLOAT_PRECISION;
			}
		}

		if ((len=proc(x_dat, puts_proc, uls_ptr(lf_ctx))) < 0) {
			buflen = -1;
			break;
		}
		buflen += len;
	}

	csz_truncate(uls_ptr(uls_lf->wbuf1), zbuf_len1);
	csz_truncate(uls_ptr(uls_lf->wbuf2), zbuf_len2);

	return buflen;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_vxprintf)(uls_lf_ptr_t uls_lf, const char* fmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_lf_xprintf)(uls_lf_ptr_t uls_lf, const char* fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_xprintf)(uls_lf_ptr_t uls_lf, const char* fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vxprintf(uls_lf, fmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_lf_vxprintf_generic)(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, va_list args)
{
	uls_voidptr_t old_dat;
	int len;

	if (x_dat == nilptr) x_dat = uls_lf->x_dat;

	old_dat = uls_lf->x_dat;
	uls_lf->x_dat = x_dat;
	len = __uls_lf_vxprintf(uls_lf, fmt, args);
	uls_lf->x_dat = old_dat;

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_vxprintf_generic)(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, va_list args)
{
	int len;

	if (x_dat == nilptr) x_dat = uls_lf->x_dat;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxprintf_generic(x_dat, uls_lf, fmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(__uls_lf_xprintf_generic)(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = __uls_lf_vxprintf_generic(x_dat, uls_lf, fmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_lf_xprintf_generic)(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vxprintf_generic(x_dat, uls_lf, fmt, args);
	va_end(args);

	return len;
}

