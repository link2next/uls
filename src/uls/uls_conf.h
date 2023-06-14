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
#ifdef _ULS_INTERNAL_USE_ONLY
#include "uls/uls_misc.h"
#endif
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define uls_ver_compatible(ver1,ver2) ((_uls_tool_(version_cmp_code)(ver1, ver2)) >= 2)
#define uls_is_config_compatible(uls,ver) uls_ver_compatible(uls_ptr((uls)->config_filever), uls_ptr((hdr)->filever))
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

#ifdef ULS_DEF_PUBLIC_TYPE
_ULS_DEFINE_STRUCT(ulc_header)
{
	uls_type_tool(version) filever;
	int  n_keys_twoplus, n_keys_idstr;
	int  tok_id_min, tok_id_max;
	int  tok_id_seed;
};

_ULS_DEFINE_STRUCT(fp_list)
{
	FILE       *fp;
	char       *tagstr;
	int        linenum;
	fp_list_ptr_t prev;
};

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

	uls_type_tool(version) ulc_ver;
	uls_type_tool(version) config_filever;
	uls_type_tool(version) stream_filever;

	uls_def_bytespool(ch_context, ULS_SYNTAX_TABLE_SIZE);

	uls_decl_array_tool_type01(idfirst_charset, uch_range);
	uls_decl_array_tool_type01(id_charset, uch_range);

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

	uls_voidptr_t shell;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_CONF__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_decl_array_type00(ulc_cmd_list,cmd, ULC_N_LEXATTRS);
#endif

#if defined(__ULS_CONF__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int numpfx_by_length_dsc(const uls_voidptr_t a, const uls_voidptr_t b);

