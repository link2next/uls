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

#include "ShellLex.h"
#include <uls/UlsUtils.h>
#include <iostream>

using namespace std;
using namespace uls::collection;

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

std::tstring&
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

ShellLex::ShellLex(tstring& config_name)
	: ShellLexBasis(config_name)
{
	tstring nil_str = _T("");
	source(nil_str);
	tok_ungot = false;
}

ShellLex::~ShellLex()
{
	tstring nil_str = _T("");
	source(nil_str);
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
ShellLex::source(tstring& fpath)
{
	if (fpath != _T("")) {
		pushFile(fpath);
	}

	tok_str = _T("");
	tok_id = NONE;

	return 0;
}

int
ShellLex::expect_number(void)
{
	LPCTSTR ptr;
	tstring *lxm;
	int tok, len;

	tok = ShellLexBasis::getTok();
	expect(NUM);

	UlsLex::getTokStr(&lxm);
	ptr = lxm->c_str();
	len = (int) lxm->length();

	tokbuf.append(ptr, len);
	return tok;
}

void
ShellLex::expect_redir(void)
{
	bool is_quote;
	uls_wch_t wch;
	TCHAR  tch;

	while ((wch=ShellLexBasis::getCh(&is_quote)) != ULS_UCH_NONE && !is_quote) {
		if (isspace(wch)) {
			ShellLexBasis::ungetCh(wch);
			break;
		}

		tch = (TCHAR) wch;
		tokbuf.append(tch);
	}

	tok_str = tokbuf.str();
}

#define SHELL_SPECIAL_CHARS _T("$+-_./=\\!*?~")

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>token id</return>
void
ShellLex::get_token(void)
{
	tstring *lxm;
	int tok, len;
	bool is_quote;
	uls_wch_t wch;
	LPCTSTR ptr;
	TCHAR  tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	skipBlanks();

	if ((wch=ShellLexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
		tok_id = tok = ShellLexBasis::getTok();
		UlsLex::getTokStr(&lxm);
		tok_str = *lxm;
		return;
	}

	tokbuf.clear();

	if (wch == _T('-')) { // Is '-' the minus sign?
		ShellLexBasis::getCh(NULL);

		tch = _T('-');
		tokbuf.append(tch);

		wch = ShellLexBasis::peekCh(&is_quote);
		if (isdigit(wch)) {
			tok = expect_number();
			// The number-token in shell-script can contain the minus sign.
			wch = ShellLexBasis::peekCh(&is_quote);
		} else {
			tok = WORD;
		}

	} else if (isdigit(wch)) {
		tok = expect_number();
		wch = ShellLexBasis::peekCh(&is_quote);

	} else if (isalpha(wch) || wch == _T('_')) {
		// keyword?
		tok = ShellLexBasis::getTok();
		UlsLex::getTokStr(&lxm);

		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();
	 	tokbuf.append(ptr, len);

		wch = ShellLexBasis::peekCh(&is_quote);

	} else if (wch == _T('&')) {
		ShellLexBasis::getCh(NULL);

		tch = _T('&');
		tokbuf.append(tch);

		tok = _T('&');
		wch = ShellLexBasis::peekCh(&is_quote);

	} else {
		tok = NONE;
	}

	if (is_quote) {
		ShellLexBasis::getTok();
		if (tok == NONE) tok = ShellLexBasis::getTokNum();
		tok_str = tokbuf.str();
		tok_id = tok;
		return;
	}

	if (wch == _T('<') || wch == _T('>')) {
		expect_redir();
		tok_id = tok = REDIRECT;
		return;
	}

	if (isalnum(wch) || uls::strFindIndex(_T("'\"`") SHELL_SPECIAL_CHARS, (TCHAR) wch) >= 0) {
		// 'Shell-Script word' is to be processed, ...
		do {
			if (isdigit(wch)) {
				expect_number();

			} else if (isalpha(wch)) {
				ShellLexBasis::getTok();
				UlsLex::getTokStr(&lxm);

				ptr = (LPCTSTR) lxm->c_str();
			 	len = (int) lxm->length();
			 	tokbuf.append(ptr, len);

			} else if (wch == _T('"') || wch == _T('\'') || wch == _T('`')) {
	 			ShellLexBasis::getTok();
				UlsLex::getTokStr(&lxm);

	 			ptr = (LPCTSTR) lxm->c_str();
	 			len = (int) lxm->length();

				tch = (TCHAR) wch;
				tokbuf.append(tch);

			 	tokbuf.append(ptr, len);

	 			tch = (TCHAR) wch;
				tokbuf.append(tch);

	 		} else {
	 			if (uls::strFindIndex(SHELL_SPECIAL_CHARS _T("@#(){}"), (TCHAR) wch) < 0) break;
				ShellLexBasis::getCh(NULL);

				tch = (TCHAR) wch;
				tokbuf.append(tch);
			}
	 	} while ((wch=ShellLexBasis::peekCh(&is_quote)) != ULS_UCH_NONE);

		tok = WORD;
	}

	if (tok == NONE) {
		tok = ShellLexBasis::getTok();
		UlsLex::getTokStr(&lxm);

		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();
	 	tokbuf.append(ptr, len);
	}

	tok_str = tokbuf.str();
	tok_id = tok;
}

int
ShellLex::getTok(void)
{
	get_token();
	ShellLexBasis::setTok(tok_id, tok_str);
	return tok_id;
}

int
ShellLex::getTokNum(void)
{
	return tok_id;
}

void
ShellLex::ungetTok(void)
{
	tok_ungot = true;
}
