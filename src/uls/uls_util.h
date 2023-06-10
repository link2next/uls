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
#include "uls/uls_prim.h"

#include <stdlib.h>
#include <errno.h>
#endif // ULS_EXCLUDE_HFILES

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(optproc,int)(int opt, char* optarg);
ULS_DEFINE_DELEGATE_END(optproc);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(isp)
{
	char *buff;
	int  len_strpool, siz_strpool;
	int  align_strpool;
};
#endif

#if defined(__ULS_UTIL__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __os_mkdir_ustr(const char *filepath);
ULS_DECL_STATIC int __uls_mkdir_ustr(const char *filepath0);
ULS_DECL_STATIC int get_ms_codepage(uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int uls_get_exeloc_by_pathenv(const char *progname, char *fpath_buf);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int is_filepath_delim(char ch);
char* path_skip_uplevel(char* fpath);
#ifdef ULS_WINDOWS
int get_volume_name(const char* filepath);
int __uls_path_normalize_ustr(const char* fpath, char* fpath2);
#endif
#endif

#ifdef ULS_DECL_PUBLIC_PROC
void uls_print_bytes(const char* srcptr, int n_bytes);
int uls_get_exeloc_dir(const char* argv0, char *fpath_buf);
const char* uls_get_dirpath(const char* fname, uls_ptrtype_tool(outparam) parms);

int is_absolute_path(const char* path);
int is_path_prefix(const char *filepath);
int uls_mkdir(const char *filepath);
ULS_DLL_EXTERN int uls_path_normalize(const char* fpath, char* fpath2);

ULS_DLL_EXTERN int skip_c_comment_file(FILE* fin);
ULS_DLL_EXTERN void uls_get_current_time_yyyymmdd_hhmm(char* buf, int buf_siz);

void isp_init(uls_isp_ptr_t isp, int init_size);
void isp_reset(uls_isp_ptr_t isp);
void isp_deinit(uls_isp_ptr_t isp);
char* isp_find(uls_isp_ptr_t isp, const char* str, int len);
char* isp_insert(uls_isp_ptr_t isp, const char* str, int len);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN int uls_getopts(int n_args, char* args[], const char* optfmt, uls_optproc_t proc);
#endif
int initialize_uls_util(void);
void finalize_uls_util(void);
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
