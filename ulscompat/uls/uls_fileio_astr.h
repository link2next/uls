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
  <file> uls_fileio_astr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#ifndef __ULS_FILEIO_ASTR_H__
#define __ULS_FILEIO_ASTR_H__

#include "uls/uls_auw.h"
#ifdef _ULSCOMPAT_INTERNALLY_USES
#include "uls/uls_fileio.h"
#endif

#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif
ULS_DLL_EXTERN int uls_dirent_exist_astr(const char *afpath);
ULS_DLL_EXTERN FILE *uls_fp_aopen(const char *fpath, int mode);
ULS_DLL_EXTERN int uls_fp_gets_astr(FILE *fp, char *abuf, int abuf_siz, int flags);
ULS_DLL_EXTERN int uls_close_tempfile_astr(uls_tempfile_ptr_t tmpfile, const char *filepath);
ULS_DLL_EXTERN int uls_fd_open_astr(const char *fpath, int mode);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_dirent_exist uls_dirent_exist_astr
#define uls_fp_open uls_fp_aopen
#define uls_fp_gets uls_fp_gets_astr
#define uls_close_tempfile uls_close_tempfile_astr
#define uls_fd_open uls_fd_open_astr
#endif // ULS_USE_ASTR
#endif // _ULS_USE_ULSCOMPAT

#endif // __ULS_FILEIO_ASTR_H__
