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
 * uls_freq.h -- gathering the frequencies of keywords --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_FREQ_H__
#define __ULS_FREQ_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_version.h"
#include "uls/idkeyw.h"
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define uls_is_ulf_compatible(ver,ver0) (_uls_tool_(version_cmp_code)(ver,ver0) >= 2)
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
_ULS_DEFINE_STRUCT(ulf_header)
{
	uls_type_tool(version) filever;
	uls_def_namebuf(hash_algorithm, ULS_LEXSTR_MAXSIZ);
	uls_def_intarray(weights);
};
#endif

#if defined(__ULS_FREQ__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int ulf_read_config_var(int lno, char* lptr, ulf_header_ptr_t hdr);
ULS_DECL_STATIC int ulf_read_header(FILE* fin, ulf_header_ptr_t hdr);
ULS_DECL_STATIC uls_keyw_stat_list_ptr_t make_keyw_stat_for_load(
	uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst, ulf_header_ptr_t hdr);
ULS_DECL_STATIC void normalize_keyw_stat_list(uls_keyw_stat_list_ptr_t kwslst);
ULS_DECL_STATIC void ulf_create_file_header(uls_hash_stat_ptr_t hs, FILE* fout);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void ulf_init_header(ulf_header_ptr_t hdr);
void ulf_deinit_header(ulf_header_ptr_t hdr);
uls_keyw_stat_list_ptr_t ulf_load(uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst,
	FILE *fin, ulf_header_ptr_t hdr);
int keyw_stat_comp_by_freq(const uls_voidptr_t a, const uls_voidptr_t b);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int ulf_create_file(uls_hash_stat_ptr_t hs, uls_keyw_stat_list_ptr_t kwslst, FILE* fout);
ULS_DLL_EXTERN uls_keyw_stat_ptr_t ulf_search_kwstat_list(
	uls_keyw_stat_list_ptr_t kwslst, const char* str);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_FREQ_H__