ULS_DECL_STATIC int parse_quotetype__options(char *line, uls_quotetype_ptr_t qmt);
ULS_DECL_STATIC int parse_quotetype__token(char *line, uls_quotetype_ptr_t qmt, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int uls_parse_quotetype_opts(uls_ptrtype_tool(wrd) wrdx, uls_quotetype_ptr_t qmt,
	uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int set_config__QUOTE_TYPE__token(int tok_id, const char *tok_nam, int l_tok_nam, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls,
	uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC void __set_config_quoute_type(uls_quotetype_ptr_t qmt,
	const char *mark1, int len_mark1, int lfs_mark1, const char *mark2, int len_mark2, int lfs_mark2);
ULS_DECL_STATIC int __read_config__QUOTE_TYPE(char *line, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char* tag_nam, int lno);

ULS_DECL_STATIC int read_config__COMMENT_TYPE(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__QUOTE_TYPE(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__ID_FIRST_CHARS(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__ID_CHARS(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__RENAME(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__NOT_CHAR_TOK(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__CASE_SENSITIVE(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__DOMAIN(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__CHAR_TOK(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__ID_MAX_LENGTH(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__NUMBER_PREFIXES(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__NUMBER_SUFFIXES(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__DECIMAL_SEPARATOR(char *line, uls_cmd_ptr_t cmd);
ULS_DECL_STATIC int read_config__PREPEND_INPUT(char *line, uls_cmd_ptr_t cmd);

ULS_DECL_STATIC int check_keyw_str(int lno, const char* str, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_tokdef_by_tokid(uls_lex_ptr_t uls, int t, int area);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_rsvd_tokdef_by_name(uls_lex_ptr_t uls, const char* name);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_reg_tokdef_by_name(uls_lex_ptr_t uls, const char* name, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC uls_tokdef_ptr_t __find_tokdef_by_keyw(uls_lex_ptr_t uls, char* keyw);

ULS_DECL_STATIC int is_str_contained_in_commtypes(uls_lex_ptr_t uls, const char *str);
ULS_DECL_STATIC int is_str_contained_in_quotetypes(uls_lex_ptr_t uls, const char *str);
ULS_DECL_STATIC void realloc_tokdef_array(uls_lex_ptr_t uls, int n1, int n2);
ULS_DECL_STATIC int make_tokdef_for_quotetype(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, const char *qmt_name);

ULS_DECL_STATIC char* find_cnst_suffix(char* cstr_pool, const char* str, int l_str, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int add_cnst_suffix(uls_lex_ptr_t uls, const char* suffix);
ULS_DECL_STATIC int cnst_suffixes_by_len(const uls_voidptr_t a, const uls_voidptr_t b);
ULS_DECL_STATIC int check_validity_of_cnst_suffix(uls_ptrtype_tool(arglst) cnst_suffixes, uls_lex_ptr_t uls);

ULS_DECL_STATIC int is_commstart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC int is_quotestart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC void __set_config_comment_type(uls_commtype_ptr_t cmt, int flags,
	const char *mark1, int len_mark1, int lfs_mark1, const char *mark2, int len_mark2, int lfs_mark2);
ULS_DECL_STATIC int read_config_var(const char* tag_nam, int lno, uls_lex_ptr_t uls,
	char* lptr, ulc_header_ptr_t hdr);

ULS_DECL_STATIC int get_ulf_filepath(const char* dirpath, int len_dirpath,
	const char *specname, char* pathbuff);

ULS_DECL_STATIC void _list_searchpath(const char *filename, uls_ptrtype_tool(arglst) title, uls_ptrtype_tool(arglst) searchpath, int n);
ULS_DECL_STATIC void set_chrmap(uls_lex_ptr_t uls, int tok_id, int keyw_type, char ch_kwd);
ULS_DECL_STATIC int get_ulc_fileformat_ver(char *linebuff, int linelen, uls_ptrtype_tool(version) ver1);
ULS_DECL_STATIC int check_ulc_file_magic(FILE* fin, uls_ptrtype_tool(version) sysver, char *ulc_lname);
ULS_DECL_STATIC void parse_id_ranges_internal(uls_lex_ptr_t uls, uls_ref_parray_tool(wrds_ranges,uch_range), int is_first);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int is_reserved_tok(uls_lex_ptr_t uls, char* name);
int check_rsvd_toks(uls_lex_ptr_t uls);
int uls_is_char_idfirst(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch);
int uls_is_char_id(uls_lex_ptr_t uls, const char* lptr, uls_uch_t *ptr_uch);

uls_tokdef_vx_ptr_t uls_find_tokdef_vx(uls_lex_ptr_t uls, int t);
uls_tokdef_vx_ptr_t uls_find_tokdef_vx_force(uls_lex_ptr_t uls, int t);
uls_tokdef_vx_ptr_t set_extra_tokdef_vx(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef);

FILE* ulc_search_for_fp(int typ_fpath, const char* fpath, uls_ptrtype_tool(outparam) parms);
FILE* uls_get_ulc_path(int typ_fpath, const char* fpath, int len_dpath, const char* specname, int len_specname, uls_ptrtype_tool(outparam) parms);

int ulc_get_searchpath_by_specname(uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list);
int ulc_get_searchpath_by_specpath(int is_abspath, uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list);

char* is_cnst_suffix_contained(char* cstr_pool, const char* str, int l_str, uls_ptrtype_tool(outparam) parms);

int ulc_prepend_searchpath_exeloc(const char* argv0);
int check_ulcf_fileformat_magic(char *linebuff, int linelen, int ftype);

fp_list_ptr_t ulc_find_fp_list(fp_list_ptr_t fp_stack_top, const char *ulc_name);
fp_list_ptr_t ulc_fp_push(fp_list_ptr_t fp_lst, FILE *fp, const char* str);
FILE* ulc_fp_peek(fp_list_ptr_t fp_lst, uls_ptrtype_tool(outparam) parms);

FILE* ulc_fp_get(uls_ptrtype_tool(outparam) parms, int do_export);
void release_ulc_fp_stack(fp_list_ptr_t fp_lst);

void init_reserved_toks(uls_lex_ptr_t uls);
int classify_char_group(uls_lex_ptr_t uls, ulc_header_ptr_t uls_conf);
int classify_tok_group(uls_lex_ptr_t uls);

uls_tokdef_ptr_t rearrange_tokname_of_quotetypes(uls_lex_ptr_t uls, int n_keys_twoplus, uls_ptrtype_tool(outparam) parms);

int calc_len_surplus_recommended(uls_lex_ptr_t uls);

int srch_vx_by_id(const uls_voidptr_t a, const uls_voidptr_t p_id);
int comp_vx_by_tokid(const uls_voidptr_t a, const uls_voidptr_t b);

int parse_id_ranges(uls_lex_ptr_t uls, int is_first, const char* tag_nam, int lno, char *line);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void ulx_set_case_sensitive(uls_lex_ptr_t uls, int is_sensitive);
ULS_DLL_EXTERN void ulx_set_id_max_length(uls_lex_ptr_t uls, int len1, int len2);
ULS_DLL_EXTERN void ulx_clear_config_ascii_id_chars(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int  ulx_set_id_first_chars(uls_lex_ptr_t uls, int k, int i1, int i2);
ULS_DLL_EXTERN int  ulx_set_id_chars(uls_lex_ptr_t uls, int k, int i1, int i2);
ULS_DLL_EXTERN void ulx_set_prepended_input(uls_lex_ptr_t uls, const char *line, int len, int lfs_is_not_token);
ULS_DLL_EXTERN void ulx_set_decimal_separator(uls_lex_ptr_t uls, uls_uch_t uch);
ULS_DLL_EXTERN void ulx_add_config_number_prefixes(uls_lex_ptr_t uls, char *wrd, int len, int radix, int slot_id);
ULS_DLL_EXTERN int  ulx_add_config_number_suffixes(uls_lex_ptr_t uls, const char *suffix);
ULS_DLL_EXTERN void ulx_set_not_char_tok(uls_lex_ptr_t uls, const char* non_ch_toks);
ULS_DLL_EXTERN void ulx_set_char_tok(uls_lex_ptr_t uls, const char* ch_toks);
ULS_DLL_EXTERN void ulx_set_comment_type(uls_lex_ptr_t uls, const char *mark1, const char *mark2);
ULS_DLL_EXTERN int  ulx_set_rename(uls_lex_ptr_t uls, const char *name1, const char *name2);
ULS_DLL_EXTERN int  ulx_set_quote_type(uls_lex_ptr_t uls, int tok_id, const char *tok_nam, const char *mark1, const char *mark2, char *line_escmap);

ULS_DLL_EXTERN void uls_want_eof(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_unwant_eof(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_is_valid_specpath(const char* confname);
ULS_DLL_EXTERN int uls_get_spectype(const char *filepath, uls_ptrtype_tool(outparam) parms);

ULS_DLL_EXTERN void ulc_list_searchpath(const char* confname);
ULS_DLL_EXTERN void ulc_set_searchpath(const char *pathlist);
ULS_DLL_EXTERN int ulc_add_searchpath(const char *pathlist, int front);

ULS_DLL_EXTERN int ulc_prepend_searchpath_pwd(void);

ULS_DLL_EXTERN int ulc_read_header(uls_lex_ptr_t uls, FILE* fin, ulc_header_ptr_t hdr, uls_ptrtype_tool(outparam) parms);
ULS_DLL_EXTERN uls_tokdef_vx_ptr_t ulc_proc_line(const char* tag_nam, int lno, char* lptr, uls_lex_ptr_t uls, ulc_header_ptr_t hdr,
	uls_ptrtype_tool(outparam) parms);

ULS_DLL_EXTERN uls_xcontext_ptr_t _uls_get_xcontext(uls_lex_ptr_t uls);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_conf_wstr.h"
#endif

#endif // __ULS_CONF_H__
