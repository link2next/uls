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
  <file> uls_context.h </file>
  <brief>
    The procedures manipulating input(fd,str).
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#ifndef __ULS_CONTEXT_H__
#define __ULS_CONTEXT_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_tokdef.h"
#include "uls/uls_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
// the flags of uls_context_t
#define ULS_CTX_FL_WANT_EOFTOK     0x01
#define ULS_CTX_FL_EOF             0x02
#define ULS_CTX_FL_UNGET_CONTEXT   0x04
#define ULS_CTX_FL_TOKEN_UNGOT     0x08
#define ULS_CTX_FL_QTOK            0x10
#define ULS_CTX_FL_EXTERN_TOKBUF   0x20
#define ULS_CTX_FL_ERR             0x40
#define ULS_CTX_FL_GETTOK_RAW      0x80
#define ULS_CTX_FL_FILL_RAW       0x100

// the flags of uls_xcontext_t
#define ULS_XCTX_FL_IGNORE_LF   0x01

#define uls_is_ch_space(uls, ch)        (ch < ULS_SYNTAX_TABLE_SIZE && (uls)->ch_context[ch] == 0)
#define uls_canbe_ch_idfirst(uls, ch)   (ch >= ULS_SYNTAX_TABLE_SIZE || (uls)->ch_context[ch] & ULS_CH_IDFIRST)
#define uls_canbe_ch_id(uls, ch)        (ch >= ULS_SYNTAX_TABLE_SIZE || (uls)->ch_context[ch] & ULS_CH_ID)
#define uls_canbe_ch_quote(uls, ch)     (ch >= ULS_SYNTAX_TABLE_SIZE || (uls)->ch_context[ch] & ULS_CH_QUOTE)
#define uls_is_ch_1ch_token(uls, ch)    (ch < ULS_SYNTAX_TABLE_SIZE && (uls)->ch_context[ch] & ULS_CH_1)
#define uls_canbe_ch_2ch_token(uls, ch) (ch >= ULS_SYNTAX_TABLE_SIZE || (uls)->ch_context[ch] & ULS_CH_2PLUS)
#define uls_canbe_ch_comm(uls, ch)      (ch >= ULS_SYNTAX_TABLE_SIZE || (uls)->ch_context[ch] & ULS_CH_COMM)

#define uls_context_get_tag(ctx) (_uls_tool(csz_text)(uls_ptr((ctx)->tag)))
#define uls_context_get_taglen(ctx) (csz_length(uls_ptr((ctx)->tag)))

#define uls_context_set_lineno(ctx,lno) uls_context_set_tag(ctx, NULL, lno)
#define uls_context_get_lineno(ctx) ((ctx)->lineno)

#endif // ULS_DECL_PROTECTED_TYPE

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DECLARE_STRUCT(xcontext);
#ifdef ULS_DOTNET
ULS_DECLARE_STRUCT(lex);
#endif

ULS_DEFINE_DELEGATE_BEGIN(gettok, int)(uls_lex_ptr_t uls);
ULS_DEFINE_DELEGATE_END(gettok);

ULS_DEFINE_DELEGATE_BEGIN(input_ungrabber, void)(uls_voidptr_t data);
ULS_DEFINE_DELEGATE_END(input_ungrabber);

ULS_DEFINE_DELEGATE_BEGIN(xcontext_filler, int)(uls_xcontext_ptr_t xctx);
ULS_DEFINE_DELEGATE_END(xcontext_filler);

ULS_DEFINE_DELEGATE_BEGIN(xctx_boundary_checker, int)(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln);
ULS_DEFINE_DELEGATE_END(xctx_boundary_checker);

ULS_DEFINE_DELEGATE_BEGIN(xctx_boundary_checker2, int)(uls_xcontext_ptr_t xctx, char* buf, int n);
ULS_DEFINE_DELEGATE_END(xctx_boundary_checker2);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(lexseg)
{
	int  offset1;
	int  len1;

	int  offset2;
	int  len_text;

	int  n_lfs_raw;
	uls_tokdef_vx_ptr_t tokdef_vx;
};
ULS_DEF_ARRAY_TYPE10(lexseg);

ULS_DEFINE_STRUCT(userdata)
{
	uls_input_ungrabber_t proc;
	uls_voidptr_t data;
	uls_userdata_ptr_t inner;
};

ULS_DEFINE_STRUCT(context)
{
	uls_flags_t flags;

	_uls_type_tool(csz_str)  tag;
	int  lineno, delta_lineno;

	uls_def_bytespool(cnst_nilstr, ULS_CNST_NILSTR_SIZE);
	uls_input_ptr_t  input;

	_uls_type_tool(csz_str)  zbuf1;
	_uls_type_tool(csz_str)  zbuf2;
	const char *lptr, *line, *line_end;

	uls_decl_array_type10(lexsegs, lexseg);
	int        i_lexsegs, n_lexsegs;

	uls_callback_type_this(gettok)  gettok;
	uls_callback_type_this(xcontext_filler) fill_proc;
	uls_callback_type_this(xctx_boundary_checker) record_boundary_checker;

	uls_tmpl_pool_ptr_t tmpls_pool;

	int        tok;
	const char *s_val;
	int        s_val_len, s_val_uchars;

	uls_type_tool(outbuf) tokbuf;
	uls_type_tool(outbuf) tokbuf_aux;
	int        l_tokbuf_aux, n_digits, n_expo;

	uls_tokdef_vx_ptr_t anonymous_uchar_vx;
	uls_userdata_ptr_t user_data;

	uls_context_ptr_t prev;
};

