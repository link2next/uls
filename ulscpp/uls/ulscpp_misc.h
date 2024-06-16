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
  <file> ulscpp_misc.h </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April, 2024.
  </author>
*/

#pragma once

#include <uls/uls_config.h>

#include <string>
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
#ifdef ULS_WINDOWS
#ifdef ULS_USE_WSTR
	#define _tmain wmain
#else
	#define _tmain main
#endif
#endif // ULS_WINDOWS
#endif // ULS_DONT_TSTR
}
