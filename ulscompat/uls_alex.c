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
  <file> uls_alex.c </file>
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

#include "uls/uls_alex.h"
#include "uls/uls_tokdef_astr.h"
#include "uls/uls_fileio_astr.h"
#include "uls/uls_emit_astr.h"
#include "uls/uls_auw.h"
#include "uls/ms_mbcs_file.h"
#include "uls/uls_alog.h"

ULS_DECL_STATIC void
init_id2astr_pairs(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
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
			__add_ms_mbcs_name_in_list(uls_ptr(auls->akeyw_list),
				auls->akeyw_list.n, uls_ptr(auls->akeyw_list.n_alloc),
				t, keyw, n_bytes);
			++auls->akeyw_list.n;
		}

		name = uls_tok2name_2(uls, t, uls_ptr(parms));
		n_bytes = parms.len;

		if (name != NULL) {
			__add_ms_mbcs_name_in_list(uls_ptr(auls->aname_list),
				auls->aname_list.n, uls_ptr(auls->aname_list.n_alloc),
				t, name, n_bytes);
			++auls->aname_list.n;
		}
	}
}

void
uls_init_astr_2(uls_alex_shell_ptr_t auls, uls_lex_ptr_t uls)
{
	auls->flags = 0;
	auls->uls = uls;
	uls->shell = auls;

	csz_init(uls_ptr(auls->atokbuf), 64);
	auls->atokbuf_len = auls->atokbuf_bytes = -1;

	csz_init(uls_ptr(auls->atag), 64);
	auls->atag_len = -1;

	uls_init_parray(uls_ptr(auls->akeyw_list), id2astr_pair, 0);
	auls->akeyw_list.n = 0;

	uls_init_parray(uls_ptr(auls->aname_list), id2astr_pair, 0);
	auls->aname_list.n = 0;
}

ULS_DLL_EXTERN const char*
uls_tok2keyw_2_astr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	uls_id2astr_pair_ptr_t pair;

	if ((pair = __find_ms_mbcs_name_in_list(uls_ptr(auls->akeyw_list), auls->akeyw_list.n, t)) == nilptr) {
		return NULL;
	}

	if (parms != nilptr) {
		parms->lptr = pair->astr;
		parms->len = pair->n_chars;
	}

	return pair->astr;
}

ULS_DLL_EXTERN const char*
uls_tok2name_2_astr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	uls_id2astr_pair_ptr_t pair;

	if ((pair = __find_ms_mbcs_name_in_list(uls_ptr(auls->aname_list), auls->aname_list.n, t)) == nilptr) {
		return NULL;
	}

	if (parms != nilptr) {
		parms->lptr = pair->astr;
		parms->len = pair->n_chars;
	}

	return pair->astr;
}

ULS_DLL_EXTERN const char*
uls_tok2keyw_astr(uls_lex_ptr_t uls, int t)
{
	return uls_tok2keyw_2_astr(uls, t, nilptr);
}

ULS_DLL_EXTERN const char*
uls_tok2name_astr(uls_lex_ptr_t uls, int t)
{
	return uls_tok2name_2_astr(uls, t, nilptr);
}

