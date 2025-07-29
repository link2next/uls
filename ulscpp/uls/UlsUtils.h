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
  <file> UlsUtils.h </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2018.
  </author>
*/

#pragma once

#include <uls/UlsAuw.h>
#include <string>
#include <stdio.h>

namespace uls {
	// <brief>
	// Copies memory from 'src' to 'dst'
	// </brief>
	ULSCPP_DLL_EXTERN void memcopy(void *dst, const void *src, int n);

	// <brief>
	// Returns the length of string.
	// Returns the number of wchar_t for 'wstr'
	// </brief>
	ULSCPP_DLL_EXTERN int strLength(const char *str);
	ULSCPP_DLL_EXTERN int strLength(const wchar_t *wstr);

	// <brief>
	// Searches 'line' for 'ch'
	// </brief>
	ULSCPP_DLL_EXTERN int strFindIndex(const char *line, char ch);
	ULSCPP_DLL_EXTERN int strFindIndex(const wchar_t *wline, wchar_t wch);

	// <brief>
	// Convert string to integer
	// </brief>
	ULSCPP_DLL_EXTERN int strToInt(const char *str);
	ULSCPP_DLL_EXTERN int strToInt(const wchar_t *wstr);

	// <brief>
	// Checks if 'fpath' exists in the file system
	// </brief>
	// <return>1 or 0 elsewhere</return>
	ULSCPP_DLL_EXTERN int direntExist(const char *fpath);
	ULSCPP_DLL_EXTERN int direntExist(const wchar_t *wfpath);

	// <brief>
	//   Opens file and return the file.
	//   The file content is read only.
	// </brief>
	// <parm name="fpath">the file path to be read</parm>
	// <return>file pointer</return>
	ULSCPP_DLL_EXTERN FILE *fileOpenReadolnly(const char *fpath);
	ULSCPP_DLL_EXTERN FILE *fileOpenReadolnly(const wchar_t *wfpath);

	// <brief>
	//   Creates a fileand return it.
	//   The file is used to write content sequentially.
	// </brief>
	// <parm name="fpath">the file path to be created</parm>
	// <return>file pointer</return>
	ULSCPP_DLL_EXTERN FILE *fileOpenWriteolnly(const char *fpath);
	ULSCPP_DLL_EXTERN FILE *fileOpenWriteolnly(const wchar_t *wfpath);

	// <brief>
	// get the base naem of 'filepath'
	// </brief>
	ULSCPP_DLL_EXTERN char *getFilename(const char *filepath, int* len_fname);
	ULSCPP_DLL_EXTERN wchar_t *getFilename(const wchar_t *wfilepath, int* len_wfname);

	// <brief>
	// parsing command args
	// </brief>
	typedef int (*optproc_t)(int opt, char *optarg);
	ULSCPP_DLL_EXTERN int parseCommandOptions(int n_args, char *args[], const char *optfmt, optproc_t proc);

	typedef int (*woptproc_t)(int opt, wchar_t *optarg);
	ULSCPP_DLL_EXTERN int parseCommandOptions(int n_args, wchar_t *args[], const wchar_t *optfmt, woptproc_t wproc);
}

#ifdef _ULSCPP_IMPLDLL
#define _ULSCPP_AUWCVT_LEN(slot_no) auwcvt.get_slot_len(slot_no)

#define _ULSCPP_USTR2WSTR(ustr, wstr, slot_no) do { \
	wstr = auwcvt.mbstr2wstr(ustr, UlsAuw::CVT_MBSTR_USTR, slot_no); \
	if (!wstr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)
#define _ULSCPP_WSTR2USTR(wstr, ustr, slot_no) do { \
	ustr = auwcvt.wstr2mbstr(wstr, UlsAuw::CVT_MBSTR_USTR, slot_no); \
	if (!ustr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)

#define _ULSCPP_ASTR2WSTR(astr, wstr, slot_no) do { \
	wstr = auwcvt.mbstr2wstr(astr, UlsAuw::CVT_MBSTR_ASTR, slot_no); \
	if (!wstr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)
#define _ULSCPP_WSTR2ASTR(wstr, astr, slot_no) do { \
	astr = auwcvt.wstr2mbstr(wstr, UlsAuw::CVT_MBSTR_ASTR, slot_no); \
	if (!astr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)

#define _ULSCPP_ASTR2USTR(astr, ustr, slot_no) do { \
	ustr = auwcvt.mbstr2mbstr(astr, UlsAuw::CVT_MBSTR_USTR, slot_no); \
	if (!ustr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)
#define _ULSCPP_USTR2ASTR(ustr, astr, slot_no) do { \
	astr = auwcvt.mbstr2mbstr(ustr, UlsAuw::CVT_MBSTR_ASTR, slot_no); \
	if (!astr) throw invalid_argument(string("invalid string encodeing")); \
	} while (0)
#endif // _ULSCPP_IMPLDLL
