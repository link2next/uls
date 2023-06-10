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

#include <string.h>
#include <ctype.h>

using namespace std;
using namespace uls::collection;

ShellLex::ShellLex(tstring& config_name)
	: ShellLexBasis(config_name)
{
	csz_init(&tokbuf, 128);

	tstring nil_str = _T("");
	source(nil_str);
	tok_ungot = false;
}

ShellLex::~ShellLex()
{
	tstring nil_str = _T("");
	source(nil_str);

	csz_deinit(&tokbuf);
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
	TCHAR  tch;
	int tok, len;
	tstring *lxm;

	tok = ShellLexBasis::getTok();
	expect(NUM);
	ShellLexBasis::getTokStr(&lxm);

	ptr = lxm->c_str();
	len = (int) lxm->length();

	tch = _T('0');
	csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

	tch = _T('x');
	csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

	csz_append(&tokbuf, (const char *) ptr, len * sizeof(TCHAR));

	return tok;
}

void
ShellLex::expect_redir(void)
{
	bool is_quote;
	uls_uch_t uch;
	TCHAR tch;

	while ((uch=ShellLexBasis::getCh(&is_quote)) != ULS_UCH_NONE && !is_quote) {
		if (isspace(uch)) {
			ShellLexBasis::ungetCh(uch);
			break;
		}

		tch = (TCHAR) uch;
		csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));
	}

	tok_str = tstring(uls_get_csz_tstr(&tokbuf));
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
	uls_uch_t uch;
	LPCTSTR ptr;
	TCHAR tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	skipBlanks();

	if ((uch=ShellLexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
		tok_id = tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);
		tok_str = *lxm;
		return;
	}

	csz_reset(&tokbuf);

	if (uch == '-') { // Is '-' the minus sign?
		ShellLexBasis::getCh(NULL);

		tch = _T('-');
		csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

		uch = ShellLexBasis::peekCh(&is_quote);
		if (isdigit(uch)) {
			tok = expect_number();
			// The number-token in shell-script can contain the minus sign.
			uch = ShellLexBasis::peekCh(&is_quote);
		} else {
			tok = WORD;
		}

	} else if (isdigit(uch)) {
		tok = expect_number();
		uch = ShellLexBasis::peekCh(&is_quote);

	} else if (isalpha(uch) || uch == '_') {
		// keyword?
		tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);

		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(TCHAR));

		uch = ShellLexBasis::peekCh(&is_quote);

	} else if (uch == '&') {
		ShellLexBasis::getCh(NULL);

		tch = _T('&');
		csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

		tok = '&';
		uch = ShellLexBasis::peekCh(&is_quote);

	} else {
		tok = NONE;
	}

	if (is_quote) {
		ShellLexBasis::getTok();
		if (tok == NONE) tok = ShellLexBasis::getTokNum();
		tok_str = tstring(uls_get_csz_tstr(&tokbuf));
		tok_id = tok;
		return;
	}

	if (uch == '<' || uch == '>') {
		expect_redir();
		tok_id = tok = REDIRECT;
		return;
	}

	if (isalnum(uch) || tcschr(_T("'\"`") SHELL_SPECIAL_CHARS, (TCHAR) uch) != NULL) {
		// 'Shell-Script word' is to be processed, ...
		do {
			if (isdigit(uch)) {
				expect_number();

			} else if (isalpha(uch)) {
				ShellLexBasis::getTok();
				ShellLexBasis::getTokStr(&lxm);

				ptr = (LPCTSTR) lxm->c_str();
			 	len = (int) lxm->length();
			 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(TCHAR));

			} else if (uch == '"' || uch == '\'' || uch == '`') {
	 			ShellLexBasis::getTok();
				ShellLexBasis::getTokStr(&lxm);

	 			ptr = (LPCTSTR) lxm->c_str();
	 			len = (int) lxm->length();

				tch = (TCHAR) uch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

	 			csz_append(&tokbuf, (const char *) ptr, len * sizeof(TCHAR));

	 			tch = (TCHAR) uch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));

	 		} else {
	 			if (tcschr(SHELL_SPECIAL_CHARS _T("@#(){}"), (TCHAR) uch) == NULL) break;
				ShellLexBasis::getCh(NULL);

				tch = (TCHAR) uch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(TCHAR));
			}
	 	} while ((uch=ShellLexBasis::peekCh(&is_quote)) != ULS_UCH_NONE);

		tok = WORD;
	}

	if (tok == NONE) {
		tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);

		ptr = (LPCTSTR) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(TCHAR));
	}

	tok_str = tstring(uls_get_csz_tstr(&tokbuf));
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

std::tstring&
ShellLex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// This is a virtual method, inherited from 'UlsLex' class.
// But we don't need this method yet to process 'Shell-Script' files.
// </brief>
tstring ShellLex::getKeywordStr(int t)
{
	return tstring(_T("<unknown>"));
}

void
ShellLex::ungetTok(void)
{
	tok_ungot = true;
}

LPCTSTR
ShellLex::tcschr(LPCTSTR tstr, TCHAR tch)
{
	int i;

	for (i=0; tstr[i] != _T('\0'); i++) {
		if (tstr[i] == tch) {
			return tstr + i;
		}
	}

	return NULL;
}
