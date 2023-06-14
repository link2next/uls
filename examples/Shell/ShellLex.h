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
  <file> ShellLex.cpp </file>
  <brief>
    Tokenize and dump the shell file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#pragma once

#include "ShellLexBasis.h"

#include <string>

#include <uls/csz_stream.h>

namespace uls
{
namespace collection
{
	class ShellLex : public uls::collection::ShellLexBasis {
		csz_str_t tokbuf;

		std::string tok_str;
		int tok_id;
		bool tok_ungot;

		const char * tcschr(const char * tstr, char tch);
		int expect_number(void);
		void expect_redir(void);
		void get_token(void);

	public:

		ShellLex(std::string& config_name);
		~ShellLex();

		// <brief>
		// These are virtual methods inherited from 'UlsLex' class and
		//    declared here to implement.
		// Those we need to tokenize 'Makefile' files are just getTok(), token(), lexeme().
		// </brief>
		virtual int getTok(void);

		virtual int getTokNum(void);
		virtual std::string& getTokStr(void);

		void ungetTok(void);
		std::string getKeywordStr(int t);

		// <brief>
		// Sets the input file to be tokenized.
		// </brief>
		// <parm name="fpath">The path of file</parm>
		// <return>none</return>
		int source(std::string& fpath);

	};
}
}
