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
  <file> UlsIStream.cpp </file>
  <brief>
 	This Implemented the wrapper class of ulslex_t.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "uls/UlsIStream.h"
#include "uls/UlsUtils.h"

#include <string>
#include <iostream>

#include <uls/uls_auw.h>

using namespace std;
using namespace uls::crux;

// <brief>
// The purpose of UlsTmplList is to store a list of template variables and their values.
// A template variable is composed of a name and its (string) value.
// It can be passed to the argument of UlsIStream().
// </brief>
// <parm name="size">Maximum number of arguments</parm>
uls::crux::UlsTmplList::UlsTmplList(int size)
{
	auwcvt = new UlsAuw();
	hashtbl = new std::map<string,string>();
	whashtbl = new std::map<wstring,wstring>();
}

uls::crux::UlsTmplList::~UlsTmplList()
{
	delete hashtbl;
	delete whashtbl;
	delete auwcvt;
}

// <brief>
// This clears the internal list of (template) variables.
// </brief>
// <return>none</return>
void
uls::crux::UlsTmplList::clear(void)
{
	hashtbl->clear();
	whashtbl->clear();
}

// <brief>
// Checks if the variable 'tnam' exists in the list.
// </brief>
// <return>true/false</return>
bool
uls::crux::UlsTmplList::exist(string& tnam)
{
	std::map<string,string>::iterator it;
	bool stat;

	it = hashtbl->find(tnam);
	if (it != hashtbl->end()) {
		stat = true;
	} else {
		stat = false;
	}

	return stat;
}

bool
uls::crux::UlsTmplList::exist(std::wstring& wtnam)
{
	std::map<wstring,wstring>::iterator it;
	bool stat;

	it = whashtbl->find(wtnam);
	if (it != whashtbl->end()) {
		stat = true;
	} else {
		stat = false;
	}

	return stat;
}

// <brief>
// Returns the size of the internal list.
// It's not the capacity but the lenghth of the interanal list.
// It must be less than or equal to the capacity of the internal list.
// </brief>
// <return># of arguments</return>
int
uls::crux::UlsTmplList::length(void)
{
	return (int) hashtbl->size();
}

// <brief>
// Append a pair <tnam, tval> to the list, increasing the length of the list.
// </brief>
// <parm name="tnam">the name of template variable</parm>
// <parm name="tval">the value of the 'tnam'</parm>
// <return>none</return>
void
uls::crux::UlsTmplList::insert(const char *tnam, const char *tval)
{
	(*hashtbl)[string(tnam)] = tval;
}

void
uls::crux::UlsTmplList::insert(const wchar_t *wtnam, const wchar_t *wtval)
{
	(*whashtbl)[wstring(wtnam)] = wtval;
}

// <brief>
// Returns the value of 'tnam' to 'tval'
// Returns 'true' if the variable 'tnam' exists
//         'false' otherwise.
// </brief>
// <parm name="tnam">the name of template variable</parm>
// <parm name="tval">the value of the 'tnam', output parameter</parm>
// <return>tval</return>
const char*
uls::crux::UlsTmplList::getValue(const char*tnam)
{
	std::map<string,string>::iterator it;
	const char *tval = NULL;

	it = hashtbl->find(string(tnam));
	if (it != hashtbl->end()) {
		std::pair<string,string> pp=*it;
		tval = pp.second.c_str();
	}

	return tval;
}

bool
uls::crux::UlsTmplList::getValue(string& tnam, string& tval)
{
	const char *ptr;
	bool stat;

	if ((ptr = getValue(tnam.c_str())) != NULL) {
		tval = ptr;
		stat = true;
	} else {
		tval = "";
		stat = false;
	}

	return stat;
}

const wchar_t*
uls::crux::UlsTmplList::getValue(const wchar_t *wtnam)
{
	std::map<wstring,wstring>::iterator it;
	const wchar_t *wtval = NULL;

	it = whashtbl->find(wstring(wtnam));
	if (it != whashtbl->end()) {
		std::pair<wstring,wstring> pp=*it;
		wtval = pp.second.c_str();
	}

	return wtval;
}

bool
uls::crux::UlsTmplList::getValue(std::wstring& wtnam, std::wstring& wtval)
{
	const wchar_t *wptr;
	bool stat;

	if ((wptr = getValue(wtnam.c_str())) != NULL) {
		wtval = wptr;
		stat = true;
	} else {
		wtval = L"";
		stat = false;
	}

	return stat;

}
// <brief>
// Modify the pair <tnam,tval> in the internal list.
// </brief>
// <parm name="tnam">the name of template variable</parm>
// <parm name="tval">the value of the 'tnam'</parm>
// <return>none</return>
bool
uls::crux::UlsTmplList::setValue(const char *tnam, const char *tval)
{
	insert(tnam, tval);

	wchar_t *wstr0, *wstr1;
	_ULSCPP_USTR2WSTR(tnam, wstr0, 0);
	_ULSCPP_USTR2WSTR(tval, wstr1, 1);

	insert(wstr0, wstr1);

	return true;
}

