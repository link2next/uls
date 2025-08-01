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
  <file> uld_conf.h </file>
  <brief>
    Loading the config spec file (*.uld) of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2014.
  </author>
*/
#ifndef __ULD_CONF_H__
#define __ULD_CONF_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_conf.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(nam_tok)
{
	const char *name;
	int tok_id;
};
ULS_DEF_ARRAY_TYPE10(nam_tok);

_ULS_DEFINE_STRUCT(uld_line)
{
	const char *name;
	const char *name2;
	int tokid_changed, tokid;
	const char *aliases;
};

_ULS_DEFINE_STRUCT(uld_names_map)
{
	uls_lex_ptr_t uls;
	int n_vx_namelist;
};

#endif

#if defined(__ULD_CONF__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int comp_vx_by_toknam(uls_const_voidptr_t a, uls_const_voidptr_t b);
ULS_DECL_STATIC int srch_vx_by_toknam(uls_const_voidptr_t a, uls_const_voidptr_t b);
ULS_DECL_STATIC void __change_tok_id(uls_tokdef_vx_ptr_t e_vx, int tok_id);
ULS_DECL_STATIC int add_aliases_to_token(uls_tokdef_vx_ptr_t e_vx, uls_ptrtype_tool(wrd) wrdx);
ULS_DECL_STATIC void writeline_uld_backup(uls_xcontext_ptr_t xctx, const char *line, int linelen);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int uld_calc_filebuff_size(int filesize);
int uld_pars_line(uls_ptrtype_tool(wrd) wrdx, uld_line_ptr_t tok_names);
int uld_proc_line(char *lptr, uls_lex_ptr_t uls, int n2_vx_namelist);
uls_tokdef_vx_ptr_t uld_find_tokdef_vx(uls_lex_ptr_t uls, int n_vx_namelist, const char *name);
int uld_add_aliases(uls_tokdef_vx_ptr_t e_vx, const char *line_aliases);
int __uld_post_names(uld_names_map_ptr_t names_map);
int uld_load_fp(uls_lex_ptr_t uls, FILE *fin_uld, int siz_uld_filebuff);

void uls_init_nam_tok(uls_nam_tok_ptr_t nam_tok);
void uls_deinit_nam_tok(uls_nam_tok_ptr_t nam_tok);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN uld_names_map_ptr_t uld_prepare_names(uls_lex_ptr_t uls, int siz_uldfile);
ULS_DLL_EXTERN int uld_change_names(uld_names_map_ptr_t names_map, uld_line_ptr_t names);
ULS_DLL_EXTERN int uld_post_names(uld_names_map_ptr_t names_map);

ULS_DLL_EXTERN int uld_change_tok_id(uls_tokdef_vx_ptr_t e_vx, int tok_id);
ULS_DLL_EXTERN int uld_change_tok_names(uls_tokdef_vx_ptr_t e_vx, const char *name2, const char *line_aliases);

ULS_DLL_EXTERN void uld_dump_sample(uls_lex_ptr_t uls);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULD_CONF_H__
