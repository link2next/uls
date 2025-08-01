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
  <file> Css3Lex.cpp </file>
  <brief>
    Tokenize and dump the css3 file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#pragma once

#include "Css3LexBasis.h"

namespace uls
{
namespace collection
{
	class StringBuilder {
		uls::otstringstream m_stream;
		uls::tstring m_sbuff;

		TCHAR *mBuff;
		int siz_mBuff;
		bool sync;

	public:
		StringBuilder();
		virtual ~StringBuilder();

		void clear();
		int len();
		void append(LPCTSTR str, int len = -1);
		void append(TCHAR ch);

		uls::tstring& str();
	};

	class Css3Lex : public uls::collection::Css3LexBasis {
		StringBuilder tokbuf;
		uls::tstring tok_str;
		int tok_id;
		bool tok_ungot;

		int prepare_url_tok;

		int concat_lexeme(LPCTSTR str, int len, int tok);
		void get_token(void);

	public:
		Css3Lex(uls::tstring& config_name);

		// <brief>
		// These are virtual methods inherited from 'UlsLex' class and
		//    declared here to implement.
		// Those we need to tokenize CSS3 files are just next(), getTokNum(), getTokStr().
		// </brief>
		virtual int next(void) override;
		virtual int getTokNum(void) override;
		virtual void getTokStr(uls::tstring& lxm) override;

		void ungetCurrentToken(void);

		// <brief>
		// Sets the input file to be tokenized.
		// </brief>
		// <parm name="fpath">The path of file</parm>
		// <return>none</return>
		void setInputFile(uls::tstring fpath);
	};
}
}
