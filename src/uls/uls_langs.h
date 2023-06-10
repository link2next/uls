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
  <file> uls_langs.h </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/

#ifndef __ULS_LANGS_H__
#define __ULS_LANGS_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PROTECTED_TYPE
#define ULC_ALIAS_HASHTABLE_SIZ  101

ULS_DECLARE_STRUCT(lang_list);

ULS_DEFINE_STRUCT(alias)
{
	char *name;
	int len, lst_id;
	uls_alias_ptr_t next;
};

ULS_DEF_PARRAY(alias);
#ifndef ULS_CLASSIFY_SOURCE
ULS_DEF_ARRAY_TYPE10(alias);
#endif

ULS_DEFINE_STRUCT(lang)
{
	uls_alias_ptr_t e0;
	int idx_alias_list, n_alias_list;
	uls_lang_list_ptr_t parent;
};
ULS_DEF_ARRAY_TYPE10(lang);

ULS_DEFINE_STRUCT_BEGIN(lang_list)
{
	uls_decl_parray(hashtbl, alias); // ULC_ALIAS_HASHTABLE_SIZ
	uls_decl_array_type10(langs, lang);
	uls_decl_array_type10(alias_pool, alias);

	char *str_pool;
	int siz_str_pool, n_str_pool;
};

#endif // ULS_DEF_PROTECTED_TYPE

#if defined(ULS_DOTNET) || defined(__ULS_LANGS__)
#define EXTERNAL
#else
#define EXTERNAL extern
#endif

#if !defined(ULS_DOTNET) || defined(ULS_DEF_PROTECTED_DATA)
EXTERNAL uls_lang_list_ptr_t uls_langs;
#endif

#if defined(__ULS_LANGS__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC unsigned int ulc_alias_hashfunc(const char *str, int n);
ULS_DECL_STATIC int strdup_cnst(uls_lang_list_ptr_t tbl, const char *str);

ULS_DECL_STATIC uls_lang_ptr_t uls_append_lang(uls_lang_list_ptr_t tbl, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int langs_proc_line(uls_lang_list_ptr_t tbl, char* line);
ULS_DECL_STATIC void construct_ulc_lang_db(uls_lang_list_ptr_t tbl);
ULS_DECL_STATIC _ULS_INLINE int __is_langs_needed_quote(const char* name);
ULS_DECL_STATIC void uls_dump_lang(uls_lang_ptr_t lang);
#endif // ULS_DECL_PRIVATE_PROC

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_alias(uls_alias_ptr_t alias);
void uls_deinit_alias(uls_alias_ptr_t alias);

uls_alias_ptr_t new_ulc_alias(uls_lang_list_ptr_t tbl, const char *wrd, int lst_id);
void uls_init_lang(uls_lang_ptr_t lang);
void uls_deinit_lang(uls_lang_ptr_t lang);

void uls_init_lang_list(uls_lang_list_ptr_t tbl);
void uls_deinit_lang_list(uls_lang_list_ptr_t tbl);

uls_lang_list_ptr_t uls_load_langdb(const char *fpath);
void uls_destroy_lang_list(uls_lang_list_ptr_t tbl);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN uls_lang_ptr_t uls_find_lang(uls_lang_list_ptr_t tbl, const char* alias);
ULS_DLL_EXTERN const char* uls_find_lang_name(const char* alias);
ULS_DLL_EXTERN int uls_list_names_of_lang(const char* alias);
ULS_DLL_EXTERN void uls_list_langs(void);
#endif

#undef EXTERNAL
#ifdef _ULS_CPLUSPLUS
}
#endif

#endif //  __ULS_LANGS_H__
