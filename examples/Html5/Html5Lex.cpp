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
#include <uls/uls_fileio.h>

using namespace std;
using namespace uls::collection;

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
Html5Lex::setFile(string fpath)
{
	FILE *fp;

	if (fpath != "") {
		if ((fp=uls_fp_open(fpath.c_str(), ULS_FIO_READ)) == NULL) {
			err_log("%s: file open error", fpath.c_str());
			return -1;
		}
	} else {
		fp = NULL;
	}

	fin_html = fp;
	prepare_html_text = 1;

	tok_id = NONE;
	tok_str = "";

	return 0;
}

Html5Lex::Html5Lex(string& config_name)
	: Html5LexBasis(config_name)
{
	csz_init(&tokbuf, 128);
	csz_init(&txt_buf, 4096);

	string fpath = "";
	setFile(fpath);
	tok_ungot = false;
}

Html5Lex::~Html5Lex()
{
	csz_deinit(&txt_buf);
	csz_deinit(&tokbuf);

	if (fin_html != NULL) {
		uls_fp_close(fin_html);
		fin_html = NULL;
	}
}

// <brief>
// This trys to find a HTML-element searching for char '<'.
// The text from '>' to '<' is an candidate for HTML-TEXT.
// If it's empty, sets *is_trivial to 1.
// </brief>
// <parm name="fp">The FILE pointer of input file</parm>
// <parm name="txt_buf">the text found is stored in this</parm>
// <parm name="is_trivial">check whether the found text is empty or not</parm>
// <return>'<' if it's found, otherwise EOF</return>
int
Html5Lex::run_to_tagbegin(FILE* fp, csz_str_t* txt_buf, int *is_trivial)
{
	int escape = 0;
	int ch, bTrivial = 1;
	char tch;

	for ( ; (ch = uls_fp_getc(fp)) != EOF; ) {
		if (escape) {
			tch = (char) ch;
			csz_append(txt_buf, (const char *) &tch, sizeof(char));
			escape = 0;

		} else if (ch == '<') {
			break;

		} else if (ch == '\\') {
			escape = 1;

			tch = '\\';
			csz_append(txt_buf, (const char *) &tch, sizeof(char));

			bTrivial = 0;
		}

		if (ch != '\n') {
			tch = (char) ch;
			csz_append(txt_buf, (const char *) &tch, sizeof(char));

			if (ch != ' ' && ch != '\t') bTrivial = 0;
		}
	}

	tch = '\0';
	csz_append(txt_buf, (const char *) &tch, sizeof(char));

	*is_trivial = bTrivial;

	return ch;
}

