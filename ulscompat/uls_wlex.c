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
  <file> uls_wlex.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#include "uls/uls_lex.h"
#include "uls/unget.h"
#include "uls/uls_conf.h"
#include "uls/uls_fileio.h"
#include "uls/uls_util.h"

#include "uls/uls_wlex.h"
#include "uls/uls_tokdef_wstr.h"
#include "uls/uls_fileio_wstr.h"
#include "uls/uls_emit_wstr.h"
#include "uls/uls_auw.h"
#include "uls/uls_wlog.h"


ULS_DECL_STATIC void
init_id2wstr_pairs(uls_lex_ptr_t uls)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	uls_tokdef_vx_t *e_vx;
	const char *keyw, *name;
	int i, t, n_bytes;
	uls_outparam_t parms;

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		t = e_vx->tok_id;

		keyw = uls_tok2keyw_2(uls, t, uls_ptr(parms));
		n_bytes = parms.len;

		if (keyw != NULL) {
			parms.data = wuls->wkeyw_list;
			parms.len = wuls->wkeyw_list_len;
			parms.n = wuls->wkeyw_list_siz;
			wuls->wkeyw_list = __add_widename_in_list(uls_ptr(parms), t, keyw, n_bytes);
			wuls->wkeyw_list_siz = parms.n;
			++wuls->wkeyw_list_len;
		}

		name = uls_tok2name_2(uls, t, uls_ptr(parms));
		n_bytes = parms.len;

		if (name != NULL) {
			parms.data = wuls->wname_list;
			parms.len = wuls->wname_list_len;
			parms.n = wuls->wname_list_siz;
			wuls->wname_list = __add_widename_in_list(uls_ptr(parms), t, name, n_bytes);
			wuls->wname_list_siz = parms.n;
			++wuls->wname_list_len;
		}
	}
}

void uls_init_wstr_2(uls_wlex_shell_ptr_t wuls, uls_lex_ptr_t uls)
{
	wuls->flags = 0;
	wuls->uls = uls;
	uls->shell = wuls;

	csz_init(uls_ptr(wuls->wtokbuf), 64*sizeof(wchar_t));
	wuls->wtokbuf_len = wuls->wtokbuf_bytes = -1;

	csz_init(uls_ptr(wuls->wtag), 64*sizeof(wchar_t));
	wuls->wtag_len = -1;

	wuls->wkeyw_list = nilptr;
  	wuls->wkeyw_list_siz = wuls->wkeyw_list_len = 0;

	wuls->wname_list = nilptr;
	wuls->wname_list_siz = wuls->wname_list_len = 0;
}

const wchar_t*
uls_tok2keyw_2_wstr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	uls_id2wstr_pair_ptr_t pair;

	if ((pair = __find_widename_in_list(wuls->wkeyw_list, wuls->wkeyw_list_len, t)) == nilptr) {
		return NULL;
	}

	if (parms != nilptr) {
		parms->lptr = (char *) pair->wstr;
		parms->len = pair->n_chars;
	}

	return pair->wstr;
}

const wchar_t*
uls_tok2name_2_wstr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	uls_id2wstr_pair_ptr_t pair;

	if ((pair = __find_widename_in_list(wuls->wname_list, wuls->wname_list_len, t)) == nilptr) {
		return NULL;
	}

	if (parms != nilptr) {
		parms->lptr = (char *) pair->wstr;
		parms->len = pair->n_chars;
	}

	return pair->wstr;
}

const wchar_t*
uls_tok2keyw_wstr(uls_lex_ptr_t uls, int t)
{
	return uls_tok2keyw_2_wstr(uls, t, nilptr);
}

const wchar_t*
uls_tok2name_wstr(uls_lex_ptr_t uls, int t)
{
	return uls_tok2name_2_wstr(uls, t, nilptr);
}

