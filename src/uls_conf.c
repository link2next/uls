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
  <file> uls_conf.c </file>
  <brief>
    Loading the lexical spec file (*.ulc) of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_CONF__
#include "uls/uls_conf.h"
#include "uls/uls_langs.h"
#include "uls/utf_file.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_keyw_str)(int lno, const char* str, _uls_tool_ptrtype_(outparam) parms)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t ) parms->data;
	char *ch_ctx = uls->ch_context, *buf = parms->line, ch;
	int case_insensitive = uls->flags & ULS_FL_CASE_INSENSITIVE;
	const char* ptr;
	int  i, len, rc;
	int  n_ch_quotes, n_ch_comms, n_ch_non_idstr;
	int  n_lfs, n_tabs;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_uch_t uch;
	uls_litstr_t lit1;
	uls_decl_parray_slots(slots_qmt, quotetype);

	if (*str == '\0' || _uls_tool(is_pure_int_number)(str) > 0) {
		*buf = '\0';
		parms->len = 0;
		return ULS_KEYW_TYPE_USER;
	}

	n_ch_quotes = n_ch_comms = n_ch_non_idstr = 0;
	n_lfs = n_tabs = 0;

	for (len=0, ptr=str; (ch=*ptr)!='\0'; ) {
		if (len > ULS_LEXSTR_MAXSIZ) {
			_uls_log(err_log)("#%d: Too long keyword '%s'", lno, str);
			return -1;
		}

		if (ch == '\\') {
			lit1.lptr = ++ptr;
			uch = uls_get_escape_char(uls_ptr(lit1));

			if (lit1.len_ch_escaped < 0) {
				buf[len++] = '\\';
				buf[len++] = *ptr++;
			} else {
				if (uch <= 0x7F && case_insensitive) {
					uch = _uls_tool_(toupper)(uch);
				}
				if ((rc = _uls_tool_(encode_utf8)(uch, buf+len, ULS_LEXSTR_MAXSIZ-len)) <= 0) {
					_uls_log(err_log)("#%d: encoding error!", lno);
					return -1;
				}
				len += rc;
				ptr = lit1.lptr;
			}

		} else {
			if (case_insensitive) ch = _uls_tool_(toupper)(ch);
			buf[len++] = ch;
			++ptr;
		}
	}

	buf[len] = '\0';
	parms->len = len;

	ptr = buf;
	if ((rc=uls_is_char_idfirst(uls, ptr, &uch)) > 0) {
		do {
			if (rc > 1) {
				_uls_log(err_log)("Unicode char in keyword not permitted!");
				return -1;
			}
			ptr += rc;
		} while ((rc=uls_is_char_id(uls, ptr, &uch)) > 0);

		if (*ptr == '\0') {
			n_ch_non_idstr = 0;
		} else {
			n_ch_non_idstr = 1;
		}

	} else {
		do {
			rc = _uls_tool_(decode_utf8)(ptr, -1, NULL);

			if ((ch=*ptr) == ' ') {
				_uls_log(err_log)("#%d: contains illegal char in keyword", lno);
				return -1;
			}

			if (ch == '\n') ++n_lfs;
			else if (ch == '\t') ++n_tabs;

			if (ch_ctx[ch] & ULS_CH_QUOTE) ++n_ch_quotes;
			if (ch_ctx[ch] & ULS_CH_COMM) ++n_ch_comms;

		} while (*(ptr += rc) != '\0');

		n_ch_non_idstr = 1;
	}

	if (len > 1 && (n_lfs > 0 || n_tabs > 0)) {
		_uls_log(err_log)("#%d: contains illegal chars in keyword.", lno);
		return -1;
	}

	if (n_ch_comms > 0) {
		for (i=0; i<uls->n_commtypes; i++) {
			cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
			if (_uls_tool_(strstr)(buf, _uls_get_namebuf_value(cmt->start_mark)) != NULL) {
				_uls_log(err_log)("#%d: keyword has the comment-string '%s'", lno, _uls_get_namebuf_value(cmt->start_mark));
				return -1;
			}
		}
	}

	if (n_ch_quotes > 0) {
		slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
		for (i=0; i<uls->quotetypes.n; i++) {
			qmt = slots_qmt[i];
			if (_uls_tool_(strstr)(buf, _uls_get_namebuf_value(qmt->start_mark)) != NULL) {
				_uls_log(err_log)("#%d: keyword has the quote-string '%s'", lno, _uls_get_namebuf_value(qmt->start_mark));
				return -1;
			}
		}
	}

	if (n_ch_non_idstr == 0) {
		rc = ULS_KEYW_TYPE_IDSTR;
	} else if (len == 1) {
		rc = ULS_KEYW_TYPE_1CHAR;
	} else {
		rc = ULS_KEYW_TYPE_TWOPLUS;
		if (len > ULS_TWOPLUS_MAXLEN) {
			_uls_log(err_log)("#%d: %s too long for 2-chars token", lno, buf);
			return -1;
		}
	}

	return rc;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_tokdef_by_tokid)(uls_lex_ptr_t uls, int t, int area)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t  e_vx, e_vx_ret=nilptr;
	int i, i0, n;

	if (area == TOKDEF_AREA_RSVD) {
		slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
		i0 = 0;
		n = N_RESERVED_TOKS;

	} else if (area == TOKDEF_AREA_REGULAR) {
		slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
		i0 = N_RESERVED_TOKS;
		n = uls->tokdef_vx_array.n - N_RESERVED_TOKS;

	} else if (area == TOKDEF_AREA_BOTH) {
		slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
		i0 = 0;
		n = uls->tokdef_vx_array.n;

	} else {
		_uls_log(err_log)("%s: unknown area:%d", __FUNCTION__, area);
		return nilptr;
	}

	for (i=0; i<n; i++) {
		e_vx = slots_vx[i0+i];
		if (e_vx->tok_id == t) {
			e_vx_ret = e_vx;
			break;
		}
	}

	return e_vx_ret;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_rsvd_tokdef_by_name)(uls_lex_ptr_t uls, const char* name)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t  e_vx, e0_vx=nilptr;
	int i;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		if (uls_streql(_uls_get_namebuf_value(e_vx->name), name)) {
			e0_vx = e_vx;
			break;
		}
	}

	return e0_vx;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_reg_tokdef_by_name)(uls_lex_ptr_t uls, const char* name, _uls_tool_ptrtype_(outparam) parms)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_name_ptr_t e_nam;
	int i;

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));

	for (i=N_RESERVED_TOKS; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (uls_streql(_uls_get_namebuf_value(e_vx->name), name)) {
			parms->data = nilptr;
			return e_vx;
		}
	}

	for (i=N_RESERVED_TOKS; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		for (e_nam=e_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->prev) {
			if (uls_streql(_uls_get_namebuf_value(e_nam->name), name)) {
				parms->data = e_nam;
				return e_vx;
			}
		}
	}

	parms->data = nilptr;
	return nilptr;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(__find_tokdef_by_keyw)(uls_lex_ptr_t uls, char* keyw)
{
	uls_decl_parray_slots_init(slots_tok, tokdef, uls_ptr(uls->tokdef_array));
	uls_tokdef_ptr_t e;
	int i;

	for (i=0; i<uls->tokdef_array.n; i++) {
		e = slots_tok[i];
		if (uls_streql(_uls_get_namebuf_value(e->keyword), keyw)) {
			return e;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_str_contained_in_commtypes)(uls_lex_ptr_t uls, const char *str)
{
	uls_commtype_ptr_t cmt;
	int i, l_str;

	if ((l_str = _uls_tool_(strlen)(str)) <= 0) return 0;

	for (i=0; i<uls->n_commtypes; i++) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (l_str <= cmt->len_start_mark && !_uls_tool_(strncmp)(str, _uls_get_namebuf_value(cmt->start_mark), l_str)) {
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_str_contained_in_quotetypes)(uls_lex_ptr_t uls, const char *str)
{
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	int i, l_str;

	if ((l_str = _uls_tool_(strlen)(str)) <= 0) return 0;

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		// qmt->len_start_mark > 0

		if (l_str <= qmt->len_start_mark && !_uls_tool_(strncmp)(str, _uls_get_namebuf_value(qmt->start_mark), l_str)) {
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(realloc_tokdef_array)(uls_lex_ptr_t uls, int n1, int n2)
{
	int siz;

	/* allocate the slot for new uls_tokdef_vx_t. */
	if (uls->tokdef_vx_array.n_alloc < (siz = uls->tokdef_vx_array.n + n1)) {
		siz = uls_roundup(siz, TOKDEF_LINES_DELTA);
		uls_resize_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, siz);
		// Need it to zerofy the newly allocated space?
	}

	/* allocate the slot for new uls_tokdef_ptr_t. */
	if (uls->tokdef_array.n_alloc < (siz = uls->tokdef_array.n + n2)) {
		siz = uls_roundup(siz, TOKDEF_LINES_DELTA);
		uls_resize_parray(uls_ptr(uls->tokdef_array), tokdef, siz);
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(make_tokdef_for_quotetype)(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, char *qmt_name)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_tok, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_ptr_t e;
	int tok_id;
	_uls_tool_type_(outparam) parms;

	tok_id = qmt->tok_id;

	if (__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_BOTH) != nilptr) {
		_uls_log(err_log)("%s: the token-name already made for token", __FUNCTION__);
		_uls_log(err_log)("\ttoken-name:%s, token:%d", qmt_name, tok_id);
		return -1;
	}

	e = uls_create_tokdef();
	e->keyw_type = ULS_KEYW_TYPE_LITERAL;

	_uls_set_namebuf_value(e->keyword, _uls_get_namebuf_value(qmt->start_mark));
	e->l_keyword = qmt->len_start_mark;

	realloc_tokdef_array(uls, 1, 1);
	slots_tok = uls_parray_slots(uls_ptr(uls->tokdef_array));
	slots_tok[uls->tokdef_array.n] = e;

	if (qmt_name[0] != '\0' && (__find_reg_tokdef_by_name(uls, qmt_name, uls_ptr(parms)) != nilptr ||
		__find_rsvd_tokdef_by_name(uls, qmt_name) != nilptr)) {
		_uls_log(err_log)("%s: can't make a tok name for token", __FUNCTION__);
		_uls_log(err_log)("\ttoken-name:%s, token:%d", qmt_name, tok_id);
		return -1;
	}

	e_vx = uls_create_tokdef_vx(tok_id, qmt_name, e);

	++uls->tokdef_array.n;
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	slots_vx[uls->tokdef_vx_array.n++] = e_vx;

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(add_id_charset)(uls_ref_array_this_type02(ranges,uch_range), int n_ary,
	uls_uch_t i1, uls_uch_t i2)
{
	_uls_ptrtype_this_(none,uch_range) ran;
	int k;

	if (i1 > i2) return 0;

	if ((k=n_ary) >= ULS_N_CHARSET_RANGES) {
		return -1;
	}

	ran = uls_get_array_slot_type02(ranges, k);

	ran->x1 = i1;
	ran->x2 = i2;

	return 1;
}

ULS_DECL_STATIC char*
ULS_QUALIFIED_METHOD(find_cnst_suffix)(char* cstr_pool, const char* str, int l_str, _uls_tool_ptrtype_(outparam) parms)
{
	char *ptr, *ret_ptr = NULL;
	int l;

	for (ptr=cstr_pool; *ptr!='\0'; ptr+=l+1) {
		if ((l=_uls_tool_(strlen)(ptr)) == l_str && _uls_tool_(memcmp)(ptr, str, l_str) == 0) {
			ret_ptr = ptr;
			break;
		}
	}

	parms->line = ptr;
	return ret_ptr; // the ptr corresponding to 'str'
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(add_cnst_suffix)(char* cnst_suffixes, const char* suffix)
{
	const char *srcptr;
	char ch, *dstptr, *ptr;
	int len, k;
	_uls_tool_type_(outparam) parms;

	if (suffix == NULL || (len=_uls_tool_(strlen)(suffix)) == 0 || len > ULS_CNST_SUFFIX_MAXSIZ)
		return 0;

	if (find_cnst_suffix(cnst_suffixes, suffix, len, uls_ptr(parms)) != NULL) {
		return 0;
	}

	ptr = parms.line;
	k = (int) (ptr - cnst_suffixes);
	if (ULS_CNST_SUFFIXES_MAXSIZ - k < len + 1) {
		return 0;
	}

	dstptr = cnst_suffixes + k;
	for (srcptr=suffix; (ch=*srcptr) != '\0'; srcptr++) {
		if (!_uls_tool_(isgraph)(ch) || _uls_tool_(isdigit)(ch)) return 0;
		*dstptr++ = ch;
	}

	*dstptr++ = '\0';
	*dstptr = '\0';

	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(cnst_suffixes_by_len)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const _uls_tool_ptrtype_(argstr) e1 = (const _uls_tool_ptrtype_(argstr)) a;
	const _uls_tool_ptrtype_(argstr) e2 = (const _uls_tool_ptrtype_(argstr)) b;

	int len1, len2, stat;

	len1 = _uls_tool_(strlen)(e1->str);
	len2 = _uls_tool_(strlen)(e2->str);

	if (len1  < len2) stat = 1;
	else if (len1  > len2) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_validity_of_cnst_suffix)(_uls_tool_ptrtype_(arglst) cnst_suffixes, char* suffixes_str)
{
	int n_cnst_suffixes = cnst_suffixes->args.n;
	_uls_decl_parray_slots_init_tool_(al, argstr, uls_ptr(cnst_suffixes->args));
	_uls_tool_ptrtype_(argstr)  arg;
	char *ptr, ch;
	int i, j;

	for (i=0; i<n_cnst_suffixes; i++) {
		arg = al[i];
		ptr = arg->str;

		for (j=0; (ch=ptr[j]) != '\0'; j++) {
			if (!_uls_tool_(isgraph)(ch) || _uls_tool_(isdigit)(ch)) {
				_uls_log(err_log)("An invalid constant suffix found. ignoring '%s' ...", ptr);
				_uls_tool_(destroy_argstr)(arg); al[i] =  nilptr;
			}
		}

		if (j == 0) {
			_uls_tool_(destroy_argstr)(arg); al[i] =  nilptr;
		}
	}

	for (i=0; i<n_cnst_suffixes; ) {
		if ((arg = al[i]) == nilptr) {
			al[i] = al[--n_cnst_suffixes];
		} else {
			++i;
		}
	}

	for (i=n_cnst_suffixes; i<cnst_suffixes->args.n; i++) {
		al[i] = nilptr;
	}
	cnst_suffixes->args.n = n_cnst_suffixes;

	_uls_quicksort_vptr(al, n_cnst_suffixes, cnst_suffixes_by_len);

	suffixes_str[0] = '\0';
	for (i=0; i<n_cnst_suffixes; i++) {
		arg = al[i];
		add_cnst_suffix(suffixes_str, arg->str);
	}

	return n_cnst_suffixes;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_commstart_valid)(uls_lex_ptr_t uls, int k)
{
	uls_commtype_ptr_t cmt, cmt0 = uls_get_array_slot_type02(uls_ptr(uls->commtypes), k);
	char *str = _uls_get_namebuf_value(cmt0->start_mark);
	int i, l_str = cmt0->len_start_mark;

	// assert: l_str > 0
	for (i=0; i<uls->n_commtypes; i++) {
		if (i == k) continue;

		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (cmt->len_start_mark == l_str && uls_streql(_uls_get_namebuf_value(cmt->start_mark), str)) {
			return 0;
		}
	}

	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_quotestart_valid)(uls_lex_ptr_t uls, int k)
{
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt, qmt0;
	int i, l_str;
	char *str;

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	qmt0 = slots_qmt[k];

	str = _uls_get_namebuf_value(qmt0->start_mark);
	l_str = qmt0->len_start_mark;
	// assert: l_str > 0

	for (i=0; i<uls->quotetypes.n; i++) {
		if (i == k) continue;

		qmt = slots_qmt[i];
		// qmt->len_start_mark > 0

		if (qmt->len_start_mark == l_str &&
			uls_streql(_uls_get_namebuf_value(qmt->start_mark), str)) {
			return 0;
		}
	}

	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__COMMENT_TYPE)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;
	int  j, k, len, n;

	_uls_tool_type_(outparam) parms1;
	uls_commtype_ptr_t cmt;

	char cmt_mark[ULS_COMM_MARK_MAXSIZ+1];
	int  l_cmt_mark;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	parms1.line = cmt_mark;
	if ((len=_uls_tool_(strlen)(wrd))==0 || len > ULS_COMM_MARK_MAXSIZ ||
		!canbe_commtype_mark(wrd, uls_ptr(parms1))) {
		_uls_log(err_log)("%s<%d>: Too short or long comment (start) mark, or not permitted chars.",
			tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", wrd);
		return -1;
	}

	l_cmt_mark = parms1.len;
	n = parms1.n;

	// Find the 'cmt' having comment start string 'wrd'.
	k = -1;
	for (j=0; j<uls->n_commtypes; j++) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), j);

		if (uls_streql(cmt_mark, _uls_get_namebuf_value(cmt->start_mark))) {
			k = j;
			break;
		}
	}

	if (k < 0) { // not found: add it as new one.
		if (uls->n_commtypes >= ULS_N_MAX_COMMTYPES) {
			_uls_log(err_log)("%s<%d>: Too many comment types", tag_nam, lno);
			return -1;
		}
		k = uls->n_commtypes++;
	}

	cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), k);

	_uls_set_namebuf_value(cmt->start_mark, cmt_mark);
	cmt->len_start_mark = l_cmt_mark;
	cmt->n_lfs = n;

	// the closing comment string of 'start_mark'
	parms1.line = cmt_mark;
	if ((len=_uls_tool_(strlen)(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))))==0 || len > ULS_COMM_MARK_MAXSIZ ||
		!canbe_commtype_mark(wrd, uls_ptr(parms1))) {
		_uls_log(err_log)("%s<%d>: Too short or long comment (end) mark, or not permitted chars.",
			tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", wrd);
		return -1;
	}

	_uls_set_namebuf_value(cmt->end_mark, cmt_mark);
	cmt->len_end_mark = parms1.len;
	cmt->n_lfs += n = parms1.n;

	if (!is_commstart_valid(uls, k)) {
		_uls_log(err_log)("%s<%d>: the comm-start-mark is collided with previous defined comment-type.", tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", _uls_get_namebuf_value(cmt->start_mark));
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(parse_quotetype__options)(char *line, uls_quotetype_ptr_t qmt)
{
	char *lptr, *wrd;
	int n=0;

	for (lptr = line; lptr != NULL; ) {
		wrd = lptr;
		if ((lptr = _uls_tool_(strchr)(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}

		if (uls_streql(wrd, "asymmetric")) {
			qmt->flags |= ULS_QSTR_ASYMMETRIC;
		} else if (uls_streql(wrd, "nothing")) {
			qmt->litstr_analyzer = _uls_ref_callback_this(nothing_lit_analyzer);
			qmt->flags |= ULS_QSTR_NOTHING;
		} else if (uls_streql(wrd, "open")) {
			qmt->litstr_analyzer = _uls_ref_callback_this(nothing_lit_analyzer);
			qmt->flags |= ULS_QSTR_OPEN;
		} else if (uls_streql(wrd, "multiline")) {
			qmt->flags |= ULS_QSTR_MULTILINE;
		} else if (uls_streql(wrd, "right_exclusive")) {
			qmt->flags |= ULS_QSTR_R_EXCLUSIVE;
		} else {
			n = -1;
			break;
		}

		++n;
	}

	return n;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(parse_quotetype__token)(char *line, uls_quotetype_ptr_t qmt, _uls_tool_ptrtype_(outparam) parms)
{
	char *lptr=line, *wrd, *tok_nam=parms->line;
	int len;

	tok_nam[0] = '\0';
	parms->n = parms->len = 0;

	wrd = lptr;
	if ((lptr = _uls_tool_(strchr)(lptr, ',')) != NULL) {
		*lptr++ = '\0';
	}

	if ((len = _uls_tool_(strlen)(wrd)) > 0) {
		if (_uls_tool_(isalpha)(wrd[0])) {
			// assert: lptr == NULL
			lptr = wrd;
		}
		if (_uls_tool_(isdigit)(wrd[0]) || wrd[0] == '-') {
			if (len > 1 || wrd[0] != '-') {
				parms->n = _uls_tool_(atoi)(wrd);
				parms->flags = 1; // tok_id specified
			}
		}
	}

	if (lptr != NULL) {
		wrd = lptr;
		if ((lptr = _uls_tool_(strchr)(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}
		parms->len = len = _uls_tool_(strlen)(wrd);
		_uls_tool_(strncpy)(tok_nam, wrd, ULS_LEXSTR_MAXSIZ);
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_parse_quotetype_opts)(_uls_tool_ptrtype_(wrd) wrdx, uls_quotetype_ptr_t qmt,
	_uls_tool_ptrtype_(outparam) parms)
{
	char *lptr, *lptr1, ch;
	int rc, proc_num, stat=0;

	parms->flags = 0; // tok_id NOT specified
	parms->n = 0; // tok_id 
	parms->len = 0;

	for (lptr = wrdx->lptr; *(lptr = _uls_tool(skip_blanks)(lptr)) != '\0'; ) {
		if (!_uls_tool_(strncmp)(lptr, "token=", 6)) {
			rc = 6;
			proc_num = 0;
		} else if (!_uls_tool_(strncmp)(lptr, "options=", 8)) {
			rc = 8;
			proc_num = 1;
		} else {
			proc_num = -1;
			break;
		}

		lptr1 = lptr + rc;
		for (lptr=lptr1; (ch=*lptr) != '\0'; lptr++) {
			if (ch == ' ' || ch == '\t') {
				*lptr = '\0';
				break;
			}
		}

		if (proc_num == 0) { // token=
			if (parse_quotetype__token(lptr1, qmt, parms) < 0) {
				stat = -1;
				break;
			}
		} else if (proc_num == 1) { // options=
			if (parse_quotetype__options(lptr1, qmt) < 0) {
				stat = -1;
				break;
			}
		} else { // NOT REACHED
			_uls_log(err_log)("%s: unknown option!", lptr1);
			stat = -1;
			break;
		}

		if (ch != '\0') *lptr++ = ch;
	}

	wrdx->lptr = lptr;
	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(set_config__QUOTE_TYPE__token)(int tok_id, char *tok_nam, int l_tok_nam,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, _uls_tool_ptrtype_(outparam) parms)
{
	int k = parms->n1;
	int lno = parms->n2;
	const char* tag_nam = parms->lptr_end;

	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt2;
	int  j;

	/* token-id */
	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (j=0; j<uls->quotetypes.n; j++) {
		if (j == k) continue;

		qmt2 = slots_qmt[j];
		if (qmt2->tok_id == tok_id) {
			_uls_log(err_log)("%s:%d: The quotation tok-id already used by other token", tag_nam, lno);
			_uls_log(err_log)("\t:tok-id:%d", tok_id);
			return -1;
		}
	}
	qmt->tok_id = tok_id;

	if (l_tok_nam > 0) {
		/* token-name */
		if (tok_nam[0] != '\0' && _uls_tool(canbe_tokname)(tok_nam) <= 0) {
			_uls_log(err_log)("%s:%d Nil-string or too long token constant!", tag_nam, lno);
			_uls_log(err_log)("\t:%s", tok_nam);
			return -1;
		}

		if (make_tokdef_for_quotetype(uls, qmt, tok_nam) < 0) {
			_uls_log(err_log)("%s:%d fail to make tokdef for the quotation type!", tag_nam, lno);
			_uls_log(err_log)("\t:%s", tok_nam);
			return -1;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__read_config__QUOTE_TYPE)(char *line,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char* tag_nam, int lno)
{
	char   *wrd, *ch_ctx = uls->ch_context;
	int  j, k, len, n_lfs, tok_id, tok_id_specified;
	_uls_tool_type_(wrd) wrdx;

	_uls_tool_type_(outparam) parms1;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt2;

	char tok_name[ULS_LEXSTR_MAXSIZ+1];
	int l_tok_name;

	char qmt_mark[ULS_QUOTE_MARK_MAXSIZ+1];
	int  l_qmt_mark;

	parms1.line = tok_name;
	parms1.lptr_end = tag_nam;
	parms1.n2 = lno;

	wrdx.lptr = line;
	if (uls_parse_quotetype_opts(uls_ptr(wrdx), qmt, uls_ptr(parms1)) < 0) {
		_uls_log(err_log)("failed to parse options for literal-string : %s", wrdx.lptr);
		return -1;
	}

	tok_id_specified = parms1.flags;
	tok_id = parms1.n;
	l_tok_name = parms1.len;

	parms1.line = qmt_mark;
	if ((len=_uls_tool_(strlen)(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
		!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms1))) {
		_uls_log(err_log)("%s<%d>: Too short or long quote (start) mark, or not permitted chars.",
			tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", wrd);
		return -1;
	}

	l_qmt_mark = parms1.len;
	n_lfs = parms1.n;
	if (!tok_id_specified) {
		tok_id = qmt_mark[0];
	}

	// Find the 'qmt' having quote-string start string 'wrd'.
	k = -1;
	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (j=0; j<uls->quotetypes.n; j++) {
		qmt2 = slots_qmt[j];
		if (uls_streql(qmt_mark, _uls_get_namebuf_value(qmt2->start_mark))) {
			uls_destroy_quotetype(qmt2);
			slots_qmt[j] = qmt;
			k = j;
			break;
		}
	}

	if (k < 0) { // not found: add it as new one.
		if (uls->quotetypes.n >= ULS_N_MAX_QUOTETYPES) {
			_uls_log(err_log)("%s<%d>: Too many quote types", tag_nam, lno);
			return -1;
		}
		k = uls->quotetypes.n++;
		// BUGFIX-213: You should have called 'uls_create_quotetype' only if k is not found.
		slots_qmt[k] = qmt;
	}

	// start mark
	_uls_set_namebuf_value(qmt->start_mark, qmt_mark);
	qmt->len_start_mark = l_qmt_mark;
	qmt->n_lfs = qmt->n_left_lfs = n_lfs;

	if (!is_quotestart_valid(uls, k)) {
		_uls_log(err_log)("%s<%d>:  the start-mark is collided with previous defined quote-type. skipping, ...",
			tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", _uls_get_namebuf_value(qmt->start_mark));
		return -1;
	}

	// the closing quote string corresponding to 'start mark'
	if (qmt->flags & ULS_QSTR_ASYMMETRIC) {
		parms1.line = qmt_mark;
		if ((len=_uls_tool_(strlen)(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
			!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms1))) {
			_uls_log(err_log)("%s<%d>: Too short or long quote (end) mark, or not permitted chars.",
				tag_nam, lno);
			_uls_log(err_log)("\t:'%s'", wrd);
			return -1;
		}

		_uls_set_namebuf_value(qmt->end_mark, qmt_mark);
		qmt->len_end_mark = parms1.len;
		qmt->n_lfs += n_lfs = parms1.n;

	} else {
		if (qmt->flags & (ULS_QSTR_OPEN | ULS_QSTR_NOTHING)) {
			qmt_mark[0] = '\0';
			len = 0;
		} else {
			len = qmt->len_start_mark;
		}

		_uls_set_namebuf_value(qmt->end_mark, qmt_mark);
		qmt->len_end_mark = len;
	}

	parms1.n1 = k;
	parms1.n2 = lno;
	parms1.lptr_end = tag_nam;

	if (set_config__QUOTE_TYPE__token(tok_id, tok_name, l_tok_name, qmt, uls, uls_ptr(parms1)) < 0) {
		return -1;
	}

	line = wrdx.lptr;

	if ((qmt->escmap=uls_parse_escmap(line, uls_ptr(uls->escstr_pool))) == nilptr) {
		_uls_log(err_log)("%s<%d>: Invalid format of escape-mapping of literal string.",
			tag_nam, lno);
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__QUOTE_TYPE)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n, stat = 0;
	uls_quotetype_ptr_t qmt;

	qmt = uls_create_quotetype();
	qmt->litstr_analyzer = _uls_ref_callback_this(dfl_lit_analyzer_escape0);

	if (__read_config__QUOTE_TYPE(line, qmt, uls, tag_nam, lno) < 0) {
		stat = -1;
		uls_destroy_quotetype(qmt);
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_FIRST_CHARS)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	char  *ch_ctx = uls->ch_context;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;
	uls_uint32 i1, i2;
	int   i, i_tmp, rc;
	_uls_tool_type_(outparam) parms1;

	wrdx.lptr = line;

	while (*(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		parms1.lptr = wrd;

		if (_uls_tool(get_range_aton)(uls_ptr(parms1)) <= 0) {
			_uls_log(err_log)("%s<%d>: improper char-range specified!", tag_nam, lno);
			_uls_log(err_log)("\t:'%s'", wrd);
			return -1;
		}

		wrd = (char *) parms1.lptr;
		i1 = parms1.x1;
		i2 = parms1.x2;

		// assert: i1, i2 >= 0, and i1 <= i2
		if (i1 < ULS_SYNTAX_TABLE_SIZE) {
			i_tmp = (int) MIN(i2, ULS_SYNTAX_TABLE_SIZE-1);
			for (i=i1; i<=i_tmp; i++) {
				if (_uls_tool_(isdigit)(i)) {
					_uls_log(err_log)("%s<%d>: digits cannot be first chars of ID.", tag_nam, lno);
					return -1;
				}
				ch_ctx[i] |= ULS_CH_IDFIRST;
			}
			i1 = i_tmp + 1;
		}

		if ((rc=add_id_charset(uls_ptr(uls->idfirst_charset), uls->n_idfirst_charsets, (uls_uch_t) i1, (uls_uch_t) i2)) < 0) {
			_uls_log(err_log)("%s<%d>: fail to add id-charset.", tag_nam, lno);
			_uls_log(err_log)("\t:range:0x%X-%X.", i1, i2);
			return -1;
		} else if (rc > 0) {
			++uls->n_idfirst_charsets;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_CHARS)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	char  *ch_ctx = uls->ch_context;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;
	uls_uint32 i1, i2;
	int   i, i_tmp, rc;
	_uls_tool_type_(outparam) parms1;

	wrdx.lptr = line;

	while (*(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		parms1.lptr = wrd;

		if (_uls_tool(get_range_aton)(uls_ptr(parms1)) <= 0) {
			_uls_log(err_log)("%s<%d>: improper char-range specified.", tag_nam, lno);
			_uls_log(err_log)("\t'%s'", wrd);
			return -1;
		}

		wrd = (char *) parms1.lptr;
		i1 = parms1.x1;
		i2 = parms1.x2;

		// assert: i1, i2 >= 0, and i1 <= i2
		if (i1 < ULS_SYNTAX_TABLE_SIZE) {
			i_tmp = (int) MIN(i2, ULS_SYNTAX_TABLE_SIZE-1);
			for (i=i1; i<=i_tmp; i++) {
				ch_ctx[i] |= ULS_CH_ID;
			}
			i1 = i_tmp + 1;
		}

		if ((rc=add_id_charset(uls_ptr(uls->id_charset), uls->n_id_charsets, (uls_uch_t) i1, (uls_uch_t) i2)) < 0) {
			_uls_log(err_log)("%s<%d>: fail to add id-charset.", tag_nam, lno);
			_uls_log(err_log)("\t:range:0x%X-%X.", i1, i2);
			return -1;
		} else if (rc > 0) {
			++uls->n_id_charsets;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__RENAME)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	uls_tokdef_vx_ptr_t e_vx;
	char  *wrd, *wrd2;

	wrdx.lptr = line;

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' || *(wrd2 = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' ||
		_uls_tool_(strlen)(wrd2) > ULS_LEXSTR_MAXSIZ) {
		_uls_log(err_log)("%s<%d>: Invalid 'RENAME' line!", tag_nam, lno);
		return -1;
	}

	if ((e_vx=__find_rsvd_tokdef_by_name(uls, wrd)) == nilptr) {
		_uls_log(err_log)("%s<%d>: Invalid 'RENAME'!", tag_nam, lno);
		_uls_log(err_log)("\t:'%s'", wrd);
		return -1;
	}

	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, wrd2);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__NOT_CHAR_TOK)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	char  *ch_ctx = uls->ch_context;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;

	wrdx.lptr = line;

	while (*(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		ch_ctx[wrd[0]] &= ~ULS_CH_1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__CASE_SENSITIVE)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (uls_streql(wrd, "false")) {
		uls->flags |= ULS_FL_CASE_INSENSITIVE;
	} else if (uls_streql(wrd, "true")) {
		uls->flags &= ~ULS_FL_CASE_INSENSITIVE;
	} else {
		_uls_log(err_log)("%s<%d>: Incorrect value of CASE_SENSITIVE. Specify it by true/false.",
			tag_nam, lno);
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__DOMAIN)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	int linelen;

	linelen = _uls_tool(str_trim_end)(line, -1);
	if (linelen <= 0 || !uls_streql(line, ULC_REPO_DOMAIN)) {
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__CHAR_TOK)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	char  *ch_ctx = uls->ch_context;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;

	wrdx.lptr = line;

	while (*(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		ch_ctx[wrd[0]] |= ULS_CH_1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_MAX_LENGTH)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;
	int   k;

	wrdx.lptr = line;

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' || (k = _uls_tool_(atoi)(wrd)) <= 0) {
		return -1;
	}

	uls->id_max_bytes = uls->id_max_uchars = k;

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0' && (k = _uls_tool_(atoi)(wrd)) > 0) {
		uls->id_max_uchars = k;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(numpfx_by_length_dsc)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_number_prefix_ptr_t e1 = (const uls_number_prefix_ptr_t) a;
	const uls_number_prefix_ptr_t e2 = (const uls_number_prefix_ptr_t) b;
	int stat;

	if (e1->l_prefix < e2->l_prefix) stat = 1;
	else if (e1->l_prefix > e2->l_prefix) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__NUMBER_PREFIXES)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	int stat = 0;

	uls_number_prefix_ptr_t numpfx;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd, *lptr;
	int k, r, len;

	wrdx.lptr = line;
	for (k=0; *(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0'; k++) {
		if (k >= ULS_N_MAX_NUMBER_PREFIXES) {
			_uls_log(err_log)("NUMBER_PREFIXES: Too many NumberPrefixes %d", ULS_N_MAX_NUMBER_PREFIXES);
			stat = -4; break;
		}

		numpfx = uls_get_array_slot_type01(uls_ptr(uls->numcnst_prefixes), k);

		if (*wrd != '0' || (lptr= _uls_tool_(strchr)(wrd, ':')) == NULL) {
			_uls_log(err_log)("NUMBER_PREFIXES: Incorrect format for NumberPrefix '%s'.", wrd);
			_uls_log(err_log)("Number prefix must be 0-prefixed.");
			return -1;
		}

		if ((len = (int) (lptr - wrd)) > ULS_MAXLEN_NUMBER_PREFIX) {
			_uls_log(err_log)("NUMBER_PREFIXES: Too long NumberPrefix, %d/%d", len, ULS_MAXLEN_NUMBER_PREFIX);
			return -2;
		}

		*lptr++ = '\0';
		if ((r = _uls_tool_(atoi)(lptr)) < 2 || r > 36) {
			_uls_log(err_log)("NUMBER_PREFIXES: assertion failed, 2 <= radix <= 36!", wrd);
			return -3;
		}

		_uls_init_namebuf(numpfx->prefix, ULS_MAXLEN_NUMBER_PREFIX);
		_uls_tool_(strncpy)(_uls_get_namebuf_value(numpfx->prefix), wrd, len);
		numpfx->l_prefix = len;
		numpfx->radix = r;
	}

#ifdef ULS_CLASSIFY_SOURCE
	_uls_quicksort_vptr(uls_array_slots_type01(uls_ptr(uls->numcnst_prefixes)), k, numpfx_by_length_dsc);
#else
	_uls_tool_(quick_sort)(uls_array_slots_type01(uls_ptr(uls->numcnst_prefixes)), k, sizeof(uls_number_prefix_t), _uls_ref_callback_this(numpfx_by_length_dsc));
#endif
	uls->n_numcnst_prefixes =  k;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__DECIMAL_SEPARATOR)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;

	_uls_tool_type_(wrd) wrdx;
	char  *wrd;
	uls_uch_t uch = ULS_DECIMAL_SEPARATOR_DFL;

	wrdx.lptr = line;
	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		uch = *wrd;
		if (!_uls_tool_(isgraph)(uch) || _uls_tool_(isalnum)(uch) || uch == '-' || uch == '.') {
			_uls_log(err_log)("%s<%d>: Invalid decimal separator!", tag_nam, lno);
			return -1;
		}
	}

	uls->numcnst_separator = uch;
	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__NUMBER_SUFFIXES)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	_uls_tool_type_(wrd) wrdx;
	_uls_tool_type_(arglst) wrdlst;

	wrdx.lptr = line;

	_uls_tool_(init_arglst)(uls_ptr(wrdlst), ULC_COLUMNS_MAXSIZ);

	__uls_tool_(explode_str)(uls_ptr(wrdx), ' ', 0, uls_ptr(wrdlst));
	check_validity_of_cnst_suffix(uls_ptr(wrdlst), uls->numcnst_suffixes);

	_uls_tool_(deinit_arglst)(uls_ptr(wrdlst));

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__LINEFEED_GUARD)(char *line, _uls_tool_ptrtype_(cmd) cmd)
{
	_uls_tool_ptrtype_cast_(outparam, parms, cmd->user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;
	_uls_tool_type_(wrd) wrdx;
	char  *wrd;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (uls_streql(wrd, "true")) {
		uls->xcontext.flags |= ULS_XCTX_FL_LINEFEED_GUARD;
	} else if (uls_streql(wrd, "false")) {
		uls->xcontext.flags &= ~ULS_XCTX_FL_LINEFEED_GUARD;
	} else {
		_uls_log(err_log)("%s<%d>: Incorrect value of LINEFEED_GUARD. Specify it by true/false.",
			tag_nam, lno);
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config_var)(const char* tag_nam, int lno, uls_lex_ptr_t uls,
	char* lptr, ulc_header_ptr_t hdr)
{
	char  *wrd;
	int len, rc, stat = 1;

	_uls_tool_type_(outparam) parms1;
	_uls_tool_type_(wrd) wrdx;

	parms1.n = lno;
	parms1.lptr_end = tag_nam;
	parms1.data = uls;
	parms1.proc = hdr;

	wrdx.lptr = lptr;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
	lptr = wrdx.lptr;

	if ((len = _uls_tool_(strlen)(wrd)) <= 0 || wrd[len-1] != ':') {
		return 0;
	}
	wrd[--len] = '\0';

	rc = _uls_tool_(cmd_run)(uls_array_slots_type01(uls_ptr(ulc_cmd_list)), ULC_N_LEXATTRS, wrd, lptr, uls_ptr(parms1));
	if (rc < 0) {
		_uls_log(err_log)("%s<%d>: unknown attribute in ULS-spec", tag_nam, lno);
		_uls_log(err_log)("\tattribute:'%s'", wrd);
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ulf_filepath)(const char* dirpath, int len_dirpath,
	const char *specname, char* pathbuff)
{
	int len;

	if (len_dirpath > 0) {
		_uls_tool_(memcopy)(pathbuff, dirpath, len_dirpath);
		len = len_dirpath;
		pathbuff[len++] = ULS_FILEPATH_DELIM;
	} else {
		len = 0;
	}

	len += _uls_log_(snprintf)(pathbuff+len, ULS_FILEPATH_MAX+1-len, "%s.ulf", specname);
	return len;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(_list_searchpath)(const char *filename,
	_uls_tool_ptrtype_(arglst) title, _uls_tool_ptrtype_(arglst) searchpath, int n)
{
	_uls_decl_parray_slots_init_tool_(al_title, argstr, uls_ptr(title->args));
	_uls_decl_parray_slots_init_tool_(al_searchpath, argstr, uls_ptr(searchpath->args));
	_uls_tool_ptrtype_(argstr) arg;

	char fpath_buff[ULS_FILEPATH_MAX+1];
	const char  *lptr, *lptr0, *fptr;
	int   len_fptr;
	int   i, len, rc;

	for (i=0; i<n; i++) {
		_uls_log(err_log)("[%s]", al_title[i]->str);

		if ((arg = al_searchpath[i]) == nilptr) {
			continue;
		}

		for (lptr0=arg->str; lptr0 != NULL; ) {
			if ((lptr = _uls_tool_(strchr)(lptr0, ULS_DIRLIST_DELIM)) != NULL) {
				len_fptr = (int) (lptr - lptr0);
				fptr = lptr0;
				lptr0 = ++lptr;
			} else {
				len_fptr = _uls_tool_(strlen)(lptr0);
				fptr = lptr0;
				lptr0 = NULL;
			}

			if (len_fptr == 0) continue;

			len = _uls_tool_(strncpy)(fpath_buff, fptr, len_fptr);
			fpath_buff[len++] = ULS_FILEPATH_DELIM;
			_uls_tool_(strcpy)(fpath_buff+len, filename);

			rc = _uls_tool_(dirent_exist)(fpath_buff);
			fpath_buff[len_fptr] = '\0';
			if (rc == ST_MODE_REG) {
				_uls_log(err_log)("\t%s ---> FOUND!", fpath_buff);
			} else {
				_uls_log(err_log)("\t%s", fpath_buff);
			}
		}
	}
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(set_chrmap)(uls_lex_ptr_t uls, int tok_id, int keyw_type, char ch_kwd)
{
	char *ch_ctx = uls->ch_context;

	switch (keyw_type) {
	case ULS_KEYW_TYPE_TWOPLUS:
		ch_ctx[ch_kwd] |= ULS_CH_2PLUS;
		if (uls->flags & ULS_FL_CASE_INSENSITIVE) {
			ch_ctx[_uls_tool_(toupper)(ch_kwd)] |= ULS_CH_2PLUS;
		}
		/* pass through */

	case ULS_KEYW_TYPE_IDSTR:
	case ULS_KEYW_TYPE_USER:
		if (tok_id >= 0 && tok_id < ULS_SYNTAX_TABLE_SIZE) {
			if (ch_ctx[tok_id] & ULS_CH_1) {
//				_uls_log(err_log)("%s<%d>: Overriding char-token,", tag_nam, lno);
//				_uls_log(err_log)("\t%d('%c') ...", tok_id, tok_id);
				ch_ctx[tok_id] &= ~ULS_CH_1;
			}
		}
		break;

	case ULS_KEYW_TYPE_1CHAR:
		ch_ctx[ch_kwd] |= ULS_CH_1;
		if (ch_kwd == '\n') uls->flags |= ULS_FL_LF_CHAR;
		else if (ch_kwd == '\t') uls->flags |= ULS_FL_TAB_CHAR;
		break;

	default:
		break;
	}
}

void
ULS_QUALIFIED_METHOD(uls_init_uch_range)(uls_uch_range_ptr_t uch_range)
{
	uch_range->x1 = 0;
	uch_range->x2 = 0;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_uch_range)(uls_uch_range_ptr_t uch_range)
{
}

int
ULS_QUALIFIED_METHOD(is_reserved_tok)(uls_lex_ptr_t uls, char* name)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		if (uls_streql(_uls_get_namebuf_value(e_vx->name), name)) {
			return i;
		}
	}

	return -1;
}

int
ULS_QUALIFIED_METHOD(check_rsvd_toks)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t e0_vx, e_vx;
	int i, j, stat=0;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e0_vx = slots_rsv[i];

		for (j=i+1; j<N_RESERVED_TOKS; j++) {
			e_vx = slots_rsv[j];

			if (uls_streql(_uls_get_namebuf_value(e0_vx->name), _uls_get_namebuf_value(e_vx->name))) {
				_uls_log(err_log)("The name '%s' is already used by '%s'.",
					_uls_get_namebuf_value(e_vx->name), _uls_get_namebuf_value(e0_vx->name));
				stat = -1;
				break;
			}

			if (e0_vx->tok_id == e_vx->tok_id) {
				_uls_log(err_log)("The token %s:%d is already used",
					_uls_get_namebuf_value(e0_vx->name), e0_vx->tok_id);
				_uls_log(err_log)("\tby %s:%d.", _uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
				stat = -1;
				break;
			}
		}
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_is_char_idfirst)(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch)
{
	// assert: ptr_uch != NULL
	// ret-val : -4,-3,-2, -1, 1, 2, 3, 4
	int n_bytes, i;
	uls_uch_t uch;
	_uls_ptrtype_this_(none,uch_range) ran;

	if ((n_bytes = _uls_tool_(decode_utf8)(lptr, -1, ptr_uch)) > 1) {
		// UTF-8: n_bytes == 1, 2, 3, 4
		uch = *ptr_uch;

		for (i=0; i<uls->n_idfirst_charsets; i++) {
			ran = uls_get_array_slot_type02(uls_ptr(uls->idfirst_charset), i);

			if (uch >= ran->x1 && uch <= ran->x2)
				return n_bytes;
		}

		if (is_utf_id(uch) > 0) {
			return n_bytes;
		}

	} else if (uls->ch_context[*lptr] & ULS_CH_IDFIRST) {
		return 1;
	}

	return -n_bytes;
}

int
ULS_QUALIFIED_METHOD(uls_is_char_id)(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch)
{
	// assert: ptr_uch != NULL
	// ret-val : -4,-3,-2, -1, 1, 2, 3, 4
	int n_bytes, i;
	uls_uch_t uch;
	_uls_ptrtype_this_(none,uch_range) ran;

	if ((n_bytes = _uls_tool_(decode_utf8)(lptr, -1, ptr_uch)) > 1) {
		// UTF-8: n_bytes == 1, 2, 3, 4
		uch = *ptr_uch;

		for (i=0; i<uls->n_id_charsets; i++) {
			ran = uls_get_array_slot_type02(uls_ptr(uls->id_charset), i);

			if (uch >= ran->x1 && uch <= ran->x2)
				return n_bytes;
		}

		if (is_utf_id(uch) > 0) {
			return n_bytes;
		}

	} else if (uls->ch_context[*lptr] & ULS_CH_ID) {
		return 1;
	}

	return -n_bytes;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_tokdef_vx)(uls_lex_ptr_t uls, int t)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_t e0_vx;
	uls_tokdef_vx_ptr_t e_vx;

	e0_vx.tok_id = t;

	if ((e_vx = (uls_tokdef_vx_ptr_t) _uls_tool_(bi_search_vptr)(uls_ptr(e0_vx),
		(_uls_type_array(uls_voidptr_t)) slots_vx, uls->tokdef_vx_array.n,
		_uls_ref_callback_this(srch_vx_by_id))) == nilptr) {
		if (t >= 0) {
			e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), t, nilptr);
		}
	}

	return e_vx;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_tokdef_vx_force)(uls_lex_ptr_t uls, int t)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_context_ptr_t  ctx = uls->xcontext.context;
	uls_tokdef_vx_t e0_vx;
	uls_tokdef_vx_ptr_t e_vx;

	e0_vx.tok_id = t;

	if ((e_vx = (uls_tokdef_vx_ptr_t) _uls_tool_(bi_search_vptr)(uls_ptr(e0_vx),
		(_uls_type_array(uls_voidptr_t)) slots_vx, uls->tokdef_vx_array.n,
		_uls_ref_callback_this(srch_vx_by_id))) == nilptr) {
		if (t >= 0 && (e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), t, nilptr)) == nilptr) {
			if (t < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[t] & ULS_CH_1)) {
				e_vx = ctx->anonymous_uchar_vx;
				e_vx->tok_id = t;
			}
		}
	}

	return e_vx;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(set_extra_tokdef_vx)(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
{
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) == nilptr) {
		if (tok_id >= 0 && tok_id < ULS_SYNTAX_TABLE_SIZE &&
			(uls->ch_context[tok_id] & ULS_CH_1)) {
			e_vx = uls_insert_1char_tokdef_uch(uls_ptr(uls->onechar_table), tok_id);
		} else {
			return nilptr;
		}
	}

	e_vx->extra_tokdef = extra_tokdef;
	return e_vx;
}

char*
ULS_QUALIFIED_METHOD(is_cnst_suffix_contained)(char* cstr_pool, const char* str, int l_str, _uls_tool_ptrtype_(outparam) parms)
{
	char *ptr, *ret_ptr = NULL;
	int l;

	if (l_str < 0) {
		if (str != NULL) {
			for (l_str=0; l_str<ULS_CNST_SUFFIX_MAXSIZ; l_str++) {
				if (str[l_str] == '\0') break;
			}
		} else {
			l_str = 0;
		}
	} else if (l_str > ULS_CNST_SUFFIX_MAXSIZ) {
		l_str = ULS_CNST_SUFFIX_MAXSIZ;
	}

	if (l_str == 0) {
		for  (ptr=cstr_pool; *ptr!='\0'; ptr+=l+1) {
			l = _uls_tool_(strlen)(ptr);
		}
		if (parms != nilptr) parms->line = ptr;
		return NULL;
	}

	for (ptr=cstr_pool; *ptr!='\0'; ptr+=l+1) {
		if ((l=_uls_tool_(strlen)(ptr)) <= l_str && _uls_tool_(memcmp)(ptr, str, l) == 0) {
			ret_ptr = ptr;
			break;
		}
	}

	if (parms != nilptr) parms->line = ptr;
	return ret_ptr; // the ptr corresponding to 'str'
}

FILE*
ULS_QUALIFIED_METHOD(ulc_search_for_fp)(int typ_fpath, const char* fpath, _uls_tool_ptrtype_(outparam) parms)
{
	_uls_tool_type_(arglst) title_list;
	_uls_tool_type_(arglst) searchpath_list;
	_uls_decl_parray_slots_tool_(al_searchpath, argstr);
	_uls_tool_ptrtype_(argstr) arg;

	FILE *fp, *fp_ret=NULL;
	char *dpath;
	int i, n;

	_uls_tool_(init_arglst)(uls_ptr(title_list), N_ULC_SEARCH_PATHS);
	_uls_tool_(init_arglst)(uls_ptr(searchpath_list), N_ULC_SEARCH_PATHS);

	if (typ_fpath == ULS_NAME_SPECNAME) {
		n = ulc_get_searchpath_by_specname(uls_ptr(title_list), uls_ptr(searchpath_list));
	} else {
		n = ulc_get_searchpath_by_specpath(is_absolute_path(fpath), uls_ptr(title_list), uls_ptr(searchpath_list));
	}

	al_searchpath = uls_parray_slots(uls_ptr(searchpath_list.args));

	for (i=0; ; i++) {
		if (i >= n) {
//			_uls_log(err_log)("%s: ulc not found!", fpath);
			_list_searchpath(fpath, uls_ptr(title_list), uls_ptr(searchpath_list), searchpath_list.args.n);
			break;
		}

		if ((arg = al_searchpath[i]) == nilptr) {
			dpath = NULL;
		} else {
			dpath = arg->str;
		}

		if ((fp=uls_get_spec_fp(dpath, fpath, parms)) != NULL) {
			if (parms != nilptr) {
				parms->line = _uls_tool_(strdup)(parms->lptr, parms->len);
			}
			fp_ret = fp;
			break;
		}
	}

	_uls_tool_(deinit_arglst)(uls_ptr(title_list));
	_uls_tool_(deinit_arglst)(uls_ptr(searchpath_list));

	return fp_ret;
}

FILE*
ULS_QUALIFIED_METHOD(uls_get_ulc_path)(int typ_fpath, const char* fpath, int len_dpath,
	const char* specname, int len_specname, _uls_tool_ptrtype_(outparam) parms)
{
	// OUT-PARAM: specname!=NULL, ptr_fp_ulf
	// assert: sizeof(specname) > ULC_LONGNAME_MAXSIZ
	char fpath_buff[ULC_LONGNAME_MAXSIZ+5], ulf_filepath[ULS_FILEPATH_MAX+1], ch;
	const char *filepath, *dirpath;
	int  len_dirpath, k;
	FILE  *fp_ulc;
	_uls_tool_type_(outparam) parms1;

	if (typ_fpath == ULS_NAME_SPECNAME) {
		for (k=0; k<len_dpath; k++) {
			if ((ch = fpath[k]) == ULS_ULCNAME_DELIM) {
				fpath_buff[k] = ULS_FILEPATH_DELIM;
			} else {
				fpath_buff[k] = ch;
			}
		}

		if (k > 0) fpath_buff[k++] = ULS_FILEPATH_DELIM;
		_uls_tool_(strcpy)(fpath_buff+k, specname);
		k += len_specname;
		_uls_tool_(strcpy)(fpath_buff+k, ".ulc");

		filepath = fpath_buff;

	} else {
		// assert: typ_fpath == ULS_NAME_FILEPATH_{ULC,ULD}
		// fpath is a file path.
		filepath = fpath;
	}

	if ((fp_ulc = ulc_search_for_fp(typ_fpath, filepath, uls_ptr(parms1))) != NULL) {
		dirpath = parms1.line;
		len_dirpath = parms1.len;

		if (parms != nilptr) {
			get_ulf_filepath(dirpath, len_dirpath, specname, ulf_filepath);
			parms->native_data = _uls_tool_(fp_open)(ulf_filepath, ULS_FIO_READ);
		}
		uls_mfree(dirpath);
	}

	return fp_ulc;
}

int
ULS_QUALIFIED_METHOD(ulc_prepend_searchpath_exeloc)(const char* argv0)
{
	char fpath_buf[ULS_FILEPATH_MAX+1];
	int fpath_len;

	if ((fpath_len=uls_get_exeloc_dir(argv0, fpath_buf)) < 0) {
		_uls_log(err_log)("can't find the location of program file");
		return -1;
	}

	if (ulc_add_searchpath(fpath_buf, 1) < 0) {
		_uls_log(err_log)("can't update the system value for ulc-search-path");
		return -1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(ulc_get_searchpath_by_specname)(
	_uls_tool_ptrtype_(arglst) nameof_searchpath, _uls_tool_ptrtype_(arglst) searchpath_list)
{
	_uls_decl_parray_slots_init_tool_(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	_uls_decl_parray_slots_tool_(al_args, argstr);
	_uls_tool_ptrtype_(argstr) arg;

	const char* title[N_ULC_SEARCH_PATHS];
	const char *cptr;
	int  n = 0, i;

	title[n] = "ULS_SPEC_PATH";
	if ((cptr=getenv(title[n])) != NULL) {
		al_searchpath[n] = arg = _uls_tool_(create_argstr)();
		_uls_tool_(copy_argstr)(arg, cptr, -1);
		++n;
	}

	title[n] = "ULS_ULCS";
	al_searchpath[n] = arg = _uls_tool_(create_argstr)();
	_uls_tool_(copy_argstr)(arg, _uls_sysinfo_(ulcs_dir), -1);
	++n;

	title[n] = "ULS_OS_SHARE_DFLDIR";
	al_searchpath[n] = arg = _uls_tool_(create_argstr)();
	_uls_tool_(copy_argstr)(arg, ULS_OS_SHARE_DFLDIR, -1);
	++n;

	searchpath_list->args.n = n;

	// assert: n < N_ULC_SEARCH_PATHS
	if (nameof_searchpath != nilptr) {
		al_args = uls_parray_slots(uls_ptr(nameof_searchpath->args));
		for (i=0; i<n; i++) {
			al_args[i] = arg = _uls_tool_(create_argstr)();
			_uls_tool_(copy_argstr)(arg, title[i], -1);
		}
		nameof_searchpath->args.n = n;
	}

	return n;
}

int
ULS_QUALIFIED_METHOD(ulc_get_searchpath_by_specpath)(int is_abspath,
	_uls_tool_ptrtype_(arglst) nameof_searchpath, _uls_tool_ptrtype_(arglst) searchpath_list)
{
	_uls_decl_parray_slots_init_tool_(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	_uls_decl_parray_slots_tool_(al_args, argstr);
	_uls_tool_ptrtype_(argstr) arg;

	const char *cptr, *title[N_ULC_SEARCH_PATHS];
	char *exeloc;
	int  n = 0, i;

	// ULS_NAME_FILEPATH_ULC | ULS_NAME_FILEPATH_ULD
	if (is_abspath) {
		title[n] = "ABS";
		al_searchpath[n] = nilptr;
		++n;

	} else {
		title[n] = "ULC_PATH";
		if ((cptr=getenv(title[n])) != NULL) {
			al_searchpath[n] = arg = _uls_tool_(create_argstr)();
			_uls_tool_(copy_argstr)(arg, cptr, -1);
			++n;
		}

		title[n] = "PWD";
		al_searchpath[n] = nilptr;
		++n;

		title[n] = "EXELOC";
		exeloc = uls_malloc_buffer(ULS_FILEPATH_MAX+1);
		if (uls_get_exeloc_dir(NULL, exeloc) >= 0) {
			al_searchpath[n] = arg = _uls_tool_(create_argstr)();
			_uls_tool_(copy_argstr)(arg, exeloc, -1);
			++n;
		}
		uls_mfree(exeloc);

		if (_uls_sysinfo_(ULC_SEARCH_PATH) != NULL) {
			title[n] = "SEARCH_PATH";
			al_searchpath[n] = arg = _uls_tool_(create_argstr)();
			_uls_tool_(copy_argstr)(arg, _uls_sysinfo_(ULC_SEARCH_PATH), -1);
			++n;
		}

		title[n] = "ULS_OS_SHARE_DFLDIR";
		al_searchpath[n] = arg = _uls_tool_(create_argstr)();
		_uls_tool_(copy_argstr)(arg, ULS_OS_SHARE_DFLDIR, -1);
		++n;
	}

	// assert: n < N_ULC_SEARCH_PATHS
	if (nameof_searchpath != nilptr) {
		al_args = uls_parray_slots(uls_ptr(nameof_searchpath->args));
		for (i=0; i<n; i++) {
			al_args[i] = arg = _uls_tool_(create_argstr)();
			_uls_tool_(copy_argstr)(arg, title[i], -1);
		}
		nameof_searchpath->args.n = n;
	}

	return n;
}

ULS_QUALIFIED_RETTYP(fp_list_ptr_t)
ULS_QUALIFIED_METHOD(ulc_find_fp_list)(fp_list_ptr_t fp_stack_top, const char *ulc_name)
{
	fp_list_ptr_t fp_lst;

	for (fp_lst = fp_stack_top; fp_lst != nilptr; fp_lst=fp_lst->prev) {
		if (uls_streql(fp_lst->tagstr, ulc_name)) {
			return fp_lst;
		}
	}

	return nilptr;
}

ULS_QUALIFIED_RETTYP(fp_list_ptr_t)
ULS_QUALIFIED_METHOD(ulc_fp_push)(fp_list_ptr_t fp_lst, FILE *fp, const char* str)
{
	fp_list_ptr_t e;
	int len;

	if (str == NULL) {
		str = "";
		len = 0;
	} else {
		len = _uls_tool_(strlen)(str);
	}

	e = uls_alloc_object(fp_list_t);
	e->tagstr = _uls_tool_(strdup)(str, len);
	e->linenum = 0;

	e->fp = fp;
	e->prev = fp_lst;

	return e;
}

FILE*
ULS_QUALIFIED_METHOD(ulc_fp_peek)(fp_list_ptr_t fp_lst, _uls_tool_ptrtype_(outparam) parms)
{
	// assert: fp_lst != nilptr
	FILE *fp;

	fp = fp_lst->fp;
	uls_assert(fp != NULL);
	if (parms != nilptr) {
		parms->line = fp_lst->tagstr;
		parms->n = fp_lst->linenum;
	}

	return fp;
}

FILE*
ULS_QUALIFIED_METHOD(ulc_fp_get)(_uls_tool_ptrtype_(outparam) parms, int do_pop)
{
	fp_list_ptr_t fp_lst = (fp_list_ptr_t ) parms->data;
	FILE *fp;

	if (fp_lst != nilptr) {
		fp = fp_lst->fp;
		uls_assert(fp != NULL);

		parms->line = fp_lst->tagstr;
		parms->n = fp_lst->linenum;
		parms->data = fp_lst->prev;

		if (do_pop) {
			uls_mfree(fp_lst->tagstr);
			uls_dealloc_object(fp_lst);
		}
	} else {
		fp = NULL;
	}

	return fp;
}

void
ULS_QUALIFIED_METHOD(release_ulc_fp_stack)(fp_list_ptr_t fp_lst)
{
	FILE *fp;
	_uls_tool_type_(outparam) parms;

	while (fp_lst != nilptr) {
		parms.data = fp_lst;
		fp = ulc_fp_get(uls_ptr(parms), 1);
		fp_lst = (fp_list_ptr_t) parms.data;
		_uls_tool_(fp_close)(fp);
	}
}

void
ULS_QUALIFIED_METHOD(init_reserved_toks)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_decl_parray_slots(slots_rsv, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	uls_resize_parray(uls_ptr(uls->tokdef_vx_rsvd), tokdef_vx, N_RESERVED_TOKS);
	slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = uls_alloc_object(uls_tokdef_vx_t);
		slots_rsv[i] = e_vx;
	}

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		__init_tokdef_vx(e_vx);
		e_vx->flags = ULS_VX_RSVD;
		slots_vx[i] = e_vx;
	}

	uls->tokdef_vx_array.n = N_RESERVED_TOKS;

	/* LINENUM */
	e_vx = slots_rsv[LINENUM_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "LINENUM");
	e_vx->tok_id = uls->xcontext.toknum_LINENUM;

	/* NONE */
	e_vx = slots_rsv[NONE_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "NONE");
	e_vx->tok_id = uls->xcontext.toknum_NONE;

	/* ERR */
	e_vx = slots_rsv[ERR_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "ERR");
	e_vx->tok_id = uls->xcontext.toknum_ERR;

	/* EOI */
	e_vx = slots_rsv[EOI_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "EOI");
	e_vx->tok_id = uls->xcontext.toknum_EOI;

	/* EOF */
	e_vx = slots_rsv[EOF_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "EOF");
	e_vx->tok_id = uls->xcontext.toknum_EOF;

	/* ID */
	e_vx = slots_rsv[ID_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "ID");
	e_vx->tok_id = uls->xcontext.toknum_ID;

	/* NUMBER */
	e_vx = slots_rsv[NUM_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "NUMBER");
	e_vx->tok_id = uls->xcontext.toknum_NUMBER;

	/* TEMPLATE */
	e_vx = slots_rsv[TMPL_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "TMPL");
	e_vx->tok_id = uls->xcontext.toknum_TMPL;

	/* LINK */
	e_vx = slots_rsv[LINK_TOK_IDX];
	e_vx->l_name = _uls_set_namebuf_value(e_vx->name, "LINK");
	e_vx->tok_id = uls->xcontext.toknum_LINK;

	for (i=0; i < N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		e_vx->extra_tokdef = nilptr;
		e_vx->base = nilptr;
		e_vx->tokdef_names = nilptr;
	}
}

int
ULS_QUALIFIED_METHOD(classify_char_group)(uls_lex_ptr_t uls, ulc_header_ptr_t uls_conf)
{
	char *ch_ctx = uls->ch_context;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	int  ch, i; // ch should not be char of type.

	/* '\0' : An exceptional char, which is used only space char. */
	if (ch_ctx['\0'] != 0) {
		_uls_log(err_log)("The null char can't be used as other usage, 0x%X", ch_ctx['\0']);
		ch_ctx['\0'] = 0;
	}

	/* ' ' : ~VISIBLE */
	if (ch_ctx[' '] != 0) {
		_uls_log(err_log)("The space char can't be used as other usage, 0x%X", ch_ctx[' ']);
		ch_ctx[' '] = 0;
	}

	if (uls->xcontext.toknum_EOI >= 0 && uls->xcontext.toknum_EOI < ULS_SYNTAX_TABLE_SIZE)
		ch_ctx[uls->xcontext.toknum_EOI] &= ~ULS_CH_1;

	if (uls->xcontext.toknum_EOF >= 0 && uls->xcontext.toknum_EOF < ULS_SYNTAX_TABLE_SIZE)
		ch_ctx[uls->xcontext.toknum_EOF] &= ~ULS_CH_1;

	if (uls->xcontext.toknum_ID >= 0 && uls->xcontext.toknum_ID < ULS_SYNTAX_TABLE_SIZE)
		ch_ctx[uls->xcontext.toknum_ID] &= ~ULS_CH_1;

	if (uls->xcontext.toknum_NUMBER >= 0 && uls->xcontext.toknum_NUMBER < ULS_SYNTAX_TABLE_SIZE)
		ch_ctx[uls->xcontext.toknum_NUMBER] &= ~ULS_CH_1;

	if (uls->xcontext.toknum_ERR >= 0 && uls->xcontext.toknum_ERR < ULS_SYNTAX_TABLE_SIZE)
		ch_ctx[uls->xcontext.toknum_ERR] &= ~ULS_CH_1;

	for (i=0; i<uls->n_commtypes; i++) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
		if (is_str_contained_in_quotetypes(uls, _uls_get_namebuf_value(cmt->start_mark))) {
			_uls_log(err_log)("comment-type '%s' is not proper as it contained in one of the quote-types[]",
				_uls_get_namebuf_value(cmt->start_mark));
			return -1;
		}
	}

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		if (is_str_contained_in_commtypes(uls, _uls_get_namebuf_value(qmt->start_mark))) {
			_uls_log(err_log)("quote-type '%s' is not proper as it contained in one of the comm-types[]",
				_uls_get_namebuf_value(qmt->start_mark));
			return -1;
		}
	}

	for (i=0; i<uls->n_commtypes; i++) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
		ch = _uls_get_namebuf_value(cmt->start_mark)[0];
		ch_ctx[ch] |= ULS_CH_COMM;
	}

	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		ch = _uls_get_namebuf_value(qmt->start_mark)[0];
		ch_ctx[ch] |= ULS_CH_QUOTE;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(classify_tok_group)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_tokdef_vx_ptr_t e_vx;
	uls_quotetype_ptr_t qmt;
	int  i;

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	_uls_quicksort_vptr(slots_vx, uls->tokdef_vx_array.n, comp_vx_by_tokid);

	// sync uls->toknum_* with the values of ulc-file.
	e_vx = slots_rsv[LINENUM_TOK_IDX];
	uls->xcontext.toknum_LINENUM = e_vx->tok_id;

	e_vx = slots_rsv[NONE_TOK_IDX];
	uls->xcontext.toknum_NONE = e_vx->tok_id;

	e_vx = slots_rsv[ERR_TOK_IDX];
	uls->xcontext.toknum_ERR = e_vx->tok_id;

	e_vx = slots_rsv[EOI_TOK_IDX];
	uls->xcontext.toknum_EOI = e_vx->tok_id;

	e_vx = slots_rsv[EOF_TOK_IDX];
	uls->xcontext.toknum_EOF = e_vx->tok_id;

	e_vx = slots_rsv[ID_TOK_IDX];
	uls->xcontext.toknum_ID = e_vx->tok_id;

	e_vx = slots_rsv[NUM_TOK_IDX];
	uls->xcontext.toknum_NUMBER = e_vx->tok_id;

	e_vx = slots_rsv[TMPL_TOK_IDX];
	uls->xcontext.toknum_TMPL = e_vx->tok_id;

	e_vx = slots_rsv[LINK_TOK_IDX];
	uls->xcontext.toknum_LINK= e_vx->tok_id;

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];

		if (qmt->flags & ULS_QSTR_NOTHING) {
			qmt->tok_id = uls->xcontext.toknum_NONE;
		}

		e_vx = uls_find_tokdef_vx(uls, qmt->tok_id);
		qmt->tokdef_vx = e_vx;
	}

	return 0;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(rearrange_tokname_of_quotetypes)(uls_lex_ptr_t uls, int n_keys_twoplus, _uls_tool_ptrtype_(outparam) parms)
{
	uls_kwtable_twoplus_ptr_t twotbl = uls_ptr(uls->twoplus_table);
	uls_decl_parray_slots(slots_tm, tokdef_vx);
	uls_decl_parray_slots(slots_tok, tokdef);
	uls_tokdef_ptr_t idtok_list;
	uls_tokdef_vx_ptr_t  e_vx;
	uls_tokdef_ptr_t e;
	int i, n1, n2;

	uls_init_parray(uls_ptr(twotbl->twoplus_mempool), tokdef_vx, n_keys_twoplus);

	n1 = n2 = 0;
	idtok_list = nilptr;

	slots_tok = uls_parray_slots(uls_ptr(uls->tokdef_array));
	slots_tm = uls_parray_slots(uls_ptr(twotbl->twoplus_mempool));

	for (i=0; i < uls->tokdef_array.n; i++) {
		e = slots_tok[i];
		e_vx = e->view;

		if (e->keyw_type == ULS_KEYW_TYPE_TWOPLUS) { /* tokdef_twoplus */
			slots_tm[n1++] = e_vx;

		} else if (e->keyw_type == ULS_KEYW_TYPE_IDSTR) { /* idkeyw_table */
			e->link = idtok_list;
			idtok_list = e;
			++n2;

		} else if (e->keyw_type == ULS_KEYW_TYPE_1CHAR) {
			_uls_log(err_panic)("%d: not permitted keyw_type", e->keyw_type);
		}
	}

	twotbl->twoplus_mempool.n = n1;
	parms->n = n2;

	return idtok_list;
}

int
ULS_QUALIFIED_METHOD(commtype_by_length_dsc)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_commtype_ptr_t e1 = (const uls_commtype_ptr_t) a;
	const uls_commtype_ptr_t e2 = (const uls_commtype_ptr_t) b;
	int stat;

	if (e1->len_start_mark < e2->len_start_mark) stat = 1;
	else if (e1->len_start_mark > e2->len_start_mark) stat = -1;
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(quotetype_by_length_dsc)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_quotetype_ptr_t e1 = (const uls_quotetype_ptr_t) a;
	const uls_quotetype_ptr_t e2 = (const uls_quotetype_ptr_t) b;
	int stat;

	if (e1->len_start_mark < e2->len_start_mark) stat = 1;
	else if (e1->len_start_mark > e2->len_start_mark) stat = -1;
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(calc_len_surplus_recommended)(uls_lex_ptr_t uls)
{
	int i, len, len_surplus = ULS_UTF8_CH_MAXLEN;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);

	for (i=0; i<uls->n_commtypes; i++) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), i);
		if ((len = cmt->len_end_mark) > len_surplus)
			len_surplus = len;
	}

	if (uls->n_commtypes > 0) {
		cmt = uls_get_array_slot_type02(uls_ptr(uls->commtypes), 0);
		// commtypes[] are sorted by len_start_mark, so the first element has longest comment start mark.
		if ((len = cmt->len_start_mark) > len_surplus)
			len_surplus = len;
	}

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));

	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		if ((len = qmt->len_end_mark) > len_surplus)
			len_surplus = len;
	}

	if (uls->quotetypes.n > 0) {
		qmt = slots_qmt[0];
		if ((len = qmt->len_start_mark) > len_surplus)
			len_surplus = len;
	}

	return len_surplus;
}

