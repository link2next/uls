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
  <file> uls_tokdef_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#include "uls/uls_tokdef_wstr.h"
#include "uls/uls_util_wstr.h"
#include "uls/uls_wlog.h"
#include "uls/uls_misc.h"

ULS_DECL_STATIC int
id2wstr_pair_finder(const uls_voidptr_t e, const uls_voidptr_t ptr_t)
{
	uls_id2wstr_pair_ptr_t pair = (uls_id2wstr_pair_ptr_t ) e;
	int  val = *((int *) ptr_t), stat;

	if (pair->tok_id < val) {
		stat = -1;
	} else if (val < pair->tok_id) {
		stat = 1;
	} else {
		stat = 0;
	}

	return stat;
}

uls_id2wstr_pair_ptr_t
__find_widename_in_list(
	uls_id2wstr_pair_ptr_t pair_list, int pair_list_len, int t)
{
	uls_id2wstr_pair_ptr_t pair;

	pair = (uls_id2wstr_pair_ptr_t ) uls_bi_search((const uls_voidptr_t) uls_ptr(t),
		pair_list, pair_list_len, sizeof(uls_id2wstr_pair_t), id2wstr_pair_finder);

	return pair;
}

uls_id2wstr_pair_ptr_t
__add_widename_in_list(
	uls_outparam_ptr_t parms, int t, const char *sval, int n_bytes)
{
	uls_id2wstr_pair_ptr_t pair_list = (uls_id2wstr_pair_ptr_t) parms->data;
	int pair_list_len = parms->len, pair_list_siz = parms->n;

	int wlen, rc;
	wchar_t *wstr, *wstr2;
	csz_str_t csz_wstr;
	uls_id2wstr_pair_ptr_t pair;

	if (pair_list_len >= pair_list_siz) {
		rc = pair_list_len + 16;
		parms->data = pair_list = (uls_id2wstr_pair_ptr_t ) uls_mrealloc(pair_list, rc*sizeof(uls_id2wstr_pair_t));
		parms->n = pair_list_siz = rc;
	}

	pair = pair_list + pair_list_len;
	pair->tok_id = t;
	pair->n_chars = ustr_num_chars(sval, n_bytes, NULL);

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(sval, -n_bytes, uls_ptr(csz_wstr))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		wstr2 = uls_wstrdup(wstr, wlen);
		pair->wlen = wlen;
		pair->wstr = wstr2;
	}

	csz_deinit(uls_ptr(csz_wstr));

	return pair_list;
}
