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
  <file> MkfLex.cpp </file>
  <brief>
    Tokenize and dump the 'Makefile' file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#pragma once

#include "MkfLexBasis.h"

#include <string>

#include <uls/csz_stream.h>

namespace uls
{
namespace collection
{
	struct mkf_tabblk_ctx_t {
		int prev_ch;
		int prefix_tabs;
		int prepare_word;
	};

	class MkfLex : public uls::collection::MkfLexBasis {
		csz_str_t tokbuf;

		std::tstring tok_str;
		int tok_id;
		bool tok_ungot;

		static int tabblk_analyzer(uls_litstr_t *lit);

		LPCTSTR tcschr(LPCTSTR tstr, TCHAR tch);
		int expect_quotestr(char ch);
		int expect_word(void);
		int expect_number(void);
		void get_token(void);

	public:
		mkf_tabblk_ctx_t tabblk_ctx;

		MkfLex(std::tstring& config_name);
		~MkfLex();

		// <brief>
		// These are virtual methods inherited from 'UlsLex' class and
		//    declared here to implement.
		// Those we need to tokenize 'Makefile' files are just getTok(), TokNum(), TokStr().
		// </brief>
		virtual int getTok(void);

		virtual int getTokNum(void);
		virtual std::tstring& getTokStr(void);

		void ungetTok(void);
		std::tstring getKeywordStr(int t);

		// <brief>
		// Sets the input file to be tokenized.
		// </brief>
		// <parm name="fpath">The path of file</parm>
		// <return>none</return>
		int include(std::tstring& fpath);
	};
}
}
