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
  <file> uls_wlex.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_WLEX_H__
#define __ULS_WLEX_H__

#include "uls/uls_lex.h"
#include "uls/uls_istream.h"
#include "uls/uls_tokdef_wstr.h"
#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif
#include "uls/uls_auw.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(wlex_shell)
{
	uls_flags_t flags;

	csz_str_t wtokbuf;
	int    wtokbuf_bytes, wtokbuf_len;

	csz_str_t wtag;
	int wtag_len;

	uls_id2wstr_pair_ptr_t wkeyw_list;
	int  wkeyw_list_siz, wkeyw_list_len;

	uls_id2wstr_pair_ptr_t wname_list;
	int  wname_list_siz, wname_list_len;

	uls_lex_ptr_t uls;
};

void uls_init_wstr_2(uls_wlex_shell_ptr_t wuls, uls_lex_ptr_t uls);

ULS_DLL_EXTERN const wchar_t* uls_tok2keyw_wstr(uls_lex_ptr_t uls, int t);
ULS_DLL_EXTERN const wchar_t* uls_tok2name_wstr(uls_lex_ptr_t uls, int t);
ULS_DLL_EXTERN const wchar_t* uls_tok2keyw_2_wstr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN const wchar_t* uls_tok2name_2_wstr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int uls_init_wstr(uls_lex_ptr_t uls, const wchar_t* confname);
ULS_DLL_EXTERN uls_lex_ptr_t uls_create_wstr(const wchar_t* confname);
ULS_DLL_EXTERN uls_lex_ptr_t uls_create_cr2lf_wstr(const wchar_t* confname);
ULS_DLL_EXTERN void uls_destroy_wstr(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_push_fd_wstr(uls_lex_ptr_t uls, int fd, int flags);
ULS_DLL_EXTERN int uls_set_fd_wstr(uls_lex_ptr_t uls, int fd, int flags);
ULS_DLL_EXTERN int uls_push_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags);
ULS_DLL_EXTERN int uls_set_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags);

ULS_DLL_EXTERN int uls_push_file_wstr(uls_lex_ptr_t uls, const wchar_t* wfilepath, int flags);
ULS_DLL_EXTERN int uls_set_file_wstr(uls_lex_ptr_t uls, const wchar_t* wfilepath, int flags);

ULS_DLL_EXTERN int uls_set_line_wstr(uls_lex_ptr_t uls, const wchar_t* wline, int wlen, int flags);
ULS_DLL_EXTERN int uls_push_line_wstr(uls_lex_ptr_t uls, const wchar_t* wline, int wlen, int flags);

ULS_DLL_EXTERN const wchar_t* uls_get_tag_wstr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_get_taglen_wstr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const wchar_t* _uls_get_tag2_wstr(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN void uls_set_tag_wstr(uls_lex_ptr_t uls, const wchar_t* wtag, int lno);

ULS_DLL_EXTERN int uls_get_wtok(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_set_wtok(uls_lex_ptr_t uls, int tokid, const wchar_t* wlexeme, int l_wlexeme);

ULS_DLL_EXTERN int uls_tokid_wstr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const wchar_t* uls_lexeme_wstr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_lexeme_len_wstr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_lexeme_chars_wstr(uls_lex_ptr_t uls);

ULS_DLL_EXTERN wchar_t uls_peek_wch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms);
ULS_DLL_EXTERN wchar_t uls_get_wch(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t parms);

ULS_DLL_EXTERN void uls_unget_lexeme_wstr(uls_lex_ptr_t uls, const wchar_t *lxm, int tok_id);
ULS_DLL_EXTERN void uls_unget_wstr(uls_lex_ptr_t uls, const wchar_t* wstr);
ULS_DLL_EXTERN void uls_unget_wch(uls_lex_ptr_t uls, wchar_t wch);

ULS_DLL_EXTERN void uls_dump_tok_wstr(uls_lex_ptr_t uls, const wchar_t *wpfx, const wchar_t *wsuff);
ULS_DLL_EXTERN void _uls_dump_tok_2_wstr(uls_lex_ptr_t uls, const wchar_t* wpfx,
	const wchar_t *id_wstr, const wchar_t *tok_wstr, const wchar_t *wsuff);

ULS_DLL_EXTERN int uls_push_istream_2_wstr(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const wchar_t** tmpl_wnams, const wchar_t** tmpl_wvals, int n_tmpls, int flags);
ULS_DLL_EXTERN const wchar_t* uls_get_tag2_wstr(uls_lex_ptr_t uls, int *ptr_len_wtag);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_WLEX_H__
