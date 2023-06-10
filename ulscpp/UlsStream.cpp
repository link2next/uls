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
  <file> UlsStream.cpp </file>
  <brief>
 	This Implemented the wrapper class of ulslex_t.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/UlsStream.h"
#include "uls/UlsUtils.h"

#include <uls/uls_lex.h>
#include <uls/uls_ostream.h>
#include <uls/uls_auw.h>

#include <string>

using namespace std;

// <brief>
// A constructor which accepts UlsLex as first parameter.
// 'UlsLex' is the the class for lexical analysis defined in the same namespace 'ULS'.
// </brief>
// <parm name="ulslex">An ULS lexical analyzer</parm>
// <return>none</return>
uls::crux::UlsStream::UlsStream()
{
	auwcvt = new UlsAuw();

	WANT_EOFTOK = _uls_const_WANT_EOFTOK();
	LINE_NUMBERING = _uls_const_LINE_NUMBERING();
	STREAM_BIN_LE = _uls_const_STREAM_BIN_LE();
	STREAM_BIN_BE = _uls_const_STREAM_BIN_BE();
	read_only = true;
}

// <brief>
// The destuctor of UlsStream.
// </brief>
// <return>none</return>
uls::crux::UlsStream::~UlsStream()
{
	delete auwcvt;
}