bool
uls::crux::UlsTmplList::setValue(string& tnam, string& tval)
{
	return setValue(tnam.c_str(), tval.c_str());
}

bool
uls::crux::UlsTmplList::setValue(const wchar_t *wtnam, const wchar_t *wtval)
{
	insert(wtnam, wtval);

	const char *nstr0, *nstr1;
	_ULSCPP_WSTR2USTR(wtnam, nstr0, 0);
	_ULSCPP_WSTR2USTR(wtval, nstr1, 1);

	insert(nstr0, nstr1);

	return true;
}

bool
uls::crux::UlsTmplList::setValue(std::wstring& wtnam, std::wstring& wtval)
{
	return setValue(wtnam.c_str(), wtval.c_str());
}
// <brief>
// Uses the structure 'uls_tmpl_list_t' to export the intenal list.
// </brief>
// <return>the array of 'uls_tmpl_t'</return>
int
uls::crux::UlsTmplList::exportTmpls(uls_tmpl_list_t *tmpl_list)
{
	uls_reset_tmpls(tmpl_list, (int) hashtbl->size());
	std::map<string,string>::iterator it;
	int n = 0;

	for (it=hashtbl->begin(); it != hashtbl->end(); ++it) {
		std::pair<string,string> pp=*it;

		const char *ustr0, *ustr1;
		ustr0 = pp.first.c_str();
		ustr1 = pp.second.c_str();

		uls_add_tmpl(tmpl_list, ustr0, ustr1);
		++n;
	}

	return n;
}

// <brief>
// Export the internal list to another UlsTmplList object.
// </brief>
// <return>the size of the list</return>
int
uls::crux::UlsTmplList::exportTmpls(UlsTmplList& tmpl_list_exp)
{
	std::map<string,string>::iterator it;
	int n=0;

	for (it=hashtbl->begin(); it != hashtbl->end(); ++it) {
		std::pair<string,string> pp=*it;
		tmpl_list_exp.setValue(pp.first.c_str(), pp.second.c_str());
		++n;
	}

	return n;
}

// <brief>
// Dumps the internal list of pairs <tnam,tval> to stdout.
// </brief>
// <return>none</return>
void
uls::crux::UlsTmplList::dump(void)
{
	std::map<string,string>::iterator it;

	for (it=hashtbl->begin(); it != hashtbl->end(); ++it) {
		std::pair<string,string> pp=*it;
		cout << pp.first << " :: '" << pp.second << "'" << endl;
	}
}

// <brief>
// UlsIStream is input file of UlsLex.
// The parameter can indicate text file or the uls-file having template variables.
// </brief>
// <parm name="filepath">inputs of UlsLex</parm>
void
uls::crux::UlsIStream::initUlsIStream_ustr(const char* filepath, UlsTmplList *uls_tmpls)
{
	read_only = true;

	if ((in_hdr = uls_open_istream_file(filepath)) == NULL) {
		cerr << "Can't create in-stream!" << endl;
		return;
	}

	if (uls_tmpls != NULL) {
		uls_tmpls->exportTmpls(tmpl_vars);
	}
}

uls::crux::UlsIStream::UlsIStream(string filepath, UlsTmplList *uls_tmpls)
	: tmpl_vars(8)
{
	const char *ustr;

	ustr = filepath.c_str();
	initUlsIStream_ustr(ustr, uls_tmpls);
}


uls::crux::UlsIStream::UlsIStream(wstring wfilepath, UlsTmplList *uls_tmpls)
	: tmpl_vars(8)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(wfilepath.c_str(), ustr, 0);
	initUlsIStream_ustr(ustr, uls_tmpls);
}

// <brief>
// The destuctor of UlsIStream.
// </brief>
// <return>none</return>
uls::crux::UlsIStream::~UlsIStream()
{
	close();
}

// <brief>
// This finalizes the task of streaming and closes the output-file.
// </brief>
// <return>none</return>
void
uls::crux::UlsIStream::close(void)
{
	if (in_hdr != NULL) {
		uls_destroy_istream(in_hdr);
		in_hdr = NULL;
	}

	clearTmpls();
}

// <brief>
// Returns the pointer of the embedded C-structure.
// </brief>
uls_istream_t*
uls::crux::UlsIStream::getCore(void)
{
	return in_hdr;
}

// <brief>
// Clears the list of template variables.
// </brief>
// <return>none</return>
void
uls::crux::UlsIStream::clearTmpls(void)
{
	tmpl_vars.clear();
}

// <brief>
// Returns the list of the intenal template variables.
// </brief>
int
uls::crux::UlsIStream::exportTmpls(uls_tmpl_list_t *tmpl_list_exp)
{
	return tmpl_vars.exportTmpls(tmpl_list_exp);
}
