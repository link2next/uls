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
  <file> uls_util_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#ifndef __ULS_UTIL_ASTR_H__
#define __ULS_UTIL_ASTR_H__

#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DLL_EXTERN void uls_aputstr(const char *astr);
ULS_DLL_EXTERN void uls_dump_utf8str_astr(const char *astr);
ULS_DLL_EXTERN int uls_astr2int(const char *astr);
ULS_DLL_EXTERN int uls_astrlen(const char *astr);
ULS_DLL_EXTERN int uls_astreql(const char *astr1, const char *astr2);
ULS_DLL_EXTERN int uls_astrcpy(char *bufptr, const char *astr);
ULS_DLL_EXTERN int uls_astrcmp(const char *astr1, const char *astr2);

ULS_DLL_EXTERN int uls_path_normalize_astr(const char *fpath, char *fpath2);
ULS_DLL_EXTERN int uls_mkdir_astr(const char *filepath);

ULS_DLL_EXTERN char *uls_astrdup(const char *astr, int alen);
ULS_DLL_EXTERN char *uls_astrchr(const char *lptr, char ch0);
ULS_DLL_EXTERN int _uls_explode_astr(uls_wrd_ptr_t uw, char delim_ch, uls_arglst_ptr_t arglst);
ULS_DLL_EXTERN int uls_explode_astr(char **ptr_line, char delim_ch, char **args, int n_args);

ULS_DLL_EXTERN int uls_getopts_astr(int n_args, char *args[], const char *optfmt, uls_optproc_t proc);
ULS_DLL_EXTERN FILE *cvt_ms_mbcs_filepath_astr(const char *filepath, uls_tempfile_ptr_t tmpfile_utf8, int flags);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_putstr uls_aputstr
#define uls_dump_utf8str uls_dump_utf8str_astr
#define uls_str2int uls_astr2int
#define uls_str_length uls_astrlen
#define uls_str_equal uls_astreql
#define uls_str_copy uls_astrcpy
#define uls_str_compare uls_astrcmp

#define uls_path_normalize uls_path_normalize_astr
#define uls_mkdir uls_mkdir_astr

#define uls_strdup uls_astrdup
#define uls_strchr uls_astrchr
#define uls_explode_str uls_explode_astr

#define uls_getopts uls_getopts_astr
#define cvt_ms_mbcs_filepath cvt_ms_mbcs_filepath_astr
#endif // ULS_USE_ASTR
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_UTIL_ASTR_H__
