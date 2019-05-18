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
  <file> uls_fileio_wstr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#ifndef __ULS_FILEIO_WSTR_H__
#define __ULS_FILEIO_WSTR_H__

#include "uls/uls_type.h"
#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DLL_EXTERN int uls_dirent_exist_wstr(const wchar_t *wfpath);
ULS_DLL_EXTERN FILE* uls_fp_wopen(const wchar_t* wfpath, int mode);
ULS_DLL_EXTERN int uls_fp_gets_wstr(FILE *fp, wchar_t *wbuf, int wbuf_siz, int flags);
ULS_DLL_EXTERN int uls_close_tempfile_wstr(uls_tempfile_ptr_t tmpfile, const wchar_t* wfilepath);
ULS_DLL_EXTERN int uls_fd_open_wstr(const wchar_t* wfpath, int mode);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_WSTR
#define uls_dirent_exist uls_dirent_exist_wstr
#define uls_fp_open uls_fp_wopen
#define uls_fp_gets uls_fp_gets_wstr
#define uls_close_tempfile uls_close_tempfile_wstr
#define uls_fd_open uls_fd_open_wstr
#endif
#endif

#endif // __ULS_FILEIO_WSTR_H__
