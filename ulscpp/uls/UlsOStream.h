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
  <file> UlsOStream.h </file>
  <brief>
 	This Implemented the wrapper class of ulslex_t.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2015.
  </author>
*/

#pragma once

#include "uls/UlsStream.h"

#include <string>

#include <uls/uls_ostream.h>

namespace uls {
	namespace crux {
		class UlsLex;

		class ULSCPP_DLL_EXTERN UlsOStream : public UlsStream {
			UlsLex *uls_lex;
			uls_ostream_t *out_hdr;
			bool do_numbering;

			bool makeOStream_ustr(const char *filepath, UlsLex *lex, const char* subtag, bool numbering);

		public:
			// <brief>
			// This makes an object writing token sequence sequentially.
			// Its purpose is to save the token sequence from input source.
			// </brief>
			// <parm name="filepath">
			// It's a file path for the output uls-stream file.
			// If the file already exists it'll be overwritten.
			// </parm>
			// <parm name="lex">The lexical analyzer associated with uls-stream file</parm>
			// <parm name="subtag">
			// A user provided tag to the file 'filepath'.
			// More often than not, it tends to be the file path.
			// This string will be written to the output uls file.
			// </parm>
			// <parm name="numbering">
			// Specify whether the line number information is inserted or not.
			// The line number is automatically inserted whenever the line of source code is changed.
			// </parm>

			UlsOStream(std::string& filepath, UlsLex* lex, const char* subtag="", bool numbering=true);
			UlsOStream(std::wstring& filepath, UlsLex* lex, const wchar_t *subtag = L"", bool numbering = true);

			// <brief>
			// The destuctor of UlsOStream.
			// </brief>
			// <return>none</return>
			virtual ~UlsOStream();

			// <brief>
			// This finalizes the task of sequential IO.
			// It flushes data buffer and closes the output-file.
			// </brief>
			// <return>none</return>
			void close(void);

			// <brief>
			// Returns the pointer of the embedded C-structure.
			// </brief>
			uls_ostream_t *getCore(void);

			// <brief>
			// This method prints a record of <tokid, tokstr> pair to the output stream.
			// </brief>
			// <parm name="tokid">the token number to be printed</parm>
			// <parm name="tokstr">the lexeme associated with the 'tokid'</parm>
			// <return>none</return>
			void printTok(int tokid, const std::string& tokstr);
			void printTok(int tokid, const std::wstring& tokstr);

			// <brief>
			// This method prints an annotation <linenum, tag> pair to the output file.
			// </brief>
			// <parm name="lno">the line number of the source file</parm>
			// <parm name="tagstr">the tag of the source file</parm>
			// <return>none</return>
			void printTokLineNum(int lno, const std::string& tagstr);
			void printTokLineNum(int lno, const std::wstring& tagstr);

			// <brief>
			// Start writing the lexical streaming with input-stream 'ifile'.
			// </brief>
			// <parm name="ifile">
			// This input uls-stream will be written to the 'UlsOStream' object.
			// </parm>
			bool startStream(UlsIStream& ifile);
		};
	}
}
