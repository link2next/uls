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
  <file> UlsAuw.h </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2018.
  </author>
*/

#pragma once

#include <uls/uls_auw.h>
#include <uls/csz_stream.h>
#include <sstream>

namespace uls {
#ifdef ULS_USE_WSTR
	typedef std::wstring tstring;
	typedef std::wostringstream otstringstream;
#else
	typedef std::string tstring;
	typedef std::ostringstream otstringstream;
#endif

#ifndef ULS_DONT_TSTR
#ifdef ULS_USE_WSTR
	#define _tcout std::wcout
	#define _tcerr std::wcerr
	#define _tendl L"\n"
#else
	#define _tcout std::cout
	#define _tcerr std::cerr
	#define _tendl "\n"
#endif
#ifdef __ULS_WINDOWS__
#ifdef ULS_USE_WSTR
	#define _tmain wmain
#else
	#define _tmain main
#endif
#endif // __ULS_WINDOWS__
#endif // ULS_DONT_TSTR

	static const int ULSCPP_NUM_CSZ_BUFFS = 4;

	// <brief>
	// the converters among ANSI-encoding(ms-mbcs), utf-8, Wide-strings(wchar_t)
	// </brief>
	class ULSCPP_DLL_EXTERN UlsAuw {
		static const int ULSCPP_DFL_CSZ_BUFFSIZE = 128;

		csz_str_t *auwstr_buf;
		int siz_auwstr_buf;

	public:
		static const int CVT_MBSTR_USTR = 0;
		static const int CVT_MBSTR_ASTR = 1;

		// <brief>
		// This is a constructor of UlsAuw.
		// </brief>
		// <parm name="size">the size of buffer to convert input string.
		// It's set to 4 if not specified.
		// </parm>
		UlsAuw(int size=-1);

		// <brief>
		// The destructor of UlsAuw.
		// </brief>
		virtual ~UlsAuw();

		// <brief>
		// converters among ANSI-encoding, UTF-8, and wide string
		// </brief>
		// <parm name="wstr">input wide string pointer</parm>
		// <parm name="mbstr">astr(ANSI) or utf8-string pointer</parm>
		// <parm name="mode">specify the encoding of the input string in case of mbstr2wstr.
		// specify the encoding of the output string in case of wstr2mbstr.
		// </parm>
		// <parm name="slot_no">the index of buffer to be used to convert the input string</parm>
		// <return>
		//  the converted string
		//   It refers the string in the buffer in auwstr_buf[]
		// </return>
		char *wstr2mbstr(const wchar_t *wstr, int mode, int slot_no);
		wchar_t *mbstr2wstr(const char *mbstr, int mode, int slot_no);
		char *mbstr2mbstr(const char *mbstr, int mode, int slot_no);

		// <brief>
		// return the #-bytes in the internal buffer specified by 'slot_no'
		// </brief>
		int get_slot_len(int slot_no);
	};

	// <brief>
	// Obtaining wide-string arguments list from the list of mbcs strings
	// </brief>
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
		// The converted list of wide string is stored internally.
		// </brief>
		// <parm name="args">list of string</parm>
		// <parm name="n_args"># of strings in the list</parm>
		bool setWArgList(char **args, int n_args);

		// <brief>
		// Getting i-th(starting from 0) args as wide-string
		// </brief>
		// <parm name="i">i-th requested</parm>
		// <return>the wide-string as a pointer</return>
		wchar_t *getWArg(int i);

		// <brief>
		// clean-up the list of internal buffers
		// </brief>
		void reset();

		// <brief>
		// return all the list of wide string stored in the internal buffer.
		// </brief>
		wchar_t **exportWArgs(int *ptr_n_args = NULL);
	};

#ifdef ULS_USE_WSTR
typedef ArgListW ArgListT;
#define ULSCPP_GET_WARGS_LIST(al,argc,argv,targv) do { \
	al.setWArgList(argv,argc); \
	targv = al.exportWArgs(); \
	} while (0)
#define ULSCPP_PUT_WARGS_LIST(al)
#else
typedef int ArgListT;
#define ULSCPP_GET_WARGS_LIST(al,argc,argv,targv) do { \
	al = 1; \
	(targv) = (argv); \
	} while (0)
#define ULSCPP_PUT_WARGS_LIST(al) do { \
	al = al - 1; \
	} while (0)
#endif
}
