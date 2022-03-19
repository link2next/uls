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
  <file> uls_core.h </file>
  <brief>
    Implements the main routines of ULS, such as uls_create(),
      uls_get_tok(), uls_destroy(), ...
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/

#ifndef __ULS_CORE_H__
#define __ULS_CORE_H__

#include "uls/uls_prim.h"
#include "uls/uls_input.h"
#include "uls/idkeyw.h"
#include "uls/onechar.h"
#include "uls/twoplus.h"
#include "uls/uls_context.h"
#include "uls/uls_version.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(tokid_simple_list)
{
	uls_lex_ptr_t uls;
	int *tokid_list;
	int n_tokid_list;
};

ULS_DEFINE_STRUCT(number_prefix)
{
	uls_def_namebuf(prefix, ULS_MAXLEN_NUMBER_PREFIX);
	int l_prefix, radix;
};
ULS_DEF_ARRAY_TYPE00(number_prefix, NUMBER_PREFIX_TYPE00_ULS_N_MAX_NUMBER_PREFIXES, ULS_N_MAX_NUMBER_PREFIXES);

ULS_DEFINE_STRUCT_BEGIN(lex)
{
	uls_flags_t flags;
	uls_def_namebuf(ulc_name, ULC_LONGNAME_MAXSIZ);
	int         ref_cnt;

	uls_version_t ulc_ver;
	uls_version_t config_filever;
	uls_version_t stream_filever;

	uls_def_bytespool(ch_context, ULS_SYNTAX_TABLE_SIZE);

	uls_decl_array_type01(idfirst_charset, uch_range);
	uls_decl_array_type01(id_charset, uch_range);

	uls_decl_array_type01(commtypes, commtype); // ULS_N_MAX_COMMTYPES
	int n1_commtypes;

	uls_decl_parray(quotetypes, quotetype); // ULS_N_MAX_QUOTETYPES

	int id_max_bytes, id_max_uchars;
	uls_uch_t numcnst_separator;

	uls_decl_array_type00(numcnst_prefixes, number_prefix, ULS_N_MAX_NUMBER_PREFIXES);
	int n_numcnst_prefixes;

	uls_def_bytespool(numcnst_suffixes, ULS_CNST_SUFFIXES_MAXSIZ + 1);

	uls_decl_parray(tokdef_vx_array, tokdef_vx);
	uls_decl_parray(tokdef_vx_rsvd, tokdef_vx); // [0,N_RESERVED_TOKS)

	uls_decl_parray(tokdef_array, tokdef); // == str_pool: main memory allocd
	uls_tokdef_vx_ptr_t tokdef_vx;

	uls_kwtable_t   idkeyw_table;
	uls_onechar_table_t onechar_table;
	uls_kwtable_twoplus_t twoplus_table;

	uls_escmap_pool_t escstr_pool;

	uls_xcontext_t xcontext;
	uls_context_ptr_t context_tower;
};

#define ULS_WANT_EOFTOK  0x01
#define ULS_DO_DUP       0x02
#define ULS_MEMFREE_LINE 0x04

#define ULS_NO_DUP       0
#define ULS_NO_EOFTOK    0

#define ULS_FILE_MS_MBCS 0x10
#define ULS_FILE_UTF8    0x20

#define uls_tok(uls) __uls_tok(uls)
#define uls_lexeme(uls) __uls_lexeme(uls)
#define uls_lexeme_len(uls) __uls_lexeme_len(uls)
#define uls_lexeme_chars(uls) __uls_lexeme_chars(uls)
#define uls_get_tag(uls) __uls_get_tag(uls)
#define uls_get_taglen(uls) __uls_get_taglen(uls)
#define uls_get_lineno(uls) __uls_get_lineno(uls)
#define uls_set_lineno(uls,lno) uls_context_set_tag(uls->xcontext.context, NULL, lno)
#define uls_inc_lineno(uls,lno) uls_context_inc_lineno(uls->xcontext.context, lno)

#define uls_tokid uls_tok
#define uls_toknum uls_tok
#define uls_token uls_tok

#define uls_token_id uls_tok
#define uls_tok_id uls_tok

#define uls_tokstr uls_lexeme
#define uls_lexeme_ulen uls_lexeme_chars
#define uls_lexeme_text uls_lexeme

#define uls_gettag uls_get_tag
#define uls_gettag_len uls_get_taglen

