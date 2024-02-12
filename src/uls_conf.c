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
#include "uls/uls_core.h"
#include "uls/uls_langs.h"
#include "uls/utf_file.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_misc.h"
#endif

void
ULS_QUALIFIED_METHOD(ulc_init_header)(ulc_header_ptr_t hdr, uls_lex_ptr_t uls)
{
	uls_initial_zerofy_object(hdr);
	_uls_tool_(version_make)(uls_ptr(hdr->filever), 0, 0, 0);
	uls_init_namebuf(hdr->tagbuf, ULS_TAGNAM_MAXSIZ);
	uls_init_bytespool(hdr->not_chrtoks, ULS_CHRTOKS_MAXSIZ + 1, 1);
	hdr->not_chrtoks[0] = '\0';
	hdr->uls = uls;
}

void
ULS_QUALIFIED_METHOD(ulc_deinit_header)(ulc_header_ptr_t hdr)
{
	uls_deinit_namebuf(hdr->tagbuf);
	uls_deinit_bytespool(hdr->not_chrtoks);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_keyw_str)(int lno, const char* str, uls_ptrtype_tool(outparam) parms)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	char *ch_ctx = uls->ch_context, *buf = parms->line, ch;
	int case_insensitive = uls->flags & ULS_FL_CASE_INSENSITIVE;
	const char *ptr;
	int  i, len, rc;
	int  n_ch_quotes, n_ch_comms, n_ch_non_idkeyw;
	int  n_lfs, n_tabs;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_wch_t wch;
	uls_litstr_t lit1;
	uls_decl_parray_slots(slots_qmt, quotetype);

	if (*str == '\0' || _uls_tool(is_pure_integer)(str, nilptr) > 0) {
		parms->len = 0;
		rc = ULS_KEYW_TYPE_USER;
		*buf = '\0';
		return rc;
	}

	n_ch_quotes = n_ch_comms = n_ch_non_idkeyw = 0;
	n_lfs = n_tabs = 0;
	for (len=0, ptr=str; (ch=*ptr)!='\0'; ) {
		if (len > ULS_LEXSTR_MAXSIZ) {
			_uls_log(err_log)("#%d: Too long keyword '%s'", lno, str);
			return -1;
		}

		if (ch == '\\') {
			lit1.lptr = ++ptr;
			if (*ptr == '\0') {
				_uls_log(err_log)("#%d: unterminated escape char", lno);
				return -1;
			}

			wch = uls_get_escape_char(uls_ptr(lit1));

			if (lit1.len_ch_escaped < 0) {
				buf[len++] = '\\';
				buf[len++] = *ptr++;
			} else {
				if (wch <= 0x7F && case_insensitive) {
					wch = _uls_tool_(toupper)(wch);
				}

				if ((rc = _uls_tool_(encode_utf8)(wch, NULL, -1)) <= 0 || len + rc > ULS_LEXSTR_MAXSIZ) {
					_uls_log(err_log)("#%d: encoding error!", lno);
					return -1;
				}

				len += rc = _uls_tool_(encode_utf8)(wch, buf+len, -1);
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
	wch = *ptr;
	rc = 1;

	if ((wch < ULS_SYNTAX_TABLE_SIZE && (ch_ctx[wch] & ULS_CH_IDFIRST)) ||
		(rc = uls_is_char_idfirst(uls, ptr, &wch)) > 0) {
		do {
			ptr += rc;
			if ((rc = _uls_tool_(decode_utf8)(ptr, -1, &wch)) <= 0) {
				_uls_log(err_log)("#%d: decoding error!", lno);
				return -1;
			}
		} while ((wch < ULS_SYNTAX_TABLE_SIZE && (ch_ctx[wch] & ULS_CH_ID)) || uls_is_char_id(uls, wch));

		if (*ptr == '\0') {
			n_ch_non_idkeyw = 0;
		} else {
			// The last checked char was not an id-char.
			n_ch_non_idkeyw = 1;
		}

	} else {
		n_ch_non_idkeyw = 1;
		for ( ; *ptr != '\0'; ptr += rc) {
			if ((rc = _uls_tool_(decode_utf8)(ptr, -1, NULL)) <= 0) {
				_uls_log(err_log)("#%d: decoding error!", lno);
				return -1;
			}

			if ((ch = *ptr) == ' ') {
				_uls_log(err_log)("#%d: contains illegal char in keyword", lno);
				return -1;
			}

			if (ch == '\n') ++n_lfs;
			else if (ch == '\t') ++n_tabs;

			if (uls_canbe_ch_quote(ch_ctx, ch)) ++n_ch_quotes;
			if (uls_canbe_ch_comm(ch_ctx, ch)) ++n_ch_comms;
		}
	}

	if (len > 1 && (n_lfs > 0 || n_tabs > 0)) {
		_uls_log(err_log)("#%d: contains illegal chars in keyword.", lno);
		return -1;
	}

	if (n_ch_comms > 0) {
		for (i=0; i<uls->n1_commtypes; i++) {
			cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
			if (_uls_tool_(strstr)(buf, uls_get_namebuf_value(cmt->start_mark)) != NULL) {
				_uls_log(err_log)("#%d: keyword conatins comment-mark:", lno);
				_uls_log(err_log)("\tkeyword('%s'), comment-mark('%s')", buf, uls_get_namebuf_value(cmt->start_mark));
				if (_uls_tool_(strncmp)(buf, uls_get_namebuf_value(cmt->start_mark), cmt->len_start_mark) == 0) {
					return -1;
				}
			}
		}
	}

	if (n_ch_quotes > 0) {
		slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
		for (i=0; i<uls->quotetypes.n; i++) {
			qmt = slots_qmt[i];
			if (_uls_tool_(strstr)(buf, uls_get_namebuf_value(qmt->start_mark)) != NULL) {
				_uls_log(err_log)("#%d: keyword conatins quotation-mark:", lno);
				_uls_log(err_log)("\tkeyword('%s'), quotation-mark('%s')", buf, uls_get_namebuf_value(qmt->start_mark));
				if (_uls_tool_(strncmp)(buf, uls_get_namebuf_value(qmt->start_mark), qmt->len_start_mark) == 0) {
					return -1;
				}
			}
		}
	}

	if (n_ch_non_idkeyw == 0) {
		rc = ULS_KEYW_TYPE_IDSTR;
	} else if (len == 1) {
		rc = ULS_KEYW_TYPE_1CHAR;
	} else {
		rc = ULS_KEYW_TYPE_TWOPLUS;
	}

	return rc;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_vx_by_name)(uls_lex_ptr_t uls, const char* name, int start_ind, int end_bound)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_ptr_t e_vx, e_vx_ret = nilptr;
	uls_tokdef_name_ptr_t e_nam;
	int i;

	if (end_bound < 0) {
		end_bound = uls->tokdef_vx_array.n;
	}

	for (i = start_ind; i < end_bound; i++) {
		e_vx = slots_vx[i];

		if (uls_streql(uls_get_namebuf_value(e_vx->name), name)) {
			e_vx_ret = e_vx;
			break;
		}

		if ((e_nam = find_tokdef_alias(e_vx, name)) != nilptr) {
			e_vx_ret = e_vx;
			break;
		}
	}

	return e_vx_ret;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_rsvd_vx_by_name)(uls_lex_ptr_t uls, const char* name)
{
	return __find_vx_by_name(uls, name, 0, N_RESERVED_TOKS);
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_reg_vx_by_name)(uls_lex_ptr_t uls, const char* name)
{
	return __find_vx_by_name(uls, name, N_RESERVED_TOKS, uls->tokdef_vx_array.n);
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(__find_tokdef_by_keyw)(uls_lex_ptr_t uls, const char* keyw)
{
	uls_decl_parray_slots_init(slots_keyw, tokdef, uls_ptr(uls->tokdef_array));
	uls_tokdef_ptr_t e;
	int i;

	for (i=0; i<uls->tokdef_array.n; i++) {
		e = slots_keyw[i];
		if (uls_streql(uls_get_namebuf_value(e->keyword), keyw)) {
			return e;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__find_vx_by_tokid)(uls_lex_ptr_t uls, int t, int area)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t  e_vx, e_vx_ret = nilptr;
	int start_indices[2], sizes[2];
	int i, k, i0, n = 0;

	if (area & TOKDEF_AREA_RSVD) {
		start_indices[n] = 0;
		sizes[n] = N_RESERVED_TOKS;
		++n;
	}

	if (area & TOKDEF_AREA_REGULAR) {
		start_indices[n] = N_RESERVED_TOKS;
		sizes[n] = uls->tokdef_vx_array.n - N_RESERVED_TOKS;
		++n;
	}

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (k = 0; k < n; k++) {
		i0 = start_indices[k];
		for (i = 0; i < sizes[k]; i++) { // linear-search
			e_vx = slots_vx[i0 + i];
			if (e_vx->tok_id == t) {
				e_vx_ret = e_vx;
				break;
			}
		}

		if (e_vx_ret != nilptr) {
			break;
		}
	}

	return e_vx_ret;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_str_contained_in_commtypes)(uls_lex_ptr_t uls, const char *str)
{
	uls_commtype_ptr_t cmt;
	int i, l_str;

	if ((l_str = _uls_tool_(strlen)(str)) <= 0) return 0;

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (l_str <= cmt->len_start_mark && !_uls_tool_(strncmp)(str, uls_get_namebuf_value(cmt->start_mark), l_str)) {
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

		if (l_str <= qmt->len_start_mark && !_uls_tool_(strncmp)(str, uls_get_namebuf_value(qmt->start_mark), l_str)) {
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(make_tokdef_for_quotetype)(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, const char *qmt_name)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_keyw, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_ptr_t e;
	int tok_id;

	tok_id = qmt->tok_id;

	if (__find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_RSVD) != nilptr) {
		_uls_log(err_log)("the token-name already used for reserved token:");
		_uls_log(err_log)("\ttoken-name('%s'), token(%d)", qmt_name, tok_id);
		return -1;
	}

	e = uls_create_tokdef();
	e->keyw_type = ULS_KEYW_TYPE_LITERAL;

	uls_set_namebuf_value(e->keyword, uls_get_namebuf_value(qmt->start_mark));
	e->ulen_keyword = qmt->len_start_mark;
	e->wlen_keyword = _uls_tool(ustr_num_wchars)(uls_get_namebuf_value(qmt->start_mark), qmt->len_start_mark, nilptr);

	realloc_tokdef_array(uls, 1, 1);
	slots_keyw = uls_parray_slots(uls_ptr(uls->tokdef_array));
	slots_keyw[uls->tokdef_array.n] = e;

	if (qmt_name[0] != '\0' && (__find_reg_vx_by_name(uls, qmt_name) != nilptr ||
		__find_rsvd_vx_by_name(uls, qmt_name) != nilptr)) {
		_uls_log(err_log)("%s: can't make a tok name for token:", __func__);
		_uls_log(err_log)("\ttoken-name('%s'), token(%d)", qmt_name, tok_id);
		return -1;
	}

	e_vx = uls_create_tokdef_vx(tok_id, qmt_name, e);

	++uls->tokdef_array.n;
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	slots_vx[uls->tokdef_vx_array.n++] = e_vx;

	return 0;
}

ULS_DECL_STATIC char*
ULS_QUALIFIED_METHOD(find_cnst_suffix)(char* cstr_pool, const char* str, int l_str, uls_ptrtype_tool(outparam) parms)
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
ULS_QUALIFIED_METHOD(add_cnst_suffix)(uls_lex_ptr_t uls, const char* suffix)
{
	char* cnst_suffixes = uls->numcnst_suffixes;
	uls_type_tool(outparam) parms;
	const char *srcptr;
	char ch, *dstptr, *ptr;
	int k, len;

	if (suffix == NULL || (len = _uls_tool_(strlen)(suffix)) == 0 || len > ULS_CNST_SUFFIX_MAXSIZ) {
		return 0; // false
	}

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

	return 1; // true
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(cnst_suffixes_by_len)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_ptrtype_tool(argstr) e1 = (const uls_ptrtype_tool(argstr)) a;
	const uls_ptrtype_tool(argstr) e2 = (const uls_ptrtype_tool(argstr)) b;

	int len1, len2, stat;

	len1 = _uls_tool_(strlen)(e1->str);
	len2 = _uls_tool_(strlen)(e2->str);

	if (len1  < len2) stat = 1;
	else if (len1  > len2) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_validity_of_cnst_suffix)(uls_ptrtype_tool(arglst) cnst_suffixes, uls_lex_ptr_t uls)
{
	int n_cnst_suffixes = cnst_suffixes->args.n;
	uls_decl_parray_slots_init_tool(al, argstr, uls_ptr(cnst_suffixes->args));
	uls_ptrtype_tool(argstr)  arg;
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

	uls->numcnst_suffixes[0] = '\0';
	for (i=0; i<n_cnst_suffixes; i++) {
		arg = al[i];
		add_cnst_suffix(uls, arg->str);
	}

	return n_cnst_suffixes;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(is_commstart_valid)(uls_lex_ptr_t uls, int k)
{
	uls_commtype_ptr_t cmt, cmt0 = uls_get_array_slot_type01(uls_ptr(uls->commtypes), k);
	char *str = uls_get_namebuf_value(cmt0->start_mark);
	int i, l_str = cmt0->len_start_mark;

	for (i=0; i<uls->n1_commtypes; i++) {
		if (i == k) continue;

		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (cmt->len_start_mark == l_str && uls_streql(uls_get_namebuf_value(cmt->start_mark), str)) {
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

	str = uls_get_namebuf_value(qmt0->start_mark);
	l_str = qmt0->len_start_mark;

	for (i=0; i<uls->quotetypes.n; i++) {
		if (i == k) continue;

		qmt = slots_qmt[i];
		// qmt->len_start_mark > 0

		if (qmt->len_start_mark == l_str &&
			uls_streql(uls_get_namebuf_value(qmt->start_mark), str)) {
			return 0;
		}
	}

	return 1;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__set_config_comment_type)(uls_commtype_ptr_t cmt, int flags,
	const char *mark1, int len_mark1, int lfs_mark1,
	const char *mark2, int len_mark2, int lfs_mark2)
{
	char cmt_mark_buff[ULS_COMM_MARK_MAXSIZ + 2];
	const char *cptr;

	if (flags & ULS_COMM_COLUMN0) {
		// prepend a line-feed at the front of 'mark1'
		cmt_mark_buff[0] = '\n';
		_uls_tool_(strcpy)(cmt_mark_buff + 1, mark1);
		cptr = cmt_mark_buff;
		++len_mark1;
		++lfs_mark1;
	} else {
		cptr = mark1;
	}

	cmt->flags = flags;
	uls_set_namebuf_value(cmt->start_mark, cptr);
	cmt->len_start_mark = len_mark1;
	cmt->n_lfs = lfs_mark1;

	uls_set_namebuf_value(cmt->end_mark, mark2);
	cmt->len_end_mark = len_mark2;
	cmt->n_lfs += lfs_mark2;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__COMMENT_TYPE)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);

	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end, *cptr;
	int lno = parms->n;
	uls_type_tool(wrd) wrdx;
	int  j, k, len, cmt_flags=0;
	char  *wrd, *wrdptr;

	uls_type_tool(outparam) parms1, parms2;
	uls_commtype_ptr_t cmt;

	char cmt_mark1[ULS_COMM_MARK_MAXSIZ+1];
	char cmt_mark2[ULS_COMM_MARK_MAXSIZ+1];

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (!_uls_tool_(strncmp)(wrd, "options=", 8)) {
		for (wrdptr = wrd + 8; wrdptr != NULL; ) {
			wrd = wrdptr;
			if ((wrdptr = _uls_tool_(strchr)(wrdptr, ',')) != NULL) {
				*wrdptr++ = '\0';
			}
			if (uls_streql(wrd, "oneline")) {
				cmt_flags |= ULS_COMM_ONELINE;
			} else if (uls_streql(wrd, "column0")) {
				cmt_flags |= ULS_COMM_COLUMN0;
			} else if (uls_streql(wrd, "nested")) {
				cmt_flags |= ULS_COMM_NESTED;
			} else {
				_uls_log(err_log)("%s<%d>: unknown attribute for a comment type:", tag_nam, lno);
				_uls_log(err_log)("\tattribute('%s')", wrd);
				return -1;
			}
		}
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
	}

	parms1.line = cmt_mark1;
	if ((len=_uls_tool_(strlen)(wrd)) == 0 || len > ULS_COMM_MARK_MAXSIZ ||
		!canbe_commtype_mark(wrd, uls_ptr(parms1))) {
		_uls_log(err_log)("%s<%d>: Too short or long comment (start) mark, or not permitted chars:", tag_nam, lno);
		_uls_log(err_log)("\tmark('%s')", wrd);
		return -1;
	}

	if (cmt_flags & ULS_COMM_ONELINE) {
		cmt_mark2[0] = '\n'; cmt_mark2[1] = '\0';
		parms2.line = cmt_mark2;
		parms2.len = parms2.n = 1;
	} else {
		// the closing comment string of 'start_mark'
		parms2.line = cmt_mark2;
		if ((len=_uls_tool_(strlen)(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))))==0 || len > ULS_COMM_MARK_MAXSIZ ||
			!canbe_commtype_mark(wrd, uls_ptr(parms2))) {
			_uls_log(err_log)("%s<%d>: Too short or long comment (end) mark, or not permitted chars:", tag_nam, lno);
			_uls_log(err_log)("\tmark('%s')", wrd);
			return -1;
		}
	}

	// Find the 'cmt' having comment start string.
	for (j=0; ; j++) {
		if (j >= uls->n1_commtypes) {
			// not found: add it as new one.
			if (uls->n1_commtypes >= ULS_N_MAX_COMMTYPES) {
				_uls_log(err_log)("%s<%d>: Too many comment types", tag_nam, lno);
				return -1;
			}
			k = uls->n1_commtypes++;
			break;
		}
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), j);
		cptr = uls_get_namebuf_value(cmt->start_mark);
		if (cmt_flags & ULS_COMM_COLUMN0) {
			++cptr; // next to '\n'
		}
		if (uls_streql(cmt_mark1, cptr)) {
			// found the same, override it
			k = j;
			break;
		}
	}

	cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), k);
	__set_config_comment_type(cmt, cmt_flags, cmt_mark1, parms1.len, parms1.n, cmt_mark2, parms2.len, parms2.n);

	if (!is_commstart_valid(uls, k)) {
		_uls_log(err_log)("%s<%d>: the comm-start-mark is collided with previous defined comment-type:", tag_nam, lno);
		_uls_log(err_log)("\tstart-mark('%s')", uls_get_namebuf_value(cmt->start_mark));
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
			qmt->litstr_analyzer = uls_ref_callback_this(nothing_lit_analyzer);
			qmt->flags |= ULS_QSTR_NOTHING;
		} else if (uls_streql(wrd, "open")) {
			qmt->litstr_analyzer = uls_ref_callback_this(nothing_lit_analyzer);
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
ULS_QUALIFIED_METHOD(parse_quotetype__token)(char *line, uls_quotetype_ptr_t qmt, uls_ptrtype_tool(outparam) parms)
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
ULS_QUALIFIED_METHOD(uls_parse_quotetype_opts)(uls_ptrtype_tool(wrd) wrdx, uls_quotetype_ptr_t qmt,
	uls_ptrtype_tool(outparam) parms)
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
ULS_QUALIFIED_METHOD(set_config__QUOTE_TYPE__token)(int tok_id, const char *tok_nam, int l_tok_nam,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
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
			_uls_log(err_log)("%s:%d: The quotation tok-id already used by other token:", tag_nam, lno);
			_uls_log(err_log)("\ttok-id(%d)", tok_id);
			return -1;
		}
	}
	qmt->tok_id = tok_id;

	if (l_tok_nam > 0) {
		/* token-name */
		if (tok_nam[0] != '\0' && canbe_tokname(tok_nam) <= 0) {
			_uls_log(err_log)("%s:%d Nil-string or too long token constant:", tag_nam, lno);
			_uls_log(err_log)("\ttoken-name('%s')", tok_nam);
			return -1;
		}

		if (make_tokdef_for_quotetype(uls, qmt, tok_nam) < 0) {
			_uls_log(err_log)("%s:%d fail to make tokdef for the quotation type:", tag_nam, lno);
			_uls_log(err_log)("\ttoken-name('%s')", tok_nam);
			return -1;
		}
	}

	return 0;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__set_config_quoute_type)(uls_quotetype_ptr_t qmt,
	const char *mark1, int len_mark1, int lfs_mark1, const char *mark2, int len_mark2, int lfs_mark2)
{
	// start mark
	uls_set_namebuf_value(qmt->start_mark, mark1);
	qmt->len_start_mark = len_mark1;
	qmt->n_lfs = qmt->n_left_lfs = lfs_mark1;

	// end mark
	uls_set_namebuf_value(qmt->end_mark, mark2);
	qmt->len_end_mark = len_mark2;
	qmt->n_lfs += lfs_mark2;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__read_config__QUOTE_TYPE)(char *line,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char* tag_nam, int lno)
{
	char   *wrd, *ch_ctx = uls->ch_context;
	int  j, k, len, tok_id, tok_id_specified;
	uls_type_tool(wrd) wrdx;

	uls_type_tool(outparam) parms, parms1, parms2;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_quotetype_ptr_t qmt2;

	char tok_name[ULS_LEXSTR_MAXSIZ+1];
	int l_tok_name;

	char qmt_mark1[ULS_QUOTE_MARK_MAXSIZ+1];
	char qmt_mark2[ULS_QUOTE_MARK_MAXSIZ+1];

	parms.line = tok_name;
	parms.lptr_end = tag_nam;
	parms.n2 = lno;

	wrdx.lptr = line;
	if (uls_parse_quotetype_opts(uls_ptr(wrdx), qmt, uls_ptr(parms)) < 0) {
		_uls_log(err_log)("failed to parse options for literal-string : %s", wrdx.lptr);
		return -1;
	}

	tok_id_specified = parms.flags;
	tok_id = parms.n;
	l_tok_name = parms.len;

	parms1.line = qmt_mark1;
	if ((len=_uls_tool_(strlen)(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
		!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms1))) {
		_uls_log(err_log)("%s<%d>: Too short or long quote (start) mark, or not permitted chars:", tag_nam, lno);
		_uls_log(err_log)("\tmark('%s')", wrd);
		return -1;
	}

	// the closing quote string corresponding to 'start mark'
	parms2.line = qmt_mark2;
	if (qmt->flags & ULS_QSTR_ASYMMETRIC) {
		if ((len=_uls_tool_(strlen)(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
			!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms2))) {
			_uls_log(err_log)("%s<%d>: Too short or long quote (end) mark, or not permitted chars.", tag_nam, lno);
			_uls_log(err_log)("\tmark('%s')", wrd);
			return -1;
		}
	} else {
		if (qmt->flags & (ULS_QSTR_OPEN | ULS_QSTR_NOTHING)) {
			qmt_mark2[0] = '\0';
			parms2.len = 0;
			parms2.n = 0;
		} else {
			_uls_tool_(strcpy)(qmt_mark2, qmt_mark1);
			parms2.len = parms1.len;
			parms2.n = parms1.n;
		}
	}

	if (!tok_id_specified) {
		tok_id = qmt_mark1[0];
	}

	// Find the 'qmt' having quote-string start string 'wrd'.
	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (j=0; ; j++) {
		if (j >= uls->quotetypes.n) {
			// not found: add it as new one.
			if (uls->quotetypes.n >= ULS_N_MAX_QUOTETYPES) {
				_uls_log(err_log)("%s<%d>: Too many quote types", tag_nam, lno);
				return -1;
			}
			k = uls->quotetypes.n++;
			slots_qmt[k] = qmt;
			break;
		}
		qmt2 = slots_qmt[j];
		if (uls_streql(qmt_mark1, uls_get_namebuf_value(qmt2->start_mark))) {
			uls_destroy_quotetype(qmt2);
			slots_qmt[j] = qmt;
			k = j;
			break;
		}
	}
	__set_config_quoute_type(qmt, qmt_mark1, parms1.len, parms1.n, qmt_mark2, parms2.len, parms2.n);

	parms.n1 = k;
	parms.n2 = lno;
	parms.lptr_end = tag_nam;
	if (set_config__QUOTE_TYPE__token(tok_id, tok_name, l_tok_name, qmt, uls, uls_ptr(parms)) < 0) {
		return -1;
	}

	if ((qmt->escmap = uls_parse_escmap(wrdx.lptr, uls_ptr(uls->escstr_pool))) == nilptr) {
		_uls_log(err_log)("%s<%d>: Invalid format of escape-mapping of literal string.", tag_nam, lno);
		return -1;
	}

	if (!is_quotestart_valid(uls, k)) {
		_uls_log(err_log)("%s<%d>:  the start-mark is collided with previous defined quote-type. skipping, ...",
			tag_nam, lno);
		_uls_log(err_log)("\tstart-mark('%s')", uls_get_namebuf_value(qmt->start_mark));
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__QUOTE_TYPE)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n, stat = 0;
	uls_quotetype_ptr_t qmt;

	qmt = uls_create_quotetype();
	qmt->litstr_analyzer = uls_ref_callback_this(dfl_lit_analyzer_escape0);

	if (__read_config__QUOTE_TYPE(line, qmt, uls, tag_nam, lno) < 0) {
		stat = -1;
		uls_destroy_quotetype(qmt);
	}

	return stat;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(parse_id_ranges_internal)(uls_lex_ptr_t uls,
	uls_ref_parray_tool(wrds_ranges,uch_range), int is_first)
{
	char  *ch_ctx = uls->ch_context;
	uls_decl_parray_slots_tool(al, uch_range);
	uls_ref_array_tool_type01(urange_list, uch_range);
	uls_decl_array_slots_tool_type01(urange_list_slots,uch_range);

	uls_ptrtype_tool(uch_range) id_range, id_range1;
	int i1, i2, i_tmp;
	int i, j, n1, k;

	al = uls_parray_slots(wrds_ranges);
	for (n1=i=0; i<wrds_ranges->n; i++) {
		id_range = al[i];

		i1 = id_range->x1;
		i2 = id_range->x2;

		if (i1 < ULS_SYNTAX_TABLE_SIZE) {
			if (i2 < ULS_SYNTAX_TABLE_SIZE) {
				i_tmp = i2;
			} else {
				i_tmp = ULS_SYNTAX_TABLE_SIZE - 1;
			}

			if (is_first) {
				for (j=i1; j<=i_tmp; j++) {
					if (_uls_tool_(isdigit)(j)) {
						_uls_log(err_log)("%d: improper char-range specified!", j);
					} else {
						ch_ctx[j] |= ULS_CH_IDFIRST;
					}
				}
			} else {
				for (j=i1; j<=i_tmp; j++) {
					ch_ctx[j] |= ULS_CH_ID;
				}
			}
			i1 = i_tmp + 1;
		}

		if (i1 <= i2) {
			++n1;
		} else {
			uls_dealloc_object(id_range);
			al[i] = nilptr;
		}
	}

	if (is_first) {
		urange_list = uls_ptr(uls->idfirst_charset);
		uls_deinit_array_tool_type01(urange_list, uch_range);
		uls_init_array_tool_type01(urange_list, uch_range, n1);
		urange_list_slots = uls_array_slots_type01(urange_list);
	} else {
		urange_list = uls_ptr(uls->id_charset);
		uls_deinit_array_tool_type01(urange_list, uch_range);
		uls_init_array_tool_type01(urange_list, uch_range, n1);
		urange_list_slots = uls_array_slots_type01(urange_list);
	}

	for (k=i=0; i<wrds_ranges->n; i++) {
		if ((id_range = al[i]) == nilptr) continue;
		i1 = id_range->x1;
		i2 = id_range->x2;
		if (i1 <= i2) {
			id_range1 = _uls_get_stdary_slot(urange_list_slots, k);
			id_range1->x1 = i1;
			id_range1->x2 = i2;
			k++;
		}
	}
}

int
ULS_QUALIFIED_METHOD(parse_id_ranges)(uls_lex_ptr_t uls, int is_first, char *line)
{
	int i;
	char  *wrd;

	uls_type_tool(outparam) parms1;
	uls_type_tool(wrd) wrdx;
	uls_decl_parray_tool(wrds_ranges, uch_range);
	uls_decl_parray_slots_tool(al, uch_range);
	uls_ptrtype_tool(uch_range) id_range;

	uls_init_parray_tool(uls_ptr(wrds_ranges), uch_range, 16);

	line = _uls_tool(skip_blanks)(line);

	wrdx.lptr = line;
	while (*(wrd=__uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		parms1.lptr = wrd;
		if (_uls_tool(get_range_aton)(uls_ptr(parms1)) <= 0) {
			return -1;
		}

		id_range = uls_alloc_object(uls_type_tool(uch_range));
		id_range->x1 = parms1.x1;
		id_range->x2 = parms1.x2;

		al = uls_parray_slots(uls_ptr(wrds_ranges));
		if ((i=wrds_ranges.n) >= wrds_ranges.n_alloc) {
			uls_resize_parray_tool(uls_ptr(wrds_ranges), uch_range, i + 16);
			al = uls_parray_slots(uls_ptr(wrds_ranges));
		}

		al[i] = id_range;
		++wrds_ranges.n;
	}

	parse_id_ranges_internal(uls, uls_ptr(wrds_ranges), is_first);

	al = uls_parray_slots(uls_ptr(wrds_ranges));
	for (i=0; i<wrds_ranges.n; i++) {
		if ((id_range = al[i]) != nilptr) {
			uls_dealloc_object(id_range);
			al[i] = nilptr;
		}
	}

	uls_deinit_parray_tool(uls_ptr(wrds_ranges));
	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_FIRST_CHARS)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
//	const char* tag_nam = parms->lptr_end;
//	int lno = parms->n;

	return parse_id_ranges(uls, 1, line);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_CHARS)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
//	const char* tag_nam = parms->lptr_end;
//	int lno = parms->n;

	return parse_id_ranges(uls, 0, line);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__RENAME)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int stat = 0, lno = parms->n;
	uls_type_tool(wrd) wrdx;
	uls_tokdef_vx_ptr_t e_vx;
	char  *wrd, *wrd2;

	wrdx.lptr = line;

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' ||
		*(wrd2 = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' ||
		_uls_tool_(strlen)(wrd2) > ULS_LEXSTR_MAXSIZ) {
		_uls_log(err_log)("%s<%d>: Invalid 'RENAME' line!", tag_nam, lno);
		return -1;
	}

	if ((e_vx = __find_rsvd_vx_by_name(uls, wrd)) != nilptr) {
		e_vx->l_name = uls_set_namebuf_value(e_vx->name, wrd2);
	} else {
		_uls_log(err_log)("%s<%d>: Invalid 'RENAME':", tag_nam, lno);
		_uls_log(err_log)("\t'%s'", wrd);
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__NOT_CHAR_TOK)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_type_tool(wrd) wrdx;

	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	ulc_header_ptr_t cfg = (ulc_header_ptr_t) parms->proc;
	char  ch, *wrd, *wrdptr;
	int k;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (!_uls_tool_(strncmp)(wrd, "multibytes=", 11)) {
		wrd += 11;
		if (uls_streql(wrd, "false")) {
			uls->flags |= ULS_FL_MULTIBYTES_CHRTOK;
		} else { // true
			uls->flags &= ~ULS_FL_MULTIBYTES_CHRTOK; // default-value
		}
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
	}

	k = 0;
	cfg->not_chrtoks[0] = '\0';

	while (*wrd != '\0') {
		for (wrdptr = wrd; (ch = *wrdptr) != '\0' && k <= ULS_CHRTOKS_MAXSIZ; wrdptr++) {
			if (_uls_tool_(isgraph)(ch) && !_uls_tool_(isalnum)(ch)) {
				if (_uls_tool_(strchr)(cfg->not_chrtoks, ch) == NULL) {
					cfg->not_chrtoks[k++] = ch;
					cfg->not_chrtoks[k] = '\0';
				}
			}
		}
		wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__CASE_SENSITIVE)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n, is_sensitive, stat = 0;
	uls_type_tool(wrd) wrdx;
	char  *wrd;

	wrdx.lptr = line;
	wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));

	if (uls_streql(wrd, "false")) {
		is_sensitive = 0;
	} else if (uls_streql(wrd, "true")) {
		is_sensitive = 1;
	} else {
		_uls_log(err_log)("%s<%d>: Incorrect value of CASE_SENSITIVE. Specify it by true/false.",
			tag_nam, lno);
		is_sensitive = -1;
	}

	if (is_sensitive >= 0) {
		if (is_sensitive > 0) {
			uls->flags &= ~ULS_FL_CASE_INSENSITIVE;
		} else {
			uls->flags |= ULS_FL_CASE_INSENSITIVE;
		}
	} else {
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__DOMAIN)(char *line, uls_voidptr_t user_data)
{
	int linelen;

	linelen = _uls_tool(str_trim_end)(line, -1);
	if (linelen <= 0 || !uls_streql(line, ULC_REPO_DOMAIN)) {
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__ID_MAX_LENGTH)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_type_tool(wrd) wrdx;
	char  *wrd;
	int   len1, len2;

	wrdx.lptr = line;
	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) == '\0' || (len1 = _uls_tool_(atoi)(wrd)) <= 0) {
		return -1;
	}

	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		len2 = _uls_tool_(atoi)(wrd);
	} else {
		len2 = -1;
	}

	if (len1 <= 0) len1 = INT_MAX;
	uls->id_max_bytes = len1;

	if (len2 <= 0) len2 = len1;
	uls->id_max_uchars = len2;

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
ULS_QUALIFIED_METHOD(read_config__NUMBER_PREFIXES)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	int stat = 0;

	uls_number_prefix_ptr_t numpfx;
	uls_type_tool(wrd) wrdx;
	char  *wrd, *lptr;
	int k, r, len;

	wrdx.lptr = line;
	for (k=0; *(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0'; k++) {
		if (k >= ULS_N_MAX_NUMBER_PREFIXES) {
			_uls_log(err_log)("NUMBER_PREFIXES: Too many NumberPrefixes %d", ULS_N_MAX_NUMBER_PREFIXES);
			stat = -4; break;
		}

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
		r = _uls_tool_(atoi)(lptr);
		if (_uls_tool_(get_standard_number_prefix)(r) == NULL) {
			_uls_log(err_log)("NUMBER_PREFIXES: Not supported radix %d!", r);
			_uls_log(err_log)("  Supported number radicies = { 2, 3, 4, 8, 16 }");
			return -3;
		}

		numpfx = uls_get_array_slot_type00(uls_ptr(uls->numcnst_prefixes), k);
		uls_init_namebuf(numpfx->prefix, ULS_MAXLEN_NUMBER_PREFIX);
		_uls_tool_(strncpy)(uls_get_namebuf_value(numpfx->prefix), wrd, len);
		numpfx->l_prefix = len;
		numpfx->radix = r;
	}

#ifdef ULS_CLASSIFY_SOURCE
	_uls_quicksort_vptr(uls_array_slots_type00(uls_ptr(uls->numcnst_prefixes)), k, numpfx_by_length_dsc);
#else
	_uls_tool_(quick_sort)(uls_array_slots_type00(uls_ptr(uls->numcnst_prefixes)), k, sizeof(uls_number_prefix_t), uls_ref_callback_this(numpfx_by_length_dsc));
#endif
	uls->n_numcnst_prefixes =  k;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__DECIMAL_SEPARATOR)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;

	uls_type_tool(wrd) wrdx;
	char  *wrd;
	uls_wch_t wch = ULS_DECIMAL_SEPARATOR_DFL;

	wrdx.lptr = line;
	if (*(wrd = __uls_tool_(splitstr)(uls_ptr(wrdx))) != '\0') {
		wch = *wrd;
		if (!_uls_tool_(isgraph)(wch) || _uls_tool_(isalnum)(wch) || wch == '-' || wch == '.') {
			_uls_log(err_log)("%s<%d>: Invalid decimal separator!", tag_nam, lno);
			return -1;
		}
	}

	uls->numcnst_separator = wch;
	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__NUMBER_SUFFIXES)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_type_tool(wrd) wrdx;
	uls_type_tool(arglst) wrdlst;

	wrdx.lptr = line;

	_uls_tool_(init_arglst)(uls_ptr(wrdlst), ULC_COLUMNS_MAXSIZ);

	__uls_tool_(explode_str)(uls_ptr(wrdx), ' ', 0, uls_ptr(wrdlst));
	check_validity_of_cnst_suffix(uls_ptr(wrdlst), uls);

	_uls_tool_(deinit_arglst)(uls_ptr(wrdlst));

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config__PREPEND_INPUT)(char *line, uls_voidptr_t user_data)
{
	uls_cast_ptrtype_tool(outparam, parms, user_data);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_type_tool(wrd) wrdx;
	const char* tag_nam = parms->lptr_end;
	int i, n, len, len2, lfs_is_not_token = 0, lno = parms->n, stat = 0;
	char ch, *line2, *wrd, *buff, *buff2 = NULL;

	line = _uls_tool(skip_blanks)(line);
	if (*line == '\0') {
		_uls_log(err_log)("%s<%d>: empty value. give a string.", tag_nam, lno);
		return -1;
	}

	len = _uls_tool_(strlen)(line);
	if (*line == '"') {
		buff2 = (char *) uls_malloc_buffer((len + 1) * sizeof(char));

		wrdx.wrd = line2 = buff2;
		wrdx.siz = len + 1;
		wrdx.lptr = line + 1;
		if ((len2 = uls_get_escape_str('"', uls_ptr(wrdx))) < 0) {
			_uls_log(err_log)("%s<%d>: an double-quoted string unterminated", tag_nam, lno);
			len2 = -1;
		} else if (len2 == 0) {
			_uls_log(err_log)("%s<%d>: an empty double-quoted string", tag_nam, lno);
			len2 = -2;
		} else {
			wrd = __uls_tool_(splitstr)(uls_ptr(wrdx));
			if (uls_streql(wrd, "true")) {
				lfs_is_not_token = 1;
			} else if (uls_streql(wrd, "false")) {
				lfs_is_not_token = 0;
			} else {
				_uls_log(err_log)("%s<%d>: specify true or false.", tag_nam, lno);
				len2 = -3;
			}
		}

	} else if (*line == '\'') {
		line2 = line + 1;
		len2 = _uls_tool_(strlen)(line2);
		for (--len2; ; len2--) {
			if (len2 <= 0) {
				_uls_log(err_log)("%s<%d>: an empty single-quoted string", tag_nam, lno);
				break;
			}
			if (!_uls_tool_(isspace)(line2[len2])) {
				if (line2[len2] != '\'') {
					_uls_log(err_log)("%s<%d>: an single-quoted string unterminated", tag_nam, lno);
					len2 = -4;
				} else {
					line2[len2] = '\0';
				}
				break;
			}
		}

	} else {
		_uls_log(err_log)("%s<%d>: PREPEND_INPUT must be started by quotation, '\\' or '\"'.", tag_nam, lno);
		len2 = -5;
	}

	if (len2 > 0) {
		uls_mfree(xctx->prepended_input);

		buff = (char *) uls_malloc_buffer(uls_ceil_log2(len2 + 1, 3) * sizeof(char));
		for (n=i=0; i<len2; i++) {
			if ((ch=line2[i]) == '\n') ++n;
			buff[i] = ch;
		}
		buff[i] = '\0';

		xctx->prepended_input = buff;
		xctx->len_prepended_input = len2;
		xctx->lfs_prepended_input = n;

		if (lfs_is_not_token) {
			xctx->flags |= ULS_XCTX_FL_IGNORE_LF;
		} else {
			xctx->flags &= ~ULS_XCTX_FL_IGNORE_LF;
		}

	} else {
		stat = -2;
	}

	uls_mfree(buff2);
	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_cmd_run)(uls_lex_ptr_t uls, const char* keyw, char *line, uls_voidptr_t data)
{
	int stat;

	if (_uls_tool_(strcmp)("CASE_SENSITIVE", keyw) == 0) {
		stat = read_config__CASE_SENSITIVE(line, data);

	} else if (_uls_tool_(strcmp)("COMMENT_TYPE", keyw) == 0) {
		stat = read_config__COMMENT_TYPE(line, data);

	} else if (_uls_tool_(strcmp)("DECIMAL_SEPARATOR", keyw) == 0) {
		stat = read_config__DECIMAL_SEPARATOR(line, data);

	} else if (_uls_tool_(strcmp)("DOMAIN", keyw) == 0) {
		stat = read_config__DOMAIN(line, data);

	} else if (_uls_tool_(strcmp)("ID_CHARS", keyw) == 0) {
		stat = read_config__ID_CHARS(line, data);

	} else if (_uls_tool_(strcmp)("ID_FIRST_CHARS", keyw) == 0) {
		stat = read_config__ID_FIRST_CHARS(line, data);

	} else if (_uls_tool_(strcmp)("ID_MAX_LENGTH", keyw) == 0) {
		stat = read_config__ID_MAX_LENGTH(line, data);

	} else if (_uls_tool_(strcmp)("NOT_CHAR_TOK", keyw) == 0) {
		stat = read_config__NOT_CHAR_TOK(line, data);

	} else if (_uls_tool_(strcmp)("NUMBER_PREFIXES", keyw) == 0) {
		stat = read_config__NUMBER_PREFIXES(line, data);

	} else if (_uls_tool_(strcmp)("NUMBER_SUFFIXES", keyw) == 0) {
		stat = read_config__NUMBER_SUFFIXES(line, data);

	} else if (_uls_tool_(strcmp)("PREPEND_INPUT", keyw) == 0) {
		stat = read_config__PREPEND_INPUT(line, data);

	} else if (_uls_tool_(strcmp)("QUOTE_TYPE", keyw) == 0) {
		stat = read_config__QUOTE_TYPE(line, data);

	} else if (_uls_tool_(strcmp)("RENAME", keyw) == 0) {
		stat = read_config__RENAME(line, data);

	} else {
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(read_config_var)(ulc_header_ptr_t hdr, const char* tag_nam, int lno, char* lptr)
{
	uls_lex_ptr_t uls = hdr->uls;
	char  *wrd;
	int len, rc, stat = 1;

	uls_type_tool(outparam) parms1;
	uls_type_tool(wrd) wrdx;

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

	rc = uls_cmd_run(uls, wrd, lptr, uls_ptr(parms1));
	if (rc < 0) {
		_uls_log(err_log)("%s<%d>: unknown attribute in ULS-spec:", tag_nam, lno);
		_uls_log(err_log)("\tattribute('%s')", wrd);
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
	uls_ptrtype_tool(arglst) title, uls_ptrtype_tool(arglst) searchpath, int n)
{
	uls_decl_parray_slots_init_tool(al_title, argstr, uls_ptr(title->args));
	uls_decl_parray_slots_init_tool(al_searchpath, argstr, uls_ptr(searchpath->args));
	uls_ptrtype_tool(argstr) arg;

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

int
ULS_QUALIFIED_METHOD(is_reserved_tok)(uls_lex_ptr_t uls, const char* name)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t e_vx;
	int i;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		if (uls_streql(uls_get_namebuf_value(e_vx->name), name)) {
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

			if (uls_streql(uls_get_namebuf_value(e0_vx->name), uls_get_namebuf_value(e_vx->name))) {
				_uls_log(err_log)("The name '%s' is already used by '%s'.",
					uls_get_namebuf_value(e_vx->name), uls_get_namebuf_value(e0_vx->name));
				stat = -1;
				break;
			}

			if (e0_vx->tok_id == e_vx->tok_id) {
				_uls_log(err_log)("The token %s:%d is already used",
					uls_get_namebuf_value(e0_vx->name), e0_vx->tok_id);
				_uls_log(err_log)("\tby %s:%d.", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
				stat = -1;
				break;
			}
		}
	}

	if (stat < 0) {
		_uls_log(err_log)("Reserved tokens:");
		for (i=0; i<N_RESERVED_TOKS; i++) {
			e_vx = slots_rsv[i];
			_uls_log(err_log)("\t%s %d", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
		}
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_is_char_idfirst)(uls_lex_ptr_t uls, const char *lptr, uls_wch_t *ptr_wch)
{
	uls_ptrtype_tool(uch_range) ran;
	int i, rc, stat;
	uls_wch_t wch;

	rc = _uls_tool_(decode_utf8)(lptr, -1, &wch);
	for (stat = -rc, i=0; ; i++) {
		if (i >= uls->idfirst_charset.n) {
			if (is_utf_id(wch) > 0) stat = rc;
			break;
		}

		ran = uls_get_array_slot_type01(uls_ptr(uls->idfirst_charset), i);
		if (wch >= ran->x1 && wch <= ran->x2) {
			stat = rc;
			break;
		}
	}

	*ptr_wch = wch;
	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_is_char_id)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	uls_ptrtype_tool(uch_range) ran;
	int i, stat = 0;

	for (i = 0; ; i++) {
		if (i >= uls->id_charset.n) {
			if (is_utf_id(wch) > 0) stat = 1;
			break;
		}

		ran = uls_get_array_slot_type01(uls_ptr(uls->id_charset), i);
		if (wch >= ran->x1 && wch <= ran->x2) {
			stat = 1;
			break;
		}
	}

	return stat;
}

ULS_DECL_STATIC int
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

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_reg_tokdef_vx)(uls_lex_ptr_t uls, int t)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_t e0_vx;
	uls_tokdef_vx_ptr_t e_vx;

	e0_vx.tok_id = t;
	e_vx = (uls_tokdef_vx_ptr_t) uls_bi_search_vptr(uls_ptr(e0_vx),
		(_uls_type_array(uls_voidptr_t)) slots_vx, uls->tokdef_vx_array.n,
		uls_ref_callback_this(srch_vx_by_id));

	return e_vx;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_tokdef_vx)(uls_lex_ptr_t uls, int t)
{
	uls_tokdef_vx_ptr_t e_vx;
	if ((e_vx = uls_find_reg_tokdef_vx(uls, t)) == nilptr) {
		if (t >= 0) {
			e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), t, nilptr);
		}
	}

	return e_vx;
}

