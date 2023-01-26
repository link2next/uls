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
  <file> uls_util_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_UTIL_WSTR_H__
#define __ULS_UTIL_WSTR_H__

#include "uls/uls_auw.h"
#ifdef _ULS_INTERNAL_USE_ONLY
#include "uls/uls_util.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

typedef int (*uls_woptproc_t)(int opt, wchar_t* optwarg);

ULS_DLL_EXTERN void uls_wputstr(const wchar_t *wstr);
ULS_DLL_EXTERN void ult_dump_bin_wstr(const wchar_t *wstr);

ULS_DLL_EXTERN int uls_wstr2int(const wchar_t *wstr);
ULS_DLL_EXTERN int uls_wstrlen(const wchar_t* wstr);
ULS_DLL_EXTERN int uls_wstreql(const wchar_t *wstr1, const wchar_t *wstr2);
ULS_DLL_EXTERN int uls_wstrcpy(wchar_t *wbufptr, const wchar_t *wstr);
ULS_DLL_EXTERN int uls_wstrcmp(const wchar_t *wstr1, const wchar_t *wstr2);
ULS_DLL_EXTERN wchar_t* ult_skip_blanks_wstr(const wchar_t* lptr);
ULS_DLL_EXTERN wchar_t* ult_splitstr_wstr(wchar_t** p_wstr, int *p_wlen);
ULS_DLL_EXTERN wchar_t* ult_split_litstr_wstr(wchar_t *wstr, wchar_t qch);

ULS_DLL_EXTERN wchar_t* uls_wfilename(const wchar_t *wfilepath, int* len_fname);
ULS_DLL_EXTERN int uls_path_normalize_wstr(const wchar_t* wfpath, wchar_t* wfpath2);
ULS_DLL_EXTERN int uls_mkdir_wstr(const wchar_t *wfilepath);

ULS_DLL_EXTERN wchar_t* uls_wstrdup(const wchar_t* wstr, int wlen);
ULS_DLL_EXTERN wchar_t* uls_wstrchr(const wchar_t* wlptr, wchar_t wch0);
ULS_DLL_EXTERN int _uls_explode_wstr(uls_wrd_ptr_t uw, wchar_t delim_wch, uls_arglst_ptr_t arglst);
ULS_DLL_EXTERN int uls_explode_wstr(wchar_t **ptr_wline, wchar_t delim_wch, wchar_t** args, int n_args);

ULS_DLL_EXTERN int uls_getopts_wstr(int n_args, wchar_t* wargs[], const wchar_t* optwfmt, uls_woptproc_t wproc);

ULS_DLL_EXTERN int uls_lf_wputs_csz(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_wputs_str(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_wputs_file(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);

ULS_DEFINE_STRUCT(warg)
{
  wchar_t *wstr;
  int wlen;
};

ULS_DEFINE_STRUCT(warg_list)
{
  uls_warg_t *wargs;
  int n_wargs;
};

ULS_DLL_EXTERN void uls_init_warg_list(uls_warg_list_t *wlist);
ULS_DLL_EXTERN void uls_deinit_warg_list(uls_warg_list_t *wlist);
ULS_DLL_EXTERN void uls_set_warg_list(uls_warg_list_t *wlist, char **args, int n_args);
ULS_DLL_EXTERN void uls_reset_warg_list(uls_warg_list_t *wlist);
ULS_DLL_EXTERN uls_warg_t*uls_get_warg(uls_warg_list_t *wlist, int ith);
ULS_DLL_EXTERN wchar_t** uls_export_warg_list(uls_warg_list_t *wlist, int *ptr_n_wargs);

ULS_DLL_EXTERN wchar_t** ulscompat_get_warg_list(char **argv, int n_argv);
ULS_DLL_EXTERN void ulscompat_put_warg_list(wchar_t **wargv, int n_wargv);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define uls_putstr uls_wputstr
#define ult_dump_bin ult_dump_bin_wstr

#define ult_str2int uls_wstr2int
#define ult_str_length uls_wstrlen
#define ult_str_equal uls_wstreql
#define ult_str_copy uls_wstrcpy
#define ult_str_compare uls_wstrcmp
#define ult_skip_blanks ult_skip_blanks_wstr
#define ult_splitstr ult_splitstr_wstr
#define ult_split_litstr ult_split_litstr_wstr

#define uls_split_filepath uls_wfilename
#define uls_path_normalize uls_path_normalize_wstr
#define uls_mkdir uls_mkdir_wstr

#define uls_strdup uls_wstrdup
#define uls_strchr uls_wstrchr
#define uls_explode_str uls_explode_wstr

#define uls_getopts uls_getopts_wstr
#define cvt_ms_mbcs_filepath cvt_ms_mbcs_filepath_wstr
#endif
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_UTIL_WSTR_H__