int
uls_init_wstr(uls_lex_ptr_t uls, const wchar_t* confname)
{
	uls_wlex_shell_ptr_t wuls;
	int stat = 0;
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(confname, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		stat = -1;

	} else if (uls_init(uls, ustr) < 0) {
		err_wlog(L"can't create uls!");
		stat = -2;
	}

	csz_deinit(uls_ptr(csz));

	if (stat >= 0) {
		wuls = uls_alloc_object(uls_wlex_shell_t);
		uls_init_wstr_2(wuls, uls);
		init_id2wstr_pairs(uls);
	}

	return stat;
}

uls_lex_ptr_t
uls_create_wstr(const wchar_t* confname)
{
	uls_lex_ptr_t uls;

	uls = uls_alloc_object(uls_lex_t);

	if (uls_init_wstr(uls, confname) < 0) {
		uls_dealloc_object(uls);
		return nilptr;
	}

	uls->flags &= ~ULS_FL_STATIC;
	return uls;
}

void
uls_destroy_wstr(uls_lex_ptr_t uls)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	uls_id2wstr_pair_ptr_t pair;
	int i;

	for (i=0; i<wuls->wkeyw_list_len; i++) {
		pair = wuls->wkeyw_list + i;
		uls_mfree(pair->wstr);
	}
	uls_dealloc_object(wuls->wkeyw_list);
	wuls->wkeyw_list_siz = wuls->wkeyw_list_len = 0;

	for (i=0; i<wuls->wname_list_len; i++) {
		pair = wuls->wname_list + i;
		uls_mfree(pair->wstr);
	}
	uls_dealloc_object(wuls->wname_list);
	wuls->wname_list_siz = wuls->wname_list_len = 0;

	csz_deinit(uls_ptr(wuls->wtag));
	wuls->wtag_len = -1;

	csz_deinit(uls_ptr(wuls->wtokbuf));
	wuls->wtokbuf_len = wuls->wtokbuf_bytes = -1;

	if (uls_destroy(uls) < 0) {
		err_wlog(L"failed to destory auls");
		return;
	}

	uls_dealloc_object(wuls);
}

int
uls_push_fd_wstr(uls_lex_ptr_t uls, int fd, int flags)
{
	if (fd < 0) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	return uls_push_fd(uls, fd, flags);
}

int
uls_set_fd_wstr(uls_lex_ptr_t uls, int fd, int flags)
{
	if (fd < 0) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	return uls_set_fd(uls, fd, flags);
}

int
uls_push_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	if (fp == NULL) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	return uls_push_fp(uls, fp, flags);
}

int
uls_set_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	if (fp == NULL) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_fp_wstr(uls, fp, flags);
}

ULS_DECL_STATIC void
fp_ungrabber_wstr(uls_voidptr_t data)
{
	uls_outparam_ptr_t parm = (uls_outparam_ptr_t) data;
	FILE *fp = (FILE *) parm->native_data;

	uls_dealloc_object(parm);
	uls_fp_close(fp);
}

int
uls_push_file_wstr(uls_lex_ptr_t uls, const wchar_t* wfilepath, int flags)
{
	uls_outparam_ptr_t parm;
	FILE *fp;

	if (wfilepath == NULL) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	if ((fp = uls_fp_wopen(wfilepath, ULS_FIO_READ)) == NULL) {
		err_wlog(L"%s: can't open '%s'!", __FUNCTION__, wfilepath);
		return -1;
	}

	if (uls_push_fp_wstr(uls, fp, flags) < 0) {
		err_wlog(L"%s: internal error", __FUNCTION__);
		uls_fp_close(fp);
		return -1;
	}

	uls_set_tag_wstr(uls, wfilepath, -1);

	parm = uls_alloc_object(uls_outparam_t);
	parm->native_data = fp;

	uls_register_ungrabber(uls, 1, fp_ungrabber_wstr, parm);

	return 0;
}

int
uls_set_file_wstr(uls_lex_ptr_t uls, const wchar_t* wfilepath, int flags)
{
	if (wfilepath == NULL) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_file_wstr(uls, wfilepath, flags);
}

