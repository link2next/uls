/**
 * This file automatically has been generated by ULS and
 *  contains an implementation of lexical analyzer
 *  for parsing the programming language 'html5'.
 *
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
#pragma once

#include <uls/UlsLex.h>
#include <string>

namespace uls {
	namespace collection {
		class Html5LexBasis : public uls::crux::UlsLex {
		  public:
			static const int              ERR = -8;
			static const int             NONE = -7;
			static const int             LINK = -6;
			static const int             TMPL = -5;
			static const int          LINENUM = -4;
			static const int              NUM = -3;
			static const int               ID = -2;
			static const int          ENDFILE = -1;

			static const int              EOI = 0;

			static const int         TAGBEGIN = 4;
			static const int           TAGEND = 5;
			static const int             TEXT = 6;
			static const int           DQUOTE = 34;
			static const int           SQUOTE = 39;

			Html5LexBasis(std::tstring &ulc_fpath)
				 : uls::crux::UlsLex(ulc_fpath) {}
		};
	}
}