int
ULS_QUALIFIED_METHOD(srch_vx_by_id)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e0 = (const uls_tokdef_vx_ptr_t) b;
	int id = e0->tok_id, stat;

	/*  (-1) mid(0) (+1) */
	if (e->tok_id < id) stat = -1;
	else if (e->tok_id > id) stat = 1;
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(comp_vx_by_tokid)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1 = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2 = (const uls_tokdef_vx_ptr_t) b;
	int stat;

	if (e1->tok_id > e2->tok_id) stat = 1;
	else if (e1->tok_id < e2->tok_id) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_want_eof)(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags |= ULS_CTX_FL_WANT_EOFTOK;

	if (__uls_tok(uls) == uls->xcontext.toknum_EOF) {
		_uls_log(err_log)("The current token is already EOF!");
	}
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_unwant_eof)(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags &= ~ULS_CTX_FL_WANT_EOFTOK;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_is_valid_specpath)(const char* confname)
{
	char ch;
	int i, len;

	for (i=0; (ch=confname[i]) != '\0'; i++) {
		if (!_uls_tool_(isprint)(ch)) return -1;
	}
	len = i;

	if (len > ULC_LONGNAME_MAXSIZ || len==0 || confname[len-1] == ULS_ULCNAME_DELIM) {
		return -2;
	}

	for (i=len-1; i>=0; i--) {
		if (confname[i] == ULS_ULCNAME_DELIM) break;
	}

	return ++i;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_get_spectype)(const char *filepath, _uls_tool_ptrtype_(outparam) parms)
{
	int k, typ, len_dpath, len_fname;
	char* specname = parms->line;
	const char *fname;

	if (filepath == NULL || specname == NULL) {
		_uls_log(err_log)("invalid parameter!");
		return -1;
	}

	for (k=_uls_tool_(strlen)(filepath)-1; ; k--) {
		if (k < 0) {
			k = 0;
			fname = filepath;
			break;
		}
		if (filepath[k] == ULS_FILEPATH_DELIM || filepath[k] == ULS_ULCNAME_DELIM) {
			fname = filepath + k + 1;
			break;
		}
	}
	len_dpath = k;

	len_fname = _uls_tool_(strlen)(fname);
	for (k=len_fname-1; k>=0; k--) {
		if (fname[k] == '.') {
			len_fname = k;
			break;
		}
	}
	if (len_fname <= 0) {
		_uls_log(err_log)("%s: invalid name of ulc!", filepath);
		return -1;
	}

	if (uls_streql(fname+len_fname, ".ulc")) {
		typ = ULS_NAME_FILEPATH_ULC;
		_uls_tool_(strncpy)(specname, fname, len_fname);

	} else if (uls_streql(fname+len_fname, ".uld")) {
		typ = ULS_NAME_FILEPATH_ULD;
		_uls_tool_(strncpy)(specname, fname, len_fname);

	} else {
		// 'fname' is a valid specname.
		typ = ULS_NAME_SPECNAME;
		if (is_path_prefix(filepath) != 0 || uls_is_valid_specpath(filepath) < 0) {
			_uls_log(err_log)("%s: invalid spec-path", filepath);
			return -1;
		}

		if ((fname = uls_find_lang_name(fname)) == NULL) {
//			_uls_log(err_log)("%s: not found!", filepath);
			return -1;
		}
		len_fname = _uls_tool_(strcpy)(specname, fname);
	}

	parms->n = len_dpath;
	parms->len = len_fname;

	return typ;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(ulc_set_searchpath)(const char *pathlist)
{
	uls_mfree(_uls_sysinfo_(ULC_SEARCH_PATH));
	if (pathlist != NULL) {
		_uls_sysinfo_(ULC_SEARCH_PATH) = _uls_tool_(strdup)(pathlist, -1);
	}
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(ulc_add_searchpath)(const char *pathlist, int front)
{
	int len1, len, siz;
	char *ptr0, *ptr;

	if (pathlist == NULL || (len=_uls_tool_(strlen)(pathlist)) <= 0) {
		_uls_log(err_log)("%s: invalid parameter!");
		return -1;
	}

	if (_uls_sysinfo_(ULC_SEARCH_PATH) == NULL) {
		len1 = 0;
		siz = len;
	} else {
		len1 = _uls_tool_(strlen)(_uls_sysinfo_(ULC_SEARCH_PATH));
		siz = len1 + 1 + len;
	}

	ptr0 = ptr = uls_malloc_buffer(siz+1);

	if (len1 > 0) {
		if (front) {
			ptr[len] = ULS_DIRLIST_DELIM;
			_uls_tool_(memcopy)(ptr+len+1, _uls_sysinfo_(ULC_SEARCH_PATH), len1);
		} else {
			ptr = (char *) _uls_tool_(memcopy)(ptr, _uls_sysinfo_(ULC_SEARCH_PATH), len1);
			*ptr++ = ULS_DIRLIST_DELIM;
		}
	}

	_uls_tool_(memcopy)(ptr, pathlist, len);
	ptr0[siz] = '\0';

	ulc_set_searchpath(ptr0);
	uls_mfree(ptr0);

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(ulc_prepend_searchpath_pwd)(void)
{
	char fpath_buf[ULS_FILEPATH_MAX+1];

	if (_uls_tool_(getcwd)(fpath_buf, ULS_FILEPATH_MAX+1) < 0) {
		_uls_log(err_log)("can't find the current directory");
		return -1;
	}

	if (ulc_add_searchpath(fpath_buf, 1) < 0) {
		_uls_log(err_log)("can't update the system value for ulc-search-path");
		return -1;
	}

	return 0;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(ulc_list_searchpath)(const char* confname)
{
	_uls_tool_type_(arglst) title_list;
	_uls_tool_type_(arglst) searchpath_list;

	char  specname[ULC_LONGNAME_MAXSIZ+1], fpath_buff[ULC_LONGNAME_MAXSIZ+5], ch;
	const char *filename;
	int len_basedir, len_specname, n, k, typ_fpath;
	_uls_tool_type_(outparam) parms1;

	if (confname == NULL) return;

	parms1.line = specname;
	typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
	len_basedir = parms1.n;
	len_specname = parms1.len;

	if (typ_fpath < 0) {
		_uls_log(err_log)("%s: Invalid name for spec-name", confname);
		return;
	}

	_uls_tool_(init_arglst)(uls_ptr(title_list), N_ULC_SEARCH_PATHS);
	_uls_tool_(init_arglst)(uls_ptr(searchpath_list), N_ULC_SEARCH_PATHS);

	if (typ_fpath == ULS_NAME_SPECNAME) {
		n = ulc_get_searchpath_by_specname(uls_ptr(title_list), uls_ptr(searchpath_list));
		// assert: confname is just a specname.

		for (k=0; k<len_basedir; k++) {
			if ((ch = confname[k]) == ULS_ULCNAME_DELIM) {
				fpath_buff[k] = ULS_FILEPATH_DELIM;
			} else {
				fpath_buff[k] = ch;
			}
		}

		if (k > 0) fpath_buff[k++] = ULS_FILEPATH_DELIM;
		_uls_tool_(strcpy)(fpath_buff+k, specname);
		k += len_specname;
		_uls_tool_(strcpy)(fpath_buff+k, ".ulc");

		filename = fpath_buff;

	} else {
		n = ulc_get_searchpath_by_specpath(is_absolute_path(confname),
			uls_ptr(title_list), uls_ptr(searchpath_list));
		// assert: typ_fpath == ULS_NAME_FILEPATH_{ULC,ULD}
		// confname is a file path.
		filename = confname;
	}

	_list_searchpath(filename, uls_ptr(title_list), uls_ptr(searchpath_list), n);

	_uls_tool_(deinit_arglst)(uls_ptr(title_list));
	_uls_tool_(deinit_arglst)(uls_ptr(searchpath_list));
}

int
ULS_QUALIFIED_METHOD(check_ulcf_fileformat_magic)(char *linebuff, int linelen, int ftype)
{
	char magic_code[9] = { (char) 0xEF, (char) 0xBB, (char) 0xBF, '#', '@', 'u', 'l', 'c', '-' };
	int  magic_code_len = 9;
	char ch;

	if (ftype == 1) {
		ch = 'f';
	} else if (ftype == 2) {
		ch = 'd';
	} else {
		ch = 'c';
	}

	magic_code[7] = ch;

	if (linelen < magic_code_len ||
		_uls_tool_(memcmp)(linebuff, magic_code, magic_code_len) != 0) {
		_uls_log(err_log)("Improper ulc/ulf file format!");
		_uls_log(err_log)("The ulc/ulf/uld file must be utf-8 encoded text file with BOM!");
		return -1;
	}

	return magic_code_len;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ulc_fileformat_ver)(char *linebuff, int linelen, _uls_tool_ptrtype_(version) ver1)
{
	int  len1, magic_code_len;
	char *lptr, *lptr1;

	if ((magic_code_len = check_ulcf_fileformat_magic(linebuff, linelen, 0)) < 0) {
		return -1;
	}

	lptr1 = linebuff + magic_code_len;
	_uls_tool(str_trim_end)(lptr1, linelen - magic_code_len);

	if ((lptr = _uls_tool_(strchr)(lptr1, ':')) != NULL) {
		len1 = (int) (lptr - lptr1);
		*lptr = '\0';
	} else {
		len1 = _uls_tool_(strlen)(lptr1);
	}

	if (len1 > ULS_VERSION_STR_MAXLEN || _uls_tool_(version_pars_str)(lptr1, ver1) < 0) {
		return -2;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_ulc_file_magic)(FILE* fin, _uls_tool_ptrtype_(version) sysver, char *ulc_lname)
{
	int  linelen, len1, len2, magic_code_len;
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr, *lptr1;
	_uls_tool_type_(version) ver1;
	char  ver_str[ULS_VERSION_STR_MAXLEN+1];

	linelen = _uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0);
	if (linelen <= ULS_EOF || (magic_code_len = check_ulcf_fileformat_magic(linebuff, linelen, 0)) < 0) {
		rewind(fin);
		return -1;
	}

	lptr1 = linebuff + magic_code_len;
	_uls_tool(str_trim_end)(lptr1, linelen - magic_code_len);

	if ((lptr = _uls_tool_(strchr)(lptr1, ':')) != NULL) {
		len1 = (int) (lptr - lptr1);
		*lptr++ = '\0';

		if ((len2=_uls_tool_(strlen)(lptr)) > ULC_LONGNAME_MAXSIZ || len2 <= 0) {
			_uls_log(err_log)("%s: Too long ulc-path", lptr);
			len2 = -2;
		} else {
			_uls_tool_(strncpy)(ulc_lname, lptr, len2);
		}
	} else {
		ulc_lname[0] = '\0';
		len1 = _uls_tool_(strlen)(lptr1);
		len2 = 0;
	}

	if (len1 > ULS_VERSION_STR_MAXLEN || _uls_tool_(version_pars_str)(lptr1, uls_ptr(ver1)) < 0 ||
		!uls_ver_compatible(uls_ptr(ver1), sysver)) {
		_uls_tool_(version_make_string)(uls_ptr(ver1), ver_str);
		_uls_log(err_log)("unsupported version: %s", ver_str);
		len2 = -3;
	}

	if (len2 < 0) {
		rewind(fin);
	}

	return len2;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(ulc_read_header)(uls_lex_ptr_t uls, FILE* fin, ulc_header_ptr_t hdr, _uls_tool_ptrtype_(outparam) parms)
{
	fp_list_ptr_t  fp_stack_ptr, fp_stack_top;
	char  specname[ULC_LONGNAME_MAXSIZ+1];
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr;

	char  ulc_lname[ULC_LONGNAME_MAXSIZ+1], *tag;
	int   linelen, lno;
	int   rc, ulc_lname_len, len_basedir, len_specname, typ_fpath;
	_uls_tool_type_(outparam) parms1;

	linelen = _uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0);
	if (linelen <= ULS_EOF || get_ulc_fileformat_ver(linebuff, linelen, uls_ptr(hdr->filever)) < 0) {
		_uls_log(err_log)("Can't get file format version of %s!", ulc_lname);
		return -1;
	}
	rewind(fin);

	// Read Header upto '%%'
	for (fp_stack_top = (fp_list_ptr_t) parms->data;
		(ulc_lname_len = check_ulc_file_magic(fin, uls_ptr((uls)->config_filever), ulc_lname)) > 0; ) {

		if (ulc_find_fp_list(fp_stack_top, ulc_lname) != nilptr) {
			_uls_log(err_log)("can't inherit ulc-spec %s from multiple parents", ulc_lname);
			return -1;
		}

		parms1.line = specname;
		typ_fpath = uls_get_spectype(ulc_lname, uls_ptr(parms1));
		len_basedir = parms1.n;
		len_specname = parms1.len;

		if (typ_fpath != ULS_NAME_SPECNAME) {
			_uls_log(err_log)("Invalid spec-name in %s.", ulc_lname);
			return -1;
		}

		if ((fin = uls_get_ulc_path(typ_fpath, ulc_lname, len_basedir, specname, len_specname, nilptr)) == NULL) {
			_uls_log(err_log)("can't open ulc-spec '%s'", ulc_lname);
			return -1;
		}

		fp_stack_top->linenum = 1;
		fp_stack_top = ulc_fp_push(fp_stack_top, fin, ulc_lname);
	}

	parms->data = fp_stack_ptr = fp_stack_top;

	tag = fp_stack_ptr->tagstr;
	lno = fp_stack_ptr->linenum;

	while (1) {
		if ((linelen=_uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) lno = -1;
			break;
		}
		++lno;

		if (_uls_tool_(strncmp)(linebuff, "%%%%", 2) == 0) {
			// The bottom of stack has been reached!
			fp_stack_ptr->linenum = lno;

			parms1.data = fp_stack_ptr;
			ulc_fp_get(uls_ptr(parms1), 0);
			fp_stack_ptr = (fp_list_ptr_t) parms1.data;

			if (fp_stack_ptr == nilptr ||
				(fin = ulc_fp_peek(fp_stack_ptr, uls_ptr(parms1))) == NULL) {
				break;
			}
			tag = parms1.line;
			lno = parms1.n;
			continue;
		}

		if (*(lptr = _uls_tool(skip_blanks)(linebuff)) == '\0' || *lptr == '#')
			continue;

		if ((rc=read_config_var(tag, lno, uls, lptr, hdr)) <= 0) {
			if (rc < 0) return -1;
			break;
		}
	}

	return lno;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(ulc_proc_line)
	(const char* tag_nam, int lno, char* lptr, uls_lex_ptr_t uls, ulc_header_ptr_t hdr, _uls_tool_ptrtype_(outparam) parms)
{
	char *ch_ctx = uls->ch_context;
	char *wrd1, *wrd2, *wrd3;
	char wrdbuf2[ULS_LEXSTR_MAXSIZ+1];
	int  len2, tok_id, rsv_idx, keyw_type;
	uls_tokdef_ptr_t e, e_keyw;
	uls_tokdef_vx_ptr_t e_vx=nilptr, e_vx_grp, e2_vx, e_vx_1ch;

	uls_decl_parray_slots(slots_tok, tokdef);
	uls_decl_parray_slots(slots_rsv, tokdef_vx);
	uls_decl_parray_slots(slots_vx, tokdef_vx);

	uls_tokdef_name_ptr_t e_nam;
	_uls_tool_type_(wrd) wrdx;
	_uls_tool_type_(outparam) parms1;

	parms->data = nilptr;
	wrdx.lptr = lptr;

/* token-name */
	wrd1 = __uls_tool_(splitstr)(uls_ptr(wrdx));
	if (_uls_tool(canbe_tokname)(wrd1) <= 0) {
		_uls_log(err_log)("%s<%d>: Nil-string or too long token constant.", tag_nam, lno);
		_uls_log(err_log)("\ttoken-name:'%s'", wrd1);
		return nilptr;
	}

	if ((rsv_idx=is_reserved_tok(uls, wrd1)) >= 0) {
		wrd2 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		if ((len2 = _uls_tool_(strlen)(wrd2)) > ULS_LEXSTR_MAXSIZ) {
			// The keywords for ID, NUMBER, .. are nonsense.
			len2 = ULS_LEXSTR_MAXSIZ;
			wrd2[len2] = '\0';
		}

		if (*wrd2 == '\0' || _uls_tool(is_pure_int_number)(wrd2) > 0) {
			wrd3 = wrd2;
			wrd2 = "";
			len2 = 0;
		} else {
			wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		}

		/* tok-id */
		if (*wrd3 != '\0') {
			tok_id = _uls_tool_(atoi)(wrd3);
			hdr->tok_id_seed = tok_id + 1;
		} else {
			tok_id = hdr->tok_id_seed++;
		}

		if ((e2_vx=__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) != nilptr) {
			_uls_log(err_log)("In %s<%d>,", tag_nam, lno);
			_uls_log(err_log)("\tthe token-id %s is already used by %s.", wrd1, _uls_get_namebuf_value(e2_vx->name));
			return nilptr;
		}

		if (tok_id >= 0 && tok_id < ULS_SYNTAX_TABLE_SIZE) {
			ch_ctx[tok_id] &= ~ULS_CH_1;
		}

		slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
		e_vx = slots_rsv[rsv_idx];
		e_vx->tok_id = tok_id;

		return e_vx;
	}

	e_vx_grp = __find_reg_tokdef_by_name(uls, wrd1, uls_ptr(parms1));
	e_nam = (uls_tokdef_name_ptr_t) parms1.data;

/* keyword */
	wrd2 = __uls_tool_(splitstr)(uls_ptr(wrdx));

	parms1.data = uls;
	parms1.line = wrdbuf2;
	keyw_type = check_keyw_str(lno, wrd2, uls_ptr(parms1));
	len2 = parms1.len;

	if (keyw_type < 0) {
		_uls_log(err_log)("%s<%d>: Invalid keyword", tag_nam, lno);
		_uls_log(err_log)("\t:keyword:'%s'", wrd2);
		return nilptr;
	}

	e_keyw = nilptr;
	e_vx_1ch = nilptr;

	if (keyw_type == ULS_KEYW_TYPE_USER) {
		wrd3 = wrd2;
		wrd2 = wrdbuf2;

	} else if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
		wrd2 = wrdbuf2;
		if ((e_vx_1ch = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), wrd2[0], nilptr)) != nilptr) {
			if (e_vx_grp == nilptr) e_vx_grp = e_vx_1ch;
		}
		wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));

	} else {
		if ((e_keyw=__find_tokdef_by_keyw(uls, wrdbuf2)) != nilptr) {
			if (e_vx_grp == nilptr) {
				e_vx_grp = e_keyw->view;
			} else {
				// There exists a previously defined tok-def(e_vx_grp) with same keyword.
				if (e_keyw->view != e_vx_grp) {
					_uls_log(err_log)("%s<%d>: the name is alrealy defined in another line.", tag_nam, lno);
					_uls_log(err_log)("\tname: '%s' of '%s'", wrd1, wrd2);
					return nilptr;
				}
			}
		}

		wrd2 = wrdbuf2;
		wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));
	}

