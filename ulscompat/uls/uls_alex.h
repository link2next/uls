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
  <file> uls_alex.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_ALEX_H__
#define __ULS_ALEX_H__

#include "uls/uls_lex.h"
#include "uls/uls_istream.h"
#include "uls/uls_tokdef_astr.h"
#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(alex_shell)
{
	uls_flags_t flags;

	csz_str_t atokbuf;
	int    atokbuf_len, atokbuf_bytes;

	csz_str_t atag;
	int  atag_len;

	uls_decl_parray(akeyw_list, id2astr_pair);
	uls_decl_parray(aname_list, id2astr_pair);

	uls_lex_ptr_t uls;
};

void uls_init_astr_2(uls_alex_shell_ptr_t auls, uls_lex_ptr_t uls);

ULS_DLL_EXTERN const char* uls_tok2keyw_astr(uls_lex_ptr_t uls, int t);
ULS_DLL_EXTERN const char* uls_tok2name_astr(uls_lex_ptr_t uls, int t);
ULS_DLL_EXTERN const char* uls_tok2keyw_2_astr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN const char* uls_tok2name_2_astr(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int uls_init_astr(uls_lex_ptr_t uls, const char* confname);
ULS_DLL_EXTERN uls_lex_ptr_t uls_create_astr(const char* confname);
ULS_DLL_EXTERN uls_lex_ptr_t uls_create_cr2lf_astr(const char* confname);
ULS_DLL_EXTERN void uls_destroy_astr(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_push_fd_astr(uls_lex_ptr_t uls, int fd, int flags);
ULS_DLL_EXTERN int uls_set_fd_astr(uls_lex_ptr_t uls, int fd, int flags);

ULS_DLL_EXTERN int uls_push_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags);
ULS_DLL_EXTERN int uls_set_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags);
ULS_DLL_EXTERN int uls_push_file_astr(uls_lex_ptr_t uls, const char* filepath, int flags);
ULS_DLL_EXTERN int uls_set_file_astr(uls_lex_ptr_t uls, const char* filepath, int flags);

ULS_DLL_EXTERN int uls_set_line_astr(uls_lex_ptr_t uls, const char* aline, int alen, int flags);
ULS_DLL_EXTERN int uls_push_line_astr(uls_lex_ptr_t uls, const char* aline, int alen, int flags);

ULS_DLL_EXTERN const char* uls_get_tag_astr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_get_taglen_astr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const char* _uls_get_tag2_astr(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN void uls_set_tag_astr(uls_lex_ptr_t uls, const char* atag, int lno);

ULS_DLL_EXTERN int uls_get_atok(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_set_atok(uls_lex_ptr_t uls, int tokid, const char* lexeme, int l_lexeme);

ULS_DLL_EXTERN int uls_tokid_astr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const char* uls_lexeme_astr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_lexeme_len_astr(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_lexeme_chars_astr(uls_lex_ptr_t uls);

ULS_DLL_EXTERN uls_uch_t uls_peek_ach(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch);
ULS_DLL_EXTERN uls_uch_t uls_get_ach(uls_lex_ptr_t uls, uls_nextch_detail_ptr_t detail_ch);

ULS_DLL_EXTERN void uls_unget_lexeme_astr(uls_lex_ptr_t uls, const char *lxm, int tok_id);
ULS_DLL_EXTERN void uls_unget_astr(uls_lex_ptr_t uls, const char* str);
ULS_DLL_EXTERN void uls_unget_ach(uls_lex_ptr_t uls, uls_uch_t uch);

ULS_DLL_EXTERN void uls_dump_tok_astr(uls_lex_ptr_t uls, const char *pfx, const char *suff);
ULS_DLL_EXTERN void _uls_dump_tok_2_astr(uls_lex_ptr_t uls, const char* apfx,
	const char *id_astr, const char *tok_astr, const char *asuff);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN int uls_push_istream_2_astr(uls_lex_ptr_t uls, uls_istream_ptr_t istr,
	const char** tmpl_nams, const char** tmpl_vals, int n_tmpls, int flags);
ULS_DLL_EXTERN const char* uls_get_tag2_astr(uls_lex_ptr_t uls, int *ptr_len_atag);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_tok2keyw uls_tok2keyw_astr
#define uls_tok2name uls_tok2name_astr
#define uls_tok2keyw_2 uls_tok2keyw_2_astr
#define uls_tok2name_2 uls_tok2name_2_astr

#define uls_init uls_init_astr
#define uls_create uls_create_astr
#define uls_destroy uls_destroy_astr

#define uls_push_fd uls_push_fd_astr
#define uls_set_fd uls_set_fd_astr
#define uls_push_fp uls_push_fp_astr
#define uls_set_fp uls_set_fp_astr

#define uls_push_file uls_push_file_astr
#define uls_set_file uls_set_file_astr

#define uls_set_line uls_set_line_astr
#define uls_push_line uls_push_line_astr

#undef uls_get_tag
#define uls_get_tag uls_get_tag_astr
#undef uls_get_taglen
#define uls_get_taglen uls_get_taglen_astr
#define uls_get_tag2 uls_get_tag2_astr
#define uls_set_tag uls_set_tag_astr

#define uls_get_tok uls_get_atok
#define uls_set_tok uls_set_atok

#undef uls_tok
#define uls_tok    uls_tokid_astr

#undef uls_lexeme
#define uls_lexeme uls_lexeme_astr

#undef uls_lexeme_len
#define uls_lexeme_len uls_lexeme_len_astr

#undef uls_lexeme_chars
#define uls_lexeme_chars uls_lexeme_chars_astr

#define uls_unget_lexeme uls_unget_lexeme_astr
#define uls_unget_str uls_unget_astr
#define uls_unget_ch uls_unget_ach

#define uls_push_istream_2 uls_push_istream_2_astr
#define uls_dump_tok uls_dump_tok_astr
#define _uls_dump_tok_2 _uls_dump_tok_2_astr

#endif
#endif

#endif // __ULS_ALEX_H__
