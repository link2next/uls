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

#include <uls/csz_stream.h>
#include <uls/uls_util_wstr.h>
#include <stdio.h>

namespace uls {
	namespace crux {
		static const int ULSCPP_NUM_CSZ_BUFFS = 4;

		// <brief>
		// the converters among ANSI-encoding, UTF8, Wide-strings
		// </brief>
		class ULSCPP_DLL_EXTERN UlsAuw {
			static const int ULSCPP_DFL_CSZ_BUFFSIZE = 128;

			csz_str_t *auwstr_buf;
			int siz_auwstr_buf;

		public:
			static const int CVT_MBSTR_USTR = 0;

			// <brief>
			// This is a constructor of UlsAuw.
			// </brief>
			UlsAuw(int size=-1);

			// <brief>
			// The destructor of UlsAuw.
			// </brief>
			virtual ~UlsAuw();

			// <brief>
			// converters among ANSI-encoding, UTF-8, and wide string
			// </brief>
			char *wstr2mbstr(const wchar_t *wstr, int mode, int slot_no);
			wchar_t *mbstr2wstr(const char *mbstr, int mode, int slot_no);

			int get_slot_len(int slot_no);
		};

		class ULSCPP_DLL_EXTERN ArgListW {
			wchar_t **wargs;
			int n_wargs;

		public:
			// <brief>
			// This is a constructor of ArgListW
			// </brief>
			ArgListW();

			// <brief>
			// The destructor of ArgListW
			// </brief>
			virtual ~ArgListW();

			// <brief>
			// convert the list of string to wide strings.
			// </brief>
			// <parm name="args">list of string</parm>
			// <parm name="n_args"># of strings in the list</parm>
			// <return>list of wide-char string</return>
			bool setWArgList(char **args, int n_args);

			// <brief>
			// return i-th args as wide-string
			// </brief>
			// <parm name="i">i-th requested</parm>
			// <return>wide-string as a pointer</return>
			wchar_t* getWArg(int i);

			// <brief>
			// clear the internal list.
			// </brief>
			void reset();

			wchar_t **exportWArgs(int *ptr_n_args);
		};
	}

	ULSCPP_DLL_EXTERN void memcopy(void *dst, const void *src, int n);

	ULSCPP_DLL_EXTERN int strLength(const char *str);
	ULSCPP_DLL_EXTERN int strLength(const wchar_t *wstr);

	ULSCPP_DLL_EXTERN int strFindIndex(const char *line, char ch0);
	ULSCPP_DLL_EXTERN int strFindIndex(const wchar_t* wline, wchar_t wch0);

	ULSCPP_DLL_EXTERN int direntExist(const char *fpath);
	ULSCPP_DLL_EXTERN int direntExist(const wchar_t *wfpath);

	ULSCPP_DLL_EXTERN FILE *fileOpenReadolnly(const char *fpath);
	ULSCPP_DLL_EXTERN FILE *fileOpenReadolnly(const wchar_t *wfpath);

	// <brief>
	// convert command args
	// </brief>
	ULSCPP_DLL_EXTERN wchar_t **getWargList(char **argv, int n_argv);
	ULSCPP_DLL_EXTERN void putWargList(wchar_t **wargv, int n_wargv);

	// <brief>
	// parsing command args
	// </brief>
	typedef int (*optproc_t)(int opt, char* optarg);
	ULSCPP_DLL_EXTERN int parseCommandOptions(int n_args, char *args[], const char *optfmt, optproc_t proc);

	typedef int (*woptproc_t)(int opt, wchar_t* optarg);
	ULSCPP_DLL_EXTERN int parseCommandOptions(int n_args, wchar_t *args[], const wchar_t *optfmt, woptproc_t wproc);

}