int
uls_push_line_wstr(uls_lex_ptr_t uls, const wchar_t* wline, int wlen, int flags)
{
	uls_uint16   *wbuff;
	int i;

	if (wline == NULL) {
		err_wlog(L"%s: fail to set wide string", __FUNCTION__);
		return -1;
	}

	if (wlen < 0) wlen = uls_wcslen(wline);
	if (wlen == 0) return -1;

	wbuff = (uls_uint16 *) uls_malloc((wlen+1) * sizeof(uls_uint16));
	for (i=0; i<wlen; i++) {
		wbuff[i] = (uls_uint16) wline[i];
	}
	wbuff[i] = L'\0';

	uls_push_utf16_line(uls, wbuff, wlen);
	uls_mfree(wbuff);

	if (flags & ULS_WANT_EOFTOK) uls_want_eof(uls);
	else uls_unwant_eof(uls);

	return 0;
}

int
uls_set_line_wstr(uls_lex_ptr_t uls, const wchar_t* wline, int wlen, int flags)
{
	uls_uint16   *wbuff;
	int i;

	if (wline == NULL) {
		err_wlog(L"%s: fail to set wide string", __FUNCTION__);
		return -1;
	}

	if (wlen < 0) wlen = uls_wcslen(wline);
	if (wlen == 0) return -1;

	wbuff = (uls_uint16 *) uls_malloc((wlen+1) * sizeof(uls_uint16));
	for (i=0; i<wlen; i++) {
		wbuff[i] = (uls_uint16) wline[i];
	}
	wbuff[i] = L'\0';

	uls_set_utf16_line(uls, wbuff, wlen);
	uls_mfree(wbuff);

	if (flags & ULS_WANT_EOFTOK) uls_want_eof(uls);
	else uls_unwant_eof(uls);

	return 0;
}

void
uls_set_tag_wstr(uls_lex_ptr_t uls, const wchar_t* wtag, int lno)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wtag, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"incorrect encoding!");
	} else {
		uls_set_tag(uls, ustr, lno);
		wuls->wtag_len = -1;
	}

	csz_deinit(uls_ptr(csz));
}

const wchar_t*
_uls_get_tag2_wstr(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	wchar_t *wtag;
	const char *tagstr;
	int k;

	if (wuls->wtag_len < 0) {
		tagstr = uls_context_get_tag(xctx->context);
		k = uls_context_get_taglen(xctx->context);
		if (tagstr == NULL) {
			tagstr = "";
			k = 0;
		}

		wtag = uls_ustr2wstr(tagstr, -k, uls_ptr(wuls->wtag));
		if (wtag == NULL) err_wpanic(L"incorrect encoding!");
		wuls->wtag_len = auw_csz_wlen(uls_ptr(wuls->wtag));

	} else {
		wtag = auw_csz_wstr(uls_ptr(wuls->wtag));
	}

	if (parms != nilptr) {
		parms->lptr = (const char *) wtag;
		parms->len = wuls->wtag_len;
	}

	return wtag;
}

const wchar_t*
uls_get_tag2_wstr(uls_lex_ptr_t uls, int *ptr_len_wtag)
{
	const wchar_t *wtag;
	uls_outparam_t parms;

	wtag = _uls_get_tag2_wstr(uls, uls_ptr(parms));
	if (ptr_len_wtag != NULL) {
		*ptr_len_wtag = parms.len;
	}

	return wtag;
}

const wchar_t*
uls_get_tag_wstr(uls_lex_ptr_t uls)
{
	return uls_get_tag2_wstr(uls, NULL);
}

int
uls_get_taglen_wstr(uls_lex_ptr_t uls)
{
	int len;
	uls_get_tag2_wstr(uls, &len);
	return len;
}

int
uls_get_wtok(uls_lex_ptr_t uls)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	int t;

	t = uls_get_tok(uls);
	wuls->wtokbuf_len = wuls->wtokbuf_bytes = -1;

	return t;
}

