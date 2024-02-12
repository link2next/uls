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

#ifdef ULS_WINDOWS

#ifdef ULS_USE_WSTR
#ifndef  tstring
#define tstring wstring
#endif
#ifndef  otstringstream
#define otstringstream wostringstream
#endif
#define _tcout wcout
#define _tcerr wcerr
#define _tendl L"\n"

#ifndef _tmain
#define _tmain wmain
#endif

#else // ULS_USE_WSTR
#ifndef  tstring
#define tstring string
#endif
#ifndef  otstringstream
#define otstringstream ostringstream
#endif
#define _tcout cout
#define _tcerr cerr
#define _tendl "\n"

#ifndef _tmain
#define _tmain main
#endif

#endif // ULS_USE_WSTR
#else // ULS_WINDOWS

#ifdef ULS_USE_WSTR
typedef wchar_t TCHAR;
#define tstring wstring
#define otstringstream wostringstream
#define _tcout wcout
#define _tcerr wcerr
#define _tendl L"\n"

#define uls_get_csz_tstr(csz) uls_get_csz_wstr(csz)
#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = ulscompat_get_warg_list(argv,argc); \
	} while (0)
#define ULS_PUT_WARGS_LIST(n_wargv,wargv) ulscompat_put_warg_list(wargv,n_wargv)

#define ULSCPP_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = uls::getWargList(argv,argc); \
	} while (0)
#define ULSCPP_PUT_WARGS_LIST(n_wargv,wargv) uls::putWargList(wargv,n_wargv)

#else // ULS_USE_WSTR

typedef char TCHAR;
#define tstring string
#define otstringstream ostringstream
#define _tcout cout
#define _tcerr cerr
#define _tendl "\n"

#define uls_get_csz_tstr(csz) csz_text(csz)
#define ULS_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = (argv); \
	} while (0)
#define ULS_PUT_WARGS_LIST(n_wargv,wargv)

#define ULSCPP_GET_WARGS_LIST(argc,argv,targv) do { \
	(targv) = (argv); \
	} while (0)
#define ULSCPP_PUT_WARGS_LIST(n_wargv,wargv)

#endif // ULS_USE_WSTR

typedef TCHAR *LPTSTR;
typedef const TCHAR *LPCTSTR;
#endif // ULS_WINDOWS

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
ULS_DLL_EXTERN unsigned char uls_nibble2ascii(unsigned char ch);
ULS_DLL_EXTERN void uls_putstr(const char* str);

ULS_DLL_EXTERN int ult_str2int(const char *str);
ULS_DLL_EXTERN int ult_str_length(const char *str);
ULS_DLL_EXTERN int ult_str_equal(const char *str1, const char *str2);
ULS_DLL_EXTERN int ult_str_copy(char *bufptr, const char *str);
ULS_DLL_EXTERN int ult_str_compare(const char* str1, const char* str2);
ULS_DLL_EXTERN char* ult_skip_blanks(const char* lptr);
ULS_DLL_EXTERN char* ult_splitstr(char** p_str, int *p_len);
ULS_DLL_EXTERN char* ult_split_litstr(char *str, char qch);

ULS_DLL_EXTERN void ult_dump_utf8str(const char *str);
void uls_print_bytes(const char* srcptr, int n_bytes);
int uls_get_exeloc_dir(const char* argv0, char *fpath_buf);
const char* uls_get_dirpath(const char* fname, uls_ptrtype_tool(outparam) parms);

int is_absolute_path(const char* path);
int is_path_prefix(const char *filepath);

ULS_DLL_EXTERN int uls_mkdir(const char *filepath);
ULS_DLL_EXTERN int uls_path_normalize(const char* fpath, char* fpath2);

ULS_DLL_EXTERN int skip_c_comment_file(FILE* fin);
ULS_DLL_EXTERN void uls_get_current_time_yyyymmdd_hhmm(char* buf, int buf_siz);

void isp_init(uls_isp_ptr_t isp, int init_size);
void isp_reset(uls_isp_ptr_t isp);
void isp_deinit(uls_isp_ptr_t isp);
char* isp_find(uls_isp_ptr_t isp, const char* str, int len);
char* isp_insert(uls_isp_ptr_t isp, const char* str, int len);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN char* uls_split_filepath(const char *filepath, int* len_fname);
ULS_DLL_EXTERN int uls_getopts(int n_args, char* args[], const char* optfmt, uls_optproc_t proc);
#endif
int initialize_uls_util(void);
void finalize_uls_util(void);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_util_wstr.h"
#endif

#endif // __ULS_UTIL_H__
