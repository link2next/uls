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
  <file> ms_mbcs_file.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jun 2015.
  </author>
*/

#ifndef __ULS_MS_MBCS_FILE_H__
#define __ULS_MS_MBCS_FILE_H__

#include "uls/uls_util.h"
#include "uls/uls_fileio.h"
#include <stdio.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define ULS_FILE_MS_MBCS 0x10
#define ULS_FILE_UTF8    0x20

ULS_DLL_EXTERN void ms_mbcs_tmpf_ungrabber(uls_voidptr_t data);
ULS_DLL_EXTERN void ms_mbcs_fp_ungrabber(uls_voidptr_t data);
ULS_DLL_EXTERN void ms_mbcs_str_ungrabber(uls_voidptr_t data);

ULS_DLL_EXTERN FILE *cvt_ms_mbcs_fp(FILE *fp, uls_tempfile_ptr_t tmpfile_utf8, int flags);
ULS_DLL_EXTERN FILE *cvt_ms_mbcs_filepath_ustr(const char *filepath, uls_tempfile_ptr_t tmpfile_utf8, int flags);
ULS_DLL_EXTERN FILE *cvt_ms_mbcs_filepath_wstr(const wchar_t *wfilepath, uls_tempfile_ptr_t tmpfile_utf8, int flags);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_MS_MBCS_FILE_H__
