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
  <file> uls_istream_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#ifndef __ULS_ISTREAM_WSTR_H__
#define __ULS_ISTREAM_WSTR_H__

#include "uls/uls_lex.h"
#include "uls/uls_istream.h"
#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(tmpl_wstr)
{
	csz_str_t name_ustr;
	csz_str_t val_ustr;

	wchar_t *val_wstr;
	int val_wsiz, val_wlen;
};
ULS_DEF_ARRAY_TYPE10(tmpl_wstr);

ULS_DEFINE_STRUCT(tmpl_list_wstr)
{
	uls_decl_array_type10(tmpls_wext, tmpl_wstr);
};

void __init_tmpls_wstr(uls_tmpl_wstr_ptr_t tmpl_ary, int n);

void uls_init_tmpl_wstr(uls_tmpl_wstr_ptr_t tmpl_wstr);
void uls_deinit_tmpl_wstr(uls_tmpl_wstr_ptr_t tmpl_wstr);

ULS_DLL_EXTERN int uls_init_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags);
ULS_DLL_EXTERN int uls_deinit_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list);
ULS_DLL_EXTERN uls_tmpl_list_ptr_t uls_create_tmpls_wstr(int n_alloc, int flags);
ULS_DLL_EXTERN void uls_reset_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc);
ULS_DLL_EXTERN int uls_destroy_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list);

ULS_DLL_EXTERN uls_tmpl_t* uls_find_tmpl_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t *name);
ULS_DLL_EXTERN const wchar_t* uls_get_tmpl_value_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t* name);
ULS_DLL_EXTERN int uls_set_tmpl_value_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t* name, const wchar_t *val);
ULS_DLL_EXTERN int uls_add_tmpl_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t *name, const wchar_t *val);

ULS_DLL_EXTERN void uls_set_istream_wtag(uls_istream_ptr_t istr, const wchar_t* wtag);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_file_wstr(const wchar_t* fpath);
#ifdef ULS_FDF_SUPPORT
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_filter_file_wstr(fdf_t* fdf, const wchar_t* wfilepath);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define uls_init_tmpls uls_init_tmpls_wstr
#define uls_deinit_tmpls uls_deinit_tmpls_wstr
#define uls_create_tmpls uls_create_tmpls_wstr
#define uls_reset_tmpls uls_reset_tmpls_wstr
#define uls_destroy_tmpls uls_destroy_tmpls_wstr

#define uls_get_tmpl_value uls_get_tmpl_value_wstr
#define uls_set_tmpl_value uls_set_tmpl_value_wstr
#define uls_add_tmpl uls_add_tmpl_wstr

#define uls_set_istream_tag uls_set_istream_wtag
#define uls_open_istream_file uls_open_istream_file_wstr
#ifdef ULS_FDF_SUPPORT
#define uls_open_istream_filter_file uls_open_istream_filter_file_wstr
#endif
#endif // ULS_USE_WSTR
#endif

#endif // __ULS_ISTREAM_WSTR_H__