// <brief>
// This processs the quotation strings in HTML-element.
// The found strings is stored in 'txt_buf'.
// </brief>
// <parm name="fp">The FILE pointer of input file</parm>
// <parm name="txt_buf">the text found is stored in this</parm>
// <parm name="quote_ch">The single or double quotation mark</parm>
// <return>0 if success, ohterwise -1</return>
int
Html5Lex::pass_html_quote(FILE* fp, csz_str_t* txt_buf, int quote_ch)
{
	int   ch, stat = 0, escape = 0;
	char tch;

	for ( ; ; ) {
		if ((ch=uls_fp_getc(fp)) == EOF) {
			err_log("%s: unexpected EOF", __FUNCTION__);
			stat = -1; break;
		}

		if (escape) {
			tch = (char) ch;
			csz_append(txt_buf, (const char *) &tch, sizeof(char));
			escape = 0;

		} else {
			if (ch == quote_ch) {
				tch = (char) ch;
				csz_append(txt_buf, (const char *) &tch, sizeof(char));
				break;

			} else if (ch=='\\') {
				tch = '\\';
				csz_append(txt_buf, (const char *) &tch, sizeof(char));
				escape = 1;

			} else {
				tch = (char) ch;
				csz_append(txt_buf, (const char *) &tch, sizeof(char));
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
// <parm name="txt_buf">The found html-element is stored in this</parm>
// <return>0, 1 if success, ohterwise -1</return>
int
Html5Lex::run_to_tagend(FILE* fp, csz_str_t* txt_buf)
{
	int stat = 1;
	int ch;
	char tch;

	for ( ; ; ) {
		if ((ch=uls_fp_getc(fp)) == EOF) {
			err_log("unexpected terminating file!");
			stat = -1;
			break;
		}

		if (ch == '\'' || ch == '"') {
			tch = (char) ch;
			csz_append(txt_buf, (const char *) &tch, sizeof(char));

			if (pass_html_quote(fp, txt_buf, ch) < 0) {
				err_log("unexpected html element!");
				stat = -1;
				break;
			}
		} else {
			tch = (char) ch;
			csz_append(txt_buf, (const char *) &tch, sizeof(char));

			if (ch == '>') {
				stat = 0;
				break;
			}
		}
	}

	tch = '\0';
	csz_append(txt_buf, (const char *) &tch, sizeof(char));

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
Html5Lex::concat_lexeme(const char * str, int len)
{
	Html5LexBasis::getTok();
	string *lxm;

	Html5LexBasis::getTokStr(&lxm);

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
Html5Lex::get_token(void)
{
	int ich;
	int tok, is_trivial;
	string *lxm;
	uls_uch_t uch;
	char tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	if (prepare_html_text < 0) {
		tok_id = EOI;
		tok_str = "";
		return;
	}

	if (prepare_html_text) {
again_1:
		csz_reset(&txt_buf);

		// From here, to '<' may be a html text.
		if ((ich=run_to_tagbegin(fin_html, &txt_buf, &is_trivial)) == EOF) {
			// The text from here to EOF is just space.
			prepare_html_text = -1;
		} else {
			prepare_html_text = 0;
		}

		tok_id = TEXT;
		if (csz_length(&txt_buf) > 0 && !is_trivial) {
			// Non-trivial text exists.
			tok_str = string(csz_text(&txt_buf));
			tok_id = TEXT;
			return;
		}

		if (ich == EOF) {
			tok_id = EOI;
			tok_str = "";
			return;
		}
	}

	if (tok_id == TEXT) {
		// If the current token was HTML-TEXT, ...
		csz_reset(&txt_buf);

		tch = '<';
		csz_append(&txt_buf, (const char *) &tch, sizeof(char));

		if (run_to_tagend(fin_html, &txt_buf) < 0) {
			tok_id = ERR;
			tok_str = "";
			return;
		}

		pushLine(csz_text(&txt_buf), csz_length(&txt_buf));

		if ((tok = Html5LexBasis::getTok()) != '<') {
			goto again_1;
		}

		tok = Html5LexBasis::getTok();

		if (tok == '/') { // The end mark of HTML-Element '/>' detected.
			tok_id = TAGEND;
			tok = Html5LexBasis::getTok();
			Html5LexBasis::getTokStr(&lxm);
			tok_str = *lxm;

		} else {
			tok_id = TAGBEGIN;
			if (tok == '!') { // '<!'
				if ((uch = Html5LexBasis::peekCh(NULL)) == '-') {
					uch = Html5LexBasis::getCh(NULL);
					if ((uch = Html5LexBasis::peekCh(NULL)) == '-') {
						// '<--'
						// skip html comment
						goto again_1;
					} else {
						tok_id = ERR;
						tok_str = "";
						return;
					}
				} else {
					concat_lexeme("!", 1);
					tok = Html5LexBasis::getTokNum();
					tok_str = string(csz_text(&tokbuf));
				}
			} else {
				Html5LexBasis::getTokStr(&lxm);
				tok_str = *lxm;
			}
		}

		if (tok != ID) {
			tok_id = ERR;
			tok_str = "";
			prepare_html_text = -2;
			return;
		}

		return;
	}

	tok = Html5LexBasis::getTok();
	Html5LexBasis::getTokStr(&lxm);
	tok_str = *lxm;

	if (tok == '-') {
		if ((uch=Html5LexBasis::peekCh(NULL)) == '.' || isdigit(uch)) {
			tok = NUM;
			concat_lexeme("-", 1);
			tok_id = tok;
			tok_str = string(csz_text(&txt_buf));
			return;
		}
	} else if (tok == '/' && Html5LexBasis::peekCh(NULL) == '>') {
		tok = TAGEND;
		tok_str = "";
	} else if (tok == '>') {
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

std::string&
Html5Lex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// This is a virtual method, inherited from 'UlsLex' class.
// We don't need this method yet to process HTML5 files.
// </brief>
string Html5Lex::getKeywordStr(int t)
{
	return string("<unknown>");
}

void
Html5Lex::ungetTok(void)
{
	tok_ungot = true;
}