void
uls_set_wtok(uls_lex_ptr_t uls, int tokid, const wchar_t* wlexeme, int l_wlexeme)
{
	char *ustr;
	int  ulen, wlen;
	csz_str_t csz;

	if (l_wlexeme < 0) {
		l_wlexeme = -1;
		wlen = uls_wcslen(wlexeme);
	} else {
		wlen = l_wlexeme;
	}

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wlexeme, wlen, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		ulen = csz_length(uls_ptr(csz));
		uls_set_tok(uls, tokid, ustr, ulen);
	}

	csz_deinit(uls_ptr(csz));
}

int
uls_tokid_wstr(uls_lex_ptr_t uls)
{
	return _uls_tok_id(uls);
}

const wchar_t*
uls_lexeme_wstr(uls_lex_ptr_t uls)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;
	const char *lxm;
	wchar_t *wlxm;
	int l_lxm;

	if (wuls->wtokbuf_bytes < 0) {
		lxm = uls_lexeme(uls);
		l_lxm = uls_lexeme_len(uls);

		wlxm = uls_ustr2wstr(lxm, -l_lxm, uls_ptr(wuls->wtokbuf));
		if (wlxm == NULL)
			err_wpanic(L"incorrect encoding!");

		wuls->wtokbuf_len = auw_csz_wlen(uls_ptr(wuls->wtokbuf));
		wuls->wtokbuf_bytes = wuls->wtokbuf_len * sizeof(wchar_t);

	} else {
		wlxm = auw_csz_wstr(uls_ptr(wuls->wtokbuf));
	}

	return wlxm;
}

int
uls_lexeme_len_wstr(uls_lex_ptr_t uls)
{
	uls_wlex_shell_ptr_t wuls = (uls_wlex_shell_ptr_t) uls->shell;

	if (wuls->wtokbuf_bytes < 0)
		uls_lexeme_wstr(uls);

	return wuls->wtokbuf_len;
}

int
uls_lexeme_chars_wstr(uls_lex_ptr_t uls)
{
	return uls_lexeme_len_wstr(uls);
}

wchar_t
uls_peek_wch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms)
{
	uls_uch_t uch;
	uch = uls_peek_uch(uls, parms);
	return (wchar_t) uch;
}

wchar_t
uls_get_wch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms)
{
	uls_uch_t uch;
	uch = uls_get_uch(uls, parms);
	return (wchar_t) uch;
}

void
uls_unget_lexeme_wstr(uls_lex_ptr_t uls, const wchar_t *lxm, int tok_id)
{
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(lxm, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		uls_unget_lexeme(uls, ustr, tok_id);
	}

	csz_deinit(uls_ptr(csz));
}

void
uls_unget_wstr(uls_lex_ptr_t uls, const wchar_t* wstr)
{
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wstr, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		uls_unget_str(uls, ustr);
	}

	csz_deinit(uls_ptr(csz));
}

void
uls_unget_wch(uls_lex_ptr_t uls, wchar_t wch)
{
	char *ustr;
	csz_str_t csz;
	wchar_t wbuff[2];

	wbuff[0] = wch;
	wbuff[1] = L'\0';

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wbuff, 1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		uls_unget_str(uls, ustr);
	}

	csz_deinit(uls_ptr(csz));
}

