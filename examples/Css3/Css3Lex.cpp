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
#include <uls/UlsUtils.h>

using namespace std;
using namespace uls::crux;
using namespace uls::collection;
using tstring = uls::tstring;

StringBuilder::StringBuilder()
{
	siz_mBuff = 32;
	mBuff = (LPTSTR) malloc(siz_mBuff * sizeof(TCHAR));
	sync = true;
}

StringBuilder::~StringBuilder()
{
	if (siz_mBuff > 0) {
		free(mBuff);
		mBuff = NULL;
		siz_mBuff = 0;
	}
}

int
StringBuilder::len()
{
	return (int) m_stream.tellp();
}

void
StringBuilder::clear()
{
	m_stream.str(_T(""));
	m_stream.clear();
	m_sbuff = _T("");
	sync = true;
}

tstring&
StringBuilder::str()
{
	if (sync == false) {
		m_sbuff = m_stream.str();
		sync = true;
	}

	return m_sbuff;
}

void
StringBuilder::append(LPCTSTR str, int len)
{
	if (len < 0) {
		len = uls::strLength(str);
		m_stream << str;
	} else if (len > 0) {
		if (len >= siz_mBuff) {
			siz_mBuff = len + 1;
			mBuff = (LPTSTR) realloc(mBuff, siz_mBuff * sizeof(TCHAR));
		}
		uls::memcopy(mBuff, str, len * sizeof(TCHAR));
		mBuff[len] = _T('\0');
		m_stream << mBuff;
	}
	sync = false;
}

void
StringBuilder::append(TCHAR ch)
{
	TCHAR buff[4];

	buff[0] = ch;
	buff[1] = _T('\0');
	m_stream << buff;
	sync = false;
}

Css3Lex::Css3Lex(tstring& config_name)
	: Css3LexBasis(config_name)
{
	prepare_url_tok = 0;

	tok_id = CSS_NONE;
	tok_str = _T("");
	tok_ungot = false;
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>none</return>
void
Css3Lex::setFile(tstring fpath)
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
Css3Lex::concat_lexeme(LPCTSTR str, int len, int tok)
{
	tstring *lxm;

	Css3LexBasis::getTok();
	UlsLex::getTokStr(&lxm);

	tokbuf.clear();
	tokbuf.append(str, len);
	tokbuf.append(lxm->c_str());

	return tokbuf.len();
}

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>token id</return>
void
Css3Lex::get_token(void)
{
	uls_wch_t wch;
	int tok, paren_lvl;
	bool is_quote;
	tstring *lxm;
	TCHAR tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	if (prepare_url_tok == 2) {
		if ((wch=Css3LexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
			if (is_quote) {
				tok = Css3LexBasis::getTok();
				UlsLex::getTokStr(&lxm);
				tok_str = *lxm;
				tok_id = CSS_PATH;
				prepare_url_tok = 0;
			} else {
				tok_str = _T("");
				tok_id = CSS_ERR;
			}
			return;
		}

		// url(../../image/a.png)
		paren_lvl = 0;
		tokbuf.clear();
		while ((wch=Css3LexBasis::peekCh(&is_quote)) != ')' || paren_lvl > 0) {
			Css3LexBasis::getCh(&is_quote);
			if (wch == ULS_UCH_NONE) {
				break;
			}

			tch = (TCHAR) wch;
			tokbuf.append(tch);

			if (wch == _T('(')) ++paren_lvl;
			else if (wch == _T(')')) --paren_lvl;
		}

		tok_str = tokbuf.str();
		tok_id = CSS_PATH;

		prepare_url_tok = 0;
		return;
	}

	tok = Css3LexBasis::getTok();

	if (prepare_url_tok == 1) {
		expect(_T('('));
		prepare_url_tok = 2;

	} else if (tok == CSS_URL) {
		prepare_url_tok = 1;

	} else if (Css3LexBasis::getTokNum() == _T('-')) {
		if ((wch = Css3LexBasis::peekCh(&is_quote)) == _T('.') || isdigit(wch)) {
			tok = CSS_NUM;
			concat_lexeme(_T("-"), 1, tok);
			tok_str = tokbuf.str();
			tok_id = CSS_NUM;
			return;

		} else if (isalpha(wch)) {
			tok = CSS_ID;
			concat_lexeme(_T("-"), 1, tok);
			tok_str = tokbuf.str();
			tok_id = CSS_ID;
			return;
		}
	}

	UlsLex::getTokStr(&lxm);
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

void
Css3Lex::ungetTok(void)
{
	tok_ungot = true;
}
