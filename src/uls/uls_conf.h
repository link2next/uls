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
  <file> uls_conf.h </file>
  <brief>
    Loading the lexical spec file (*.ulc) of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/

#ifndef __ULS_CONF_H__
#define __ULS_CONF_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_context.h"
#include "uls/uls_version.h"
#include "uls/idkeyw.h"
#include "uls/onechar.h"
#include "uls/twoplus.h"
#include "uls/litesc.h"
#include "uls/uls_misc.h"

#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define uls_ver_compatible(ver1,ver2) ((_uls_tool_(version_cmp_code)(ver1, ver2)) >= 2)
#define uls_is_config_compatible(uls,ver) uls_ver_compatible(uls_ptr((uls)->config_filever), uls_ptr((hdr)->filever))
#define ULC_N_LEXATTRS 14
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define __uls_tok(uls) ((uls)->xcontext.context->tok)
#define __uls_lexeme(uls) ((uls)->xcontext.context->s_val)
#define __uls_lexeme_len(uls) ((uls)->xcontext.context->s_val_len)
#define __uls_lexeme_chars(uls) ((uls)->xcontext.context->s_val_uchars)

#define __uls_get_tag(uls) uls_context_get_tag((uls)->xcontext.context)
#define __uls_get_taglen(uls) uls_context_get_taglen((uls)->xcontext.context)

#define __uls_get_lineno(uls) uls_context_get_lineno((uls)->xcontext.context)
#define __uls_set_lineno(uls,lno) uls_context_set_lineno((uls)->xcontext.context,lno)
#define __uls_inc_lineno(uls,n_delta) uls_context_inc_lineno((uls)->xcontext.context, n_delta)

#define ULS_CNST_SUFFIX_MAXSIZ   3
#define ULS_CNST_SUFFIXES_MAXSIZ 31

#define TOKDEF_AREA_RSVD    0
#define TOKDEF_AREA_REGULAR 1
#define TOKDEF_AREA_BOTH    2

#define ULS_NAME_SPECNAME     0
#define ULS_NAME_FILEPATH_ULC 1
#define ULS_NAME_FILEPATH_ULD 2

#define ULS_FL_LF_CHAR           0x01
#define ULS_FL_TAB_CHAR          0x02
#define ULS_FL_CASE_INSENSITIVE  0x04

int initialize_ulc_lexattr();
void finalize_ulc_lexattr();
#endif

#ifdef ULS_DEF_PRIVATE_TYPE
ULS_DEF_ARRAY_TYPE01(UlsToolbase, cmd, ULC_N_LEXATTRS);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(ulc_header)
{
	_uls_tool_type_(version) filever;
	int  n_keys_twoplus, n_keys_idstr;
	int  tok_id_min, tok_id_max;
	int  tok_id_seed;
};

ULS_DEFINE_STRUCT(fp_list)
{
	FILE       *fp;
	char       *tagstr;
	int        linenum;
	fp_list_ptr_t prev;
};

ULS_DEFINE_STRUCT(uls_tokid_simple_list)
{
	uls_lex_ptr_t uls;
	int *tokid_list;
	int n_tokid_list;
};

ULS_DEFINE_STRUCT(uls_uch_range)
{
	uls_uch_t x1;
	uls_uch_t x2;
};
ULS_DEF_ARRAY_THIS_TYPE02(uch_range);

ULS_DEFINE_STRUCT(uls_number_prefix)
{
	_uls_def_namebuf(prefix, ULS_MAXLEN_NUMBER_PREFIX);
	int l_prefix, radix;
};
ULS_DEF_ARRAY_THIS_TYPE01(number_prefix, ULS_N_MAX_NUMBER_PREFIXES);

