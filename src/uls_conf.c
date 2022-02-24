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

ULS_DECL_STATIC int
check_keyw_str(int lno, const char* str, uls_outparam_ptr_t parms)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t ) parms->data;
	char *ch_ctx = uls->ch_context, *buf = parms->line, ch;
	int case_insensitive = uls->flags & ULS_FL_CASE_INSENSITIVE;
	const char *ptr;
	int  i, len, rc;
	int  n_ch_quotes, n_ch_comms, n_ch_non_idstr;
	int  n_lfs, n_tabs;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_uch_t uch;
	uls_litstr_t lit1;
	uls_decl_parray_slots(slots_qmt, quotetype);

	if (*str == '\0' || is_pure_int_number(str) > 0) {
		*buf = '\0';
		parms->len = 0;
		return ULS_KEYW_TYPE_USER;
	}

	n_ch_quotes = n_ch_comms = n_ch_non_idstr = 0;
	n_lfs = n_tabs = 0;
	for (len=0, ptr=str; (ch=*ptr)!='\0'; ) {
		if (len > ULS_LEXSTR_MAXSIZ) {
			err_log("#%d: Too long keyword '%s'", lno, str);
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
					uch = uls_toupper(uch);
				}
				if ((rc = uls_encode_utf8(uch, buf+len, ULS_LEXSTR_MAXSIZ-len)) <= 0) {
					err_log("#%d: encoding error!", lno);
					return -1;
				}
				len += rc;
				ptr = lit1.lptr;
			}

		} else {
			if (case_insensitive) ch = uls_toupper(ch);
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
				err_log("Unicode char in keyword not permitted!");
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
			if ((rc = uls_decode_utf8(ptr, -1, NULL)) <= 0) {
				err_log("#%d: encoding error!", lno);
				return -1;
			}

			if ((ch=*ptr) == ' ') {
				err_log("#%d: contains illegal char in keyword", lno);
				return -1;
			}

			if (ch == '\n') ++n_lfs;
			else if (ch == '\t') ++n_tabs;

			if (ch < ULS_SYNTAX_TABLE_SIZE && (ch_ctx[ch] & ULS_CH_QUOTE)) ++n_ch_quotes;
			if (ch < ULS_SYNTAX_TABLE_SIZE && (ch_ctx[ch] & ULS_CH_COMM)) ++n_ch_comms;

		} while (*(ptr += rc) != '\0');

		n_ch_non_idstr = 1;
	}

	if (len > 1 && (n_lfs > 0 || n_tabs > 0)) {
		err_log("#%d: contains illegal chars in keyword.", lno);
		return -1;
	}

	if (n_ch_comms > 0) {
		for (i=0; i<uls->n1_commtypes; i++) {
			cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
			if (uls_strstr(buf, uls_get_namebuf_value(cmt->start_mark)) != NULL) {
				err_log("#%d: keyword has the comment-string '%s'", lno, uls_get_namebuf_value(cmt->start_mark));
				return -1;
			}
		}
	}

	if (n_ch_quotes > 0) {
		slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
		for (i=0; i<uls->quotetypes.n; i++) {
			qmt = slots_qmt[i];
			if (uls_strstr(buf, uls_get_namebuf_value(qmt->start_mark)) != NULL) {
				err_log("#%d: keyword has the quote-string '%s'", lno, uls_get_namebuf_value(qmt->start_mark));
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
			err_log("#%d: %s too long for 2-chars token", lno, buf);
			return -1;
		}
	}

	return rc;
}

ULS_DECL_STATIC uls_tokdef_vx_ptr_t
__find_tokdef_by_tokid(uls_lex_ptr_t uls, int t, int area)
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
		err_log("%s: unknown area:%d", __FUNCTION__, area);
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

ULS_DECL_STATIC uls_tokdef_vx_ptr_t
__find_rsvd_tokdef_by_name(uls_lex_ptr_t uls, const char* name)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t  e_vx, e0_vx=nilptr;
	int i;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e_vx = slots_rsv[i];
		if (uls_streql(uls_get_namebuf_value(e_vx->name), name)) {
			e0_vx = e_vx;
			break;
		}
	}

	return e0_vx;
}

ULS_DECL_STATIC uls_tokdef_vx_ptr_t
__find_reg_tokdef_by_name(uls_lex_ptr_t uls, const char* name, uls_outparam_ptr_t parms)
{
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_name_ptr_t e_nam;
	int i;

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));

	for (i=N_RESERVED_TOKS; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (uls_streql(uls_get_namebuf_value(e_vx->name), name)) {
			parms->data = nilptr;
			return e_vx;
		}
	}

	for (i=N_RESERVED_TOKS; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		for (e_nam=e_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->prev) {
			if (uls_streql(uls_get_namebuf_value(e_nam->name), name)) {
				parms->data = e_nam;
				return e_vx;
			}
		}
	}

	parms->data = nilptr;
	return nilptr;
}

