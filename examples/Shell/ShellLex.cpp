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

ShellLex::ShellLex(string& config_name)
	: ShellLexBasis(config_name)
{
	csz_init(&tokbuf, 128);

	string nil_str = "";
	source(nil_str);
	tok_ungot = false;
}

ShellLex::~ShellLex()
{
	string nil_str = "";
	source(nil_str);

	csz_deinit(&tokbuf);
}

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
ShellLex::source(string& fpath)
{
	if (fpath != "") {
		pushFile(fpath);
	}

	tok_str = "";
	tok_id = NONE;

	return 0;
}

int
ShellLex::expect_number(void)
{
	const char * ptr;
	char  tch;
	int tok, len;
	string *lxm;

	tok = ShellLexBasis::getTok();
	expect(NUM);
	ShellLexBasis::getTokStr(&lxm);

	ptr = lxm->c_str();
	len = (int) lxm->length();

	tch = '0';
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	tch = 'x';
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

	return tok;
}

void
ShellLex::expect_redir(void)
{
	bool is_quote;
	uls_wch_t wch;
	char tch;

	while ((wch=ShellLexBasis::getCh(&is_quote)) != ULS_UCH_NONE && !is_quote) {
		if (isspace(wch)) {
			ShellLexBasis::ungetCh(wch);
			break;
		}

		tch = (char) wch;
		csz_append(&tokbuf, (const char *) &tch, sizeof(char));
	}

	tok_str = string(csz_text(&tokbuf));
}

#define SHELL_SPECIAL_CHARS "$+-_./=\\!*?~"

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>token id</return>
void
ShellLex::get_token(void)
{
	string *lxm;
	int tok, len;
	bool is_quote;
	uls_wch_t wch;
	const char * ptr;
	char tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	skipBlanks();

	if ((wch=ShellLexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
		tok_id = tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);
		tok_str = *lxm;
		return;
	}

	csz_reset(&tokbuf);

	if (wch == '-') { // Is '-' the minus sign?
		ShellLexBasis::getCh(NULL);

		tch = '-';
		csz_append(&tokbuf, (const char *) &tch, sizeof(char));

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

	} else if (isalpha(wch) || wch == '_') {
		// keyword?
		tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);

		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

		wch = ShellLexBasis::peekCh(&is_quote);

	} else if (wch == '&') {
		ShellLexBasis::getCh(NULL);

		tch = '&';
		csz_append(&tokbuf, (const char *) &tch, sizeof(char));

		tok = '&';
		wch = ShellLexBasis::peekCh(&is_quote);

	} else {
		tok = NONE;
	}

	if (is_quote) {
		ShellLexBasis::getTok();
		if (tok == NONE) tok = ShellLexBasis::getTokNum();
		tok_str = string(csz_text(&tokbuf));
		tok_id = tok;
		return;
	}

	if (wch == '<' || wch == '>') {
		expect_redir();
		tok_id = tok = REDIRECT;
		return;
	}

	if (isalnum(wch) || tcschr("'\"`" SHELL_SPECIAL_CHARS, (char) wch) != NULL) {
		// 'Shell-Script word' is to be processed, ...
		do {
			if (isdigit(wch)) {
				expect_number();

			} else if (isalpha(wch)) {
				ShellLexBasis::getTok();
				ShellLexBasis::getTokStr(&lxm);

				ptr = (const char *) lxm->c_str();
			 	len = (int) lxm->length();
			 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

			} else if (wch == '"' || wch == '\'' || wch == '`') {
	 			ShellLexBasis::getTok();
				ShellLexBasis::getTokStr(&lxm);

	 			ptr = (const char *) lxm->c_str();
	 			len = (int) lxm->length();

				tch = (char) wch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	 			csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

	 			tch = (char) wch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	 		} else {
	 			if (tcschr(SHELL_SPECIAL_CHARS "@#(){}", (char) wch) == NULL) break;
				ShellLexBasis::getCh(NULL);

				tch = (char) wch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));
			}
	 	} while ((wch=ShellLexBasis::peekCh(&is_quote)) != ULS_UCH_NONE);

		tok = WORD;
	}

	if (tok == NONE) {
		tok = ShellLexBasis::getTok();
		ShellLexBasis::getTokStr(&lxm);

		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));
	}

	tok_str = string(csz_text(&tokbuf));
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

std::string&
ShellLex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// This is a virtual method, inherited from 'UlsLex' class.
// But we don't need this method yet to process 'Shell-Script' files.
// </brief>
string ShellLex::getKeywordStr(int t)
{
	return string("<unknown>");
}

void
ShellLex::ungetTok(void)
{
	tok_ungot = true;
}

const char *
ShellLex::tcschr(const char * tstr, char tch)
{
	int i;

	for (i=0; tstr[i] != '\0'; i++) {
		if (tstr[i] == tch) {
			return tstr + i;
		}
	}

	return NULL;
}