char*
ULS_QUALIFIED_METHOD(is_cnst_suffix_contained)(char* cstr_pool, const char* str, int l_str, uls_ptrtype_tool(outparam) parms)
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
ULS_QUALIFIED_METHOD(ulc_search_for_fp)(int typ_fpath, const char* fpath, uls_ptrtype_tool(outparam) parms)
{
	uls_type_tool(arglst) title_list;
	uls_type_tool(arglst) searchpath_list;
	uls_decl_parray_slots_tool(al_searchpath, argstr);
	uls_ptrtype_tool(argstr) arg;

	FILE *fp, *fp_ret = NULL;
	char *dpath;
	int i, n;

	_uls_tool_(init_arglst)(uls_ptr(title_list), N_ULC_SEARCH_PATHS);
	_uls_tool_(init_arglst)(uls_ptr(searchpath_list), N_ULC_SEARCH_PATHS);

	if (typ_fpath == ULS_NAME_SPECNAME) {
		n = ulc_get_searchpath_by_specname(uls_ptr(title_list), uls_ptr(searchpath_list));
	} else {
		n = ulc_get_searchpath_by_specpath(_uls_tool(is_absolute_path)(fpath), uls_ptr(title_list), uls_ptr(searchpath_list));
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

		if ((fp = uls_get_spec_fp(dpath, fpath, parms)) != NULL) {
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
	const char* specname, int len_specname, uls_ptrtype_tool(outparam) parms)
{
	char fpath_buff[ULC_LONGNAME_MAXSIZ+5], ulf_filepath[ULS_FILEPATH_MAX+1], ch;
	const char *filepath, *dirpath;
	int  len_dirpath, k;
	FILE  *fp_ulc;
	uls_type_tool(outparam) parms1;

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

	if ((fpath_len=_uls_tool_(get_exeloc_dir)(argv0, fpath_buf)) < 0) {
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
	uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list)
{
	uls_decl_parray_slots_init_tool(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	uls_decl_parray_slots_tool(al_args, argstr);
	uls_ptrtype_tool(argstr) arg;

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
	uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list)
{
	uls_decl_parray_slots_init_tool(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	uls_decl_parray_slots_tool(al_args, argstr);
	uls_ptrtype_tool(argstr) arg;

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
		if (_uls_tool_(get_exeloc_dir)(NULL, exeloc) >= 0) {
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
	searchpath_list->args.n = n;

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
		if (uls_streql(uls_get_namebuf_value(fp_lst->tagbuf), ulc_name)) {
			return fp_lst;
		}
	}

	return nilptr;
}

ULS_QUALIFIED_RETTYP(fp_list_ptr_t)
ULS_QUALIFIED_METHOD(ulc_fp_push)(fp_list_ptr_t fp_lst, FILE *fp, const char* str)
{
	fp_list_ptr_t e;

	if (str == NULL) str = "";

	e = uls_alloc_object(fp_list_t);
	uls_init_namebuf(e->tagbuf, ULS_TAGNAM_MAXSIZ);
	uls_set_namebuf_value(e->tagbuf, str);
	e->linenum = 0;

	e->fp = fp;
	e->prev = fp_lst;

	return e;
}

FILE*
ULS_QUALIFIED_METHOD(ulc_fp_peek)(fp_list_ptr_t fp_lst, uls_ptrtype_tool(outparam) parms)
{
	FILE *fp;

	fp = fp_lst->fp;
	uls_assert(fp != NULL);
	if (parms != nilptr) {
		parms->line = uls_get_namebuf_value(fp_lst->tagbuf);
		parms->n = fp_lst->linenum;
	}

	return fp;
}

FILE*
ULS_QUALIFIED_METHOD(ulc_fp_pop)(uls_ptrtype_tool(outparam) parms, int do_export)
{
	fp_list_ptr_t fp_lst = (fp_list_ptr_t ) parms->data;
	FILE *fp;

	if (fp_lst != nilptr) {
		fp = fp_lst->fp;
		uls_assert(fp != NULL);

		if (parms->line != NULL) {
			_uls_tool_(strncpy)(parms->line, uls_get_namebuf_value(fp_lst->tagbuf), ULS_TAGNAM_MAXSIZ);
		}
		parms->n = fp_lst->linenum;
		parms->data = fp_lst->prev;

		if (do_export) {
			uls_deinit_namebuf(fp_lst->tagbuf);
			uls_dealloc_object(fp_lst);
		}
	} else {
		if (parms->line != NULL) {
			parms->line[0] = '\0';
		}
		parms->n = -1;
		parms->data = nilptr;
		fp = NULL;
	}

	return fp;
}

void
ULS_QUALIFIED_METHOD(release_ulc_fp_stack)(fp_list_ptr_t fp_lst)
{
	FILE *fp;
	uls_type_tool(outparam) parms;

	while (fp_lst != nilptr) {
		parms.data = fp_lst;
		parms.line = NULL;
		fp = ulc_fp_pop(uls_ptr(parms), 1);
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

	// tokdef_vx_rsvd[0..N_RESERVED_TOKS-1] is shared by tokdef_vx_array[]
	uls->tokdef_vx_rsvd.n = N_RESERVED_TOKS;
	uls->tokdef_vx_array.n = N_RESERVED_TOKS;

	/* LINENUM */
	e_vx = slots_rsv[LINENUM_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "LINENUM");
	e_vx->tok_id = uls->xcontext.toknum_LINENUM;

	/* NONE */
	e_vx = slots_rsv[NONE_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "NONE");
	e_vx->tok_id = uls->xcontext.toknum_NONE;

	/* ERR */
	e_vx = slots_rsv[ERR_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "ERR");
	e_vx->tok_id = uls->xcontext.toknum_ERR;

	/* EOI */
	e_vx = slots_rsv[EOI_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "EOI");
	e_vx->tok_id = uls->xcontext.toknum_EOI;

	/* EOF */
	e_vx = slots_rsv[EOF_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "EOF");
	e_vx->tok_id = uls->xcontext.toknum_EOF;

	/* ID */
	e_vx = slots_rsv[ID_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "ID");
	e_vx->tok_id = uls->xcontext.toknum_ID;

	/* NUMBER */
	e_vx = slots_rsv[NUM_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "NUMBER");
	e_vx->tok_id = uls->xcontext.toknum_NUMBER;

	/* TEMPLATE */
	e_vx = slots_rsv[TMPL_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "TMPL");
	e_vx->tok_id = uls->xcontext.toknum_TMPL;

	/* LINK */
	e_vx = slots_rsv[LINK_TOK_IDX];
	e_vx->l_name = uls_set_namebuf_value(e_vx->name, "LINK");
	e_vx->tok_id = uls->xcontext.toknum_LINK;

	for (i=0; i < N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		e_vx->extra_tokdef = nilptr;
		e_vx->base = nilptr;
		e_vx->tokdef_names = nilptr;
	}
}

int
ULS_QUALIFIED_METHOD(classify_tok_group)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_tokdef_vx_ptr_t e_vx;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	char *ch_ctx;
	int  ch, i, n;

	if (check_rsvd_toks(uls) < 0) {
		_uls_log(err_log)("%s: failed to check rsvd_toks", __func__);
		return -1;
	}

	if ((n=uls->tokdef_array.n) < uls->tokdef_array.n_alloc) {
		// shrink the size of uls->tokdef_array to the compact size.
		uls_resize_parray(uls_ptr(uls->tokdef_array), tokdef, n);
	}

	if ((n=uls->tokdef_vx_array.n) < uls->tokdef_vx_array.n_alloc) {
		// shrink the size of uls->tokdef_vx_array to the compact size.
		uls_resize_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, n);
	}

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	_uls_quicksort_vptr(slots_vx, uls->tokdef_vx_array.n, comp_vx_by_tokid);

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		if (is_str_contained_in_quotetypes(uls, uls_get_namebuf_value(cmt->start_mark))) {
			_uls_log(err_log)("comment-type '%s' is not proper as it's contained in one of the quote-types[]",
				uls_get_namebuf_value(cmt->start_mark));
			return -1;
		}
	}

	ch_ctx = uls->ch_context;
	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		ch = uls_get_namebuf_value(cmt->start_mark)[0];
		if (ch < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch] |= ULS_CH_COMM;
	}

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i = 0; i < uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		if (is_str_contained_in_commtypes(uls, uls_get_namebuf_value(qmt->start_mark))) {
			_uls_log(err_log)("quote-type '%s' is not proper as it's contained in one of the comm-types[]",
				uls_get_namebuf_value(qmt->start_mark));
			return -1;
		}

		if (qmt->flags & ULS_QSTR_NOTHING) {
			qmt->tok_id = uls->xcontext.toknum_NONE;
		}

		e_vx = uls_find_tokdef_vx(uls, qmt->tok_id);
		qmt->tokdef_vx = e_vx;

		ch = uls_get_namebuf_value(qmt->start_mark)[0];
		if (ch < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch] |= ULS_CH_QUOTE;
	}

	for (i = 1; i < ULS_SYNTAX_TABLE_SIZE; i++) {
		if (_uls_tool_(isgraph)(i) && ch_ctx[i] == 0) {
			ch_ctx[i] = ULS_CH_GUARD;
		}
	}
	return 0;
}

void
ULS_QUALIFIED_METHOD(assign_tok_group)(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = xctx->context;
	uls_tokdef_vx_ptr_t e_vx;
	char *ch_ctx;

	// LINENUM
	e_vx = slots_rsv[LINENUM_TOK_IDX];
	xctx->toknum_LINENUM = e_vx->tok_id;

	// NONE
	e_vx = slots_rsv[NONE_TOK_IDX];
	xctx->toknum_NONE = e_vx->tok_id;

	// ERR
	e_vx = slots_rsv[ERR_TOK_IDX];
	xctx->toknum_ERR = e_vx->tok_id;

	// EOI
	e_vx = slots_rsv[EOI_TOK_IDX];
	xctx->toknum_EOI = e_vx->tok_id;

	// EOF
	e_vx = slots_rsv[EOF_TOK_IDX];
	xctx->toknum_EOF = e_vx->tok_id;

	// ID
	e_vx = slots_rsv[ID_TOK_IDX];
	xctx->toknum_ID = e_vx->tok_id;

	// NUMBER
	e_vx = slots_rsv[NUM_TOK_IDX];
	xctx->toknum_NUMBER = e_vx->tok_id;

	// TMPL
	e_vx = slots_rsv[TMPL_TOK_IDX];
	xctx->toknum_TMPL = e_vx->tok_id;

	// LINK
	e_vx = slots_rsv[LINK_TOK_IDX];
	xctx->toknum_LINK = e_vx->tok_id;

	ch_ctx = uls->ch_context;

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

	xctx->ch_context = ch_ctx;

	xctx->commtypes = uls_ptr(uls->commtypes);
	xctx->n2_commtypes = uls->n1_commtypes;

	xctx->quotetypes = uls_ptr(uls->quotetypes);
	xctx->len_surplus = calc_len_surplus_recommended(uls);

	ctx->tok = xctx->toknum_EOI;
	ctx->tokbuf.buf[0] = '\0';
	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = ctx->s_val_wchars = 0;
}

int
ULS_QUALIFIED_METHOD(rearrange_tokname_of_twoplus)(uls_lex_ptr_t uls, int n_keys_twoplus)
{
	uls_kwtable_twoplus_ptr_t twotbl = uls_ptr(uls->twoplus_table);
	uls_decl_parray_slots(slots_tm, tokdef);
	uls_decl_parray_slots(slots_keyw, tokdef);

	uls_tokdef_ptr_t e;
	int i, n1 = 0;

	uls_init_parray(uls_ptr(twotbl->twoplus_mempool), tokdef, n_keys_twoplus);

	slots_tm = uls_parray_slots(uls_ptr(twotbl->twoplus_mempool));
	slots_keyw = uls_parray_slots(uls_ptr(uls->tokdef_array));

	for (i=0; i < uls->tokdef_array.n; i++) {
		e = slots_keyw[i];

		if (e->keyw_type == ULS_KEYW_TYPE_TWOPLUS) { /* tokdef_twoplus */
			slots_tm[n1++] = e;
		}
	}

	twotbl->twoplus_mempool.n = n1;
	return n1;
}

int
ULS_QUALIFIED_METHOD(calc_len_surplus_recommended)(uls_lex_ptr_t uls)
{
	int i, len, len_surplus = ULS_UTF8_CH_MAXLEN;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		if ((len = cmt->len_end_mark) > len_surplus)
			len_surplus = len;
	}

	if (uls->n1_commtypes > 0) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), 0);
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

