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
#ifdef _ULSCOMPAT_INTERNALLY_USES
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
#define ULS_CNST_SUFFIX_MAXSIZ   3
#define ULS_CNST_SUFFIXES_MAXSIZ 31
#define ULS_CHRTOKS_MAXSIZ   63

#define TOKDEF_AREA_RSVD    0x01
#define TOKDEF_AREA_REGULAR 0x02
#define TOKDEF_AREA_BOTH    (TOKDEF_AREA_RSVD|TOKDEF_AREA_REGULAR)

#define ULS_NAME_SPECNAME     0
#define ULS_NAME_FILEPATH_ULC 1
#define ULS_NAME_FILEPATH_ULD 2
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
_ULS_DEFINE_STRUCT(ulc_header)
{
	uls_type_tool(version) filever;
	int  n_keys_twoplus, n_keys_idstr;
	int  tok_id_min, tok_id_max;
	int  tok_id_seed;

	uls_def_namebuf(tagbuf, ULS_TAGNAM_MAXSIZ);
	int lno, keyw_type;

	uls_lex_ptr_t uls;
	uls_tokdef_ptr_t e_ret;
	uls_tokdef_vx_ptr_t e_vx_ret;
	uls_tokdef_name_ptr_t e_nam_ret;

	uls_def_bytespool(not_chrtoks, ULS_CHRTOKS_MAXSIZ + 1);
	uls_voidptr_t fp_stack;
};

