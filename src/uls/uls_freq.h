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

#include "uls/uls_version.h"
#include "uls/idkeyw.h"
#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define uls_is_ulf_compatible(ver,ver0) (uls_version_cmp_code(ver,ver0) >= 2)

_ULS_DEFINE_STRUCT(ulf_header)
{
	uls_version_t filever;
	uls_uint32    init_hcode;

	uls_def_namebuf(hash_algorithm, ULS_LEXSTR_MAXSIZ);
	uls_version_t hfunc_ver;
	int           hash_table_size;
};

#if defined(__ULS_FREQ__)
ULS_DECL_STATIC void normalize_keyw_stat_list(uls_keyw_stat_list_ptr_t kwslst);
ULS_DECL_STATIC int ulf_read_config_var(int lno, char* lptr, ulf_header_ptr_t hdr);
ULS_DECL_STATIC int ulf_read_header(FILE* fin, ulf_header_ptr_t hdr);
ULS_DECL_STATIC uls_keyw_stat_list_ptr_t make_keyw_stat_for_load(
	uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst, ulf_header_ptr_t hdr);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void ulf_init_header(ulf_header_ptr_t hdr);
void ulf_deinit_header(ulf_header_ptr_t hdr);
uls_keyw_stat_list_ptr_t ulf_load(uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst,
	FILE *fin, ulf_header_ptr_t hdr, uls_kwtable_ptr_t uls_kwtable);

int keyw_stat_comp_by_freq(const uls_voidptr_t a, const uls_voidptr_t b);
#endif

ULS_DLL_EXTERN int ulf_create_file(int n_hcodes, uls_uint32 *hcodes,
	int htab_siz, uls_keyw_stat_list_ptr_t kwslst, FILE* fout);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_FREQ_H__
