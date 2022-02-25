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

namespace uls {
	namespace crux {
		static const int ULSCPP_NUM_CSZ_BUFFS = 2;

		// <brief>
		// the converters among ANSI-encoding, UTF8, Wide-strings
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
			UlsAuw(int size=-1);

			// <brief>
			// The destructor of UlsAuw.
			// </brief>
			virtual ~UlsAuw();

			// <brief>
			// converters among ANSI-encoding, UTF-8, and wide string
			// </brief>
			char *mbstr2mbstr(const char *mbstr, int mode, int slot_no);
			char *wstr2mbstr(const wchar_t *wstr, int mode, int slot_no);
			wchar_t *mbstr2wstr(const char *mbstr, int mode, int slot_no);

			int get_slot_len(int slot_no);
		};
	}
}