ULS_DEFINE_STRUCT_BEGIN(xcontext)
{
	int flags;

	int toknum_EOI, toknum_EOF, toknum_ID, toknum_NUMBER;
	int toknum_LINENUM, toknum_TMPL, toknum_LINK, toknum_NONE, toknum_ERR;

	uls_ref_bytespool(ch_context);

	uls_ref_array_type01(commtypes, commtype); // ULS_N_MAX_COMMTYPES
	int n2_commtypes;

	char *prepended_input;
	int len_prepended_input, lfs_prepended_input;

	uls_ref_parray(quotetypes, quotetype);  // ULS_N_MAX_QUOTETYPES

	int len_surplus;
	uls_context_ptr_t context;
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_CONTEXT__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __xcontext_binfd_filler(uls_xcontext_ptr_t xctx);
ULS_DECL_STATIC void add_bin_packet_to_zbuf(int tok_id, int txtlen, const char* txtptr, _uls_ptrtype_tool(csz_str) ss_dst);
ULS_DECL_STATIC void insert_txt_record_into_stream(int tokid, int tokid_TMPL, const char *txtptr, int txtlen,
	uls_context_ptr_t ctx, _uls_ptrtype_tool(csz_str)  ss_dst);

ULS_DECL_STATIC int get_txthdr_1(uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int get_txthdr_2(uls_ptrtype_tool(outparam) parms);

ULS_DECL_STATIC int __xcontext_txtfd_filler(uls_xcontext_ptr_t xctx,
	uls_ptrtype_tool(outparam) parms);

ULS_DECL_STATIC int __check_rec_boundary_host_order(uls_xcontext_ptr_t xctx, char* buf, int n);
ULS_DECL_STATIC int __check_rec_boundary_reverse_order(uls_xcontext_ptr_t xctx, char* buf, int n);
ULS_DECL_STATIC int __check_rec_boundary_bin(uls_xcontext_ptr_t xctx, uls_xctx_boundary_checker2_t checker);

ULS_DECL_STATIC uls_commtype_ptr_t is_commtype_start(uls_xcontext_ptr_t xctx, const char *ptr, int len);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
const char* uls_xcontext_quotetype_start_mark(uls_xcontext_ptr_t xctx, int tok_id);
uls_quotetype_ptr_t uls_xcontext_find_quotetype(uls_xcontext_ptr_t xctx, const char *ptr, int len);

void uls_init_lexseg(uls_lexseg_ptr_t lexseg);
void uls_deinit_lexseg(uls_lexseg_ptr_t lexseg);
void uls_reset_lexseg(uls_lexseg_ptr_t lexseg,
	int offset1, int len1, int offset2, int len2, uls_tokdef_vx_ptr_t e_vx);

void uls_init_context(uls_context_ptr_t ctx, uls_gettok_t gettok, int tok0);
void uls_deinit_context(uls_context_ptr_t ctx);

void uls_xcontext_init(uls_xcontext_ptr_t xctx, uls_gettok_t gettok);
void uls_xcontext_reset(uls_xcontext_ptr_t xctx);
void uls_xcontext_deinit(uls_xcontext_ptr_t xctx);

int check_rec_boundary_null(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln);
int check_rec_boundary_host_order(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln);
int check_rec_boundary_reverse_order(uls_xcontext_ptr_t xctx, uls_ptrtype_tool(parm_line) parm_ln);

int xcontext_raw_filler(uls_xcontext_ptr_t xctx);
int xcontext_binfd_filler(uls_xcontext_ptr_t xctx);
int xcontext_txtfd_filler(uls_xcontext_ptr_t xctx);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void uls_context_set_tag(uls_context_ptr_t ctx, const char* tagstr, int lno);
ULS_DLL_EXTERN void uls_context_inc_lineno(uls_context_ptr_t ctx, int delta);
ULS_DLL_EXTERN void uls_context_set_line(uls_context_ptr_t ctx, const char* line, int len);

ULS_DLL_EXTERN int uls_xcontext_delete_litstr_analyzer(uls_xcontext_ptr_t xctx, const char* prefix);
ULS_DLL_EXTERN int uls_xcontext_change_litstr_analyzer(uls_xcontext_ptr_t xctx,
	const char* prefix, uls_litstr_analyzer_t lit_analyzer, uls_voidptr_t data);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_CONTEXT_H__