#define uls_toknum_eoi(uls) ((uls)->xcontext.toknum_EOI)
#define uls_toknum_eof(uls) ((uls)->xcontext.toknum_EOF)
#define uls_toknum_id(uls) ((uls)->xcontext.toknum_ID)
#define uls_toknum_number(uls) ((uls)->xcontext.toknum_NUMBER)
#define uls_toknum_linenum(uls) ((uls)->xcontext.toknum_LINENUM)
#define uls_toknum_tmpl(uls) ((uls)->xcontext.toknum_TMPL)
#define uls_toknum_link(uls) ((uls)->xcontext.toknum_LINK)
#define uls_toknum_none(uls) ((uls)->xcontext.toknum_NONE)
#define uls_toknum_err(uls) ((uls)->xcontext.toknum_ERR)

#define uls_is_eoi(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_EOI)
#define uls_is_eof(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_EOF)
#define uls_is_id(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_ID)
#define uls_is_number(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_NUMBER)
#define uls_is_linenum(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_LINENUM)
#define uls_is_tmpl(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_TMPL)
#define uls_is_link(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_LINK)
#define uls_is_none(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_NONE)
#define uls_is_err(uls) (__uls_tok(uls) == (uls)->xcontext.toknum_ERR)
#define uls_is_quote(uls) uls_is_quote_tok(uls, __uls_tok(uls))

#define uls_dismiss_input uls_pop
#define uls_dismiss_inputs uls_pop_all
// OBSOLETE: uls_dismiss, uls_dismiss_all
#define uls_dismiss uls_pop
#define uls_dismiss_all uls_pop_all

#define uls_delete_litstr_analyzer(uls,pfx) uls_xcontext_delete_litstr_analyzer(uls_ptr((uls)->xcontext),pfx)
#define uls_change_litstr_analyzer(uls,pfx,proc,dat) uls_xcontext_change_litstr_analyzer(uls_ptr((uls)->xcontext),pfx,proc,dat)

#define uls_gettok(uls) uls_get_tok(uls)
#define uls_settok(uls,tokid,lexeme,len) uls_set_tok(uls,tokid,lexeme,len)

#define uls_lexeme_int uls_lexeme_d
#define uls_lexeme_uint uls_lexeme_u

#define uls_lexeme_long uls_lexeme_ld
#define uls_lexeme_ulong uls_lexeme_lu

#define uls_lexeme_longlong uls_lexeme_lld
#define uls_lexeme_ulonglong uls_lexeme_llu

#define uls_lexeme_Ld uls_lexeme_lld
#define uls_lexeme_Lu uls_lexeme_llu

#define uls_lexeme_float(uls) ((float)uls_lexeme_double(uls))

#define ULS_UCH_NONE '\0'
#define ULS_CH_NONE ULS_UCH_NONE
#define uls_peek_ch uls_peek_uch
#define uls_get_ch uls_get_uch
#define uls_peekch uls_peek_uch
#define uls_getch uls_get_uch

#define uls_current_extra_tokdef(uls) uls_get_current_extra_tokdef(uls)
#define uls_extra_tokdef(uls,tok_id) uls_get_extra_tokdef(uls,tok_id)

#define uls_grab(uls) (++(uls)->ref_cnt)
#define uls_ungrab(uls) uls_destroy(uls)

#define uls_n_buckets_kwtable(uls) ((uls)->idkeyw_table.bucket_head.n)
#define uls_is_context_initial(uls) ((uls)->xcontext.context->prev == nilptr)

#define uls_set_verbose_level(uls, level) ((uls)->verbose=(level))

#if defined(__ULS_CORE__)
ULS_DECL_STATIC _ULS_INLINE void __ready_to_use_lexseg(uls_context_ptr_t ctx);

ULS_DECL_STATIC int find_prefix_radix(uls_outparam_ptr_t parms, uls_lex_ptr_t uls, const char *str);
ULS_DECL_STATIC int get_number(uls_lex_ptr_t uls, uls_context_ptr_t ctx, uls_parm_line_ptr_t parm_ln);
ULS_DECL_STATIC void make_eof_lexeme(uls_lex_ptr_t uls);
ULS_DECL_STATIC void make_eoi_lexeme(uls_lex_ptr_t uls);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __uls_onechar_lexeme(uls_lex_ptr_t uls, uls_uch_t uch, const char *lptr, int len);
ULS_DECL_STATIC _ULS_INLINE int __uls_is_real(const char *ptr);
ULS_DECL_STATIC _ULS_INLINE double __uls_lexeme_unsigned_double(const char *ptr);
ULS_DECL_STATIC uls_uint32 __uls_lexeme_uint32(const char *ptr);
ULS_DECL_STATIC uls_uint64 __uls_lexeme_uint64(const char *ptr);