/* token-id */
	if (*wrd3 != '\0') {
		tok_id = _uls_tool_(atoi)(wrd3);
		hdr->tok_id_seed = tok_id + 1;
	} else {
		if (e_vx_grp != nilptr) {
			tok_id = e_vx_grp->tok_id;
		} else {
			if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
				tok_id = wrd2[0];
			} else {
				tok_id = hdr->tok_id_seed++;
			}
		}
	}

	set_chrmap(uls, tok_id, keyw_type, wrd2[0]);

	if (e_vx_grp != nilptr) {
		// Either name or keyword is the same as the one of 'e_vx_grp'.
		if (e_vx_grp->tok_id != tok_id) {
			_uls_log(err_log)("%s<%d>: The keyword conflicts with the previous designation for token.", tag_nam, lno);
			_uls_log(err_log)("\tkeyword:'%s'", wrd2);
			_uls_log(err_log)("\ttoken %s(%d)", _uls_get_namebuf_value(e_vx_grp->name), e_vx_grp->tok_id);
			return nilptr;
		}

		if (e_keyw != nilptr || e_vx_1ch != nilptr) {
			if (e_nam == nilptr) {
				e_nam = alloc_tokdef_name(wrd1, e_vx_grp);
				append_tokdef_name_to_group(e_vx_grp, e_nam);
			}
			return e_vx_grp;
		}

		// The keyword of the current is not equals to the one of e_vx_grp.
		// This means that the names of the two lines should be same
		// because the current is associated with the 'e_vx_grp'.
		// uls_streql(wrd1, e_vx_grp->name) == true

	} else if ((e2_vx=__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_RSVD)) != nilptr) {
		_uls_log(err_log)("%s<%d>: Aliasing of reserved-tok isn't permitted!", tag_nam, lno);
		_uls_log(err_log)("\treserved-tok:%s", wrd1);
		return nilptr;

	} else if ((e2_vx=__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) != nilptr) {
		e_vx_grp = e2_vx;

		if (_uls_get_namebuf_value(e2_vx->name)[0] == '\0') {
			e2_vx->l_name = _uls_set_namebuf_value(e2_vx->name, wrd1);
			if (wrd2[0] == '\0') { // user-defined token name
				return e_vx_grp;
			}

		} else {
			e_nam = alloc_tokdef_name(wrd1, e_vx_grp);
			if (wrd2[0] == '\0') {
				append_tokdef_name_to_group(e_vx_grp, e_nam);
				return e_vx_grp;
			} else if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
				uls_insert_1char_tokdef_vx(uls_ptr(uls->onechar_table), wrd2[0], e_vx_grp);
				append_tokdef_name_to_group(e_vx_grp, e_nam);
				return e_vx_grp;
			}
		}
	}

	// allocate the slot for new uls_tokdef_ptr_t
	realloc_tokdef_array(uls, 1, 1);

	if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
		e = nilptr;
	} else {
		e = uls_create_tokdef();

		// check keyword
		e->keyw_type = keyw_type;
		_uls_set_namebuf_value(e->keyword, wrd2);
		e->l_keyword = len2;

		if (keyw_type == ULS_KEYW_TYPE_TWOPLUS) {
			++hdr->n_keys_twoplus;
		} else if (keyw_type == ULS_KEYW_TYPE_IDSTR) {
			++hdr->n_keys_idstr;
		}
	}

	slots_tok = uls_parray_slots(uls_ptr(uls->tokdef_array));
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));

	if (e_vx_grp == nilptr) { // group-leader
		uls_assert(e_nam == nilptr);

		e_vx = e_vx_grp = uls_create_tokdef_vx(tok_id, wrd1, e);

		if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
			if (uls_insert_1char_tokdef_vx(uls_ptr(uls->onechar_table), wrd2[0], e_vx) == 0) {
				uls_destroy_tokdef_vx(e_vx);
				e_vx = nilptr;
			} else {
				slots_vx[uls->tokdef_vx_array.n++] = e_vx;
			}
		} else {
			e->view = e_vx;
			e->name = _uls_get_namebuf_value(e_vx->name);
			slots_tok[uls->tokdef_array.n++] = e;
			slots_vx[uls->tokdef_vx_array.n++] = e_vx;
		}

	} else if (e != nilptr) {
		e->view = e_vx = e_vx_grp;

		if (e_nam != nilptr) {
			e->name = _uls_get_namebuf_value(e_nam->name);
			append_tokdef_name_to_group(e_vx_grp, e_nam);
		} else {
			e->name = _uls_get_namebuf_value(e_vx_grp->name);
		}

		slots_tok[uls->tokdef_array.n++] = e;
		append_tokdef_to_group(e_vx_grp, e);
	}

	parms->data = e;
	return e_vx;
}

