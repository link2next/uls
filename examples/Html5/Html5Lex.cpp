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

#include "Html5Lex.h"
#include <iostream>

#include <uls/UlsUtils.h>
#include <uls/uls_fileio.h>

using namespace std;
using namespace uls::crux;
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
// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
Html5Lex::setFile(tstring fpath)
{
	FILE *fp;

	if (fpath != _T("")) {
		if ((fp = uls::fileOpenReadolnly(fpath.c_str())) == NULL) {
			_tcerr << fpath << _T(": file open error") << _tendl;
			return -1;
		}
	} else {
		fp = NULL;
	}

	fin_html = fp;
	prepare_html_text = 1;

	tok_id = NONE;
	tok_str = _T("");

	return 0;
}

Html5Lex::Html5Lex(tstring& config_name)
	: Html5LexBasis(config_name)
{
	setFile(_T(""));
	tok_ungot = false;
}

Html5Lex::~Html5Lex()
{
	if (fin_html != NULL) {
		fclose(fin_html);
		fin_html = NULL;
	}
}

// <brief>
// This trys to find a HTML-element searching for char '<'.
// The text from '>' to '<' is an candidate for HTML-TEXT.
// If it's empty, sets *is_trivial to 1.
// </brief>
// <parm name="fp">The FILE pointer of input file</parm>
// <parm name="txt">the text found is stored in this</parm>
// <parm name="is_trivial">check whether the found text is empty or not</parm>
// <return>'<' if it's found, otherwise EOF</return>
int
Html5Lex::run_to_tagbegin(FILE* fp, int *is_trivial)
{
	int escape = 0;
	int ch, bTrivial = 1;
	TCHAR tch;

	for ( ; (ch = fgetc(fp)) != EOF; ) {
		if (escape) {
			tch = (TCHAR) ch;
			txtbuf.append(tch);
			escape = 0;

		} else if (ch == '<') {
			break;

		} else if (ch == '\\') {
			escape = 1;

			tch = '\\';
			txtbuf.append(tch);
			bTrivial = 0;
		}

		if (ch != '\n') {
			tch = (TCHAR) ch;
			txtbuf.append(tch);
			if (ch != ' ' && ch != '\t') bTrivial = 0;
		}
	}

	tch = _T('\0');
	txtbuf.append(tch);

	*is_trivial = bTrivial;

	return ch;
}

// <brief>
// This processs the quotation strings in HTML-element.
// The found strings is stored in 'txt'.
// </brief>
// <parm name="fp">The FILE pointer of input file</parm>
// <parm name="txt">the text found is stored in this</parm>
// <parm name="quote_ch">The single or double quotation mark</parm>
// <return>0 if success, ohterwise -1</return>
int
Html5Lex::pass_html_quote(FILE* fp, int quote_ch)
{
	int   ch, stat = 0, escape = 0;
	TCHAR tch;

	for ( ; ; ) {
		if ((ch = fgetc(fp)) == EOF) {
			_tcerr << _T("pass_html_quote: unexpected EOF") << _tendl;
			stat = -1; break;
		}

		if (escape) {
			tch = (TCHAR) ch;
			txtbuf.append(tch);
			escape = 0;

		} else {
			if (ch == quote_ch) {
				tch = (TCHAR) ch;
				txtbuf.append(tch);
				break;

			} else if (ch=='\\') {
				tch = '\\';
				txtbuf.append(tch);
				escape = 1;

			} else {
				tch = (TCHAR) ch;
				txtbuf.append(tch);
			}
		}
	}

	return stat;
}

// <brief>
// This trys to find the end HTML-element searching for char '>'.
// The text from '<' to '>' is to be tokenized.
// If it's empty, sets *is_trivial to 1.
// </brief>
// <parm name="fp">The FILE pointer of input file</parm>
// <parm name="txt">The found html-element is stored in this</parm>
// <return>0, 1 if success, ohterwise -1</return>
int
Html5Lex::run_to_tagend(FILE* fp)
{
	int stat = 1;
	int ch;
	TCHAR tch;

	for ( ; ; ) {
		if ((ch = fgetc(fp)) == EOF) {
			_tcerr << _T("Unexpected terminating file!") << _tendl;
			stat = -1;
			break;
		}

		if (ch == '\'' || ch == '"') {
			tch = (TCHAR) ch;
			txtbuf.append(tch);

			if (pass_html_quote(fp, ch) < 0) {
				_tcerr << _T("unexpected html element!") << _tendl;
				stat = -1;
				break;
			}
		} else {
			tch = (TCHAR) ch;
			txtbuf.append(tch);

			if (ch == '>') {
				stat = 0;
				break;
			}
		}
	}

	tch = '\0';
	txtbuf.append(tch);

	return stat;
}

