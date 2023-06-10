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
  <file> uls_tokdef_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#include "uls/uls_lex.h"
#include "uls/uls_misc.h"
#include "uls/uls_util.h"
#include "uls/uls_tokdef.h"
#include "uls/uls_auw.h"

#include "uls/uls_alex.h"
#include "uls/uls_tokdef_astr.h"
#include "uls/uls_alog.h"


ULS_DECL_STATIC int
id2astr_pair_finder(const uls_voidptr_t e, const uls_voidptr_t ptr_t)
{
	uls_id2astr_pair_ptr_t pair = (uls_id2astr_pair_ptr_t) e;
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

uls_id2astr_pair_ptr_t
__find_ms_mbcs_name_in_list(
	uls_ref_parray(pair_list,id2astr_pair), int pair_list_len, int t)
{
	uls_id2astr_pair_ptr_t pair;

	pair = (uls_id2astr_pair_ptr_t ) uls_bi_search((const uls_voidptr_t) uls_ptr(t),
		uls_parray_slots(pair_list), pair_list_len, sizeof(uls_id2astr_pair_t), id2astr_pair_finder);

	return pair;
}

uls_id2astr_pair_ptr_t
__add_ms_mbcs_name_in_list(
	uls_ref_parray(pair_list,id2astr_pair), int pair_list_len, int *ptr_pair_list_siz,
	int t, const char *sval, int n_bytes)
{
	int alen, rc;
	char *astr;
	csz_str_t csz_astr;
	uls_id2astr_pair_ptr_t pair;
	uls_decl_parray_slots(slots_pair, id2astr_pair);

	if (pair_list_len >= *ptr_pair_list_siz) {
		rc = pair_list_len + 16;
		uls_resize_parray(pair_list, id2astr_pair, rc);
		*ptr_pair_list_siz = rc;
	}

	slots_pair = uls_parray_slots(pair_list);
	pair = slots_pair[pair_list_len] = uls_alloc_object(uls_id2astr_pair_t);
	pair->tok_id = t;
	pair->n_chars = ustr_num_chars(sval, n_bytes, NULL);

	csz_init(uls_ptr(csz_astr), -1);

	if ((astr = uls_ustr2astr(sval, -n_bytes, uls_ptr(csz_astr))) == NULL) {
		err_alog("encoding error!");
	} else {
		pair->alen = alen = csz_length(uls_ptr(csz_astr));
		pair->astr = uls_strdup(astr, alen);
	}

	csz_deinit(uls_ptr(csz_astr));
	return pair;
}