ULS_DLL_EXTERN ULS_QUALIFIED_RETTYP(uls_xcontext_ptr_t)
ULS_QUALIFIED_METHOD(_uls_get_xcontext)(uls_lex_ptr_t uls)
{
	return uls_ptr(uls->xcontext);
}

int
ULS_QUALIFIED_METHOD(initialize_ulc_lexattr)()
{
	_uls_tool_ptrtype_(cmd) lexattr;

	uls_init_array_type01(uls_ptr(ulc_cmd_list), UlsToolbase, cmd, ULC_N_LEXATTRS);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 0);
	lexattr->name = "CASE_SENSITIVE";
	lexattr->proc = _uls_ref_callback_this(read_config__CASE_SENSITIVE);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 1);
	lexattr->name = "CHAR_TOK";
	lexattr->proc = _uls_ref_callback_this(read_config__CHAR_TOK);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 2);
	lexattr->name = "COMMENT_TYPE";
	lexattr->proc = _uls_ref_callback_this(read_config__COMMENT_TYPE);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 3);
	lexattr->name = "DECIMAL_SEPARATOR";
	lexattr->proc = _uls_ref_callback_this(read_config__DECIMAL_SEPARATOR);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 4);
	lexattr->name = "DOMAIN";
	lexattr->proc = _uls_ref_callback_this(read_config__DOMAIN);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 5);
	lexattr->name = "ID_CHARS";
	lexattr->proc = _uls_ref_callback_this(read_config__ID_CHARS);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 6);
	lexattr->name = "ID_FIRST_CHARS";
	lexattr->proc = _uls_ref_callback_this(read_config__ID_FIRST_CHARS);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 7);
	lexattr->name = "ID_MAX_LENGTH";
	lexattr->proc = _uls_ref_callback_this(read_config__ID_MAX_LENGTH);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 8);
	lexattr->name = "LINEFEED_GUARD";
	lexattr->proc = _uls_ref_callback_this(read_config__LINEFEED_GUARD);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 9);
	lexattr->name = "NOT_CHAR_TOK";
	lexattr->proc = _uls_ref_callback_this(read_config__NOT_CHAR_TOK);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 10);
	lexattr->name = "NUMBER_PREFIXES";
	lexattr->proc = _uls_ref_callback_this(read_config__NUMBER_PREFIXES);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 11);
	lexattr->name = "NUMBER_SUFFIXES";
	lexattr->proc = _uls_ref_callback_this(read_config__NUMBER_SUFFIXES);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 12);
	lexattr->name = "QUOTE_TYPE";
	lexattr->proc = _uls_ref_callback_this(read_config__QUOTE_TYPE);

	lexattr = uls_get_array_slot_type01(uls_ptr(ulc_cmd_list), 13);
	lexattr->name = "RENAME";
	lexattr->proc = _uls_ref_callback_this(read_config__RENAME);

	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_ulc_lexattr)()
{
	uls_deinit_array_type01(uls_ptr(ulc_cmd_list), UlsToolbase, cmd);
}
