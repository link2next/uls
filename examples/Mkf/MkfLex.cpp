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
  <file> MkfLex.cpp </file>
  <brief>
    Tokenize and dump the 'Makefile' file specified by argv[].
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#include "MkfLex.h"

#include <string.h>
#include <ctype.h>

using namespace std;
using namespace uls::collection;

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
MkfLex::include(string& fpath)
{
	if (fpath != "") {
		if (pushFile(fpath) == false) {
			return -1;
		}
	}

	tabblk_ctx.prev_ch = -1;
	tabblk_ctx.prepare_word = -1;

	tok_str = "";
	tok_id = NONE;

	return 0;
}

MkfLex::MkfLex(string& config_name)
	: MkfLexBasis(config_name)
{
	string comm_patstr = string("\n\t");
	changeLiteralAnalyzer(comm_patstr, tabblk_analyzer, &tabblk_ctx);
	csz_init(&tokbuf, 128);

	tabblk_ctx.prev_ch = -1;
	tabblk_ctx.prefix_tabs = 1;

	string nil_fpath = "";
	include(nil_fpath);
	tok_ungot = false;
}

MkfLex::~MkfLex()
{
	string nil_fpath = "";
	include(nil_fpath);
	csz_deinit(&tokbuf);
}

// <brief>
// This is the literal analyzer for tab block 'Makefile', the shell-script part of rules.
// </brief>
// <parm name="p_lptr">This is input/output type pararmeter</parm>
// <parm name="lptr_end">You can read from *p_lptr to lptr_end-1</parm>
// <parm name="lit_ctx">An auxiliary data. You can use 'ss_dst'.
//     You should set 'n_lfs' if necessary</parm>
// <return>
//    ULS_LITPROC_ENDOFQUOTE: the right quotation mark was detected.
//    ULS_LITPROC_ERROR: An error occurred.
//</return>
int
MkfLex::tabblk_analyzer(uls_litstr_t *lit)
{
	uls_litstr_context_ptr_t lit_ctx = uls_get_litstr__context(lit);
	uls_quotetype_t *qmt = uls_get_litstr__quoteinfo(lit);
	mkf_tabblk_ctx_t  *mkf_ctx = (mkf_tabblk_ctx_t *) uls_get_litstr__user_data(lit);
	const char *lptr = lit->lptr, *lptr_end = lit->lptr_end;
	char ch;

	if (lptr == lptr_end) {
		mkf_ctx->prev_ch = -1;
		mkf_ctx->prefix_tabs = 1;
		return ULS_LITPROC_ENDOFQUOTE;
	}

	ch = *lptr;

	if (mkf_ctx->prev_ch == '\n') {
		if (ch == '\t') {
			mkf_ctx->prefix_tabs = 1;
			mkf_ctx->prev_ch = ch;
			lit->lptr = ++lptr;
			return 1; // # of required bytes at the next stage.
		}

		mkf_ctx->prev_ch = -1;
		mkf_ctx->prefix_tabs = 1;
		return ULS_LITPROC_ENDOFQUOTE;
	}

	if (ch == '\n') {
		++lit_ctx->n_lfs;
		if (mkf_ctx->prev_ch != '\\') {
			csz_putc(lit_ctx->ss_dst, ch);
		}

	} else if (ch == '\\') {

	} else {
		if (mkf_ctx->prefix_tabs) {
			if (ch != '\t') { // remove the front tabs in all the command lines.
				mkf_ctx->prefix_tabs = 0;
				csz_putc(lit_ctx->ss_dst, ch);
			}
		} else {
			if (mkf_ctx->prev_ch == '\\') csz_putc(lit_ctx->ss_dst, '\\');
			csz_putc(lit_ctx->ss_dst, ch);
		}
	}

	mkf_ctx->prev_ch = ch;
	lit->lptr = ++lptr;

	return 1; // # of required bytes at the next stage.
}

// <brief>
// Finds the end of quotation string.
// </brief>
// <parm name="k">The cursor of 'tokbuf'</parm>
// <parm name="ch">The right quotation mark</parm>
// <return>The updated cursor of 'tokbuf'</return>
int
MkfLex::expect_quotestr(char ch)
{
	string *lxm;
	int len;
	const char *  ptr;
	char tch;

	MkfLexBasis::getTok();
	MkfLexBasis::getTokStr(&lxm);

	ptr = (const char *) lxm->c_str();
	len = (int) lxm->length();

	tch = (char) ch;
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

	tch = (char) ch;
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	return csz_length(&tokbuf);
}

// <brief>
// Completes the 'makefile-word' by finding the end of it.
// </brief>
// <parm name="k">The cursor of 'tokbuf'</parm>
// <return>The updated cursor of 'tokbuf'</return>
int
MkfLex::expect_word(void)
{
	int escape = 0;
	bool is_quote;
	uls_wch_t wch;
	char tch;

	while (1) {
		if ((wch=MkfLexBasis::peekCh(&is_quote)) == ULS_UCH_NONE) {
			if (is_quote == false) break;
		}

		if (is_quote == true) {
			if (wch == '\n') {
				break;
			}
			expect_quotestr(wch);
			continue;
		}

		if (escape) {
			if ((wch=MkfLexBasis::getCh(NULL)) != '\n') {
				tch = '\\';
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));

				tch = (char) wch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));
			}
			escape = 0;
			continue;
		}

		if (wch == '\n') {
			break;
		}

		if ((wch = MkfLexBasis::getCh(&is_quote)) == '\\') {
			escape = 1;

		} else if (is_quote) {
			expect_quotestr(wch);

		} else {
			tch = (char) wch;
			csz_append(&tokbuf, (const char *) &tch, sizeof(char));
		}
	}

	tok_str = string(csz_text(&tokbuf));
	tabblk_ctx.prepare_word = -1;

	return csz_length(&tokbuf);
}