ULS_DECL_STATIC int __uls_change_line(uls_lex_ptr_t uls, const char* line, int len, int flags);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
const char* skip_white_spaces(uls_lex_ptr_t uls);

void free_tokdef_array(uls_lex_ptr_t uls);
int ulc_load(uls_lex_ptr_t uls, FILE *fin_ulc, FILE *fin_ulf);

int uls_init_fp(uls_lex_ptr_t uls, const char *specname,
  FILE *fin_ulc, FILE *fin_ulf);

void uls_dealloc_lex(uls_lex_ptr_t uls);
int uls_spec_compatible(uls_lex_ptr_t uls, const char* specname, uls_version_ptr_t filever);

uls_tokdef_vx_ptr_t set_err_tok(uls_lex_ptr_t uls, const char* errmsg);

const char* __uls_eof_tag(const char *ptr, uls_outparam_ptr_t parms);
int __uls_make_eoftok_lexeme(uls_outbuf_ptr_t outbuf, int lno, const char *tagstr);

int uls_fillbuff(uls_lex_ptr_t uls);
int uls_clear_and_fillbuff(uls_lex_ptr_t uls);
int uls_fillbuff_and_reset(uls_lex_ptr_t uls);

int uls_gettok_raw(uls_lex_ptr_t uls);

int __uls_change_isrc(uls_lex_ptr_t uls, int bufsiz, uls_voidptr_t isrc,
  uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc);
uls_context_ptr_t uls_push_context(uls_lex_ptr_t uls, uls_context_ptr_t ctx_new);
#endif

ULS_DLL_EXTERN const char* uls_tok2keyw(uls_lex_ptr_t uls, int t);
ULS_DLL_EXTERN const char* uls_tok2name(uls_lex_ptr_t uls, int t);

