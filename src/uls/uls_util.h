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

#ifndef __ULS_WINDOWS__
#ifdef ULS_USE_WSTR
#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = uls_getWargList(argv,argc); \
	} while (0)
#define ULS_PUT_WARGS_LIST(n_wargv,wargv) uls_putWargList(wargv,n_wargv)

#else // ULS_USE_WSTR
#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = (argv); \
	} while (0)
#define ULS_PUT_WARGS_LIST(n_wargv,wargv)
#endif // ULS_USE_WSTR
#endif // __ULS_WINDOWS__

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(optproc,int)(int opt, char *optarg);
ULS_DEFINE_DELEGATE_END(optproc);
#endif

#if defined(__ULS_UTIL__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __os_mkdir_ustr(const char *filepath);
ULS_DECL_STATIC int __uls_mkdir_ustr(const char *filepath0);
ULS_DECL_STATIC int get_ms_codepage(uls_ptrtype_tool(outparam) parms);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
char *path_skip_uplevel(char *fpath);
#ifdef __ULS_WINDOWS__
int get_volume_name(const char *filepath);
int __uls_path_normalize_ustr(const char *fpath, char *fpath2);
#endif
unsigned char uls_nibble2ascii(unsigned char ch);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void uls_putstr(const char *str);
ULS_DLL_EXTERN void uls_dump_utf8str(const char *str);

ULS_DLL_EXTERN int uls_str2int(const char *str);
ULS_DLL_EXTERN int uls_str_length(const char *str);
ULS_DLL_EXTERN int uls_str_equal(const char *str1, const char *str2);
ULS_DLL_EXTERN int uls_str_copy(char *bufptr, const char *str);
ULS_DLL_EXTERN int uls_str_compare(const char *str1, const char *str2);

ULS_DLL_EXTERN int uls_mkdir(const char *filepath);
ULS_DLL_EXTERN int uls_path_normalize(const char *fpath, char *fpath2);

ULS_DLL_EXTERN int skip_c_comment_file(FILE* fin);

#ifndef ULS_DOTNET
typedef int (*uls_optproc_t)(int opt, char *optarg);
ULS_DLL_EXTERN int uls_getopts(int n_args, char *args[], const char *optfmt, uls_optproc_t proc);
#endif
int initialize_uls_util(void);
void finalize_uls_util(void);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#include "uls/uls_util_wstr.h"
#elif defined(ULS_USE_ASTR)
#include "uls/uls_util_astr.h"
#endif
#endif

#endif // __ULS_UTIL_H__