int
uls_push_istream_2_wstr(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const wchar_t** tmpl_wnams, const wchar_t** tmpl_wvals, int n_tmpls, int flags)
{
	int i, stat=0;
	char **ustr_nams=NULL, **ustr_vals=NULL;
	csz_str_ptr_t csz_nam_ary, csz_val_ary;

	if (n_tmpls <= 0) {
		tmpl_wnams = tmpl_wvals = NULL;
		n_tmpls = 0;
	} else if (tmpl_wnams == NULL || tmpl_wvals == NULL) {
		err_wlog(L"%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	for (i=0; i<n_tmpls; i++) {
		if (tmpl_wnams[i] == NULL || tmpl_wvals[i] == NULL) {
			return -2;
		}
	}

	ustr_nams = (char **) uls_malloc(n_tmpls * sizeof(char *));
	ustr_vals = (char **) uls_malloc(n_tmpls * sizeof(char *));

	csz_nam_ary = (csz_str_ptr_t) uls_malloc(n_tmpls * sizeof(csz_str_t));
	csz_val_ary = (csz_str_ptr_t) uls_malloc(n_tmpls * sizeof(csz_str_t));

	for (i=0; ; i++) {
		if (i >= n_tmpls) {
			stat = uls_push_istream_2(uls, istr,
				(const char **) ustr_nams, (const char **) ustr_vals, n_tmpls, flags);
			--i;
			break;
		}

		csz_init(csz_nam_ary + i, -1);

		if ((ustr_nams[i] = uls_wstr2ustr(tmpl_wnams[i], -1, csz_nam_ary + i)) == NULL) {
			err_wlog(L"encoding error!");
			csz_deinit(csz_nam_ary + i);
			--i;
			stat = -1;
			break;
		}

		csz_init(csz_val_ary + i, -1);

		if ((ustr_vals[i] = uls_wstr2ustr(tmpl_wvals[i], -1, csz_val_ary + i)) == NULL) {
			err_wlog(L"encoding error!");
			stat = -1;
			break;
		}
	}

	for ( ; i>=0; i--) {
		csz_deinit(csz_nam_ary + i);
		csz_deinit(csz_val_ary + i);
	}

	for (i=0; i<n_tmpls; i++) {
		uls_mfree(ustr_nams[i]);
		uls_mfree(ustr_vals[i]);
	}

	uls_mfree(ustr_nams);
	uls_mfree(ustr_vals);

	return stat;
}

void
uls_dump_tok_wstr(uls_lex_ptr_t uls, const wchar_t *wpfx, const wchar_t *wsuff)
{
	int tok_id = uls_tok(uls), has_lxm;
	const wchar_t *tok_wstr = uls_lexeme_wstr(uls);
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];
	char lxmpfx[ULS_CARDINAL_LXMPFX_MAXSIZ+1];

	csz_str_t csz;
	const char *tok_str;
	uls_outparam_t parms;

	if (wpfx == NULL) wpfx = L"";
	if (wsuff == NULL) wsuff = L"";

	csz_init(uls_ptr(csz), -1);

	if ((tok_str = uls_wstr2ustr(tok_wstr, -1, uls_ptr(csz))) == NULL) {
		csz_deinit(uls_ptr(csz));
		err_wpanic(L"encoding error!");
	}

	parms.lptr = tok_str;
	has_lxm = uls_cardinal_toknam_deco_lxmpfx(toknam_buff, lxmpfx, uls, tok_id, uls_ptr(parms));
	tok_str = parms.lptr;

	if (has_lxm) {
		uls_printf("%S%s %s%s%S", wpfx, toknam_buff, lxmpfx, tok_str, wsuff);
	} else {
		uls_printf("%S%s%S", wpfx, toknam_buff, wsuff);
	}

	csz_deinit(uls_ptr(csz));
}

void
_uls_dump_tok_2_wstr(uls_lex_ptr_t uls, const wchar_t* wpfx,
	const wchar_t *id_wstr, const wchar_t *tok_wstr, const wchar_t *wsuff)
{
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];
	char *ustr;
	csz_str_t csz;

	if (wpfx == NULL) wpfx = L"";
	if (wsuff == NULL) wsuff = L"";

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(id_wstr, -1, uls_ptr(csz))) == NULL) {
		csz_deinit(uls_ptr(csz));
		err_wpanic(L"encoding error!");
	}

	uls_cardinal_toknam_deco(toknam_buff, ustr);
	uls_printf("%S%s %S%S", wpfx, toknam_buff, tok_wstr, wsuff);

	csz_deinit(uls_ptr(csz));
}