// <brief>
// Completes the number by finding the end of it.
// </brief>
// <parm name="k">The cursor of 'tokbuf'</parm>
// <return>The updated cursor of 'tokbuf'</return>
int
MkfLex::expect_number(void)
{
	string *lxm;
	const char * ptr;
	int tok, len;
	char  tch;

	tok = MkfLexBasis::getTok();
	expect(NUM);

	MkfLexBasis::getTokStr(&lxm);

	ptr = (const char *) lxm->c_str();
	len = (int) lxm->length();

	tch = '0';
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	tch = 'x';
	csz_append(&tokbuf, (const char *) &tch, sizeof(char));

	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

	tabblk_ctx.prepare_word = -1;

	return csz_length(&tokbuf);
}

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>TokNum id</return>
void
MkfLex::get_token(void)
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

	csz_reset(&tokbuf);

	if (tabblk_ctx.prepare_word > 0) {
		expect_word();
		tok_id = WORD;
		return;
	}

	skipBlanks();

	if ((wch = MkfLexBasis::peekCh(&is_quote))  == '-') {
		MkfLexBasis::getCh(NULL);

		tch = '-';
		csz_append(&tokbuf, (const char *) &tch, sizeof(char));

		wch = MkfLexBasis::peekCh(&is_quote);
		if (isdigit(wch)) {
			expect_number();
			// The number-TokNum in 'Makefile' can contain the minus sign.
			tok = NUM;
			wch = MkfLexBasis::peekCh(&is_quote);
		} else {
			tok = WORD;
		}

	} else if (isdigit(wch)) {
		expect_number();
		tok = NUM;
		wch = MkfLexBasis::peekCh(&is_quote);

	} else if (isalpha(wch) || wch == '_') {
		// keyword?
		tok = MkfLexBasis::getTok();
		MkfLexBasis::getTokStr(&lxm);

		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

		wch = MkfLexBasis::peekCh(&is_quote);

	} else {
		tok = NONE;
	}

	if (wch == ULS_UCH_NONE) {
		MkfLexBasis::getTok();
		MkfLexBasis::getTokStr(&lxm);

		if (tok == NONE) tok = MkfLexBasis::getTokNum();

		ptr = (const char *) lxm->c_str();
		len = (int) lxm->length();

		csz_reset(&tokbuf);
		csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

		tch = '\0';
		csz_append(&tokbuf, (const char *) &tch, sizeof(char));

		tabblk_ctx.prepare_word = -1;

		tok_str = string(csz_text(&tokbuf));
		tok_id = tok;

		return;
	}

	if (isalnum(wch) || tcschr("./-%", (char) wch) != NULL) {
		// 'makefile-word' processing, ...
		do {
			if (isdigit(wch)) {
				expect_number();

			} else if (isalpha(wch)) {
				MkfLexBasis::getTok();
				MkfLexBasis::getTokStr(&lxm);

				ptr = (const char *) lxm->c_str();
			 	len = (int) lxm->length();
			 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));

	 		} else {
	 			if (tcschr("./-%" "@#(){}", (char) wch) == NULL) break;

				wch = MkfLexBasis::getCh(NULL);

				tch = (char) wch;
				csz_append(&tokbuf, (const char *) &tch, sizeof(char));
			}
	 	} while ((wch=MkfLexBasis::peekCh(&is_quote)) != ULS_UCH_NONE);

		tok = WORD;
	}

	if (tok == NONE) {
		tok = MkfLexBasis::getTok();
		MkfLexBasis::getTokStr(&lxm);

		ptr = (const char *) lxm->c_str();
	 	len = (int) lxm->length();
	 	csz_append(&tokbuf, (const char *) ptr, len * sizeof(char));
	}

	tok_str = string(csz_text(&tokbuf));

	// The 'prepare_word' is the value of assignment, '=', '+=', ':=', '?='.
	if (tabblk_ctx.prepare_word < 0) {
		if (tok == ID) tabblk_ctx.prepare_word = 0;
		else if (tok == INCLUDE) tabblk_ctx.prepare_word = 1;

	} else if (tabblk_ctx.prepare_word == 0) {
		if (tok == '=' || tok == ADDASGN ||
			tok == QUESTASGN || tok == COLONASGN)
			tabblk_ctx.prepare_word = 1;
		else
			tabblk_ctx.prepare_word = -1;

	} else {
		tabblk_ctx.prepare_word = -1;
	}

	tok_id = tok;
}

int
MkfLex::getTok(void)
{
	get_token();
	setTok(tok_id, tok_str);
	return tok_id;
}

int
MkfLex::getTokNum(void)
{
	return tok_id;
}

std::string&
MkfLex::getTokStr(void)
{
	return tok_str;
}

// <brief>
// This is a virtual method, inherited from 'UlsLex' class.
// But we don't need this method yet to process 'Makefile' files.
// </brief>
string MkfLex::getKeywordStr(int t)
{
	return string("<unknown>");
}

void
MkfLex::ungetTok(void)
{
	tok_ungot = true;
}


const char *
MkfLex::tcschr(const char * tstr, char tch)
{
	int i;

	for (i=0; tstr[i] != '\0'; i++) {
		if (tstr[i] == tch) {
			return tstr + i;
		}
	}

	return NULL;
}