ULS_DECL_STATIC uls_tokdef_ptr_t
__find_tokdef_by_keyw(uls_lex_ptr_t uls, char* keyw)
{
	uls_decl_parray_slots_init(slots_tok, tokdef, uls_ptr(uls->tokdef_array));
	uls_tokdef_ptr_t e;
	int i;

	for (i=0; i<uls->tokdef_array.n; i++) {
		e = slots_tok[i];
		if (uls_streql(uls_get_namebuf_value(e->keyword), keyw)) {
			return e;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC int
is_str_contained_in_commtypes(uls_lex_ptr_t uls, const char *str)
{
	uls_commtype_ptr_t cmt;
	int i, l_str;

	if ((l_str = uls_strlen(str)) <= 0) return 0;

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (l_str <= cmt->len_start_mark && !uls_strncmp(str, uls_get_namebuf_value(cmt->start_mark), l_str)) {
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC int
is_str_contained_in_quotetypes(uls_lex_ptr_t uls, const char *str)
{
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	int i, l_str;

	if ((l_str = uls_strlen(str)) <= 0) return 0;

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		// qmt->len_start_mark > 0

		if (l_str <= qmt->len_start_mark && !uls_strncmp(str, uls_get_namebuf_value(qmt->start_mark), l_str)) {
			return 1;
		}
	}

	return 0;
}

ULS_DECL_STATIC void
realloc_tokdef_array(uls_lex_ptr_t uls, int n1, int n2)
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
make_tokdef_for_quotetype(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, const char *qmt_name)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_decl_parray_slots(slots_tok, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_tokdef_ptr_t e;
	int tok_id;
	uls_outparam_t parms;

	tok_id = qmt->tok_id;

	if (__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_BOTH) != nilptr) {
		err_log("%s: the token-name already made for token", __FUNCTION__);
		err_log("\ttoken-name:%s, token:%d", qmt_name, tok_id);
		return -1;
	}

	e = uls_create_tokdef();
	e->keyw_type = ULS_KEYW_TYPE_LITERAL;

	uls_set_namebuf_value(e->keyword, uls_get_namebuf_value(qmt->start_mark));
	e->l_keyword = qmt->len_start_mark;

	realloc_tokdef_array(uls, 1, 1);
	slots_tok = uls_parray_slots(uls_ptr(uls->tokdef_array));
	slots_tok[uls->tokdef_array.n] = e;

	if (qmt_name[0] != '\0' && (__find_reg_tokdef_by_name(uls, qmt_name, uls_ptr(parms)) != nilptr ||
		__find_rsvd_tokdef_by_name(uls, qmt_name) != nilptr)) {
		err_log("%s: can't make a tok name for token", __FUNCTION__);
		err_log("\ttoken-name:%s, token:%d", qmt_name, tok_id);
		return -1;
	}

	e_vx = uls_create_tokdef_vx(tok_id, qmt_name, e);

	++uls->tokdef_array.n;
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	slots_vx[uls->tokdef_vx_array.n++] = e_vx;

	return 0;
}

ULS_DECL_STATIC char*
find_cnst_suffix(char* cstr_pool, const char* str, int l_str, uls_outparam_ptr_t parms)
{
	char *ptr, *ret_ptr = NULL;
	int l;

	for (ptr=cstr_pool; *ptr!='\0'; ptr+=l+1) {
		if ((l=uls_strlen(ptr)) == l_str && uls_memcmp(ptr, str, l_str) == 0) {
			ret_ptr = ptr;
			break;
		}
	}

	parms->line = ptr;
	return ret_ptr; // the ptr corresponding to 'str'
}

int
ulx_add_config_number_suffixes(uls_lex_ptr_t uls, const char *suffix)
{
	char* cnst_suffixes = uls->numcnst_suffixes;
	uls_outparam_t parms;
	const char *srcptr;
	char ch, *dstptr, *ptr;
	int len, k;

	len = uls_strlen(suffix);
	if (find_cnst_suffix(cnst_suffixes, suffix, len, uls_ptr(parms)) != NULL) {
		return -1;
	}

	ptr = parms.line;
	k = (int) (ptr - cnst_suffixes);
	if (ULS_CNST_SUFFIXES_MAXSIZ - k < len + 1) {
		return -1;
	}

	dstptr = cnst_suffixes + k;
	for (srcptr=suffix; (ch=*srcptr) != '\0'; srcptr++) {
		if (!uls_isgraph(ch) || uls_isdigit(ch)) return 0;
		*dstptr++ = ch;
	}

	*dstptr++ = '\0';
	*dstptr = '\0';

	return 0;
}

ULS_DECL_STATIC int
add_cnst_suffix(uls_lex_ptr_t uls, const char* suffix)
{
	int len, stat = 1;

	if (suffix == NULL || (len = uls_strlen(suffix)) == 0 || len > ULS_CNST_SUFFIX_MAXSIZ) {
		stat = 0;
	} else if (ulx_add_config_number_suffixes(uls, suffix) < 0) {
		stat = 0;
	}

	return stat;
}

ULS_DECL_STATIC int
cnst_suffixes_by_len(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_argstr_ptr_t e1 = (const uls_argstr_ptr_t) a;
	const uls_argstr_ptr_t e2 = (const uls_argstr_ptr_t) b;

	int len1, len2, stat;

	len1 = uls_strlen(e1->str);
	len2 = uls_strlen(e2->str);

	if (len1  < len2) stat = 1;
	else if (len1  > len2) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DECL_STATIC int
check_validity_of_cnst_suffix(uls_arglst_ptr_t cnst_suffixes, uls_lex_ptr_t uls)
{
	int n_cnst_suffixes = cnst_suffixes->args.n;
	uls_decl_parray_slots_init(al, argstr, uls_ptr(cnst_suffixes->args));
	uls_argstr_ptr_t  arg;
	char *ptr, ch;
	int i, j;

	for (i=0; i<n_cnst_suffixes; i++) {
		arg = al[i];
		ptr = arg->str;

		for (j=0; (ch=ptr[j]) != '\0'; j++) {
			if (!uls_isgraph(ch) || uls_isdigit(ch)) {
				err_log("An invalid constant suffix found. ignoring '%s' ...", ptr);
				uls_destroy_argstr(arg); al[i] =  nilptr;
			}
		}

		if (j == 0) {
			uls_destroy_argstr(arg); al[i] =  nilptr;
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
is_commstart_valid(uls_lex_ptr_t uls, int k)
{
	uls_commtype_ptr_t cmt, cmt0 = uls_array_get_slot_type01(uls_ptr(uls->commtypes), k);
	char *str = uls_get_namebuf_value(cmt0->start_mark);
	int i, l_str = cmt0->len_start_mark;

	for (i=0; i<uls->n1_commtypes; i++) {
		if (i == k) continue;

		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
		// cmt->len_start_mark > 0

		if (cmt->len_start_mark == l_str && uls_streql(uls_get_namebuf_value(cmt->start_mark), str)) {
			return 0;
		}
	}

	return 1;
}

ULS_DECL_STATIC int
is_quotestart_valid(uls_lex_ptr_t uls, int k)
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
__set_config_comment_type(uls_commtype_ptr_t cmt, int flags,
	const char *mark1, int len_mark1, int lfs_mark1,
	const char *mark2, int len_mark2, int lfs_mark2)
{
	char cmt_mark_buff[ULS_COMM_MARK_MAXSIZ + 2];
	const char *cptr;

	if (flags & ULS_COMM_COLUMN0) {
		// prepend a line-feed at the front of 'mark1'
		cmt_mark_buff[0] = '\n';
		uls_strcpy(cmt_mark_buff + 1, mark1);
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

void
ulx_set_comment_type(uls_lex_ptr_t uls, const char *mark1, const char *mark2)
{
	uls_commtype_ptr_t cmt;
	int len1, len2, n1, n2;
	char ch;

	for (n1=len1=0; (ch=mark1[len1]) != '\0'; len1++) {
		if (ch == '\n') ++n1;
	}

	if (mark2 != NULL) {
		for (n2=len2=0; (ch=mark2[len2]) != '\0'; len2++) {
			if (ch == '\n') ++n2;
		}
	} else {
		mark2 = mark1;
		len2 = len1;
		n2 = n1;
	}

	if (uls->n1_commtypes >= ULS_N_MAX_COMMTYPES) {
		err_log("Too many comment types defined!");
		return;
	}

	cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), uls->n1_commtypes);
	__set_config_comment_type(cmt, 0, mark1, len1, n1, mark2, len2, n2);
	++uls->n1_commtypes;
}

ULS_DECL_STATIC int
read_config__COMMENT_TYPE(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end, *cptr;
	int lno = parms->n;
	uls_wrd_t wrdx;
	int  j, k, len, cmt_flags=0;
	char  *wrd, *wrdptr;

	uls_outparam_t parms1, parms2;
	uls_commtype_ptr_t cmt;

	char cmt_mark1[ULS_COMM_MARK_MAXSIZ+1];
	char cmt_mark2[ULS_COMM_MARK_MAXSIZ+1];

	wrdx.lptr = line;
	wrd = _uls_splitstr(uls_ptr(wrdx));

	if (!uls_strncmp(wrd, "options=", 8)) {
		for (wrdptr = wrd + 8; wrdptr != NULL; ) {
			wrd = wrdptr;
			if ((wrdptr = uls_strchr(wrdptr, ',')) != NULL) {
				*wrdptr++ = '\0';
			}
			if (uls_streql(wrd, "oneline")) {
				cmt_flags |= ULS_COMM_ONELINE;
			} else if (uls_streql(wrd, "column0")) {
				cmt_flags |= ULS_COMM_COLUMN0;
			} else if (uls_streql(wrd, "nested")) {
				cmt_flags |= ULS_COMM_NESTED;
			} else {
				err_log("%s<%d>: unknown attribute for a comment type", tag_nam, lno);
				err_log(" attribute '%s'", wrd);
				return -1;
			}
		}
		wrd = _uls_splitstr(uls_ptr(wrdx));
	}

	parms1.line = cmt_mark1;
	if ((len=uls_strlen(wrd)) == 0 || len > ULS_COMM_MARK_MAXSIZ ||
		!canbe_commtype_mark(wrd, uls_ptr(parms1))) {
		err_log("%s<%d>: Too short or long comment (start) mark, or not permitted chars.",
			tag_nam, lno);
		err_log("\t:'%s'", wrd);
		return -1;
	}

	if (cmt_flags & ULS_COMM_ONELINE) {
		cmt_mark2[0] = '\n'; cmt_mark2[1] = '\0';
		parms2.line = cmt_mark2;
		parms2.len = parms2.n = 1;
	} else {
		// the closing comment string of 'start_mark'
		parms2.line = cmt_mark2;
		if ((len=uls_strlen(wrd = _uls_splitstr(uls_ptr(wrdx))))==0 || len > ULS_COMM_MARK_MAXSIZ ||
			!canbe_commtype_mark(wrd, uls_ptr(parms2))) {
			err_log("%s<%d>: Too short or long comment (end) mark, or not permitted chars.",
				tag_nam, lno);
			err_log("\t:'%s'", wrd);
			return -1;
		}
	}

	// Find the 'cmt' having comment start string.
	for (j=0; ; j++) {
		if (j >= uls->n1_commtypes) {
			// not found: add it as new one.
			if (uls->n1_commtypes >= ULS_N_MAX_COMMTYPES) {
				err_log("%s<%d>: Too many comment types", tag_nam, lno);
				return -1;
			}
			k = uls->n1_commtypes++;
			break;
		}
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), j);
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

	cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), k);
	__set_config_comment_type(cmt, cmt_flags, cmt_mark1, parms1.len, parms1.n, cmt_mark2, parms2.len, parms2.n);

	if (!is_commstart_valid(uls, k)) {
		err_log("%s<%d>: the comm-start-mark is collided with previous defined comment-type.", tag_nam, lno);
		err_log("\t:'%s'", uls_get_namebuf_value(cmt->start_mark));
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
parse_quotetype__options(char *line, uls_quotetype_ptr_t qmt)
{
	char *lptr, *wrd;
	int n=0;

	for (lptr = line; lptr != NULL; ) {
		wrd = lptr;
		if ((lptr = uls_strchr(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}

		if (uls_streql(wrd, "asymmetric")) {
			qmt->flags |= ULS_QSTR_ASYMMETRIC;
		} else if (uls_streql(wrd, "nothing")) {
			qmt->litstr_analyzer = uls_ref_callback(nothing_lit_analyzer);
			qmt->flags |= ULS_QSTR_NOTHING;
		} else if (uls_streql(wrd, "open")) {
			qmt->litstr_analyzer = uls_ref_callback(nothing_lit_analyzer);
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
parse_quotetype__token(char *line, uls_quotetype_ptr_t qmt, uls_outparam_ptr_t parms)
{
	char *lptr=line, *wrd, *tok_nam=parms->line;
	int len;

	tok_nam[0] = '\0';
	parms->n = parms->len = 0;

	wrd = lptr;
	if ((lptr = uls_strchr(lptr, ',')) != NULL) {
		*lptr++ = '\0';
	}

	if ((len = uls_strlen(wrd)) > 0) {
		if (uls_isalpha(wrd[0])) {
			lptr = wrd;
		}
		if (uls_isdigit(wrd[0]) || wrd[0] == '-') {
			if (len > 1 || wrd[0] != '-') {
				parms->n = uls_atoi(wrd);
				parms->flags = 1; // tok_id specified
			}
		}
	}

	if (lptr != NULL) {
		wrd = lptr;
		if ((lptr = uls_strchr(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}
		parms->len = len = uls_strlen(wrd);
		uls_strncpy(tok_nam, wrd, ULS_LEXSTR_MAXSIZ);
	}

	return 0;
}

ULS_DECL_STATIC int
uls_parse_quotetype_opts(uls_wrd_ptr_t wrdx, uls_quotetype_ptr_t qmt,
	uls_outparam_ptr_t parms)
{
	char *lptr, *lptr1, ch;
	int rc, proc_num, stat=0;

	parms->flags = 0; // tok_id NOT specified
	parms->n = 0; // tok_id
	parms->len = 0;

	for (lptr = wrdx->lptr; *(lptr = skip_blanks(lptr)) != '\0'; ) {
		if (!uls_strncmp(lptr, "token=", 6)) {
			rc = 6;
			proc_num = 0;
		} else if (!uls_strncmp(lptr, "options=", 8)) {
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
			err_log("%s: unknown option!", lptr1);
			stat = -1;
			break;
		}

		if (ch != '\0') *lptr++ = ch;
	}

	wrdx->lptr = lptr;
	return stat;
}

ULS_DECL_STATIC int
set_config__QUOTE_TYPE__token(int tok_id, const char *tok_nam, int l_tok_nam,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
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
			err_log("%s:%d: The quotation tok-id already used by other token", tag_nam, lno);
			err_log("\t:tok-id:%d", tok_id);
			return -1;
		}
	}
	qmt->tok_id = tok_id;

	if (l_tok_nam > 0) {
		/* token-name */
		if (tok_nam[0] != '\0' && canbe_tokname(tok_nam) <= 0) {
			err_log("%s:%d Nil-string or too long token constant!", tag_nam, lno);
			err_log("\t:%s", tok_nam);
			return -1;
		}

		if (make_tokdef_for_quotetype(uls, qmt, tok_nam) < 0) {
			err_log("%s:%d fail to make tokdef for the quotation type!", tag_nam, lno);
			err_log("\t:%s", tok_nam);
			return -1;
		}
	}

	return 0;
}

ULS_DECL_STATIC void
__set_config_quoute_type(uls_quotetype_ptr_t qmt,
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

int
ulx_set_quote_type(uls_lex_ptr_t uls, int tok_id, const char *tok_nam,
	const char *mark1, const char *mark2, char *line_escmap)
{
	uls_quotetype_ptr_t qmt;
	int len1, len2, n1, n2, l_tok_nam, stat = 0;
	char ch;

	for (n1=len1=0; (ch=mark1[len1]) != '\0'; len1++) {
		if (ch == '\n') ++n1;
	}

	if (mark2 != NULL) {
		for (n2=len2=0; (ch=mark2[len2]) != '\0'; len2++) {
			if (ch == '\n') ++n2;
		}
	} else {
		mark2 = mark1;
		len2 = len1;
		n2 = n1;
	}

	qmt = uls_create_quotetype();
	qmt->litstr_analyzer = uls_ref_callback(dfl_lit_analyzer_escape0);
	__set_config_quoute_type(qmt, mark1, len1, n1, mark2, len2, n2);

	qmt->tok_id = tok_id;

	if (tok_nam != NULL) {
		l_tok_nam = uls_strlen(tok_nam);
	} else {
		l_tok_nam = -1;
	}

	if (l_tok_nam > 0 && make_tokdef_for_quotetype(uls, qmt, tok_nam) < 0) {
		stat = -1;
	} else if (line_escmap != NULL && (qmt->escmap=uls_parse_escmap(line_escmap, uls_ptr(uls->escstr_pool))) == nilptr) {
		stat = -2;
	}

	return stat;
}

ULS_DECL_STATIC int
__read_config__QUOTE_TYPE(char *line,
	uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char* tag_nam, int lno)
{
	char   *wrd, *ch_ctx = uls->ch_context;
	int  j, k, len, tok_id, tok_id_specified;
	uls_wrd_t wrdx;

	uls_outparam_t parms, parms1, parms2;
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
		err_log("failed to parse options for literal-string : %s", wrdx.lptr);
		return -1;
	}

	tok_id_specified = parms.flags;
	tok_id = parms.n;
	l_tok_name = parms.len;

	parms1.line = qmt_mark1;
	if ((len=uls_strlen(wrd=_uls_splitstr(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
		!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms1))) {
		err_log("%s<%d>: Too short or long quote (start) mark, or not permitted chars.",
			tag_nam, lno);
		err_log("\t:'%s'", wrd);
		return -1;
	}

	// the closing quote string corresponding to 'start mark'
	parms2.line = qmt_mark2;
	if (qmt->flags & ULS_QSTR_ASYMMETRIC) {
		if ((len=uls_strlen(wrd=_uls_splitstr(uls_ptr(wrdx)))) == 0 || len > ULS_QUOTE_MARK_MAXSIZ ||
			!canbe_quotetype_mark(ch_ctx, wrd, uls_ptr(parms2))) {
			err_log("%s<%d>: Too short or long quote (end) mark, or not permitted chars.",
				tag_nam, lno);
			err_log("\t:'%s'", wrd);
			return -1;
		}
	} else {
		if (qmt->flags & (ULS_QSTR_OPEN | ULS_QSTR_NOTHING)) {
			qmt_mark2[0] = '\0';
			parms2.len = 0;
			parms2.n = 0;
		} else {
			uls_strcpy(qmt_mark2, qmt_mark1);
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
				err_log("%s<%d>: Too many quote types", tag_nam, lno);
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

	if ((qmt->escmap=uls_parse_escmap(wrdx.lptr, uls_ptr(uls->escstr_pool))) == nilptr) {
		err_log("%s<%d>: Invalid format of escape-mapping of literal string.",
			tag_nam, lno);
		return -1;
	}

	if (!is_quotestart_valid(uls, k)) {
		err_log("%s<%d>:  the start-mark is collided with previous defined quote-type. skipping, ...",
			tag_nam, lno);
		err_log("\t:'%s'", uls_get_namebuf_value(qmt->start_mark));
		return -1;
	}

	return 0;
}

ULS_DECL_STATIC int
read_config__QUOTE_TYPE(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n, stat = 0;
	uls_quotetype_ptr_t qmt;

	qmt = uls_create_quotetype();
	qmt->litstr_analyzer = uls_ref_callback(dfl_lit_analyzer_escape0);

	if (__read_config__QUOTE_TYPE(line, qmt, uls, tag_nam, lno) < 0) {
		stat = -1;
		uls_destroy_quotetype(qmt);
	}

	return stat;
}

void
ulx_clear_config_ascii_id_chars(uls_lex_ptr_t uls)
{
	char  *ch_ctx = uls->ch_context;
	int i;

	for (i=0; i<ULS_SYNTAX_TABLE_SIZE; i++) {
		ch_ctx[i] &= ~(ULS_CH_IDFIRST | ULS_CH_ID);
	}
}

int
ulx_set_id_first_chars(uls_lex_ptr_t uls, int k, int i1, int i2)
{
	uls_ref_array_type01(ranges,uch_range);
	uls_uch_range_ptr_t ran;

	ranges = uls_ptr(uls->idfirst_charset);
	ran = uls_array_get_slot_type01(ranges, k);
	ran->x1 = i1;
	ran->x2 = i2;

	return 0;
}

ULS_DECL_STATIC void
parse_id_ranges_internal(uls_lex_ptr_t uls,
	uls_ref_parray(wrds_ranges,uch_range), int is_first)
{
	char  *ch_ctx = uls->ch_context;
	uls_decl_parray_slots(al, uch_range);
	uls_ref_array_type01(urange_list, uch_range);
	uls_decl_array_slots_type01(urange_list_slots,uch_range);

	uls_uch_range_ptr_t id_range, id_range1;
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
					if (uls_isdigit(j)) {
						err_log("%d: improper char-range specified!", j);
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
		uls_deinit_array_type01(urange_list, uch_range);
		uls_init_array_type01(urange_list, uch_range, n1);
		urange_list_slots = uls_array_slots_type01(urange_list);
	} else {
		urange_list = uls_ptr(uls->id_charset);
		uls_deinit_array_type01(urange_list, uch_range);
		uls_init_array_type01(urange_list, uch_range, n1);
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
parse_id_ranges(uls_lex_ptr_t uls, int is_first, char *line)
{
	int i;
	char  *wrd;

	uls_outparam_t parms1;
	uls_wrd_t wrdx;
	uls_decl_parray(wrds_ranges, uch_range);
	uls_decl_parray_slots(al, uch_range);
	uls_uch_range_ptr_t id_range;

	uls_init_parray(uls_ptr(wrds_ranges), uch_range, 16);

	line = skip_blanks(line);

	wrdx.lptr = line;
	while (*(wrd=_uls_splitstr(uls_ptr(wrdx))) != '\0') {
		parms1.lptr = wrd;
		if (get_range_aton(uls_ptr(parms1)) <= 0) {
			return -1;
		}

		id_range = uls_alloc_object(uls_uch_range_t);
		id_range->x1 = parms1.x1;
		id_range->x2 = parms1.x2;

		al = uls_parray_slots(uls_ptr(wrds_ranges));
		if ((i=wrds_ranges.n) >= wrds_ranges.n_alloc) {
			uls_resize_parray(uls_ptr(wrds_ranges), uch_range, i + 16);
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

	uls_deinit_parray(uls_ptr(wrds_ranges));
	return 0;
}

ULS_DECL_STATIC int
read_config__ID_FIRST_CHARS(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
//	const char* tag_nam = parms->lptr_end;
//	int lno = parms->n;

	return parse_id_ranges(uls, 1, line);
}

int
ulx_set_id_chars(uls_lex_ptr_t uls, int k, int i1, int i2)
{
	uls_ref_array_type01(ranges,uch_range);
	uls_uch_range_ptr_t ran;

	ranges = uls_ptr(uls->id_charset);
	ran = uls_array_get_slot_type01(ranges, k);
	ran->x1 = i1;
	ran->x2 = i2;

	return 0;
}

ULS_DECL_STATIC int
read_config__ID_CHARS(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
//	const char* tag_nam = parms->lptr_end;
//	int lno = parms->n;

	return parse_id_ranges(uls, 0, line);
}

int
ulx_set_rename(uls_lex_ptr_t uls, const char *name1, const char *name2)
{
	uls_tokdef_vx_ptr_t e_vx;
	int stat = 0;

	if ((e_vx=__find_rsvd_tokdef_by_name(uls, name1)) != nilptr) {
		e_vx->l_name = uls_set_namebuf_value(e_vx->name, name2);
	} else {
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
read_config__RENAME(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int stat = 0, lno = parms->n;
	uls_wrd_t wrdx;
	char  *wrd, *wrd2;

	wrdx.lptr = line;

	if (*(wrd = _uls_splitstr(uls_ptr(wrdx))) == '\0' ||
		*(wrd2 = _uls_splitstr(uls_ptr(wrdx))) == '\0' ||
		uls_strlen(wrd2) > ULS_LEXSTR_MAXSIZ) {
		err_log("%s<%d>: Invalid 'RENAME' line!", tag_nam, lno);
		return -1;
	}

	if (ulx_set_rename(uls, wrd, wrd2) < 0) {
		err_log("%s<%d>: Invalid 'RENAME'!", tag_nam, lno);
		err_log("\t:'%s'", wrd);
		stat = -1;
	}

	return stat;
}

void
ulx_set_not_char_tok(uls_lex_ptr_t uls, const char* non_ch_toks)
{
	char  ch, *ch_ctx = uls->ch_context;
	int i;

	for (i=0; (ch=non_ch_toks[i]) != '\0'; i++) {
		if (ch < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch] &= ~ULS_CH_1;
	}
}

ULS_DECL_STATIC int
read_config__NOT_CHAR_TOK(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_wrd_t wrdx;
	char  *wrd;

	wrdx.lptr = line;
	while (*(wrd=_uls_splitstr(uls_ptr(wrdx))) != '\0') {
		ulx_set_not_char_tok(uls, wrd);
	}

	return 0;
}

void
ulx_set_case_sensitive(uls_lex_ptr_t uls, int is_sensitive)
{
	if (is_sensitive > 0) {
		uls->flags &= ~ULS_FL_CASE_INSENSITIVE;
	} else {
		uls->flags |= ULS_FL_CASE_INSENSITIVE;
	}
}

ULS_DECL_STATIC int
read_config__CASE_SENSITIVE(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n, is_sensitive, stat = 0;
	uls_wrd_t wrdx;
	char  *wrd;

	wrdx.lptr = line;
	wrd = _uls_splitstr(uls_ptr(wrdx));

	if (uls_streql(wrd, "false")) {
		is_sensitive = 0;
	} else if (uls_streql(wrd, "true")) {
		is_sensitive = 1;
	} else {
		err_log("%s<%d>: Incorrect value of CASE_SENSITIVE. Specify it by true/false.",
			tag_nam, lno);
		is_sensitive = -1;
	}

	if (is_sensitive >= 0) {
		ulx_set_case_sensitive(uls, is_sensitive);
	} else {
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
read_config__DOMAIN(char *line, uls_cmd_ptr_t cmd)
{
	int linelen;

	linelen = str_trim_end(line, -1);
	if (linelen <= 0 || !uls_streql(line, ULC_REPO_DOMAIN)) {
		return -1;
	}

	return 0;
}

void
ulx_set_id_max_length(uls_lex_ptr_t uls, int len1, int len2)
{
	if (len1 <= 0) len1 = INT_MAX;
	uls->id_max_bytes = len1;

	if (len2 <= 0) len2 = len1;
	uls->id_max_uchars = len2;
}

ULS_DECL_STATIC int
read_config__ID_MAX_LENGTH(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_wrd_t wrdx;
	char  *wrd;
	int   len1, len2;

	wrdx.lptr = line;
	if (*(wrd = _uls_splitstr(uls_ptr(wrdx))) == '\0' || (len1 = uls_atoi(wrd)) <= 0) {
		return -1;
	}

	if (*(wrd = _uls_splitstr(uls_ptr(wrdx))) != '\0') {
		len2 = uls_atoi(wrd);
	} else {
		len2 = -1;
	}

	ulx_set_id_max_length(uls, len1, len2);
	return 0;
}

ULS_DECL_STATIC int
numpfx_by_length_dsc(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_number_prefix_ptr_t e1 = (const uls_number_prefix_ptr_t) a;
	const uls_number_prefix_ptr_t e2 = (const uls_number_prefix_ptr_t) b;
	int stat;

	if (e1->l_prefix < e2->l_prefix) stat = 1;
	else if (e1->l_prefix > e2->l_prefix) stat = -1;
	else stat = 0;

	return stat;
}

void
ulx_add_config_number_prefixes(uls_lex_ptr_t uls, char *wrd, int len, int radix, int slot_id)
{
	uls_number_prefix_ptr_t numpfx;

	numpfx = uls_array_get_slot_type00(uls_ptr(uls->numcnst_prefixes), slot_id);
	uls_init_namebuf(numpfx->prefix, ULS_MAXLEN_NUMBER_PREFIX);
	uls_strncpy(uls_get_namebuf_value(numpfx->prefix), wrd, len);
	numpfx->l_prefix = len;
	numpfx->radix = radix;
}

ULS_DECL_STATIC int
read_config__NUMBER_PREFIXES(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	int stat = 0;

	uls_wrd_t wrdx;
	char  *wrd, *lptr;
	int k, r, len;

	wrdx.lptr = line;
	for (k=0; *(wrd = _uls_splitstr(uls_ptr(wrdx))) != '\0'; k++) {
		if (k >= ULS_N_MAX_NUMBER_PREFIXES) {
			err_log("NUMBER_PREFIXES: Too many NumberPrefixes %d", ULS_N_MAX_NUMBER_PREFIXES);
			stat = -4; break;
		}

		if (*wrd != '0' || (lptr= uls_strchr(wrd, ':')) == NULL) {
			err_log("NUMBER_PREFIXES: Incorrect format for NumberPrefix '%s'.", wrd);
			err_log("Number prefix must be 0-prefixed.");
			return -1;
		}

		if ((len = (int) (lptr - wrd)) > ULS_MAXLEN_NUMBER_PREFIX) {
			err_log("NUMBER_PREFIXES: Too long NumberPrefix, %d/%d", len, ULS_MAXLEN_NUMBER_PREFIX);
			return -2;
		}

		*lptr++ = '\0';
		if ((r = uls_atoi(lptr)) < 2 || r > 36) {
			err_log("NUMBER_PREFIXES: assertion failed, 2 <= radix <= 36!", wrd);
			return -3;
		}

		ulx_add_config_number_prefixes(uls, wrd, len, r, k);
	}

	uls_quick_sort(uls_array_slots_type00(uls_ptr(uls->numcnst_prefixes)), k, sizeof(uls_number_prefix_t), uls_ref_callback(numpfx_by_length_dsc));
	uls->n_numcnst_prefixes =  k;

	return stat;
}

void
ulx_set_decimal_separator(uls_lex_ptr_t uls, uls_uch_t uch)
{
	uls->numcnst_separator = uch;
}

ULS_DECL_STATIC int
read_config__DECIMAL_SEPARATOR(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	const char* tag_nam = parms->lptr_end;
	int lno = parms->n;

	uls_wrd_t wrdx;
	char  *wrd;
	uls_uch_t uch = ULS_DECIMAL_SEPARATOR_DFL;

	wrdx.lptr = line;
	if (*(wrd = _uls_splitstr(uls_ptr(wrdx))) != '\0') {
		uch = *wrd;
		if (!uls_isgraph(uch) || uls_isalnum(uch) || uch == '-' || uch == '.') {
			err_log("%s<%d>: Invalid decimal separator!", tag_nam, lno);
			return -1;
		}
	}

	ulx_set_decimal_separator(uls, uch);
	return 0;
}

ULS_DECL_STATIC int
read_config__NUMBER_SUFFIXES(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_wrd_t wrdx;
	uls_arglst_t wrdlst;

	wrdx.lptr = line;

	uls_init_arglst(uls_ptr(wrdlst), ULC_COLUMNS_MAXSIZ);

	_uls_explode_str(uls_ptr(wrdx), ' ', 0, uls_ptr(wrdlst));
	check_validity_of_cnst_suffix(uls_ptr(wrdlst), uls);

	uls_deinit_arglst(uls_ptr(wrdlst));

	return 0;
}

void
ulx_set_prepended_input(uls_lex_ptr_t uls, const char *line, int len, int lfs_is_not_token)
{
	char ch, *buff;
	int i, n;

	uls_mfree(uls->xcontext.prepended_input);
	if (line == NULL || *line == '\0') {
		uls->xcontext.len_prepended_input = uls->xcontext.lfs_prepended_input = 0;
		return;
	}

	if (len < 0) len = uls_strlen(line);
	buff = (char *) uls_malloc_buffer(uls_ceil_log2(len + 1, 3) * sizeof(char));
	for (n=i=0; i<len; i++) {
		if ((ch=line[i]) == '\n') ++n;
		buff[i] = ch;
	}
	buff[i] = '\0';

	uls->xcontext.prepended_input = buff;
	uls->xcontext.len_prepended_input = len;
	uls->xcontext.lfs_prepended_input = n;
	if (lfs_is_not_token) {
		uls->xcontext.flags |= ULS_XCTX_FL_IGNORE_LF;
	} else {
		uls->xcontext.flags &= ~ULS_XCTX_FL_IGNORE_LF;
	}
}

ULS_DECL_STATIC int
read_config__PREPEND_INPUT(char *line, uls_cmd_ptr_t cmd)
{
	uls_outparam_ptr_t parms = (uls_outparam_ptr_t) cmd->user_data;
	uls_lex_ptr_t uls = (uls_lex_ptr_t) parms->data;
	uls_wrd_t wrdx;
	const char* tag_nam = parms->lptr_end;
	int len, len2, lfs_is_not_token = 0, lno = parms->n, stat = 0;
	char *buff2 = NULL, *line2, *wrd;

	line = skip_blanks(line);
	if (*line == '\0') {
		err_log("%s<%d>: empty value. give a string.", tag_nam, lno);
		return -1;
	}

	len = uls_strlen(line);
	if (*line == '"') {
		buff2 = (char *) uls_malloc_buffer((len + 1) * sizeof(char));

		wrdx.wrd = line2 = buff2;
		wrdx.siz = len + 1;
		wrdx.lptr = line + 1;
		if ((len2=uls_get_escape_str('"', uls_ptr(wrdx))) < 0) {
			err_log("%s<%d>: an double-quoted string unterminated", tag_nam, lno);
			stat = -1;
		} else if (len2 == 0) {
			err_log("%s<%d>: an empty double-quoted string", tag_nam, lno);
		}

		wrd = _uls_splitstr(uls_ptr(wrdx));
		if (uls_streql(wrd, "true")) {
			lfs_is_not_token = 1;
		} else if (uls_streql(wrd, "false")) {
			lfs_is_not_token = 0;
		} else {
			err_log("%s<%d>: specify true or false.", tag_nam, lno);
		}

	} else if (*line == '\'') {
		line2 = line + 1;
		len2 = uls_strlen(line2);
		for (--len2; len2 >= 0; len2--) {
			if (!uls_isspace(line2[len2])) {
				if (line2[len2] != '\'') {
					err_log("%s<%d>: an single-quoted string unterminated", tag_nam, lno);
					return -3;
				} else {
					line2[len2] = '\0';
				}
				break;
			}
		}
		if (len2 <= 0) {
			err_log("%s<%d>: an empty single-quoted string", tag_nam, lno);
		}
	} else {
		err_log("%s<%d>: PREPEND_INPUT must be started by quotation, '\\' or '\"'.", tag_nam, lno);
		stat = -2;
	}

	if (stat >= 0) {
		ulx_set_prepended_input(uls, line2, len2, lfs_is_not_token);
	}
	uls_mfree(buff2);
	return stat;
}

ULS_DECL_STATIC int
read_config_var(const char* tag_nam, int lno, uls_lex_ptr_t uls,
	char* lptr, ulc_header_ptr_t hdr)
{
	char  *wrd;
	int len, rc, stat = 1;

	uls_outparam_t parms1;
	uls_wrd_t wrdx;

	parms1.n = lno;
	parms1.lptr_end = tag_nam;
	parms1.data = uls;
	parms1.proc = hdr;

	wrdx.lptr = lptr;
	wrd = _uls_splitstr(uls_ptr(wrdx));
	lptr = wrdx.lptr;

	if ((len = uls_strlen(wrd)) <= 0 || wrd[len-1] != ':') {
		return 0;
	}
	wrd[--len] = '\0';

	rc = uls_cmd_run(uls_array_slots_type00(uls_ptr(ulc_cmd_list)), ULC_N_LEXATTRS, wrd, lptr, uls_ptr(parms1));
	if (rc < 0) {
		err_log("%s<%d>: unknown attribute in ULS-spec", tag_nam, lno);
		err_log("\tattribute:'%s'", wrd);
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
get_ulf_filepath(const char* dirpath, int len_dirpath,
	const char *specname, char* pathbuff)
{
	int len;

	if (len_dirpath > 0) {
		uls_memcopy(pathbuff, dirpath, len_dirpath);
		len = len_dirpath;
		pathbuff[len++] = ULS_FILEPATH_DELIM;
	} else {
		len = 0;
	}

	len += uls_snprintf(pathbuff+len, ULS_FILEPATH_MAX+1-len, "%s.ulf", specname);
	return len;
}

ULS_DECL_STATIC void
_list_searchpath(const char *filename,
	uls_arglst_ptr_t title, uls_arglst_ptr_t searchpath, int n)
{
	uls_decl_parray_slots_init(al_title, argstr, uls_ptr(title->args));
	uls_decl_parray_slots_init(al_searchpath, argstr, uls_ptr(searchpath->args));
	uls_argstr_ptr_t arg;

	char fpath_buff[ULS_FILEPATH_MAX+1];
	const char  *lptr, *lptr0, *fptr;
	int   len_fptr;
	int   i, len, rc;

	for (i=0; i<n; i++) {
		err_log("[%s]", al_title[i]->str);

		if ((arg = al_searchpath[i]) == nilptr) {
			continue;
		}

		for (lptr0=arg->str; lptr0 != NULL; ) {
			if ((lptr = uls_strchr(lptr0, ULS_DIRLIST_DELIM)) != NULL) {
				len_fptr = (int) (lptr - lptr0);
				fptr = lptr0;
				lptr0 = ++lptr;
			} else {
				len_fptr = uls_strlen(lptr0);
				fptr = lptr0;
				lptr0 = NULL;
			}

			if (len_fptr == 0) continue;

			len = uls_strncpy(fpath_buff, fptr, len_fptr);
			fpath_buff[len++] = ULS_FILEPATH_DELIM;
			uls_strcpy(fpath_buff+len, filename);

			rc = uls_dirent_exist(fpath_buff);
			fpath_buff[len_fptr] = '\0';
			if (rc == ST_MODE_REG) {
				err_log("\t%s ---> FOUND!", fpath_buff);
			} else {
				err_log("\t%s", fpath_buff);
			}
		}
	}
}

ULS_DECL_STATIC void
set_chrmap(uls_lex_ptr_t uls, int tok_id, int keyw_type, char ch_kwd)
{
	char *ch_ctx = uls->ch_context;

	switch (keyw_type) {
	case ULS_KEYW_TYPE_TWOPLUS:
		if (ch_kwd < ULS_SYNTAX_TABLE_SIZE) {
			ch_ctx[ch_kwd] |= ULS_CH_2PLUS;
			if (uls->flags & ULS_FL_CASE_INSENSITIVE) {
				ch_ctx[uls_toupper(ch_kwd)] |= ULS_CH_2PLUS;
			}
		}
		/* pass through */

	case ULS_KEYW_TYPE_IDSTR:
	case ULS_KEYW_TYPE_USER:
		if (tok_id >= 0 && tok_id < ULS_SYNTAX_TABLE_SIZE) {
			if (ch_ctx[tok_id] & ULS_CH_1) {
//				err_log("%s<%d>: Overriding char-token,", tag_nam, lno);
//				err_log("\t%d('%c') ...", tok_id, tok_id);
				ch_ctx[tok_id] &= ~ULS_CH_1;
			}
		}
		break;

	case ULS_KEYW_TYPE_1CHAR:
		if (ch_kwd < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch_kwd] |= ULS_CH_1;
		if (ch_kwd == '\n') uls->flags |= ULS_FL_LF_CHAR;
		else if (ch_kwd == '\t') uls->flags |= ULS_FL_TAB_CHAR;
		break;

	default:
		break;
	}
}

int
is_reserved_tok(uls_lex_ptr_t uls, char* name)
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
check_rsvd_toks(uls_lex_ptr_t uls)
{
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	uls_tokdef_vx_ptr_t e0_vx, e_vx;
	int i, j, stat=0;

	for (i=0; i<N_RESERVED_TOKS; i++) {
		e0_vx = slots_rsv[i];

		for (j=i+1; j<N_RESERVED_TOKS; j++) {
			e_vx = slots_rsv[j];

			if (uls_streql(uls_get_namebuf_value(e0_vx->name), uls_get_namebuf_value(e_vx->name))) {
				err_log("The name '%s' is already used by '%s'.",
					uls_get_namebuf_value(e_vx->name), uls_get_namebuf_value(e0_vx->name));
				stat = -1;
				break;
			}

			if (e0_vx->tok_id == e_vx->tok_id) {
				err_log("The token %s:%d is already used",
					uls_get_namebuf_value(e0_vx->name), e0_vx->tok_id);
				err_log("\tby %s:%d.", uls_get_namebuf_value(e_vx->name), e_vx->tok_id);
				stat = -1;
				break;
			}
		}
	}

	return stat;
}

int
uls_is_char_idfirst(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch)
{
	int n_bytes, i;
	uls_uch_t uch;
	uls_uch_range_ptr_t ran;

	if ((n_bytes = uls_decode_utf8(lptr, -1, &uch)) <= 0) {
		return n_bytes;
	}
	// UTF-8: n_bytes == 1, 2, 3, 4
	if (ptr_uch != NULL) *ptr_uch = uch;

	if (n_bytes > 1) {
		for (i=0; i<uls->idfirst_charset.n; i++) {
			ran = uls_array_get_slot_type01(uls_ptr(uls->idfirst_charset), i);
			if (uch >= ran->x1 && uch <= ran->x2)
				return n_bytes;
		}

		if (is_utf_id(uch) > 0) {
			return n_bytes;
		}

	} else if (uch < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[uch] & ULS_CH_IDFIRST)) {
		return 1;
	}

	return -n_bytes;
}

int
uls_is_char_id(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch)
{
	int n_bytes, i;
	uls_uch_t uch;
	uls_uch_range_ptr_t ran;

	if ((n_bytes = uls_decode_utf8(lptr, -1, &uch)) <= 0) {
		return n_bytes;
	}

	// UTF-8: n_bytes == 1, 2, 3, 4
	if (ptr_uch != NULL) *ptr_uch = uch;

	if (n_bytes > 1) {
		for (i=0; i<uls->id_charset.n; i++) {
			ran = uls_array_get_slot_type01(uls_ptr(uls->id_charset), i);
			if (uch >= ran->x1 && uch <= ran->x2)
				return n_bytes;
		}

		if (is_utf_id(uch) > 0) {
			return n_bytes;
		}

	} else if (uch < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[uch] & ULS_CH_ID)) {
		return 1;
	}

	return -n_bytes;
}

uls_tokdef_vx_ptr_t
uls_find_tokdef_vx(uls_lex_ptr_t uls, int t)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_t e0_vx;
	uls_tokdef_vx_ptr_t e_vx;

	e0_vx.tok_id = t;

	if ((e_vx = (uls_tokdef_vx_ptr_t) uls_bi_search_vptr(uls_ptr(e0_vx),
		(_uls_type_array(uls_voidptr_t)) slots_vx, uls->tokdef_vx_array.n,
		uls_ref_callback(srch_vx_by_id))) == nilptr) {
		if (t >= 0) {
			e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), t, nilptr);
		}
	}

	return e_vx;
}

uls_tokdef_vx_ptr_t
uls_find_tokdef_vx_force(uls_lex_ptr_t uls, int t)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_context_ptr_t  ctx = uls->xcontext.context;
	uls_tokdef_vx_t e0_vx;
	uls_tokdef_vx_ptr_t e_vx;

	e0_vx.tok_id = t;

	if ((e_vx = (uls_tokdef_vx_ptr_t) uls_bi_search_vptr(uls_ptr(e0_vx),
		(_uls_type_array(uls_voidptr_t)) slots_vx, uls->tokdef_vx_array.n,
		uls_ref_callback(srch_vx_by_id))) == nilptr) {
		if (t >= 0 && (e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), t, nilptr)) == nilptr) {
			if (t < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[t] & ULS_CH_1)) {
				e_vx = ctx->anonymous_uchar_vx;
				e_vx->tok_id = t;
			}
		}
	}

	return e_vx;
}

uls_tokdef_vx_ptr_t
set_extra_tokdef_vx(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
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
is_cnst_suffix_contained(char* cstr_pool, const char* str, int l_str, uls_outparam_ptr_t parms)
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
			l = uls_strlen(ptr);
		}
		if (parms != nilptr) parms->line = ptr;
		return NULL;
	}

	for (ptr=cstr_pool; *ptr!='\0'; ptr+=l+1) {
		if ((l=uls_strlen(ptr)) <= l_str && uls_memcmp(ptr, str, l) == 0) {
			ret_ptr = ptr;
			break;
		}
	}

	if (parms != nilptr) parms->line = ptr;
	return ret_ptr; // the ptr corresponding to 'str'
}

FILE*
ulc_search_for_fp(int typ_fpath, const char* fpath, uls_outparam_ptr_t parms)
{
	uls_arglst_t title_list;
	uls_arglst_t searchpath_list;
	uls_decl_parray_slots(al_searchpath, argstr);
	uls_argstr_ptr_t arg;

	FILE *fp, *fp_ret=NULL;
	char *dpath;
	int i, n;

	uls_init_arglst(uls_ptr(title_list), N_ULC_SEARCH_PATHS);
	uls_init_arglst(uls_ptr(searchpath_list), N_ULC_SEARCH_PATHS);

	if (typ_fpath == ULS_NAME_SPECNAME) {
		n = ulc_get_searchpath_by_specname(uls_ptr(title_list), uls_ptr(searchpath_list));
	} else {
		n = ulc_get_searchpath_by_specpath(is_absolute_path(fpath), uls_ptr(title_list), uls_ptr(searchpath_list));
	}

	al_searchpath = uls_parray_slots(uls_ptr(searchpath_list.args));

	for (i=0; ; i++) {
		if (i >= n) {
//			err_log("%s: ulc not found!", fpath);
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
				parms->line = uls_strdup(parms->lptr, parms->len);
			}
			fp_ret = fp;
			break;
		}
	}

	uls_deinit_arglst(uls_ptr(title_list));
	uls_deinit_arglst(uls_ptr(searchpath_list));

	return fp_ret;
}

FILE*
uls_get_ulc_path(int typ_fpath, const char* fpath, int len_dpath,
	const char* specname, int len_specname, uls_outparam_ptr_t parms)
{
	char fpath_buff[ULC_LONGNAME_MAXSIZ+5], ulf_filepath[ULS_FILEPATH_MAX+1], ch;
	const char *filepath, *dirpath;
	int  len_dirpath, k;
	FILE  *fp_ulc;
	uls_outparam_t parms1;

	if (typ_fpath == ULS_NAME_SPECNAME) {
		for (k=0; k<len_dpath; k++) {
			if ((ch = fpath[k]) == ULS_ULCNAME_DELIM) {
				fpath_buff[k] = ULS_FILEPATH_DELIM;
			} else {
				fpath_buff[k] = ch;
			}
		}

		if (k > 0) fpath_buff[k++] = ULS_FILEPATH_DELIM;
		uls_strcpy(fpath_buff+k, specname);
		k += len_specname;
		uls_strcpy(fpath_buff+k, ".ulc");

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
			parms->native_data = uls_fp_open(ulf_filepath, ULS_FIO_READ);
		}
		uls_mfree(dirpath);
	}

	return fp_ulc;
}

int
ulc_prepend_searchpath_exeloc(const char* argv0)
{
	char fpath_buf[ULS_FILEPATH_MAX+1];
	int fpath_len;

	if ((fpath_len=uls_get_exeloc_dir(argv0, fpath_buf)) < 0) {
		err_log("can't find the location of program file");
		return -1;
	}

	if (ulc_add_searchpath(fpath_buf, 1) < 0) {
		err_log("can't update the system value for ulc-search-path");
		return -1;
	}

	return 0;
}

int
ulc_get_searchpath_by_specname(
	uls_arglst_ptr_t nameof_searchpath, uls_arglst_ptr_t searchpath_list)
{
	uls_decl_parray_slots_init(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	uls_decl_parray_slots(al_args, argstr);
	uls_argstr_ptr_t arg;

	const char* title[N_ULC_SEARCH_PATHS];
	const char *cptr;
	int  n = 0, i;

	title[n] = "ULS_SPEC_PATH";
	if ((cptr=getenv(title[n])) != NULL) {
		al_searchpath[n] = arg = uls_create_argstr();
		uls_copy_argstr(arg, cptr, -1);
		++n;
	}

	title[n] = "ULS_ULCS";
	al_searchpath[n] = arg = uls_create_argstr();
	uls_copy_argstr(arg, _uls_sysinfo_(ulcs_dir), -1);
	++n;

	title[n] = "ULS_OS_SHARE_DFLDIR";
	al_searchpath[n] = arg = uls_create_argstr();
	uls_copy_argstr(arg, ULS_OS_SHARE_DFLDIR, -1);
	++n;

	searchpath_list->args.n = n;

	if (nameof_searchpath != nilptr) {
		al_args = uls_parray_slots(uls_ptr(nameof_searchpath->args));
		for (i=0; i<n; i++) {
			al_args[i] = arg = uls_create_argstr();
			uls_copy_argstr(arg, title[i], -1);
		}
		nameof_searchpath->args.n = n;
	}

	return n;
}

int
ulc_get_searchpath_by_specpath(int is_abspath,
	uls_arglst_ptr_t nameof_searchpath, uls_arglst_ptr_t searchpath_list)
{
	uls_decl_parray_slots_init(al_searchpath, argstr, uls_ptr(searchpath_list->args));
	uls_decl_parray_slots(al_args, argstr);
	uls_argstr_ptr_t arg;

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
			al_searchpath[n] = arg = uls_create_argstr();
			uls_copy_argstr(arg, cptr, -1);
			++n;
		}

		title[n] = "PWD";
		al_searchpath[n] = nilptr;
		++n;

		title[n] = "EXELOC";
		exeloc = uls_malloc_buffer(ULS_FILEPATH_MAX+1);
		if (uls_get_exeloc_dir(NULL, exeloc) >= 0) {
			al_searchpath[n] = arg = uls_create_argstr();
			uls_copy_argstr(arg, exeloc, -1);
			++n;
		}
		uls_mfree(exeloc);

		if (_uls_sysinfo_(ULC_SEARCH_PATH) != NULL) {
			title[n] = "SEARCH_PATH";
			al_searchpath[n] = arg = uls_create_argstr();
			uls_copy_argstr(arg, _uls_sysinfo_(ULC_SEARCH_PATH), -1);
			++n;
		}

		title[n] = "ULS_OS_SHARE_DFLDIR";
		al_searchpath[n] = arg = uls_create_argstr();
		uls_copy_argstr(arg, ULS_OS_SHARE_DFLDIR, -1);
		++n;
	}

	if (nameof_searchpath != nilptr) {
		al_args = uls_parray_slots(uls_ptr(nameof_searchpath->args));
		for (i=0; i<n; i++) {
			al_args[i] = arg = uls_create_argstr();
			uls_copy_argstr(arg, title[i], -1);
		}
		nameof_searchpath->args.n = n;
	}

	return n;
}

fp_list_ptr_t
ulc_find_fp_list(fp_list_ptr_t fp_stack_top, const char *ulc_name)
{
	fp_list_ptr_t fp_lst;

	for (fp_lst = fp_stack_top; fp_lst != nilptr; fp_lst=fp_lst->prev) {
		if (uls_streql(fp_lst->tagstr, ulc_name)) {
			return fp_lst;
		}
	}

	return nilptr;
}

fp_list_ptr_t
ulc_fp_push(fp_list_ptr_t fp_lst, FILE *fp, const char* str)
{
	fp_list_ptr_t e;
	int len;

	if (str == NULL) {
		str = "";
		len = 0;
	} else {
		len = uls_strlen(str);
	}

	e = uls_alloc_object(fp_list_t);
	e->tagstr = uls_strdup(str, len);
	e->linenum = 0;

	e->fp = fp;
	e->prev = fp_lst;

	return e;
}

FILE*
ulc_fp_peek(fp_list_ptr_t fp_lst, uls_outparam_ptr_t parms)
{
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
ulc_fp_get(uls_outparam_ptr_t parms, int do_pop)
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
release_ulc_fp_stack(fp_list_ptr_t fp_lst)
{
	FILE *fp;
	uls_outparam_t parms;

	while (fp_lst != nilptr) {
		parms.data = fp_lst;
		fp = ulc_fp_get(uls_ptr(parms), 1);
		fp_lst = (fp_list_ptr_t) parms.data;
		uls_fp_close(fp);
	}
}

void
init_reserved_toks(uls_lex_ptr_t uls)
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
classify_char_group(uls_lex_ptr_t uls, ulc_header_ptr_t uls_conf)
{
	char *ch_ctx = uls->ch_context;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	int  ch, i; // ch should not be char of type.

	/* '\0' : An exceptional char, which is used only space char. */
	if (ch_ctx['\0'] != 0) {
		err_log("The null char can't be used as other usage, 0x%X", ch_ctx['\0']);
		ch_ctx['\0'] = 0;
	}

	/* ' ' : ~VISIBLE */
	if (ch_ctx[' '] != 0) {
		err_log("The space char can't be used as other usage, 0x%X", ch_ctx[' ']);
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

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
		if (is_str_contained_in_quotetypes(uls, uls_get_namebuf_value(cmt->start_mark))) {
			err_log("comment-type '%s' is not proper as it contained in one of the quote-types[]",
				uls_get_namebuf_value(cmt->start_mark));
			return -1;
		}
	}

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		if (is_str_contained_in_commtypes(uls, uls_get_namebuf_value(qmt->start_mark))) {
			err_log("quote-type '%s' is not proper as it contained in one of the comm-types[]",
				uls_get_namebuf_value(qmt->start_mark));
			return -1;
		}
	}

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
		ch = uls_get_namebuf_value(cmt->start_mark)[0];
		if (ch < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch] |= ULS_CH_COMM;
	}

	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		ch = uls_get_namebuf_value(qmt->start_mark)[0];
		if (ch < ULS_SYNTAX_TABLE_SIZE) ch_ctx[ch] |= ULS_CH_QUOTE;
	}

	return 0;
}

int
classify_tok_group(uls_lex_ptr_t uls)
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

uls_tokdef_ptr_t
rearrange_tokname_of_quotetypes(uls_lex_ptr_t uls, int n_keys_twoplus, uls_outparam_ptr_t parms)
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
			err_panic("%d: not permitted keyw_type", e->keyw_type);
		}
	}

	twotbl->twoplus_mempool.n = n1;
	parms->n = n2;

	return idtok_list;
}

int
calc_len_surplus_recommended(uls_lex_ptr_t uls)
{
	int i, len, len_surplus = ULS_UTF8_CH_MAXLEN;
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);

	for (i=0; i<uls->n1_commtypes; i++) {
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), i);
		if ((len = cmt->len_end_mark) > len_surplus)
			len_surplus = len;
	}

	if (uls->n1_commtypes > 0) {
		cmt = uls_array_get_slot_type01(uls_ptr(uls->commtypes), 0);
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
srch_vx_by_id(const uls_voidptr_t a, const uls_voidptr_t b)
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
comp_vx_by_tokid(const uls_voidptr_t a, const uls_voidptr_t b)
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
uls_want_eof(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags |= ULS_CTX_FL_WANT_EOFTOK;

	if (__uls_tok(uls) == uls->xcontext.toknum_EOF) {
		err_log("The current token is already EOF!");
	}
}

void
uls_unwant_eof(uls_lex_ptr_t uls)
{
	uls->xcontext.context->flags &= ~ULS_CTX_FL_WANT_EOFTOK;
}

int
uls_is_valid_specpath(const char* confname)
{
	char ch;
	int i, len;

	for (i=0; (ch=confname[i]) != '\0'; i++) {
		if (!uls_isprint(ch)) return -1;
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
uls_get_spectype(const char *filepath, uls_outparam_ptr_t parms)
{
	int k, typ, len_dpath, len_fname;
	char* specname = parms->line;
	const char *fname;

	if (filepath == NULL || specname == NULL) {
		err_log("invalid parameter!");
		return -1;
	}

	for (k=uls_strlen(filepath)-1; ; k--) {
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

	len_fname = uls_strlen(fname);
	for (k=len_fname-1; k>=0; k--) {
		if (fname[k] == '.') {
			len_fname = k;
			break;
		}
	}
	if (len_fname <= 0) {
		err_log("%s: invalid name of ulc!", filepath);
		return -1;
	}

	if (uls_streql(fname+len_fname, ".ulc")) {
		typ = ULS_NAME_FILEPATH_ULC;
		uls_strncpy(specname, fname, len_fname);

	} else if (uls_streql(fname+len_fname, ".uld")) {
		typ = ULS_NAME_FILEPATH_ULD;
		uls_strncpy(specname, fname, len_fname);

	} else {
		// 'fname' is a valid specname.
		typ = ULS_NAME_SPECNAME;
		if (is_path_prefix(filepath) != 0 || uls_is_valid_specpath(filepath) < 0) {
			err_log("%s: invalid spec-path", filepath);
			return -1;
		}

		if ((fname = uls_find_lang_name(fname)) == NULL) {
//			err_log("%s: not found!", filepath);
			return -1;
		}
		len_fname = uls_strcpy(specname, fname);
	}

	parms->n = len_dpath;
	parms->len = len_fname;

	return typ;
}

void
ulc_set_searchpath(const char *pathlist)
{
	uls_mfree(_uls_sysinfo_(ULC_SEARCH_PATH));
	if (pathlist != NULL) {
		_uls_sysinfo_(ULC_SEARCH_PATH) = uls_strdup(pathlist, -1);
	}
}

int
ulc_add_searchpath(const char *pathlist, int front)
{
	int len1, len, siz;
	char *ptr0, *ptr;

	if (pathlist == NULL || (len=uls_strlen(pathlist)) <= 0) {
		err_log("%s: invalid parameter!");
		return -1;
	}

	if (_uls_sysinfo_(ULC_SEARCH_PATH) == NULL) {
		len1 = 0;
		siz = len;
	} else {
		len1 = uls_strlen(_uls_sysinfo_(ULC_SEARCH_PATH));
		siz = len1 + 1 + len;
	}

	ptr0 = ptr = uls_malloc_buffer(siz+1);

	if (len1 > 0) {
		if (front) {
			ptr[len] = ULS_DIRLIST_DELIM;
			uls_memcopy(ptr+len+1, _uls_sysinfo_(ULC_SEARCH_PATH), len1);
		} else {
			ptr = (char *) uls_memcopy(ptr, _uls_sysinfo_(ULC_SEARCH_PATH), len1);
			*ptr++ = ULS_DIRLIST_DELIM;
		}
	}

	uls_memcopy(ptr, pathlist, len);
	ptr0[siz] = '\0';

	ulc_set_searchpath(ptr0);
	uls_mfree(ptr0);

	return 0;
}

int
ulc_prepend_searchpath_pwd(void)
{
	char fpath_buf[ULS_FILEPATH_MAX+1];

	if (uls_getcwd(fpath_buf, ULS_FILEPATH_MAX+1) < 0) {
		err_log("can't find the current directory");
		return -1;
	}

	if (ulc_add_searchpath(fpath_buf, 1) < 0) {
		err_log("can't update the system value for ulc-search-path");
		return -1;
	}

	return 0;
}

void
ulc_list_searchpath(const char* confname)
{
	uls_arglst_t title_list;
	uls_arglst_t searchpath_list;

	char  specname[ULC_LONGNAME_MAXSIZ+1], fpath_buff[ULC_LONGNAME_MAXSIZ+5], ch;
	const char *filename;
	int len_basedir, len_specname, n, k, typ_fpath;
	uls_outparam_t parms1;

	if (confname == NULL) return;

	parms1.line = specname;
	typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
	len_basedir = parms1.n;
	len_specname = parms1.len;

	if (typ_fpath < 0) {
		err_log("%s: Invalid name for spec-name", confname);
		return;
	}

	uls_init_arglst(uls_ptr(title_list), N_ULC_SEARCH_PATHS);
	uls_init_arglst(uls_ptr(searchpath_list), N_ULC_SEARCH_PATHS);

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
		uls_strcpy(fpath_buff+k, specname);
		k += len_specname;
		uls_strcpy(fpath_buff+k, ".ulc");

		filename = fpath_buff;

	} else {
		n = ulc_get_searchpath_by_specpath(is_absolute_path(confname),
			uls_ptr(title_list), uls_ptr(searchpath_list));
		// confname is a file path.
		filename = confname;
	}

	_list_searchpath(filename, uls_ptr(title_list), uls_ptr(searchpath_list), n);

	uls_deinit_arglst(uls_ptr(title_list));
	uls_deinit_arglst(uls_ptr(searchpath_list));
}

int
check_ulcf_fileformat_magic(char *linebuff, int linelen, int ftype)
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
		uls_memcmp(linebuff, magic_code, magic_code_len) != 0) {
		err_log("Improper ulc/ulf file format!");
		err_log("The ulc/ulf/uld file must be utf-8 encoded text file with BOM!");
		return -1;
	}

	return magic_code_len;
}

ULS_DECL_STATIC int
get_ulc_fileformat_ver(char *linebuff, int linelen, uls_version_ptr_t ver1)
{
	int  len1, magic_code_len;
	char *lptr, *lptr1;

	if ((magic_code_len = check_ulcf_fileformat_magic(linebuff, linelen, 0)) < 0) {
		return -1;
	}

	lptr1 = linebuff + magic_code_len;
	str_trim_end(lptr1, linelen - magic_code_len);

	if ((lptr = uls_strchr(lptr1, ':')) != NULL) {
		len1 = (int) (lptr - lptr1);
		*lptr = '\0';
	} else {
		len1 = uls_strlen(lptr1);
	}

	if (len1 > ULS_VERSION_STR_MAXLEN || uls_version_pars_str(lptr1, ver1) < 0) {
		return -2;
	}

	return 0;
}

ULS_DECL_STATIC int
check_ulc_file_magic(FILE* fin, uls_version_ptr_t sysver, char *ulc_lname)
{
	int  linelen, len1, len2, magic_code_len;
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr, *lptr1;
	uls_version_t ver1;
	char  ver_str[ULS_VERSION_STR_MAXLEN+1];

	linelen = uls_fp_gets(fin, linebuff, sizeof(linebuff), 0);
	if (linelen <= ULS_EOF || (magic_code_len = check_ulcf_fileformat_magic(linebuff, linelen, 0)) < 0) {
		rewind(fin);
		return -1;
	}

	lptr1 = linebuff + magic_code_len;
	str_trim_end(lptr1, linelen - magic_code_len);

	if ((lptr = uls_strchr(lptr1, ':')) != NULL) {
		len1 = (int) (lptr - lptr1);
		*lptr++ = '\0';

		if ((len2=uls_strlen(lptr)) > ULC_LONGNAME_MAXSIZ || len2 <= 0) {
			err_log("%s: Too long ulc-path", lptr);
			len2 = -2;
		} else {
			uls_strncpy(ulc_lname, lptr, len2);
		}
	} else {
		ulc_lname[0] = '\0';
		len1 = uls_strlen(lptr1);
		len2 = 0;
	}

	if (len1 > ULS_VERSION_STR_MAXLEN || uls_version_pars_str(lptr1, uls_ptr(ver1)) < 0 ||
		!uls_ver_compatible(uls_ptr(ver1), sysver)) {
		uls_version_make_string(uls_ptr(ver1), ver_str);
		err_log("unsupported version: %s", ver_str);
		len2 = -3;
	}

	if (len2 < 0) {
		rewind(fin);
	}

	return len2;
}

int
ulc_read_header(uls_lex_ptr_t uls, FILE* fin, ulc_header_ptr_t hdr, uls_outparam_ptr_t parms)
{
	fp_list_ptr_t  fp_stack_ptr, fp_stack_top;
	char  specname[ULC_LONGNAME_MAXSIZ+1];
	char  linebuff[ULS_LINEBUFF_SIZ+1], *lptr;

	char  ulc_lname[ULC_LONGNAME_MAXSIZ+1], *tag;
	int   linelen, lno;
	int   rc, ulc_lname_len, len_basedir, len_specname, typ_fpath;
	uls_outparam_t parms1;

	linelen = uls_fp_gets(fin, linebuff, sizeof(linebuff), 0);
	if (linelen <= ULS_EOF || get_ulc_fileformat_ver(linebuff, linelen, uls_ptr(hdr->filever)) < 0) {
		err_log("Can't get file format version!");
		return -1;
	}
	rewind(fin);

	// Read Header upto '%%'
	for (fp_stack_top = (fp_list_ptr_t) parms->data;
		(ulc_lname_len = check_ulc_file_magic(fin, uls_ptr((uls)->config_filever), ulc_lname)) > 0; ) {
		if (ulc_find_fp_list(fp_stack_top, ulc_lname) != nilptr) {
			err_log("can't inherit ulc-spec %s from multiple parents", ulc_lname);
			return -1;
		}

		parms1.line = specname;
		typ_fpath = uls_get_spectype(ulc_lname, uls_ptr(parms1));
		len_basedir = parms1.n;
		len_specname = parms1.len;

		if (typ_fpath != ULS_NAME_SPECNAME) {
			err_log("Invalid spec-name in %s.", ulc_lname);
			return -1;
		}

		if ((fin = uls_get_ulc_path(typ_fpath, ulc_lname, len_basedir, specname, len_specname, nilptr)) == NULL) {
			err_log("can't open ulc-spec '%s'", ulc_lname);
			return -1;
		}
		fp_stack_top->linenum = 1;
		fp_stack_top = ulc_fp_push(fp_stack_top, fin, ulc_lname);
		fp_stack_top->linenum = 0;
	}

	parms->data = fp_stack_ptr = fp_stack_top;

	tag = fp_stack_ptr->tagstr;
	lno = fp_stack_ptr->linenum;

	while (1) {
		if ((linelen=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) lno = -1;
			break;
		}
		++lno;

		if (uls_strncmp(linebuff, "%%%%", 2) == 0) {
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

		if (*(lptr = skip_blanks(linebuff)) == '\0' || *lptr == '#')
			continue;

		if ((rc=read_config_var(tag, lno, uls, lptr, hdr)) <= 0) {
			if (rc < 0) return -1;
			break;
		}
	}

	return lno;
}

uls_tokdef_vx_ptr_t
ulc_proc_line
	(const char* tag_nam, int lno, char* lptr, uls_lex_ptr_t uls, ulc_header_ptr_t hdr, uls_outparam_ptr_t parms)
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
	uls_wrd_t wrdx;
	uls_outparam_t parms1;

	parms->data = nilptr;
	wrdx.lptr = lptr;

/* token-name */
	wrd1 = _uls_splitstr(uls_ptr(wrdx));
	if (canbe_tokname(wrd1) <= 0) {
		err_log("%s<%d>: Nil-string or too long token constant.", tag_nam, lno);
		err_log("\ttoken-name:'%s'", wrd1);
		return nilptr;
	}

	if ((rsv_idx=is_reserved_tok(uls, wrd1)) >= 0) {
		wrd2 = _uls_splitstr(uls_ptr(wrdx));
		if ((len2 = uls_strlen(wrd2)) > ULS_LEXSTR_MAXSIZ) {
			// The keywords for ID, NUMBER, .. are nonsense.
			len2 = ULS_LEXSTR_MAXSIZ;
			wrd2[len2] = '\0';
		}

		if (*wrd2 == '\0' || is_pure_int_number(wrd2) > 0) {
			wrd3 = wrd2;
			wrd2 = "";
			len2 = 0;
		} else {
			wrd3 = _uls_splitstr(uls_ptr(wrdx));
		}

		/* tok-id */
		if (*wrd3 != '\0') {
			tok_id = uls_atoi(wrd3);
			hdr->tok_id_seed = tok_id + 1;
		} else {
			tok_id = hdr->tok_id_seed++;
		}

		if ((e2_vx=__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) != nilptr) {
			err_log("In %s<%d>,", tag_nam, lno);
			err_log("\tthe token-id %s is already used by %s.", wrd1, uls_get_namebuf_value(e2_vx->name));
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
	wrd2 = _uls_splitstr(uls_ptr(wrdx));

	parms1.data = uls;
	parms1.line = wrdbuf2;
	keyw_type = check_keyw_str(lno, wrd2, uls_ptr(parms1));
	len2 = parms1.len;

	if (keyw_type < 0) {
		err_log("%s<%d>: Invalid keyword", tag_nam, lno);
		err_log("\t:keyword:'%s'", wrd2);
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
		wrd3 = _uls_splitstr(uls_ptr(wrdx));

	} else {
		if ((e_keyw=__find_tokdef_by_keyw(uls, wrdbuf2)) != nilptr) {
			if (e_vx_grp == nilptr) {
				e_vx_grp = e_keyw->view;
			} else {
				// There exists a previously defined tok-def(e_vx_grp) with same keyword.
				if (e_keyw->view != e_vx_grp) {
					err_log("%s<%d>: the name is alrealy defined in another line.", tag_nam, lno);
					err_log("\tname: '%s' of '%s'", wrd1, wrd2);
					return nilptr;
				}
			}
		}

		wrd2 = wrdbuf2;
		wrd3 = _uls_splitstr(uls_ptr(wrdx));
	}

/* token-id */
	if (*wrd3 != '\0') {
		tok_id = uls_atoi(wrd3);
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
			err_log("%s<%d>: The keyword conflicts with the previous designation for token.", tag_nam, lno);
			err_log("\tkeyword:'%s'", wrd2);
			err_log("\ttoken %s(%d)", uls_get_namebuf_value(e_vx_grp->name), e_vx_grp->tok_id);
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
		err_log("%s<%d>: Aliasing of reserved-tok isn't permitted!", tag_nam, lno);
		err_log("\treserved-tok:%s", wrd1);
		return nilptr;

	} else if ((e2_vx=__find_tokdef_by_tokid(uls, tok_id, TOKDEF_AREA_REGULAR)) != nilptr) {
		e_vx_grp = e2_vx;

		if (uls_get_namebuf_value(e2_vx->name)[0] == '\0') {
			e2_vx->l_name = uls_set_namebuf_value(e2_vx->name, wrd1);
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
		uls_set_namebuf_value(e->keyword, wrd2);
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
			e->name = uls_get_namebuf_value(e_vx->name);
			slots_tok[uls->tokdef_array.n++] = e;
			slots_vx[uls->tokdef_vx_array.n++] = e_vx;
		}

	} else if (e != nilptr) {
		e->view = e_vx = e_vx_grp;

		if (e_nam != nilptr) {
			e->name = uls_get_namebuf_value(e_nam->name);
			append_tokdef_name_to_group(e_vx_grp, e_nam);
		} else {
			e->name = uls_get_namebuf_value(e_vx_grp->name);
		}

		slots_tok[uls->tokdef_array.n++] = e;
		append_tokdef_to_group(e_vx_grp, e);
	}

	parms->data = e;
	return e_vx;
}

uls_xcontext_ptr_t
_uls_get_xcontext(uls_lex_ptr_t uls)
{
	return uls_ptr(uls->xcontext);
}

int
initialize_ulc_lexattr()
{
	uls_cmd_ptr_t lexattr;

	uls_init_array_type00(uls_ptr(ulc_cmd_list), cmd, ULC_N_LEXATTRS);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 0);
	lexattr->name = "CASE_SENSITIVE";
	lexattr->proc = uls_ref_callback(read_config__CASE_SENSITIVE);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 1);
	lexattr->name = "COMMENT_TYPE";
	lexattr->proc = uls_ref_callback(read_config__COMMENT_TYPE);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 2);
	lexattr->name = "DECIMAL_SEPARATOR";
	lexattr->proc = uls_ref_callback(read_config__DECIMAL_SEPARATOR);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 3);
	lexattr->name = "DOMAIN";
	lexattr->proc = uls_ref_callback(read_config__DOMAIN);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 4);
	lexattr->name = "ID_CHARS";
	lexattr->proc = uls_ref_callback(read_config__ID_CHARS);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 5);
	lexattr->name = "ID_FIRST_CHARS";
	lexattr->proc = uls_ref_callback(read_config__ID_FIRST_CHARS);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 6);
	lexattr->name = "ID_MAX_LENGTH";
	lexattr->proc = uls_ref_callback(read_config__ID_MAX_LENGTH);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 7);
	lexattr->name = "NOT_CHAR_TOK";
	lexattr->proc = uls_ref_callback(read_config__NOT_CHAR_TOK);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 8);
	lexattr->name = "NUMBER_PREFIXES";
	lexattr->proc = uls_ref_callback(read_config__NUMBER_PREFIXES);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 9);
	lexattr->name = "NUMBER_SUFFIXES";
	lexattr->proc = uls_ref_callback(read_config__NUMBER_SUFFIXES);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 10);
	lexattr->name = "PREPEND_INPUT";
	lexattr->proc = uls_ref_callback(read_config__PREPEND_INPUT);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 11);
	lexattr->name = "QUOTE_TYPE";
	lexattr->proc = uls_ref_callback(read_config__QUOTE_TYPE);

	lexattr = uls_array_get_slot_type00(uls_ptr(ulc_cmd_list), 12);
	lexattr->name = "RENAME";
	lexattr->proc = uls_ref_callback(read_config__RENAME);

	return 0;
}

void
finalize_ulc_lexattr()
{
	uls_deinit_array_type00(uls_ptr(ulc_cmd_list), cmd);
}
