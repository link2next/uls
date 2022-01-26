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

#include "uls/uls_util.h"
#include "uls/uls_auw.h"
#include "uls/uls_fileio.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

typedef int (*uls_woptproc_t)(int opt, wchar_t* optwarg);

ULS_DLL_EXTERN void uls_wputstr(const wchar_t *wstr);

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

#endif