void
ULS_QUALIFIED_METHOD(uls_want_eof)(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags |= ULS_CTX_FL_WANT_EOFTOK;

	if (__uls_tok(uls) == uls->xcontext.toknum_EOF) {
		_uls_log(err_log)("The current token is already EOF!");
	}
}

void
ULS_QUALIFIED_METHOD(uls_unwant_eof)(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags &= ~ULS_CTX_FL_WANT_EOFTOK;
}

int
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

int
ULS_QUALIFIED_METHOD(uls_get_spectype)(const char *filepath, uls_ptrtype_tool(outparam) parms)
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
		if (_uls_tool(is_path_prefix)(filepath) != 0 || uls_is_valid_specpath(filepath) < 0) {
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

void
ULS_QUALIFIED_METHOD(ulc_set_searchpath)(const char *pathlist)
{
	uls_mfree(_uls_sysinfo_(ULC_SEARCH_PATH));
	if (pathlist != NULL) {
		_uls_sysinfo_(ULC_SEARCH_PATH) = _uls_tool_(strdup)(pathlist, -1);
	}
}

int
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

int
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

void
ULS_QUALIFIED_METHOD(ulc_list_searchpath)(const char* confname)
{
	uls_type_tool(arglst) title_list;
	uls_type_tool(arglst) searchpath_list;

	char  specname[ULC_LONGNAME_MAXSIZ+1], fpath_buff[ULC_LONGNAME_MAXSIZ+5], ch;
	const char *filename;
	int len_basedir, len_specname, n, k, typ_fpath;
	uls_type_tool(outparam) parms1;

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
		n = ulc_get_searchpath_by_specpath(_uls_tool(is_absolute_path)(confname),
			uls_ptr(title_list), uls_ptr(searchpath_list));
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
		_uls_log(err_log)("Notice ulc/ulf/uld file must be utf-8 encoded text file with BOM!");
		return -1;
	}

	return magic_code_len;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ulc_fileformat_ver)(char *linebuff, int linelen, uls_ptrtype_tool(version) ver1)
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
ULS_QUALIFIED_METHOD(check_ulc_file_magic)(FILE* fin, uls_ptrtype_tool(version) sysver, char *ulc_lname)
{
	int  linelen, len1, len2, magic_code_len;
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr, *lptr1;
	uls_type_tool(version) ver1;
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

int
ULS_QUALIFIED_METHOD(ulc_read_header)(FILE* fin, ulc_header_ptr_t hdr)
{
	uls_lex_ptr_t uls = hdr->uls;
	fp_list_ptr_t  fp_stack_ptr, fp_stack_top;
	char  specname[ULC_LONGNAME_MAXSIZ+1];
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr;
	char *tag, tagbuf1[ULS_TAGNAM_MAXSIZ + 1];
	char  ulc_lname[ULC_LONGNAME_MAXSIZ+1];
	int   linelen, lno;
	int   rc, ulc_lname_len, len_basedir, len_specname, typ_fpath;
	uls_type_tool(outparam) parms1;

	linelen = _uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0);
	if (linelen <= ULS_EOF || get_ulc_fileformat_ver(linebuff, linelen, uls_ptr(hdr->filever)) < 0) {
		_uls_log(err_log)("Can't get file format version!");
		return -1;
	}
	rewind(fin);

	// Read Header upto '%%'
	for (fp_stack_top = (fp_list_ptr_t) hdr->fp_stack; ; ) {
		if ((ulc_lname_len = check_ulc_file_magic(fin, uls_ptr((uls)->config_filever), ulc_lname)) <= 0) {
			if (ulc_lname_len < 0) {
				_uls_log(err_log)("%s: Can't find the header of ulc",
					uls_get_namebuf_value(fp_stack_top->tagbuf));
				return -1;
			}
			fp_stack_top->linenum = 1;
			break;
		}

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

	hdr->fp_stack = fp_stack_ptr = fp_stack_top;

	tag = uls_get_namebuf_value(fp_stack_ptr->tagbuf);
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
			parms1.line = tagbuf1;
			ulc_fp_pop(uls_ptr(parms1), 0);
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

		if ((rc = read_config_var(hdr, tag, lno, lptr)) <= 0) {
			if (rc < 0) return -1;
			break;
		}
	}

	return lno;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(gen_next_tok_id)(ulc_header_ptr_t hdr,
	uls_tokdef_vx_ptr_t e_vx_grp, const char *tok_idstr, int ch_kwd)
{
	int tok_id;

	if (*tok_idstr != '\0') {
		tok_id = _uls_tool_(atoi)(tok_idstr);
		hdr->tok_id_seed = tok_id + 1;
	} else {
		if (e_vx_grp != nilptr) {
			tok_id = e_vx_grp->tok_id;
		} else {
			if (ch_kwd > 0) {
				tok_id = ch_kwd;
			} else {
				tok_id = hdr->tok_id_seed++;
			}
		}
	}

	return tok_id;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(__new_regular_tokdef)(int keyw_type, const char *keyw, int ulen, int wlen)
{
	uls_tokdef_ptr_t e = uls_create_tokdef();

	e->keyw_type = keyw_type;
	uls_set_namebuf_value(e->keyword, keyw);
	e->ulen_keyword = ulen;
	e->wlen_keyword = wlen;

	return e;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__ulc_proc_line__reserved)(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *keyw, const char *tok_idstr)
{
	uls_lex_ptr_t uls = hdr->uls;
	const char *tag_nam = uls_get_namebuf_value(hdr->tagbuf);
	int tok_id, rsv_idx, lno = hdr->lno;
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_rsv, tokdef_vx);

	tok_id = gen_next_tok_id(hdr, nilptr, tok_idstr, 0);

	if ((e_vx = __find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) != nilptr) {
		_uls_log(err_log)("In %s<%d>,", tag_nam, lno);
		_uls_log(err_log)("\tthe token('%s') is already used by %s.", tok_nam, uls_get_namebuf_value(e_vx->name));
		e_vx = nilptr;
	} else {
		rsv_idx = is_reserved_tok(uls, tok_nam);
		slots_rsv = uls_parray_slots(uls_ptr(uls->tokdef_vx_rsvd));
		e_vx = slots_rsv[rsv_idx];
		e_vx->tok_id = tok_id;
	}

	return e_vx;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__ulc_proc_line__1char)(ulc_header_ptr_t hdr,
	const char *tok_nam, int ch_kwd, const char *tok_idstr)
{
	uls_lex_ptr_t uls = hdr->uls;
	uls_onechar_table_ptr_t tbl = uls_ptr(uls->onechar_table);
	const char *tag_nam = uls_get_namebuf_value(hdr->tagbuf);
	int tok_id, lno = hdr->lno;

	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx, e2_vx;
	uls_tokdef_outparam_t parms2;

	if ((e_vx = __find_reg_vx_by_name(uls, tok_nam)) != nilptr) {
		if (!(e_vx->flags & ULS_VX_REFERRED)) e_vx->flags |= ULS_VX_REFERRED;

		if (*tok_idstr != '\0') {
			tok_id = _uls_tool_(atoi)(tok_idstr);
			if (e_vx->tok_id != tok_id) {
				_uls_log(err_log)("%s<%d>: token-id mismatch:", lno, tag_nam);
				_uls_log(err_log)("\ttoken %s:%d", tok_nam, tok_id);
				return nilptr;
			}
		} else {
			tok_id = e_vx->tok_id;
		}

	} else {
		tok_id = gen_next_tok_id(hdr, nilptr, tok_idstr, ch_kwd);

		if ((e_vx = __find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) == nilptr) {
			e_vx = uls_create_tokdef_vx(tok_id, "", nilptr);
			if (ch_kwd == tok_id) {
				e_vx->flags |= ULS_VX_CHRMAP;
			} else {
				e_vx->flags |= ULS_VX_REFERRED;
			}

			realloc_tokdef_array(uls, 1, 0);
			slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
			slots_vx[uls->tokdef_vx_array.n++] = e_vx;
		} else {
			if (!(e_vx->flags & ULS_VX_REFERRED)) e_vx->flags |= ULS_VX_REFERRED;
		}

		uls_add_tokdef_vx_name(e_vx, tok_nam);
	}

	if ((e2_vx = uls_find_1char_tokdef_vx(tbl, ch_kwd, uls_ptr(parms2))) == nilptr) {
		if (parms2.tokgrp != nilptr) {
			insert_1char_tokdef_map(parms2.tokgrp, ch_kwd, e_vx);
		} else {
			insert_1char_tokdef_etc(tbl, ch_kwd, e_vx);
		}

	} else if (e2_vx->tok_id != e_vx->tok_id || e2_vx != e_vx) {
		_uls_log(err_log)("%s<%d>: token is already occuppied by another", tag_nam, lno);
		_uls_log(err_log)("\ttoken:%s :: %s", tok_nam, uls_get_namebuf_value(e_vx->name));
		return nilptr;
	}

	if (ch_kwd == '\n') {
		uls->flags |= ULS_FL_LF_CHAR;
		uls->ch_context['\n'] |= ULS_CH_1;
	} else if (ch_kwd == '\t') {
		uls->flags |= ULS_FL_TAB_CHAR;
		uls->ch_context['\t'] |= ULS_CH_1;
	}

	return e_vx;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(check_tokid_duplicity)(const char* tag_nam, int lno,
	int tok_id, uls_tokdef_vx_ptr_t e_vx_grp, uls_lex_ptr_t uls)
{
	int stat = 1;

	if (__find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_RSVD) != nilptr) {
		_uls_log(err_log)("%s<%d>: Aliasing of token isn't permitted:", tag_nam, lno);
		_uls_log(err_log)("\ttok_id(%d)", tok_id);
		stat = 0;
	} else if (e_vx_grp != nilptr) {
		if (e_vx_grp->tok_id != tok_id) {
			_uls_log(err_log)("%s<%d>: The keyword conflicts with the previous designation for token:", tag_nam, lno);
			_uls_log(err_log)("\ttoken %s(%d)", uls_get_namebuf_value(e_vx_grp->name), e_vx_grp->tok_id);
			stat = 0;
		}
	}

	return stat;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__ulc_proc_line__userdef)(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *tok_idstr)
{
	uls_lex_ptr_t uls = hdr->uls;
	const char *tag_nam = uls_get_namebuf_value(hdr->tagbuf);
	int tok_id, lno = hdr->lno;

	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = __find_vx_by_name(uls, tok_nam, 0, -1)) != nilptr) {
		if (*tok_idstr != '\0') {
			tok_id = _uls_tool_(atoi)(tok_idstr);
			if (e_vx->tok_id != tok_id) {
				_uls_log(err_log)("%s<%d>: token-id mismatch:", tag_nam, lno);
				_uls_log(err_log)("\ttoken %s:%d", tok_nam, tok_id);
				return nilptr;
			}
		} else {
			tok_id = e_vx->tok_id;
		}

	} else {
		tok_id = gen_next_tok_id(hdr, nilptr, tok_idstr, 0);
		e_vx = __find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_BOTH);
		if (e_vx == nilptr) {
			e_vx = uls_create_tokdef_vx(tok_id, tok_nam, nilptr);
			realloc_tokdef_array(uls, 1, 0);
			slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
			slots_vx[uls->tokdef_vx_array.n++] = e_vx;
		} else {
			uls_add_tokdef_vx_name(e_vx, tok_nam);
		}
	}

	hdr->e_ret = nilptr;
	return e_vx;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__ulc_proc_line__regular)(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *keyw, const char *tok_idstr)
{
	uls_lex_ptr_t uls = hdr->uls;
	const char *tag_nam = uls_get_namebuf_value(hdr->tagbuf);
	int tok_id, lno = hdr->lno;
	int ulen, wlen, keyw_type = hdr->keyw_type;
	char *ch_ctx, ch_kwd = keyw[0];

	uls_decl_parray_slots(slots_keyw, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);

	uls_tokdef_vx_ptr_t e_vx, e_vx_grp;
	uls_tokdef_ptr_t e;

	ulen = _uls_tool_(strlen)(keyw);
	wlen = _uls_tool(ustr_num_wchars)(keyw, ulen, nilptr);

	e_vx = e_vx_grp = __find_reg_vx_by_name(uls, tok_nam);
	if (e_vx != nilptr) {
		if (*tok_idstr != '\0') {
			tok_id = _uls_tool_(atoi)(tok_idstr);
			if (e_vx->tok_id != tok_id) {
				_uls_log(err_log)("%s<%d>: token-id mismatch:", tag_nam, lno);
				_uls_log(err_log)("\ttoken %s:%d", tok_nam, tok_id);
				return nilptr;
			}
		} else {
			tok_id = e_vx->tok_id;
		}

	} else {
		tok_id = gen_next_tok_id(hdr, nilptr, tok_idstr, 0);
		e_vx = __find_vx_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR);
	}

	if (!check_tokid_duplicity(tag_nam, lno, tok_id, e_vx_grp, uls)) {
		_uls_log(err_log)("\ttoken %s:%d token-duplicity", tok_nam, tok_id);
		return nilptr;
	}

	if (ulen > 0) {
	 	if ((e = __find_tokdef_by_keyw(uls, keyw)) != nilptr) {
			e_vx = e->view;
		} else {
			realloc_tokdef_array(uls, 1, 1);
			slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
			slots_keyw = uls_parray_slots(uls_ptr(uls->tokdef_array));

			e = __new_regular_tokdef(keyw_type, keyw, ulen, wlen);
			slots_keyw[uls->tokdef_array.n++] = e;

			if (e_vx == nilptr) {
				e_vx = e_vx_grp = uls_create_tokdef_vx(tok_id, tok_nam, e);
				slots_vx[uls->tokdef_vx_array.n++] = e_vx;
			} else {
				e->view = e_vx;
				e_vx->base = e;
			}
		}
	} else {
		e = nilptr;
	}

	if (e_vx_grp == nilptr) { // name-group
		uls_add_tokdef_vx_name(e_vx, tok_nam);
	}

	ch_ctx = uls->ch_context;
	// set ch_context map
	if (keyw_type == ULS_KEYW_TYPE_TWOPLUS) {
		if (ch_kwd < ULS_SYNTAX_TABLE_SIZE) {
			ch_ctx[ch_kwd] |= ULS_CH_2PLUS;
			if (uls->flags & ULS_FL_CASE_INSENSITIVE) {
				ch_ctx[_uls_tool_(toupper)(ch_kwd)] |= ULS_CH_2PLUS;
			}
		}
		if (wlen > ULS_TWOPLUS_WMAXLEN) {
			_uls_log(err_log)("%s<%d>: Too long punctuation token:", tag_nam, lno);
			_uls_log(err_log)("\tlength(%d), keyword('%s')", wlen, keyw);
			return nilptr;
		}
	}

	if (keyw_type == ULS_KEYW_TYPE_TWOPLUS) {
		++hdr->n_keys_twoplus;
	} else if (keyw_type == ULS_KEYW_TYPE_IDSTR) {
		++hdr->n_keys_idstr;
	}

	hdr->e_ret = e;
	return e_vx;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(ulc_proc_line)(ulc_header_ptr_t hdr,
	char* line, uls_ptrtype_tool(outparam) parms)
{
	uls_lex_ptr_t uls = hdr->uls;
	const char *tag_nam = uls_get_namebuf_value(hdr->tagbuf);
	int lno = hdr->lno;

	char *wrd1, *wrd2, *wrd3;
	char wrdbuf2[ULS_LEXSTR_MAXSIZ+1];
	int  n_wchars, keyw_type;
	uls_wch_t wch;

	uls_tokdef_vx_ptr_t e_vx;
	uls_type_tool(outparam) parms1;
	uls_type_tool(wrd) wrdx;

	parms->data = nilptr;
	wrdx.lptr = line;

/* token-name */
	wrd1 = __uls_tool_(splitstr)(uls_ptr(wrdx));
	if (canbe_tokname(wrd1) <= 0) {
		_uls_log(err_log)("%s<%d>: Nil-string or too long token constant:", tag_nam, lno);
		_uls_log(err_log)("\ttoken-name('%s')", wrd1);
		return nilptr;
	}

/* keyword */
	wrd2 = __uls_tool_(splitstr)(uls_ptr(wrdx));
	wrdbuf2[0] = '\0';

	if (is_reserved_tok(uls, wrd1) >= 0) {
		keyw_type = ULS_KEYW_TYPE_RESERVED;
	} else {
		parms1.data = uls;
		parms1.line = wrdbuf2;

		if ((keyw_type = check_keyw_str(lno, wrd2, uls_ptr(parms1))) < 0) {
			_uls_log(err_log)("%s<%d>: Invalid keyword:", tag_nam, lno);
			_uls_log(err_log)("\tkeyword('%s')", wrd2);
			return nilptr;
		}
	}

	hdr->keyw_type = keyw_type;
	hdr->e_ret = nilptr;

	if (keyw_type == ULS_KEYW_TYPE_USER) {
		wrd3 = wrd2;
		wrd2 = wrdbuf2;
		e_vx = __ulc_proc_line__userdef(hdr, wrd1, wrd3);

	} else if (keyw_type == ULS_KEYW_TYPE_1CHAR) {
		wrd2 = wrdbuf2;
		wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		_uls_tool_(decode_utf8)(wrd2, -1, &wch);
		e_vx = __ulc_proc_line__1char(hdr, wrd1, wch, wrd3);

	} else if (keyw_type == ULS_KEYW_TYPE_RESERVED) {
		if (_uls_tool(is_pure_integer)(wrd2, nilptr) > 0) {
			wrd3 = wrd2;
			wrd2 = wrdbuf2;
		} else {
			wrd2 = wrdbuf2;
			wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		}
		e_vx = __ulc_proc_line__reserved(hdr, wrd1, wrd2, wrd3);

	} else {
		wrd2 = wrdbuf2;
		wrd3 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		if (keyw_type == ULS_KEYW_TYPE_TWOPLUS) {
			n_wchars = _uls_tool(ustr_num_wchars)(wrd2, -1, nilptr);
			if (n_wchars > ULS_TWOPLUS_WMAXLEN) {
				_uls_log(err_log)("#%d: too long for punctuation token", lno);
				_uls_log(err_log)("\ttoken('%s'), wide-length(%d)", wrd2, n_wchars);
				return nilptr;
			}
		}
		e_vx = __ulc_proc_line__regular(hdr, wrd1, wrd2, wrd3);
	}

	parms->data = hdr->e_ret;
	return e_vx;
}

ULS_QUALIFIED_RETTYP(uls_xcontext_ptr_t)
ULS_QUALIFIED_METHOD(_uls_get_xcontext)(uls_lex_ptr_t uls)
{
	return uls_ptr(uls->xcontext);
}
