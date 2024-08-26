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

void
StringBuilder::append(int n)
{
	m_stream << n;
	sync = false;
}

EngLex::EngLex(tstring& config_name)
	: EngLexBasis(config_name)
{
	tstring nil_str = _T("");

	tok_str = _T("");
	tok_id = NONE;

	tok_ungot = false;
}

EngLex::~EngLex()
{
	tstring nil_str = _T("");

	tok_str = _T("");
	tok_id = NONE;
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
EngLex::set_input_file(tstring& fpath)
{
	pushFile(fpath);

	tok_str = _T("");
	tok_id = NONE;

	return 0;
}

void
EngLex::expect_number(void)
{
	int num;
	tstring lxm;

	expect(NUM);
	EngLexBasis::getLexeme(lxm);
	num = LexemeAsInt(lxm);

	tokbuf.append(num);
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
	tstring *lxm;
	LPCTSTR ptr;
	uls_wch_t wch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	tokbuf.clear();

	tok = EngLexBasis::getTok();
	UlsLex::getTokStr(&lxm);

	if (tok != NUM) {
		tok_id = tok;
		tok_str = *lxm;
		return;
	}

	expect_number();

	wch = EngLexBasis::peekCh(&is_quote);
	if (isalpha(wch)) {
		tok = EngLexBasis::getTok();

		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();

	 	tok = WORD;
	 	tokbuf.append(ptr, len);
	}

	if (tok == NONE) {
		tok = EngLexBasis::getTok();
		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();
	 	tokbuf.append(ptr, len);
	}

	tok_str = tokbuf.str();
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

void
EngLex::ungetTok(void)
{
	tok_ungot = true;
}