ULS_DLL_EXTERN int
uls_init_astr(uls_lex_ptr_t uls, const char* confname)
{
	const char *ustr;
	int stat = 0;
	auw_outparam_t buf_csz;
	uls_alex_shell_ptr_t auls;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(confname, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		stat = -1;

	} else if (uls_init(uls, ustr) < 0) {
		err_alog("can't create uls!");
		stat = -2;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	if (stat >= 0) {
		auls = uls_alloc_object(uls_alex_shell_t);
		uls_init_astr_2(auls, uls);
		init_id2astr_pairs(uls);
	}

	return 0;
}

ULS_DLL_EXTERN uls_lex_ptr_t
uls_create_astr(const char* confname)
{
	uls_lex_ptr_t uls;

	uls = uls_alloc_object(uls_lex_t);

	if (uls_init_astr(uls, confname) < 0) {
		uls_dealloc_object(uls);
		return nilptr;
	}

	uls->flags &= ~ULS_FL_STATIC;
	return uls;
}

ULS_DLL_EXTERN void
uls_destroy_astr(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	uls_decl_parray_slots(slots_pair, id2astr_pair);
	uls_id2astr_pair_ptr_t pair;
	int i;

	slots_pair = uls_parray_slots(uls_ptr(auls->akeyw_list));
	for (i=0; i<auls->akeyw_list.n; i++) {
		pair = slots_pair[i];
		uls_dealloc_object(pair->astr);
	}
	uls_deinit_parray(uls_ptr(auls->akeyw_list));

	slots_pair = uls_parray_slots(uls_ptr(auls->aname_list));
	for (i=0; i<auls->aname_list.n; i++) {
		pair = slots_pair[i];
		uls_dealloc_object(pair->astr);
	}
	uls_deinit_parray(uls_ptr(auls->aname_list));

	csz_deinit(uls_ptr(auls->atag));
	auls->atag_len = -1;

	csz_deinit(uls_ptr(auls->atokbuf));
	auls->atokbuf_len = auls->atokbuf_bytes = -1;

	if (uls_destroy(uls) < 0) {
		err_alog("failed to destory auls");
		return;
	}

	uls_dealloc_object(auls);
}

ULS_DLL_EXTERN int
uls_push_fd_astr(uls_lex_ptr_t uls, int fd, int flags)
{
	if (fd < 0) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	if (flags & ULS_FILE_MS_MBCS) {
		err_alog("%s: unsupported flag ULS_FILE_MS_MBCS", __FUNCTION__);
		return -1;
	}

	return uls_push_fd(uls, fd, flags);
}

ULS_DLL_EXTERN int
uls_set_fd_astr(uls_lex_ptr_t uls, int fd, int flags)
{
	if (fd < 0) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	if (flags & ULS_FILE_MS_MBCS) {
		err_alog("%s: unsupported flag ULS_FILE_MS_MBCS", __FUNCTION__);
		return -1;
	}

	return uls_set_fd(uls, fd, flags);
}

ULS_DLL_EXTERN int
uls_push_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	uls_tempfile_ptr_t tmpfile_utf8;
	FILE *fp2;

	if (fp == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	tmpfile_utf8 = uls_create_tempfile();

	if ((fp2 = cvt_ms_mbcs_fp(fp, tmpfile_utf8, flags)) == NULL) {
		err_alog("%s: encoding error!", __FUNCTION__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	if (uls_push_fp(uls, fp2, flags) < 0) {
		err_alog("%s: can't prepare input!", __FUNCTION__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	uls_register_ungrabber(uls, 0, ms_mbcs_tmpf_ungrabber, tmpfile_utf8);

	return 0;
}

ULS_DLL_EXTERN int
uls_set_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	if (fp == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_fp_astr(uls, fp, flags);
}

ULS_DLL_EXTERN int
uls_push_file_astr(uls_lex_ptr_t uls, const char* filepath, int flags)
{
	uls_outparam_ptr_t parm;
	FILE *fp;

	if (filepath == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	if ((fp = uls_fp_aopen(filepath, ULS_FIO_READ)) == NULL) {
		err_alog("%s: can't open '%s'!", __FUNCTION__, filepath);
		return -1;
	}

	if (uls_push_fp_astr(uls, fp, flags) < 0) {
		err_alog("%s: internal error", __FUNCTION__);
		uls_fp_close(fp);
		return -1;
	}

	uls_set_tag_astr(uls, filepath, -1);

	parm = uls_alloc_object(uls_outparam_t);
	parm->native_data = fp;
	uls_register_ungrabber(uls, 1, ms_mbcs_fp_ungrabber, parm);

	return 0;
}

ULS_DLL_EXTERN int
uls_set_file_astr(uls_lex_ptr_t uls, const char* filepath, int flags)
{
	if (filepath == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_file_astr(uls, filepath, flags);
}

ULS_DLL_EXTERN int
uls_push_line_astr(uls_lex_ptr_t uls, const char* aline, int alen, int flags)
{
	char *ustr;
	int ulen, stat=0;
	csz_str_t csz;

	if (aline == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	if (alen < 0) alen = -1;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_astr2ustr(aline, alen, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
		stat = -1;

	} else {
		ulen = csz_length(uls_ptr(csz));
		if (uls_push_line(uls, ustr, ulen, flags | ULS_DO_DUP) < 0) {
			stat = -1;
		}
	}

	csz_deinit(uls_ptr(csz));

	return stat;
}

ULS_DLL_EXTERN int
uls_set_line_astr(uls_lex_ptr_t uls, const char* aline, int alen, int flags)
{
	if (aline == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	uls_pop(uls);
	return uls_push_line_astr(uls, aline, alen, flags);
}

ULS_DLL_EXTERN void
uls_set_tag_astr(uls_lex_ptr_t uls, const char* atag, int lno)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(atag, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		uls_set_tag(uls, ustr, lno);
		auls->atag_len = -1;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

ULS_DLL_EXTERN const char*
_uls_get_tag2_astr(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	const char *tagstr, *atag;
	int k;

	if (auls->atag_len < 0) {
		tagstr = uls_context_get_tag(xctx->context);
		k = uls_context_get_taglen(xctx->context);
		if (tagstr == NULL) {
			tagstr = "";
			k = 0;
		}

		atag = uls_ustr2astr(tagstr, -k, uls_ptr(auls->atag));
		if (atag == NULL) err_apanic("encoding error!");
		auls->atag_len = astr_num_chars(atag, -1, nilptr);
		// auls->atag_len: # of chars

	} else {
		atag = csz_data_ptr(uls_ptr(auls->atag));
	}

	if (parms != nilptr) {
		parms->lptr = atag;
		parms->len = csz_length(uls_ptr(auls->atag));
	}

	return atag;
}

ULS_DLL_EXTERN const char*
uls_get_tag2_astr(uls_lex_ptr_t uls, int *ptr_len_atag)
{
	const char *atag;
	uls_outparam_t parms;

	atag = _uls_get_tag2_astr(uls, uls_ptr(parms));
	if (ptr_len_atag != NULL) {
		*ptr_len_atag = parms.len;
	}

	return atag;
}

ULS_DLL_EXTERN const char*
uls_get_tag_astr(uls_lex_ptr_t uls)
{
	return uls_get_tag2_astr(uls, NULL);
}

ULS_DLL_EXTERN int
uls_get_taglen_astr(uls_lex_ptr_t uls)
{
	int len;
	uls_get_tag2_astr(uls, &len);
	return len;
}

ULS_DLL_EXTERN int
uls_get_atok(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	int t;

	t = uls_get_tok(uls);
	auls->atokbuf_len = auls->atokbuf_bytes = -1;

	return t;
}

ULS_DLL_EXTERN void
uls_set_atok(uls_lex_ptr_t uls, int tokid, const char* lexeme, int l_lexeme)
{
	char *ustr;
	int ulen;
	csz_str_t csz;

	if (l_lexeme < 0) {
		l_lexeme = -1;
	}

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_astr2ustr(lexeme, l_lexeme, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		ulen = csz_length(uls_ptr(csz));
		uls_set_tok(uls, tokid, ustr, ulen);
	}

	csz_deinit(uls_ptr(csz));
}

ULS_DLL_EXTERN int
uls_tokid_astr(uls_lex_ptr_t uls)
{
	return _uls_tok_id(uls);
}

ULS_DLL_EXTERN const char*
uls_lexeme_astr(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;
	const char *lxm, *alxm;
	int l_lxm;

	if (auls->atokbuf_len < 0) {
		lxm = uls_lexeme(uls);
		l_lxm = uls_lexeme_len(uls);

		alxm = uls_ustr2astr(lxm, -l_lxm, uls_ptr(auls->atokbuf));
		if (alxm == NULL)
			err_apanic("encoding error!");

		auls->atokbuf_bytes = csz_length(uls_ptr(auls->atokbuf));
		auls->atokbuf_len = astr_num_chars(alxm, auls->atokbuf_bytes, nilptr);

	} else {
		alxm = csz_data_ptr(uls_ptr(auls->atokbuf));
	}

	return alxm;
}

ULS_DLL_EXTERN int
uls_lexeme_len_astr(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;

	if (auls->atokbuf_len < 0)
		uls_lexeme_astr(uls);

	return auls->atokbuf_bytes;
}

ULS_DLL_EXTERN int
uls_lexeme_chars_astr(uls_lex_ptr_t uls)
{
	uls_alex_shell_ptr_t auls = (uls_alex_shell_ptr_t) uls->shell;

	if (auls->atokbuf_len < 0)
		uls_lexeme_astr(uls);

	return auls->atokbuf_len;
}

ULS_DLL_EXTERN uls_uch_t
uls_peek_ach(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms)
{
	uls_uch_t uch;
	uch = uls_peek_uch(uls, parms);
	return uch;
}

ULS_DLL_EXTERN uls_uch_t
uls_get_ach(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms)
{
	uls_uch_t uch;
	uch = uls_get_uch(uls, parms);
	return uch;
}

ULS_DLL_EXTERN void
uls_unget_lexeme_astr(uls_lex_ptr_t uls, const char *lxm, int tok_id)
{
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(lxm, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		uls_unget_lexeme(uls, ustr, tok_id);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

ULS_DLL_EXTERN void
uls_unget_astr(uls_lex_ptr_t uls, const char* astr)
{
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(astr, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		uls_unget_str(uls, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

ULS_DLL_EXTERN void
uls_unget_ach(uls_lex_ptr_t uls, uls_uch_t uch)
{
	char ch = (char) uch;
	uls_unget_str(uls, uls_ptr(ch));
}

ULS_DLL_EXTERN int
uls_push_istream_2_astr(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const char** tmpl_nams, const char** tmpl_vals, int n_tmpls, int flags)
{
	int i, rc1, rc2, stat=0;
	char **ustr_nams=NULL, **ustr_vals=NULL;
	csz_str_ptr_t csz_nam_ary, csz_val_ary;
	uls_outparam_t parms1;

	if (n_tmpls <= 0) {
		tmpl_nams = tmpl_vals = NULL;
		n_tmpls = 0;
	} else if (tmpl_nams == NULL || tmpl_vals == NULL) {
		err_alog("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	for (i=0; i<n_tmpls; i++) {
		if (tmpl_nams[i] == NULL || tmpl_vals[i] == NULL) {
			return -1;
		}

		rc2 = ustr_num_chars(tmpl_nams[i], -1, uls_ptr(parms1));
		rc1 = parms1.len;

		if (rc2 != rc1 || rc2 == 0) {
			// tmpl_nams[i] contains non-ascii chars.
			return -2;
		}
	}

	ustr_nams = (char **) uls_malloc(n_tmpls * sizeof(const char *));
	ustr_vals = (char **) uls_malloc(n_tmpls * sizeof(const char *));

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

		if ((ustr_nams[i] = uls_astr2ustr(tmpl_nams[i], -1, csz_nam_ary + i)) == NULL) {
			err_alog("encoding error!");
			csz_deinit(csz_nam_ary + i);
			--i;
			stat = -1;
			break;
		}

		csz_init(csz_val_ary + i, -1);

		if ((ustr_vals[i] = uls_astr2ustr(tmpl_vals[i], -1, csz_val_ary + i)) == NULL) {
			err_alog("encoding error!");
			stat = -1;
			break;
		}
	}

	uls_mfree(ustr_nams);
	uls_mfree(ustr_vals);

	for ( ; i>=0; i--) {
		csz_deinit(csz_nam_ary + i);
		csz_deinit(csz_val_ary + i);
	}

	uls_mfree(csz_nam_ary);
	uls_mfree(csz_val_ary);

	return stat;
}

ULS_DLL_EXTERN void
uls_dump_tok_astr(uls_lex_ptr_t uls, const char *pfx, const char *suff)
{
	int tok_id = uls_tok(uls), has_lxm;
	const char *tok_str = uls_lexeme_astr(uls);
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];
	char lxmpfx[ULS_CARDINAL_LXMPFX_MAXSIZ+1];
	uls_outparam_t parms;

	if (pfx == NULL) pfx = "";
	if (suff == NULL) suff = "";

	parms.lptr = tok_str;
	has_lxm = uls_cardinal_toknam_deco_lxmpfx(toknam_buff, lxmpfx, uls, tok_id, uls_ptr(parms));
	tok_str = parms.lptr;

	if (has_lxm) {
		uls_aprintf("%s%s %s%s%s", pfx, toknam_buff, lxmpfx, tok_str, suff);
	} else {
		uls_aprintf("%s%s%s", pfx, toknam_buff, suff);
	}
}

ULS_DLL_EXTERN void
_uls_dump_tok_2_astr(uls_lex_ptr_t uls, const char* apfx,
	const char *id_astr, const char *tok_astr, const char *asuff)
{
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];

	if (apfx == NULL) apfx = "";
	if (asuff == NULL) asuff = "";

	uls_cardinal_toknam_deco(toknam_buff, id_astr);
	uls_aprintf("%s%s %s%s", apfx, toknam_buff, tok_astr, asuff);
}