// <brief>
// concatenate the lexeme of the next token to the 'str'.
// The length of 'str' is 'len'.
// </brief>
// <parm name="str">A string to be concatenated</parm>
// <parm name="len">The length of 'str'</parm>
// <return>the length of the concatenated string</return>
int
Html5Lex::concat_lexeme(LPCTSTR str, int len)
{
	Html5LexBasis::getTok();
	tstring *lxm;

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
Html5Lex::get_token(void)
{
	int ich;
	int tok, is_trivial;
	tstring *lxm;
	uls_wch_t wch;
	TCHAR tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	if (prepare_html_text < 0) {
		tok_id = EOI;
		tok_str = _T("");
		return;
	}

	if (prepare_html_text) {
again_1:
		txtbuf.clear();

		// From here, to '<' may be a html text.
		if ((ich=run_to_tagbegin(fin_html, &is_trivial)) == EOF) {
			// The text from here to EOF is just space.
			prepare_html_text = -1;
		} else {
			prepare_html_text = 0;
		}

		tok_id = TEXT;
		if (txtbuf.len() > 0 && !is_trivial) {
			// Non-trivial text exists.
			tok_str = txtbuf.str();
			tok_id = TEXT;
			return;
		}

		if (ich == EOF) {
			tok_id = EOI;
			tok_str = _T("");
			return;
		}
	}

	if (tok_id == TEXT) {
		// If the current token was HTML-TEXT, ...
		txtbuf.clear();

		tch = _T('<');
		txtbuf.append(tch);

		if (run_to_tagend(fin_html) < 0) {
			tok_id = ERR;
			tok_str = _T("");
			return;
		}

		tstring str1 = txtbuf.str();
		pushLine(str1.c_str(), str1.length());

		if ((tok = Html5LexBasis::getTok()) != '<') {
			goto again_1;
		}

		tok = Html5LexBasis::getTok();

		if (tok == _T('/')) { // The end mark of HTML-Element '/>' detected.
			tok_id = TAGEND;
			tok = Html5LexBasis::getTok();
			UlsLex::getTokStr(&lxm);
			tok_str = *lxm;

		} else {
			tok_id = TAGBEGIN;
			if (tok == _T('!')) { // '<!'
				if ((wch = Html5LexBasis::peekCh(NULL)) == _T('-')) {
					wch = Html5LexBasis::getCh(NULL);
					if ((wch = Html5LexBasis::peekCh(NULL)) == _T('-')) {
						// '<--'
						// skip html comment
						goto again_1;
					} else {
						tok_id = ERR;
						tok_str = _T("");
						return;
					}
				} else {
					concat_lexeme(_T("!"), 1);
					tok = Html5LexBasis::getTokNum();
					tok_str = tokbuf.str();
				}
			} else {
				UlsLex::getTokStr(&lxm);
				tok_str = *lxm;
			}
		}

		if (tok != ID) {
			tok_id = ERR;
			tok_str = _T("");
			prepare_html_text = -2;
			return;
		}

		return;
	}

	tok = Html5LexBasis::getTok();
	UlsLex::getTokStr(&lxm);
	tok_str = *lxm;

	if (tok == '-') {
		if ((wch=Html5LexBasis::peekCh(NULL)) == _T('.') || isdigit(wch)) {
			tok = NUM;
			concat_lexeme(_T("-"), 1);
			tok_id = tok;
			tok_str = txtbuf.str();
			return;
		}
	} else if (tok == _T('/') && Html5LexBasis::peekCh(NULL) == _T('>')) {
		tok = TAGEND;
		tok_str = _T("");
	} else if (tok == _T('>')) {
		goto again_1;
	}

	tok_id = tok;
}

int
Html5Lex::getTok(void)
{
	get_token();
	Html5LexBasis::setTok(tok_id, tok_str);

	return tok_id;
}

int
Html5Lex::getTokNum(void)
{
	return tok_id;
}

void
Html5Lex::ungetTok(void)
{
	tok_ungot = true;
}
