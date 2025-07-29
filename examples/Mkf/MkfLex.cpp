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

// <brief>
// Sets the input file to be tokenized.
// </brief>
// <parm name="fpath">The path of file</parm>
// <return>0 if it succeeds, otherwise -1</return>
int
MkfLex::include(tstring& fpath)
{
	if (fpath != _T("")) {
		if (pushFile(fpath) == false) {
			return -1;
		}
	}

	tabblk_ctx.prev_ch = -1;
	tabblk_ctx.prepare_word = -1;

	tok_str = _T("");
	tok_id = NONE;

	return 0;
}

MkfLex::MkfLex(tstring& config_name)
	: MkfLexBasis(config_name)
{
	tstring comm_patstr = _T("\n\t");
	changeLiteralAnalyzer(comm_patstr, tabblk_analyzer, &tabblk_ctx);

	tabblk_ctx.prev_ch = -1;
	tabblk_ctx.prefix_tabs = 1;

	tstring nil_fpath = _T("");
	include(nil_fpath);
	tok_ungot = false;
}

MkfLex::~MkfLex()
{
	tstring nil_fpath = _T("");
	include(nil_fpath);
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
			uls_litstr_putc(lit_ctx, ch);
		}

	} else if (ch == '\\') {

	} else {
		if (mkf_ctx->prefix_tabs) {
			if (ch != '\t') { // remove the front tabs in all the command lines.
				mkf_ctx->prefix_tabs = 0;
				uls_litstr_putc(lit_ctx, ch);
			}
		} else {
			if (mkf_ctx->prev_ch == '\\')
				uls_litstr_putc(lit_ctx, '\\');
			uls_litstr_putc(lit_ctx, ch);
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
MkfLex::expect_quotestr(TCHAR ch)
{
	tstring lxm;
	int len;
	LPCTSTR  ptr;
	TCHAR tch;

	MkfLexBasis::next();
	UlsLex::getTokStr(lxm);

	ptr = (LPCTSTR) lxm.c_str();
	len = (int) lxm.length();

	tch = (TCHAR) ch;
	tokbuf.append(tch);

	tokbuf.append(ptr, len);

	tch = (TCHAR) ch;
	tokbuf.append(tch);

	return tokbuf.len();
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
	uls_wch_t wch;
	TCHAR tch;

	while (1) {
		if ((wch = peekChar()) >= ULS_CH_QSTR) {
			break;
		}

		if (wch == ULS_CH_QSTR) {
			if (wch == _T('\n')) {
				break;
			}
			expect_quotestr(wch);
			continue;
		}

		if (escape) {
			if ((wch = getChar()) != _T('\n')) {
				tch = _T('\\');
				tokbuf.append(tch);

				tch = (TCHAR) wch;
				tokbuf.append(tch);
			}
			escape = 0;
			continue;
		}

		if (wch == _T('\n')) {
			break;
		}

		if ((wch = getChar()) == _T('\\')) {
			escape = 1;

		} else if (wch == ULS_CH_QSTR) {
			expect_quotestr(wch);

		} else {
			tch = (TCHAR) wch;
			tokbuf.append(tch);
		}
	}

	tok_str = tokbuf.str();
	tabblk_ctx.prepare_word = -1;

	return tokbuf.len();
}

// <brief>
// Completes the number by finding the end of it.
// </brief>
// <parm name="k">The cursor of 'tokbuf'</parm>
// <return>The updated cursor of 'tokbuf'</return>
int
MkfLex::expect_number(void)
{
	tstring lxm;
	LPCTSTR ptr;
	int len;
	TCHAR  tch;

	MkfLexBasis::next();
	expect(NUM);

	UlsLex::getTokStr(lxm);

	ptr = (LPCTSTR) lxm.c_str();
	len = (int) lxm.length();

	tch = _T('0');
	tokbuf.append(tch);

	tch = _T('x');
	tokbuf.append(tch);

	tokbuf.append(ptr, len);

	tabblk_ctx.prepare_word = -1;
	return tokbuf.len();
}

// <brief>
// This is a virtual method to be implemented, inherited from 'UlsLex' class.
// </brief>
// <return>TokNum id</return>
void
MkfLex::get_token(void)
{
	tstring lxm;
	int tok, len;
	uls_wch_t wch;
	LPCTSTR ptr;
	TCHAR tch;

	if (tok_ungot == true) {
		tok_ungot = false;
		return;
	}

	tokbuf.clear();

	if (tabblk_ctx.prepare_word > 0) {
		expect_word();
		tok_id = WORD;
		return;
	}

	skipBlanks();

	if ((wch = peekChar()) >= ULS_CH_QSTR) {
		tok = NONE;

	} else if (wch == '-') {
		getChar();

		tch = _T('-');
		tokbuf.append(tch);

		wch = peekChar();
		if (isdigit(wch)) {
			expect_number();
			// The number-TokNum in 'Makefile' can contain the minus sign.
			tok = NUM;
			wch = peekChar();
		} else {
			tok = WORD;
		}

	} else if (isdigit(wch)) {
		expect_number();
		tok = NUM;
		wch = peekChar();

	} else if (isalpha(wch) || wch == _T('_')) {
		// keyword?
		tok = MkfLexBasis::next();
		UlsLex::getTokStr(lxm);

		ptr = (LPCTSTR) lxm.c_str();
	 	len = (int) lxm.length();
	 	tokbuf.append(ptr, len);

		wch = peekChar();

	} else {
		tok = NONE;
	}

	if (wch >= ULS_CH_QSTR) {
		MkfLexBasis::next();
		UlsLex::getTokStr(lxm);

		if (tok == NONE) tok = MkfLexBasis::getTokNum();

		ptr = (LPCTSTR) lxm.c_str();
		len = (int) lxm.length();

		tokbuf.clear();
		tokbuf.append(ptr, len);

		tch = _T('\0');
		tokbuf.append(tch);

		tabblk_ctx.prepare_word = -1;

		tok_str = tokbuf.str();
		tok_id = tok;

		return;
	}

	if (isalnum(wch) || uls::strFindIndex(_T("./-%"), (TCHAR) wch) >= 0) {
		// 'makefile-word' processing, ...
		do {
			if (isdigit(wch)) {
				expect_number();

			} else if (isalpha(wch)) {
				MkfLexBasis::next();
				UlsLex::getTokStr(lxm);

				ptr = (LPCTSTR) lxm.c_str();
			 	len = (int) lxm.length();
			 	tokbuf.append(ptr, len);

	 		} else {
	 			if (uls::strFindIndex(_T("./-%" "@#(){}"), (TCHAR) wch) < 0) break;

				wch = getChar();
				tch = (TCHAR) wch;
				tokbuf.append(tch);
			}
	 	} while ((wch = peekChar()) < ULS_CH_QSTR);

		tok = WORD;
	}

	if (tok == NONE) {
		tok = MkfLexBasis::next();
		UlsLex::getTokStr(lxm);

		ptr = (LPCTSTR) lxm.c_str();
	 	len = (int) lxm.length();
	 	tokbuf.append(ptr, len);
	}

	tok_str = tokbuf.str();

	// The 'prepare_word' is the value of assignment, '=', '+=', ':=', '?='.
	if (tabblk_ctx.prepare_word < 0) {
		if (tok == ID) tabblk_ctx.prepare_word = 0;
		else if (tok == INCLUDE) tabblk_ctx.prepare_word = 1;

	} else if (tabblk_ctx.prepare_word == 0) {
		if (tok == _T('=') || tok == ADDASGN ||
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
MkfLex::next(void)
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

void
MkfLex::ungetCurrentToken(void)
{
	tok_ungot = true;
}