ULS_DLL_EXTERN const char* uls_tok2keyw_2(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN const char* uls_tok2name_2(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int _uls_get_tokid_list(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN void _uls_put_tokid_list(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int ulsjava_get_tokid_list(uls_lex_t* uls, int **ptr_outbuf);
ULS_DLL_EXTERN void ulsjava_put_tokid_list(uls_lex_t* uls, int **ptr_outbuf);

ULS_DLL_EXTERN uls_tokid_simple_list_ptr_t _uls_get_tokid_list_2(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void _uls_put_tokid_list_2(uls_tokid_simple_list_ptr_t lst);

ULS_DLL_EXTERN uls_lex_ptr_t uls_create(const char* confname);
ULS_DLL_EXTERN int uls_init(uls_lex_ptr_t uls, const char* confname);
ULS_DLL_EXTERN void uls_reset(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_destroy(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_skip_blanks(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_get_tok(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_set_tok(uls_lex_ptr_t uls, int tokid, const char* lexeme, int l_lexeme);
ULS_DLL_EXTERN void uls_expect(uls_lex_ptr_t uls, int value);

ULS_DLL_EXTERN void uls_push(uls_lex_ptr_t uls, uls_context_ptr_t ctx);
ULS_DLL_EXTERN uls_context_ptr_t uls_pop(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_pop_all(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_push_isrc(uls_lex_ptr_t uls,
  uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc);
ULS_DLL_EXTERN int uls_set_isrc(uls_lex_ptr_t uls,
  uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc);

ULS_DLL_EXTERN int uls_push_line(uls_lex_ptr_t uls, const char* line, int len, int flags);
ULS_DLL_EXTERN int uls_set_line(uls_lex_ptr_t uls, const char* line, int len, int flags);

ULS_DLL_EXTERN int uls_cardinal_toknam(char* toknam, uls_lex_ptr_t uls, int tok_id);
ULS_DLL_EXTERN int uls_cardinal_toknam_deco(char *toknam_buff, const char *toknam);
ULS_DLL_EXTERN int uls_cardinal_toknam_deco_lxmpfx(char *toknam_buff, char *lxmpfx, uls_lex_ptr_t uls,
	int tok_id, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN int uls_get_number_prefix(uls_outparam_ptr_t parms, char *prefix);
ULS_DLL_EXTERN void uls_dump_tok(uls_lex_ptr_t uls, const char *pfx, const char *suff);
ULS_DLL_EXTERN void uls_dumpln_tok(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_is_int(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_is_real(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_is_zero(uls_lex_ptr_t uls);

ULS_DLL_EXTERN const char* uls_number_suffix(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const char* uls_eof_tag(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN uls_uint32 uls_lexeme_uint32(uls_lex_ptr_t uls);
ULS_DLL_EXTERN uls_int32 uls_lexeme_int32(uls_lex_ptr_t uls);

ULS_DLL_EXTERN uls_uint64 uls_lexeme_uint64(uls_lex_ptr_t uls);
ULS_DLL_EXTERN uls_int64 uls_lexeme_int64(uls_lex_ptr_t uls);

ULS_DLL_EXTERN unsigned int uls_lexeme_u(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_lexeme_d(uls_lex_ptr_t uls);

ULS_DLL_EXTERN unsigned long uls_lexeme_lu(uls_lex_ptr_t uls);
ULS_DLL_EXTERN long uls_lexeme_ld(uls_lex_ptr_t uls);

ULS_DLL_EXTERN unsigned long long uls_lexeme_llu(uls_lex_ptr_t uls);
ULS_DLL_EXTERN long long uls_lexeme_lld(uls_lex_ptr_t uls);

ULS_DLL_EXTERN double uls_lexeme_double(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_is_quote_tok(uls_lex_ptr_t uls, int tok_id);

ULS_DLL_EXTERN uls_voidptr_t uls_get_current_extra_tokdef(uls_lex_ptr_t uls);
ULS_DLL_EXTERN uls_voidptr_t uls_get_extra_tokdef(uls_lex_ptr_t uls, int tok_id);
ULS_DLL_EXTERN void uls_set_current_extra_tokdef(uls_lex_ptr_t uls, uls_voidptr_t extra_tokdef);
ULS_DLL_EXTERN int uls_set_extra_tokdef(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef);

ULS_DLL_EXTERN int _uls_const_WANT_EOFTOK(void);
ULS_DLL_EXTERN int _uls_const_DO_DUP(void);
ULS_DLL_EXTERN int _uls_const_FILE_MS_MBCS(void);
ULS_DLL_EXTERN int _uls_const_FILE_UTF8(void);

ULS_DLL_EXTERN uls_uch_t _uls_const_NEXTCH_NONE(void);

ULS_DLL_EXTERN int _uls_toknum_EOI(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_EOF(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_ERR(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_NONE(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_ID(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_NUMBER(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_toknum_TMPL(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int _uls_is_ch_space(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN int _uls_is_ch_idfirst(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN int _uls_is_ch_id(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN int _uls_is_ch_quote(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN int _uls_is_ch_1ch_token(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN int _uls_is_ch_2ch_token(uls_lex_ptr_t uls, uls_uch_t uch);

ULS_DLL_EXTERN int _uls_get_lineno(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void _uls_set_lineno(uls_lex_ptr_t uls, int lineno);
ULS_DLL_EXTERN void _uls_inc_lineno(uls_lex_ptr_t uls, int amount);

ULS_DLL_EXTERN int _uls_tok_id(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const char* _uls_lexeme(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_lexeme_len(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_lexeme_chars(uls_lex_ptr_t uls);

ULS_DLL_EXTERN const char* _uls_get_tag(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int _uls_get_taglen(uls_lex_ptr_t uls);
ULS_DLL_EXTERN const char* _uls_get_tag2(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);
ULS_DLL_EXTERN void uls_set_tag(uls_lex_ptr_t uls, const char* tag, int lno);

ULS_DLL_EXTERN void _uls_dump_tok_2(uls_lex_ptr_t uls, const char* pfx,
	const char *id_str, const char *tok_str, const char *suff);

ULS_DLL_EXTERN const char* _uls_number_suffix(uls_lex_ptr_t uls, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN uls_lex_ptr_t ulsjava_create(uls_native_vptr_t confname, int len_confname);

ULS_DLL_EXTERN void ulsjava_dump_tok(uls_lex_ptr_t uls, const void *pfx, int len_pfx, const void *suff, int len_suff);
ULS_DLL_EXTERN void ulsjava_set_tag(uls_lex_ptr_t uls, const void *tag, int len_tag, int lineno);

ULS_DLL_EXTERN const char* uls_get_tag2(uls_lex_ptr_t uls, int* ptr_n_bytes);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_CORE_H__
