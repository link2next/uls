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
  <file> uls_util.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef __ULS_UTIL_H__
#define __ULS_UTIL_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_version.h"
#include "uls/uls_prim.h"
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define OPT_NOT_OPTCHAR   0
#define OPT_USAGE_BRIEF   1
#define OPT_USAGE_DESC    2
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(optproc,int)(int opt, char* optarg);
ULS_DEFINE_DELEGATE_END(optproc);
#endif

#if defined(__ULS_UTIL__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __os_mkdir_ustr(const char *filepath);
ULS_DECL_STATIC int __uls_mkdir_ustr(const char *filepath0);
ULS_DECL_STATIC int uls_get_exeloc_by_pathenv(const char *progname, char *fpath_buf);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int is_filepath_delim(char ch);
int is_absolute_path(const char* path);
int is_path_prefix(const char *filepath);

FILE* uls_get_spec_fp(const char* dirpath_list, const char* fpath, _uls_tool_ptrtype_(outparam) parms);
const char* uls_get_dirpath(const char* fname, _uls_tool_ptrtype_(outparam) parms);

#ifdef ULS_WINDOWS
int get_volume_name(const char* filepath);
int __uls_path_normalize_ustr(const char* fpath, char* fpath2);
#endif
char* path_skip_uplevel(char* fpath);

int uls_get_exeloc_dir(const char* argv0, char *fpath_buf);
void uls_print_bytes(const char* srcptr, int n_bytes);

void initialize_uls_util(void);
void finalize_uls_util(void);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int skip_c_comment_file(FILE* fin);
ULS_DLL_EXTERN int uls_path_normalize(const char* fpath, char* fpath2);

int uls_mkdir(const char *filepath);
void uls_get_current_time_yyyymmdd_hhmm(char* buf, int buf_siz);
#ifndef ULS_DOTNET
ULS_DLL_EXTERN int uls_getopts(int n_args, char* args[], const char* optfmt, uls_optproc_t proc);
#endif

#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USEDLL
#include "uls/uls_print.h"
#include "uls/uls_log.h"

#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = (argv); \
	} while (0)
#define ULS_PUT_WARGS_LIST(n_wargv,wargv)

#if defined(ULS_USE_WSTR)
#include "uls/uls_util_wstr.h"

#ifndef ULS_WINDOWS
#undef ULS_GET_WARGS_LIST
#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = ulscompat_get_warg_list(argv,argc); \
	} while (0)
#undef ULS_PUT_WARGS_LIST
#define ULS_PUT_WARGS_LIST(n_wargv,wargv) ulscompat_put_warg_list(wargv,n_wargv)
#endif

#elif defined(ULS_USE_ASTR)
#include "uls/uls_util_astr.h"
#endif

#endif // _ULS_USEDLL

#endif // __ULS_UTIL_H__