ULS_DEFINE_STRUCT_BEGIN(uls_lex)
{
	uls_flags_t flags;
	_uls_def_namebuf(ulc_name, ULC_LONGNAME_MAXSIZ);
	int         ref_cnt;

	_uls_tool_type_(version) ulc_ver;
	_uls_tool_type_(version) config_filever;
	_uls_tool_type_(version) stream_filever;

	_uls_def_bytespool(ch_context, ULS_SYNTAX_TABLE_SIZE);

	uls_decl_array_this_type02(idfirst_charset, uch_range); // ULS_N_CHARSET_RANGES
	int n_idfirst_charsets;

	uls_decl_array_this_type02(id_charset, uch_range); // ULS_N_CHARSET_RANGES
	int n_id_charsets;

	uls_decl_array_this_type02(commtypes, commtype); // ULS_N_MAX_COMMTYPES
	int n_commtypes;

	uls_decl_parray(quotetypes, quotetype); // ULS_N_MAX_QUOTETYPES

	int id_max_bytes, id_max_uchars;
	uls_uch_t numcnst_separator;

	uls_decl_array_this_type01(numcnst_prefixes, number_prefix, ULS_N_MAX_NUMBER_PREFIXES);
	int n_numcnst_prefixes;

	_uls_def_bytespool(numcnst_suffixes, ULS_CNST_SUFFIXES_MAXSIZ + 1);

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

	uls_voidptr_t shell;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_CONF__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_decl_array_this_type01(ulc_cmd_list,cmd, ULC_N_LEXATTRS);
#endif

#if defined(__ULS_CONF__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int numpfx_by_length_dsc(const uls_voidptr_t a, const uls_voidptr_t b);

ULS_DECL_STATIC int parse_quotetype__options(char *line, uls_quotetype_ptr_t qmt);
ULS_DECL_STATIC int parse_quotetype__token(char *line, uls_quotetype_ptr_t qmt, _uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int uls_parse_quotetype_opts(_uls_tool_ptrtype_(wrd) wrdx, uls_quotetype_ptr_t qmt,
	_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int set_config__QUOTE_TYPE__token(int tok_id, char *tok_nam, int l_tok_nam, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls,
	_uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC int __read_config__QUOTE_TYPE(char *line, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char* tag_nam, int lno);

ULS_DECL_STATIC int read_config__COMMENT_TYPE(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__QUOTE_TYPE(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__ID_FIRST_CHARS(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__ID_CHARS(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__RENAME(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__NOT_CHAR_TOK(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__CASE_SENSITIVE(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__DOMAIN(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__CHAR_TOK(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__ID_MAX_LENGTH(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__NUMBER_PREFIXES(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__NUMBER_SUFFIXES(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__DECIMAL_SEPARATOR(char *line, _uls_tool_ptrtype_(cmd) cmd);
ULS_DECL_STATIC int read_config__LINEFEED_GUARD(char *line, _uls_tool_ptrtype_(cmd) cmd);

ULS_DECL_STATIC int check_keyw_str(int lno, const char* str, _uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_tokdef_by_tokid(uls_lex_ptr_t uls, int t, int area);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_rsvd_tokdef_by_name(uls_lex_ptr_t uls, const char* name);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_reg_tokdef_by_name(uls_lex_ptr_t uls, const char* name, _uls_tool_ptrtype_(outparam) parms);
ULS_DECL_STATIC uls_tokdef_ptr_t __find_tokdef_by_keyw(uls_lex_ptr_t uls, char* keyw);

ULS_DECL_STATIC int is_str_contained_in_commtypes(uls_lex_ptr_t uls, const char *str);
ULS_DECL_STATIC int is_str_contained_in_quotetypes(uls_lex_ptr_t uls, const char *str);
ULS_DECL_STATIC void realloc_tokdef_array(uls_lex_ptr_t uls, int n1, int n2);
ULS_DECL_STATIC int make_tokdef_for_quotetype(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, char *qmt_name);

ULS_DECL_STATIC int add_id_charset(uls_ref_array_this_type02(ranges,uch_range), int n_ary, uls_uch_t i1, uls_uch_t i2);
ULS_DECL_STATIC char* find_cnst_suffix(char* cstr_pool, const char* str, int l_str, _uls_tool_ptrtype_(outparam) parms);

ULS_DECL_STATIC int add_cnst_suffix(char* cnst_suffixes, const char* suffix);
ULS_DECL_STATIC int cnst_suffixes_by_len(const uls_voidptr_t a, const uls_voidptr_t b);
ULS_DECL_STATIC int check_validity_of_cnst_suffix(_uls_tool_ptrtype_(arglst) cnst_suffixes, char* suffixes_str);

ULS_DECL_STATIC int is_commstart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC int is_quotestart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC int read_config_var(const char* tag_nam, int lno, uls_lex_ptr_t uls,
	char* lptr, ulc_header_ptr_t hdr);

ULS_DECL_STATIC int get_ulf_filepath(const char* dirpath, int len_dirpath,
	const char *specname, char* pathbuff);

ULS_DECL_STATIC void _list_searchpath(const char *filename, _uls_tool_ptrtype_(arglst) title, _uls_tool_ptrtype_(arglst) searchpath, int n);
ULS_DECL_STATIC void set_chrmap(uls_lex_ptr_t uls, int tok_id, int keyw_type, char ch_kwd);
ULS_DECL_STATIC int get_ulc_fileformat_ver(char *linebuff, int linelen, _uls_tool_ptrtype_(version) ver1);
ULS_DECL_STATIC int check_ulc_file_magic(FILE* fin, _uls_tool_ptrtype_(version) sysver, char *ulc_lname);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_uch_range(_uls_ptrtype_this_(none,uch_range) uch_range);
void uls_deinit_uch_range(_uls_ptrtype_this_(none,uch_range) uch_range);

int is_reserved_tok(uls_lex_ptr_t uls, char* name);
int check_rsvd_toks(uls_lex_ptr_t uls);
int uls_is_char_idfirst(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch);
int uls_is_char_id(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch);

uls_tokdef_vx_ptr_t uls_find_tokdef_vx(uls_lex_ptr_t uls, int t);
uls_tokdef_vx_ptr_t uls_find_tokdef_vx_force(uls_lex_ptr_t uls, int t);
uls_tokdef_vx_ptr_t set_extra_tokdef_vx(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef);

FILE* ulc_search_for_fp(int typ_fpath, const char* fpath, _uls_tool_ptrtype_(outparam) parms);
FILE* uls_get_ulc_path(int typ_fpath, const char* fpath, int len_dpath, const char* specname, int len_specname, _uls_tool_ptrtype_(outparam) parms);

int ulc_get_searchpath_by_specname(_uls_tool_ptrtype_(arglst) nameof_searchpath, _uls_tool_ptrtype_(arglst) searchpath_list);
int ulc_get_searchpath_by_specpath(int is_abspath, _uls_tool_ptrtype_(arglst) nameof_searchpath, _uls_tool_ptrtype_(arglst) searchpath_list);

char* is_cnst_suffix_contained(char* cstr_pool, const char* str, int l_str, _uls_tool_ptrtype_(outparam) parms);

int ulc_prepend_searchpath_exeloc(const char* argv0);
int check_ulcf_fileformat_magic(char *linebuff, int linelen, int ftype);

fp_list_ptr_t ulc_find_fp_list(fp_list_ptr_t fp_stack_top, const char *ulc_name);
fp_list_ptr_t ulc_fp_push(fp_list_ptr_t fp_lst, FILE *fp, const char* str);
FILE* ulc_fp_peek(fp_list_ptr_t fp_lst, _uls_tool_ptrtype_(outparam) parms);

FILE* ulc_fp_get(_uls_tool_ptrtype_(outparam) parms, int do_export);
void release_ulc_fp_stack(fp_list_ptr_t fp_lst);

void init_reserved_toks(uls_lex_ptr_t uls);
int classify_char_group(uls_lex_ptr_t uls, ulc_header_ptr_t uls_conf);
int classify_tok_group(uls_lex_ptr_t uls);

uls_tokdef_ptr_t rearrange_tokname_of_quotetypes(uls_lex_ptr_t uls, int n_keys_twoplus, _uls_tool_ptrtype_(outparam) parms);

int commtype_by_length_dsc(const uls_voidptr_t a, const uls_voidptr_t b);
int quotetype_by_length_dsc(const uls_voidptr_t a, const uls_voidptr_t b);
int calc_len_surplus_recommended(uls_lex_ptr_t uls);

int srch_vx_by_id(const uls_voidptr_t a, const uls_voidptr_t p_id);
int comp_vx_by_tokid(const uls_voidptr_t a, const uls_voidptr_t b);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void uls_want_eof(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_unwant_eof(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_is_valid_specpath(const char* confname);
ULS_DLL_EXTERN int uls_get_spectype(const char *filepath, _uls_tool_ptrtype_(outparam) parms);

ULS_DLL_EXTERN void ulc_list_searchpath(const char* confname);
ULS_DLL_EXTERN void ulc_set_searchpath(const char *pathlist);
ULS_DLL_EXTERN int ulc_add_searchpath(const char *pathlist, int front);

ULS_DLL_EXTERN int ulc_prepend_searchpath_pwd(void);

ULS_DLL_EXTERN int ulc_read_header(uls_lex_ptr_t uls, FILE* fin, ulc_header_ptr_t hdr, _uls_tool_ptrtype_(outparam) parms);
ULS_DLL_EXTERN uls_tokdef_vx_ptr_t ulc_proc_line(const char* tag_nam, int lno, char* lptr, uls_lex_ptr_t uls, ulc_header_ptr_t hdr,
	_uls_tool_ptrtype_(outparam) parms);

ULS_DLL_EXTERN uls_xcontext_ptr_t _uls_get_xcontext(uls_lex_ptr_t uls);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USEDLL
#if defined(ULS_USE_WSTR)
#include "uls/uls_conf_wstr.h"
#elif defined(ULS_USE_ASTR)
#include "uls/uls_conf_astr.h"
#endif
#endif

#endif // __ULS_CONF_H__