_ULS_DEFINE_STRUCT(ulc_fpitem)
{
	FILE       *fp;
	uls_def_namebuf(tagbuf, ULS_TAGNAM_MAXSIZ);
	int        linenum;
	ulc_fpitem_ptr_t prev;
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_CONF__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int numpfx_by_length_dsc(uls_const_voidptr_t a, uls_const_voidptr_t b);

ULS_DECL_STATIC int parse_quotetype__options(char *line, uls_quotetype_ptr_t qmt);
ULS_DECL_STATIC int parse_quotetype__token(char *line, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int uls_parse_quotetype_opts(uls_ptrtype_tool(wrd) wrdx, uls_quotetype_ptr_t qmt,
	uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int set_config__QUOTE_TYPE__token(int tok_id, const char *tok_nam, int l_tok_nam, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls,
	uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC void __set_config_quoute_type(uls_quotetype_ptr_t qmt,
	const char *mark1, int len_mark1, int lfs_mark1, const char *mark2, int len_mark2, int lfs_mark2);
ULS_DECL_STATIC int __read_config__QUOTE_TYPE(char *line, uls_quotetype_ptr_t qmt, uls_lex_ptr_t uls, const char *tagstr, int lno);

ULS_DECL_STATIC int read_config__COMMENT_TYPE(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__QUOTE_TYPE(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__ID_FIRST_CHARS(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__ID_CHARS(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__RENAME(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__NOT_CHAR_TOK(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__CASE_SENSITIVE(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__DOMAIN(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__ID_MAX_LENGTH(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__NUMBER_PREFIXES(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__NUMBER_SUFFIXES(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__DECIMAL_SEPARATOR(char *line, uls_voidptr_t user_data);
ULS_DECL_STATIC int read_config__PREPEND_INPUT(char *line, uls_voidptr_t user_data);

ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_vx_by_name(uls_lex_ptr_t uls, const char *name, int start_ind, int end_bound);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_rsvd_vx_by_name(uls_lex_ptr_t uls, const char *name);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_reg_vx_by_name(uls_lex_ptr_t uls, const char *name);
ULS_DECL_STATIC uls_tokdef_ptr_t __find_tokdef_by_keyw(uls_lex_ptr_t uls, const char *keyw);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __find_vx_by_tokid(uls_lex_ptr_t uls, int t, int area);

ULS_DECL_STATIC int check_keyw_str(int lno, const char *str, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int is_str_contained_in_commtypes(uls_lex_ptr_t uls, const char *str, int l_str);
ULS_DECL_STATIC int is_str_contained_in_quotetypes(uls_lex_ptr_t uls, const char *str, int l_str);
ULS_DECL_STATIC int make_tokdef_for_quotetype(uls_lex_ptr_t uls, uls_quotetype_ptr_t qmt, const char *qmt_name);

ULS_DECL_STATIC char *find_cnst_suffix(char *cstr_pool, const char *str, int l_str, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int add_cnst_suffix(uls_lex_ptr_t uls, const char *suffix);
ULS_DECL_STATIC int cnst_suffixes_by_len(uls_const_voidptr_t a, uls_const_voidptr_t b);
ULS_DECL_STATIC int check_validity_of_cnst_suffix(uls_ptrtype_tool(arglst) cnst_suffixes, uls_lex_ptr_t uls);

ULS_DECL_STATIC int is_commstart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC int is_quotestart_valid(uls_lex_ptr_t uls, int k);
ULS_DECL_STATIC void __set_config_comment_type(uls_commtype_ptr_t cmt, int flags,
	const char *mark1, int len_mark1, int lfs_mark1, const char *mark2, int len_mark2, int lfs_mark2);
ULS_DECL_STATIC int read_config_var(ulc_header_ptr_t hdr, const char *tagstr, int lno, char *lptr);
ULS_DECL_STATIC int get_ulf_filepath(const char *dirpath, int len_dirpath,
	const char *specname, char *pathbuff);

ULS_DECL_STATIC void _list_searchpath(const char *filename, uls_ptrtype_tool(arglst) title, uls_ptrtype_tool(arglst) searchpath, int n);
ULS_DECL_STATIC int get_ulc_fileformat_ver(char *linebuff, int linelen, uls_ptrtype_tool(version) ver1);
ULS_DECL_STATIC int check_ulc_file_magic(FILE* fin, uls_ptrtype_tool(version) sysver, char *ulc_lname);
ULS_DECL_STATIC int parse_id_ranges_internal(uls_lex_ptr_t uls, uls_ref_parray_tool(wrds_ranges,uch_range), int is_first);

ULS_DECL_STATIC int gen_next_tok_id(ulc_header_ptr_t hdr,
	uls_tokdef_vx_ptr_t e_vx_grp, const char *tok_idstr, int ch_kwd);
ULS_DECL_STATIC uls_tokdef_ptr_t __new_regular_tokdef(int keyw_type, const char *keyw, int ulen, int wlen);

ULS_DECL_STATIC int check_tokid_duplicity(const char *tagstr, int lno,
	int tok_id, uls_tokdef_vx_ptr_t e_vx_grp, uls_lex_ptr_t uls);

ULS_DECL_STATIC uls_tokdef_vx_ptr_t __ulc_proc_line__1char(ulc_header_ptr_t hdr,
	const char *tok_nam, int ch_kwd, const char *tok_idstr);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __ulc_proc_line__reserved(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *tok_idstr);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __ulc_proc_line__userdef(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *tok_idstr);
ULS_DECL_STATIC uls_tokdef_vx_ptr_t __ulc_proc_line__regular(ulc_header_ptr_t hdr,
	const char *tok_nam, const char *keyw, const char *tok_idstr);
ULS_DECL_STATIC int srch_vx_by_id(uls_const_voidptr_t a, uls_const_voidptr_t p_id);
ULS_DECL_STATIC void __check_char_context(uls_lex_ptr_t uls);
ULS_DECL_STATIC int initial_fill_twoplus_mempool(uls_lex_ptr_t uls);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void ulc_init_header(ulc_header_ptr_t hdr, uls_lex_ptr_t uls);
void ulc_deinit_header(ulc_header_ptr_t hdr);
void reset_tokid_seed(uls_lex_ptr_t uls, ulc_header_ptr_t hdr);

int is_reserved_tok(uls_lex_ptr_t uls, const char *name);
int check_rsvd_toks(uls_lex_ptr_t uls);
int uls_is_char_idfirst(uls_lex_ptr_t uls, const char *lptr, uls_wch_t* ptr_wch);
int uls_is_char_id(uls_lex_ptr_t uls, uls_wch_t wch);

uls_tokdef_vx_ptr_t uls_find_reg_tokdef_vx(uls_lex_ptr_t uls, int t);
uls_tokdef_vx_ptr_t uls_find_tokdef_vx(uls_lex_ptr_t uls, int t);

FILE* ulc_search_for_fp(int typ_fpath, const char *fpath, uls_ptrtype_tool(outparam) parms);
FILE* uls_get_ulc_path(int typ_fpath, const char *fpath, int len_dpath, const char *specname, int len_specname, uls_ptrtype_tool(outparam) parms);

int ulc_get_searchpath_by_specname(uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list);
int ulc_get_searchpath_by_specpath(int is_abspath, uls_ptrtype_tool(arglst) nameof_searchpath, uls_ptrtype_tool(arglst) searchpath_list);

char *is_cnst_suffix_contained(char *cstr_pool, const char *str, int l_str, uls_ptrtype_tool(outparam) parms);

int ulc_prepend_searchpath_exeloc(const char *argv0);
int check_ulc_fileformat_magic(char *linebuff, int linelen, int ftype);

ulc_fpitem_ptr_t ulc_find_fp_list(ulc_fpitem_ptr_t fp_stack_top, const char *ulc_name);
ulc_fpitem_ptr_t ulc_fp_push(ulc_fpitem_ptr_t fp_lst, FILE *fp, const char *str);
FILE* ulc_fp_peek(ulc_fpitem_ptr_t fp_lst, uls_ptrtype_tool(outparam) parms);
ulc_fpitem_ptr_t ulc_fp_pop(ulc_fpitem_ptr_t fp_lst, uls_ptrtype_tool(outparam) parms);
void release_ulc_fp_stack(ulc_fpitem_ptr_t fp_lst);

void init_reserved_toks(uls_lex_ptr_t uls);
int check_if_keyw(uls_lex_ptr_t uls);
int uls_classify_tok_group(uls_lex_ptr_t uls);

int comp_vx_by_tokid(uls_const_voidptr_t a, uls_const_voidptr_t b);
int parse_id_ranges(uls_lex_ptr_t uls, int is_first, char *line);

int uls_cmd_run(const char *keyw, char *line, uls_voidptr_t data);

void reset_context_ready(uls_lex_ptr_t uls);
void reset_xcontext_tokid(uls_lex_ptr_t uls);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void uls_want_eof(uls_lex_ptr_t uls);
ULS_DLL_EXTERN void uls_unwant_eof(uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_is_valid_specpath(const char *confname);
ULS_DLL_EXTERN int uls_get_spectype(const char *filepath, uls_ptrtype_tool(outparam) parms);

ULS_DLL_EXTERN void ulc_list_searchpath(const char *confname);
ULS_DLL_EXTERN void ulc_set_searchpath(const char *pathlist);
ULS_DLL_EXTERN int ulc_add_searchpath(const char *pathlist, int front);

ULS_DLL_EXTERN int ulc_prepend_searchpath_pwd(void);

ULS_DLL_EXTERN int ulc_read_header(FILE* fin, ulc_header_ptr_t hdr);
ULS_DLL_EXTERN uls_tokdef_vx_ptr_t ulc_proc_line(ulc_header_ptr_t hdr, char *line, uls_ptrtype_tool(outparam) parms);

ULS_DLL_EXTERN uls_xcontext_ptr_t _uls_get_xcontext(uls_lex_ptr_t uls);
ULS_DLL_EXTERN int uls_is_ch_1ch_token(uls_lex_ptr_t uls, int ch);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#if defined(ULS_USE_WSTR)
#include "uls/uls_conf_wstr.h"
#elif defined(ULS_USE_ASTR)
#include "uls/uls_conf_astr.h"
#endif
#endif

#endif // __ULS_CONF_H__
