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
 * litstr.h -- utility routines for constituting ULS --
 *     written by Stanley Hong <link2next@gmail.com>, August 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_LITSTR_H__
#define __ULS_LITSTR_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#include "uls/uls_tokdef.h"
#include "uls/litesc.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define ULS_LITPROC_ERROR        -2
#define ULS_LITPROC_DISMISSQUOTE -1
#define ULS_LITPROC_ENDOFQUOTE    0
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
#define ULS_QSTR_MULTILINE     0x01
#define ULS_QSTR_NOTHING       0x02
#define ULS_QSTR_OPEN          0x04
#define ULS_QSTR_R_EXCLUSIVE   0x08
#define ULS_QSTR_ASYMMETRIC    0x10

ULS_DECLARE_STRUCT(litstr);
ULS_DECLARE_STRUCT(litstr_context);

ULS_DEFINE_DELEGATE_BEGIN(litstr_analyzer,int)(uls_litstr_ptr_t lit);
ULS_DEFINE_DELEGATE_END(litstr_analyzer);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(quotetype)
{
	int  tok_id;

	uls_flags_t flags;
	uls_tokdef_vx_ptr_t tokdef_vx;

	uls_def_namebuf(start_mark, ULS_QUOTE_MARK_MAXSIZ);
	int  len_start_mark;

	uls_def_namebuf(end_mark, ULS_QUOTE_MARK_MAXSIZ);
	int  len_end_mark;

	uls_escmap_ptr_t escmap;
	int  n_left_lfs, n_lfs;

	uls_callback_type_this(litstr_analyzer) litstr_analyzer;
	uls_voidptr_t litstr_context;
};
ULS_DEF_PARRAY(quotetype);

ULS_DEFINE_STRUCT_BEGIN(litstr_context)
{
	uls_quotetype_ptr_t qmt;
	uls_callback_type_this(litstr_analyzer) litstr_proc;

	_uls_ptrtype_tool(csz_str) ss_dst;
	int n_lfs;
};

ULS_DEFINE_STRUCT_BEGIN(litstr)
{
	char *line;
	const char *lptr, *lptr_end;
	int len;

	int map_flags;
	uls_wch_t ch_escaped;
	int  len_ch_escaped;
	uls_wch_t wch;

	uls_litstr_context_t context;
};

#endif // ULS_DEF_PUBLIC_TYPE

#ifdef ULS_DECL_PROTECTED_PROC
uls_wch_t __dec_escaped_char_cont(char quote_ch, uls_litstr_ptr_t lit);
uls_wch_t uls_get_escape_char_initial(uls_litstr_ptr_t lit);
uls_wch_t uls_get_escape_char_cont(uls_litstr_ptr_t lit);

int __uls_analyze_esc_ch(uls_litstr_ptr_t lit, uls_escmap_ptr_t escmap, _uls_ptrtype_tool(csz_str) outbuf);
uls_wch_t uls_get_escape_char(uls_litstr_ptr_t lit);
int uls_get_escape_str(char quote_ch, uls_ptrtype_tool(wrd) wrdx);
void uls_init_quotetype(uls_quotetype_ptr_t qmt);
void uls_deinit_quotetype(uls_quotetype_ptr_t qmt);

int nothing_lit_analyzer(uls_litstr_ptr_t lit);

int dfl_lit_analyzer_escape0(uls_litstr_ptr_t lit);
int dfl_lit_analyzer_escape1(uls_litstr_ptr_t lit);
int dfl_lit_analyzer_escape2(uls_litstr_ptr_t lit);

#endif

#ifdef ULS_DECL_PUBLIC_PROC
int canbe_commtype_mark(char* wrd, uls_ptrtype_tool(outparam) parms);
int canbe_quotetype_mark(char *chr_tbl, char* wrd, uls_ptrtype_tool(outparam) parms);

uls_quotetype_ptr_t uls_create_quotetype(void);
void uls_destroy_quotetype(uls_quotetype_ptr_t qmt);

uls_quotetype_ptr_t uls_find_quotetype_by_tokid(uls_ref_parray(quotetypes,quotetype),
	int n_quotetypes, int tok_id);

ULS_DLL_EXTERN uls_quotetype_ptr_t uls_get_litstr__quoteinfo(uls_litstr_ptr_t lit);
ULS_DLL_EXTERN uls_voidptr_t uls_get_litstr__user_data(uls_litstr_ptr_t lit);
ULS_DLL_EXTERN uls_litstr_context_ptr_t uls_get_litstr__context(uls_litstr_ptr_t lit);

ULS_DLL_EXTERN void uls_litstr_putc(uls_litstr_context_ptr_t lit_ctx, char ch);
ULS_DLL_EXTERN void uls_litstr_puts(uls_litstr_context_ptr_t lit_ctx, const char *str, int len);

#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_LITSTR_H__
