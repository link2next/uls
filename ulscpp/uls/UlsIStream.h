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
  <file> UlsStream.h </file>
  <brief>
 	This Implemented the wrapper class of ulslex_t.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#pragma once

#include "uls/UlsStream.h"

#include <string>
#include <map>

#include <uls/uls_istream.h>

namespace uls {
	namespace crux {
		class UlsAuw;

		class ULSCPP_DLL_EXTERN UlsTmplList {
			UlsAuw *auwcvt;
			std::map<std::string,std::string> *hashtbl;
			std::map<std::wstring,std::wstring> *whashtbl;

			// <brief>
			// Append a pair <tnam, tval> to the list, increasing the length of the list.
			// </brief>
			// <parm name="tnam">the name of template variable</parm>
			// <parm name="tval">the value of the 'tnam'</parm>
			// <return>none</return>
			void insert(const char *tnam, const char *tval);
			void insert(const wchar_t *tnam, const wchar_t *tval);

		public:
			// <brief>
			// The purpose of UlsTmplList is to store a list of template variables and their values.
			// A template variable is composed of a name and its (string) value.
			// It can be passed to the argument of UlsIStream().
			// </brief>
			// <parm name="size">The capacity of the internal list.</parm>
			UlsTmplList(int size=8);
			~UlsTmplList();

			// <brief>
			// This clears the internal list of (template) variables.
			// The length of the list will be zero.
			// </brief>
			// <return>none</return>
			void clear(void);

			// <brief>
			// Checks if the variable 'tnam' exists in the list.
			// </brief>
			// <return>true/false</return>
			bool exist(std::string& tnam);
			bool exist(std::wstring& tnam);

			// <brief>
			// It returns the length of the internal (template variable) list.
			// It's less than or equal to the capacity of the list.
			// </brief>
			// <return># of arguments</return>
			int length(void);

			// <brief>
			// It returns the value of 'tnam'.
			// In case of no template variable named 'tnam', it returns false.
			// Otherwise the value of the 'tnam' is copied to 'tval' and it returns true.
			// </brief>
			// <parm name="tnam">the name of template variable</parm>
			// <parm name="tval">the value of the 'tnam', output parameter</parm>
			// <return></return>
			bool getValue(std::string& tnam, std::string& tval);
			bool getValue(std::wstring& wtnam, std::wstring& wtval);
			const char *getValue(const char*tnam);
			const wchar_t *getValue(const wchar_t *wtnam);

			// <brief>
			// This modifies the pair <tnam, tval> in the list if the item named 'tnam' exists.
			// </brief>
			// <parm name="tnam">the name of template variable</parm>
			// <parm name="tval">the value of the 'tnam'</parm>
			// <return>none</return>
			bool setValue(std::string& tnam, std::string& tval);
			bool setValue(std::wstring& wtnam, std::wstring& wtval);
			bool setValue(const char *tnam, const char *tval);
			bool setValue(const wchar_t *nam, const wchar_t *tval);

			// <brief>
			// Dumps the internal list of pairs <tnam,tval> to stdout.
			// </brief>
			// <return>none</return>
			void dump(void);

			// <brief>
			// This exports the internal list to another UlsTmplList object.
			// </brief>
			// <return>the array of 'uls_tmpl_t'</return>
			int exportTmpls(uls_tmpl_list_t *tmpl_list_exp);

			// <brief>
			// Export the internal list to another UlsTmplList object.
			// </brief>
			// <parm name="tmpl_list_exp">A new UlsTmplList object to which the current list is exported.</parm>
			// <return>the length of the list</return>
			int exportTmpls(UlsTmplList& tmpl_list_exp);
		};

		class ULSCPP_DLL_EXTERN UlsIStream : public UlsStream {
			uls_istream_t *in_hdr;
			UlsTmplList tmpl_vars;

		protected:
			void initUlsIStream_ustr(const char* filepath, UlsTmplList *uls_tmpls);

		public:
			// <brief>
			// UlsIStream is also used as an input file of UlsLex.
			// It's an abstraction of input source, together with text file and uls-fil.
			// The parameter 'uls_tmpls' is optional.
			// In case that the file is a token sequence file and has template variables,
			//     the parameter 'uls_tmpls' must be given.
			// </brief>
			// <parm name="filepath">inputs of UlsLex</parm>
			// <parm name="uls_tmpls">A list of template variables having its values too.</parm>
			UlsIStream(std::string filepath, UlsTmplList *uls_tmpls=NULL);
			UlsIStream(std::wstring filepath, UlsTmplList *uls_tmpls=NULL);

			// <brief>
			// The destuctor of UlsIStream.
			// </brief>
			// <return>none</return>
			virtual ~UlsIStream();

			// <brief>
			// This finalizes the task of streaming and closes the output-file.
			// </brief>
			// <return>none</return>
			void close(void);

			// <brief>
			// Returns the pointer of the embedded C-structure.
			// </brief>
			uls_istream_t *getCore(void);

			// <brief>
			// Returns the list of the intenal template variables.
			// </brief>
			int exportTmpls(uls_tmpl_list_t *tmpl_list_exp);

			// <brief>
			// Clears the list of template variables.
			// </brief>
			// <return>none</return>
			void clearTmpls(void);
		};
	}
}
