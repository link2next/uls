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
  <file> Html5Lex.cpp </file>
  <brief>
    Tokenize and dump the html5 file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#pragma once

#include "Html5LexBasis.h"

#include <string>
#include <sstream>

namespace uls
{
namespace collection
{
	class StringBuilder {
		std::otstringstream m_stream;
		std::tstring m_sbuff;

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

		std::tstring& str();
	};

	class Html5Lex : public uls::collection::Html5LexBasis {
		StringBuilder tokbuf;
		StringBuilder txtbuf;
		int prepare_html_text;

		FILE *fin_html;

		std::tstring tok_str;
		int tok_id;
		bool tok_ungot;

		int run_to_tagbegin(FILE* fp, int *is_trivial);
		int pass_html_quote(FILE* fp, int quote_ch);
		int run_to_tagend(FILE* fp);
		int concat_lexeme(LPCTSTR str, int len);
		void get_token(void);

	public:
		Html5Lex(std::tstring& config_name);
		~Html5Lex();

		// <brief>
		// These are virtual methods inherited from 'UlsLex' class and
		//    declared here to implement.
		// Those we need to tokenize HTML5 files are just getTok(), TokNum(), TokStr().
		// </brief>
		virtual int getTok(void);
		virtual int getTokNum(void);

		void ungetTok(void);

		// <brief>
		// Sets the input file to be tokenized.
		// </brief>
		// <parm name="fpath">The path of file</parm>
		// <return>none</return>
		int setFile(std::tstring fpath);
	};
}
}
