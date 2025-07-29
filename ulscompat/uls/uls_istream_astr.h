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
  <file> uls_istream_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#ifndef __ULS_ISTREAM_ASTR_H__
#define __ULS_ISTREAM_ASTR_H__

#include "uls/uls_auw.h"
#ifdef _ULSCOMPAT_INTERNALLY_USES
#include "uls/uls_istream.h"
#endif

#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(tmpl_astr)
{
	csz_str_t name_ustr;
	csz_str_t val_ustr;

	char *val_astr;
	int val_asiz, val_alen;
};
ULS_DEF_ARRAY_TYPE10(tmpl_astr);

ULS_DEFINE_STRUCT(tmpl_list_astr)
{
	uls_decl_array_type10(tmpls_aext, tmpl_astr);
};

void uls_init_tmpl_astr(uls_tmpl_astr_ptr_t tmpl_astr);
void uls_deinit_tmpl_astr(uls_tmpl_astr_ptr_t tmpl_astr);

ULS_DLL_EXTERN int uls_init_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags);
ULS_DLL_EXTERN int uls_deinit_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list);
ULS_DLL_EXTERN uls_tmpl_list_ptr_t uls_create_tmpls_astr(int n_alloc, int flags);
ULS_DLL_EXTERN void uls_reset_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc);
ULS_DLL_EXTERN int uls_destroy_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list);

ULS_DLL_EXTERN uls_tmpl_t* uls_find_tmpl_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name);
ULS_DLL_EXTERN const char *uls_get_tmpl_value_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name);
ULS_DLL_EXTERN int uls_set_tmpl_value_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val);
ULS_DLL_EXTERN int uls_add_tmpl_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val);

ULS_DLL_EXTERN void uls_set_istream_atag(uls_istream_ptr_t istr, const char *atag);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_file_astr(const char *fpath);
#ifdef ULS_FDF_SUPPORT
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_filter_file_astr(fdf_t* fdf, const char *filepath);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_init_tmpls uls_init_tmpls_astr
#define uls_deinit_tmpls uls_deinit_tmpls_astr
#define uls_create_tmpls uls_create_tmpls_astr
#define uls_reset_tmpls uls_reset_tmpls_astr
#define uls_destroy_tmpls uls_destroy_tmpls_astr

#define uls_get_tmpl_value uls_get_tmpl_value_astr
#define uls_set_tmpl_value uls_set_tmpl_value_astr
#define uls_add_tmpl uls_add_tmpl_astr

#define uls_set_istream_tag uls_set_istream_atag
#define uls_open_istream_file uls_open_istream_file_astr
#ifdef ULS_FDF_SUPPORT
#define uls_open_istream_filter_file uls_open_istream_filter_file_astr
#endif
#endif // ULS_USE_ASTR
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_ISTREAM_ASTR_H__
