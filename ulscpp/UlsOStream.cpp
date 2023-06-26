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
  <file> UlsOStream.cpp </file>
  <brief>
 	This Implemented the wrapper class of ulslex_t.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/UlsLex.h"
#include "uls/UlsOStream.h"
#include "uls/UlsUtils.h"

#include <string>
#include <iostream>

#include <uls/uls_auw.h>

using namespace std;
using namespace uls::crux;

bool
uls::crux::UlsOStream::makeOStream_ustr(const char *filepath, UlsLex *lex, const char *subtag, bool numbering)
{
	int fd_out;

	if (lex == NULL) {
		cerr << "invalid param 'UlsLex'!" << endl;
		return false;
	}

	string fpath = filepath;
	if ((fd_out = create_fd_wronly(fpath)) < 0) {
		cerr << "can't create file for uls!" << endl;
		return false;
	}

	if ((out_hdr = uls_create_ostream(fd_out, lex->getLexCore(), subtag)) == NULL) {
		cerr << "fail to create output stream object!" << endl;
		close_fd(fd_out);
		return false;
	}

	do_numbering = numbering;
	read_only = false;
	uls_lex = lex;

	return true;
}

// <brief>
// A constructor of UlsOStream.
// This makes an object writing token sequence sequentially.
// </brief>
// <parm name="filepath">uls-stream file(*.uls) path</parm>
// <parm name="lex">The lexical analyzer associated with the uls-stream file</parm>
// <parm name="subtag">user provided tag to the 'filepath'</parm>
// <parm name="numbering">Specifies whether number informatioin is inserted.
//     The information is automatically inserted whenever source code line is changed</parm>


uls::crux::UlsOStream::UlsOStream
	(string& filepath, UlsLex *lex, const char* subtag, bool numbering)
{
	const char *ustr0, *ustr1;

	ustr0 = filepath.c_str();
	ustr1 = subtag;

	if (makeOStream_ustr(ustr0, lex, ustr1, numbering) == false) {
		cerr << "can't create uls (output) stream object!" << endl;
	}
}

uls::crux::UlsOStream::UlsOStream
	(std::wstring& wfilepath, UlsLex *lex, const wchar_t *wsubtag, bool numbering)
{
	const char *ustr0, *ustr1;

	_ULSCPP_WSTR2USTR(wfilepath.c_str(), ustr0, 0);
	_ULSCPP_WSTR2USTR(wsubtag, ustr1, 1);

	if (makeOStream_ustr(ustr0, lex, ustr1, numbering) == false) {
		cerr << "can't create uls (output) stream object!" << endl;
	}
}

// <brief>
// The destuctor of UlsOStream.
// </brief>
// <return>none</return>
uls::crux::UlsOStream::~UlsOStream()
{
	close();
}

// <brief>
// This finalizes the task of streaming,
//     flushing data buffer and closing the output-file.
// </brief>
// <return>none</return>
void
uls::crux::UlsOStream::close(void)
{
	if (out_hdr != NULL) {
		uls_destroy_ostream(out_hdr);
		out_hdr = NULL;
	}
}

// <brief>
// Returns the pointer of embedded C-structure.
// </brief>
uls_ostream_t *
uls::crux::UlsOStream::getCore(void)
{
	return out_hdr;
}

// <brief>
// print a record of <tokid,tokstr> pair to the output stream.
// </brief>
// <parm name="tokid">token id to be printed</parm>
// <parm name="tokstr">the lexeme associated with the 'tokid'</parm>
// <return>none</return>
void
uls::crux::UlsOStream::printTok(int tokid, string& tokstr)
{
	const char *ustr;
	int len, rc;

	ustr = tokstr.c_str();
	len = uls_strlen(ustr);

	rc = __uls_print_tok(out_hdr, tokid, ustr, len);
	if (rc < 0)
		cerr << "can't a print token!" << endl;
}

void
uls::crux::UlsOStream::printTok(int tokid, std::wstring& wtokstr)
{
	const char *ustr;
	int len, rc;

	len = _ULSCPP_WSTR2USTR(wtokstr.c_str(), ustr, 0);
	rc = __uls_print_tok(out_hdr, tokid, ustr, len);
	if (rc < 0)
		cerr << "can't a print token!" << endl;
}

// <brief>
// print a anotation for <linenum,tag> pair.
// </brief>
// <parm name="lno">the line number of the source file</parm>
// <parm name="tagstr">the tag of the source file.</parm>
// <return>none</return>
void
uls::crux::UlsOStream::printTokLineNum(int lno, string& tagstr)
{
	const char *ustr;
	int len, rc;

	ustr = tagstr.c_str();
	len = uls_strlen(ustr);
	rc = __uls_print_tok_linenum(out_hdr, lno, ustr, len);
	if (rc < 0)
		cerr << "can't a print linenum-token!" << endl;
}

void
uls::crux::UlsOStream::printTokLineNum(int lno, std::wstring& wtagstr)
{
	const char *ustr;
	int len, rc;

	len = _ULSCPP_WSTR2USTR(wtagstr.c_str(), ustr, 0);
	rc = __uls_print_tok_linenum(out_hdr, lno, ustr, len);
	if (rc < 0)
		cerr << "can't a print linenum-token!" << endl;
}

// <brief>
// Start writing the lexical streaming with input-stream 'ifile'.
// </brief>
// <parm name="ifile">This input uls-stream will be written to the 'UlsOStream' object.</parm>
bool
uls::crux::UlsOStream::startStream(UlsIStream& ifile)
{
	int rc, flags=0;

	if (out_hdr == NULL || read_only == true) return false;

	if (do_numbering) flags |= ULS_LINE_NUMBERING;

	uls_lex->pushInput(ifile);

	if ((rc=uls_start_stream(out_hdr, flags)) < 0) {
		cerr << "can't do lex-streaming!" << endl;
		return false;
	}

	return true;
}

