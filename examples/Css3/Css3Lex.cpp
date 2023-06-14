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

#include "Css3Lex.h"

using namespace std;
using namespace uls::collection;

Css3Lex::Css3Lex(string& config_name)
	: Css3LexBasis(config_name)
{
	csz_init(&tokbuf, 128);
	prepare_url_tok = 0;

	tok_id = CSS_NONE;
	tok_str = "";
	tok_ungot = false;
}

Css3Lex::~Css3Lex()
{
	csz_deinit(&tokbuf);
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>none</return>
void
Css3Lex::setFile(string fpath)
{
	pushFile(fpath);
}

// <brief>
// concatenate the lexeme of the next token to the 'str'.
// The length of 'str' is 'len' and 'tok' is the current.
// </brief>
// <parm name="str">A string to be concatenated</parm>
// <parm name="len">The length of 'str'</parm>
// <parm name="tok">The current token id</parm>
// <return>the length of the result string</return>
int
Css3Lex::concat_lexeme(const char * str, int len, int tok)
{
	Css3LexBasis::getTok();
	string *lxm;

	Css3LexBasis::getTokStr(&lxm);

	csz_reset(&tokbuf);
	csz_append(&tokbuf, (const char *) str, len * sizeof(char));
	csz_append(&tokbuf, (const char *) lxm->c_str(), (int) lxm->length() * sizeof(char));

	return csz_length(&tokbuf);
}

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>token id</return>
void
Css3Lex::get_token(void)
{
	uls_uch_t uch;
	int tok, k;
	int paren_lvl;
	bool is_quote;
	string *lxm;
	char tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	if (prepare_url_tok == 2) {
		if ((uch=Css3LexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
			if (is_quote) {
				tok = Css3LexBasis::getTok();
				Css3LexBasis::getTokStr(&lxm);
				tok_str = *lxm;
				tok_id = CSS_PATH;
				prepare_url_tok = 0;
			} else {
				tok_str = "";
				tok_id = CSS_ERR;
			}
			return;
		}

		// url(../../image/a.png)
		k = paren_lvl = 0;
		csz_reset(&tokbuf);
		while ((uch=Css3LexBasis::peekCh(&is_quote)) != ')' || paren_lvl > 0) {
			Css3LexBasis::getCh(&is_quote);
			if (uch == ULS_UCH_NONE) {
				break;
			}

			tch = (char) uch;
			csz_append(&tokbuf, (const char *) &tch, sizeof(char));

			if (uch == '(') ++paren_lvl;
			else if (uch == ')') --paren_lvl;
		}

		k = csz_length(&tokbuf);
		tok_str = string(csz_text(&tokbuf));
		tok_id = CSS_PATH;

		prepare_url_tok = 0;

		return;
	}

	tok = Css3LexBasis::getTok();

	if (prepare_url_tok == 1) {
		expect('(');
		prepare_url_tok = 2;

	} else if (tok == CSS_URL) {
		prepare_url_tok = 1;

	} else if (Css3LexBasis::getTokNum() == '-') {
		if ((uch = Css3LexBasis::peekCh(&is_quote)) == '.' || isdigit(uch)) {
			tok = CSS_NUM;
			concat_lexeme("-", 1, tok);
			tok_str = string(csz_text(&tokbuf));
			tok_id = CSS_NUM;
			return;

		} else if (is_ch_id(uch)) {
			tok = CSS_ID;
			concat_lexeme("-", 1, tok);
			tok_str = string(csz_text(&tokbuf));
			tok_id = CSS_ID;
			return;
		}
	}

	Css3LexBasis::getTokStr(&lxm);

	tok_str = *lxm;
	tok_id = Css3LexBasis::getTokNum();
}

int
Css3Lex::getTok(void)
{
	get_token();
	Css3LexBasis::setTok(tok_id, tok_str);
	return tok_id;
}

int
Css3Lex::getTokNum(void)
{
	return tok_id;
}

std::string&
Css3Lex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// This is a virtual method, inherited from 'UlsLex' class.
// We don't need this method yet to process CSS3 files.
// </brief>
// <return></return>
string Css3Lex::getKeywordStr(int t)
{
	return string("<unknown>");
}

void
Css3Lex::ungetTok(void)
{
	tok_ungot = true;
}
