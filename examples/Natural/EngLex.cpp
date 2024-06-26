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
  <file> EngLex.cpp </file>
  <brief>
    Tokenize and dump English sentense.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Nov. 2017
  </author>
*/

#include "EngLex.h"

#include <string.h>
#include <ctype.h>

using namespace std;
using namespace uls::crux;
using namespace uls::collection;

EngLex::EngLex(string& config_name)
	: EngLexBasis(config_name)
{
	csz_init(&tokbuf, 128);

	string nil_str = "";

	tok_str = "";
	tok_id = NONE;

	tok_ungot = false;
}

EngLex::~EngLex()
{
	string nil_str = "";

	tok_str = "";
	tok_id = NONE;

	csz_deinit(&tokbuf);
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
EngLex::set_input_file(string& fpath)
{
	pushFile(fpath);

	tok_str = "";
	tok_id = NONE;

	return 0;
}

void
EngLex::expect_number(void)
{
	unsigned int num;
	char num_buf[32];
	char ch;
	const char * ptr;
	int len, i;
	string lxm;

	expect(NUM);
	EngLexBasis::getLexeme(lxm);
	num = LexemeAsInt(lxm);
	len = uls_zprintf(&tokbuf, "%u", num);
}

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>token id</return>
void
EngLex::get_token(void)
{
	int tok, len;
	bool is_quote;
	const char * ptr;
	uls_wch_t wch;
	string *lxm;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	csz_reset(&tokbuf);

	tok = EngLexBasis::getTok();
	EngLexBasis::getTokStr(&lxm);

	if (tok != NUM) {
		tok_id = tok;
		tok_str = *lxm;
		return;
	}

	expect_number();

	wch = EngLexBasis::peekCh(&is_quote);
	if (isalpha(wch)) {
		tok = EngLexBasis::getTok();

		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();

	 	tok = WORD;
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));
	}

	if (tok == NONE) {
		tok = EngLexBasis::getTok();
		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));
	}

	tok_str = string(csz_text(&tokbuf));
	tok_id = tok;
}

int
EngLex::getTok(void)
{
	get_token();
	EngLexBasis::setTok(tok_id, tok_str);
	return tok_id;
}

int
EngLex::getTokNum(void)
{
	return tok_id;
}

std::string&
EngLex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// </brief>
string EngLex::getKeywordStr(int t)
{
	return string("<unknown>");
}

void
EngLex::ungetTok(void)
{
	tok_ungot = true;
}
