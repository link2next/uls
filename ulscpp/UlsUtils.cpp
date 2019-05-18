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
  <file> UlsUtils.cpp </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2018.
  </author>
*/

#include "uls/UlsUtils.h"
#include <uls/uls_auw.h>
#include <uls/uls_log.h>

#include <string>
#include <stdlib.h>

using namespace std;
using namespace uls::crux;
//
//
// UlsAuw
//
//
UlsAuw::UlsAuw(int size)
{
	int i;

	if (size < 0) {
		size = ULSCPP_NUM_CSZ_BUFFS;
	}

	auwstr_buf = (csz_str_t *) malloc(size * sizeof(csz_str_t));
	siz_auwstr_buf = size;

	for (i = 0; i < siz_auwstr_buf; i++) {
		csz_init(auwstr_buf + i, ULSCPP_DFL_CSZ_BUFFSIZE);
	}
}
	
// <brief>
// The destructor of UlsLex.
// </brief>
// <return>none</return>
UlsAuw::~UlsAuw()
{
	int i;

	for (i = 0; i < siz_auwstr_buf; i++) {
		csz_deinit(auwstr_buf + i);
	}

	free(auwstr_buf);
}

char*
UlsAuw::mbstr2mbstr(const char *mbstr, int mode, int slot_no)
{
	char *mbstr2;

	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	if (mode == CVT_MBSTR_USTR) {
		mbstr2 = uls_astr2ustr(mbstr, -1, auwstr_buf + slot_no);
	}
	else if (mode == CVT_MBSTR_ASTR) {
		mbstr2 = uls_ustr2astr(mbstr, -1, auwstr_buf + slot_no);
	}
	else {
		mbstr2 = NULL;
	}

	return mbstr2;
}

char *
UlsAuw::wstr2mbstr(const wchar_t *wstr, int mode, int slot_no)
{
	char *mbstr2;

	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	if (mode == CVT_MBSTR_USTR) {
		mbstr2 = uls_wstr2ustr(wstr, -1, auwstr_buf + slot_no);
	}
	else if (mode == CVT_MBSTR_ASTR) {
		mbstr2 = uls_wstr2astr(wstr, -1, auwstr_buf + slot_no);
	}
	else {
		mbstr2 = NULL;
	}

	return mbstr2;
}

wchar_t *
UlsAuw::mbstr2wstr(const char *mbstr, int mode, int slot_no)
{
	wchar_t *wstr2;

	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	if (mode == CVT_MBSTR_USTR) {
		wstr2 = uls_ustr2wstr(mbstr, -1, auwstr_buf + slot_no);
	}
	else if (mode == CVT_MBSTR_ASTR) {
		wstr2 = uls_astr2wstr(mbstr, -1, auwstr_buf + slot_no);
	}
	else {
		wstr2 = NULL;
	}

	return wstr2;
}

int
UlsAuw::get_slot_len(int slot_no)
{
	if (slot_no >= siz_auwstr_buf) {
		err_panic("Internal error slot_no = %d/%d", slot_no, siz_auwstr_buf);
	}

	return auwstr_buf[slot_no].len;
}

// <brief>
// This is a constructor of ArgListW
// </brief>
ArgListW::ArgListW()
{
	wargs = NULL;
	n_wargs = 0;
}

// <brief>
// The destructor of ArgListW
// </brief>
ArgListW::~ArgListW()
{
	reset();
}

// <brief>
// convert the list of string to wide strings.
// </brief>
// <parm name="args">list of string</parm>
// <parm name="n_args"># of strings in the list</parm>
// <return>list of wide-char string</return>
bool
ArgListW::setWArgList(char **args, int n_args)
{
	reset();

	wchar_t *wstr;
	int i, wlen;

	if (n_args <= 0) {
		return false;
	}

	UlsAuw *auw_converter = new UlsAuw(n_args);

	n_wargs = n_args;
	wargs = (wchar_t **) uls_malloc(n_args * sizeof(wchar_t *));

	for (i=0; i < n_args; i++) {
#ifdef ULS_WINDOWS
		wstr = auw_converter->mbstr2wstr(args[i], UlsAuw::CVT_MBSTR_ASTR, i);
#else
		wstr = auw_converter->mbstr2wstr(args[i], UlsAuw::CVT_MBSTR_USTR, i);
#endif
		wlen = auw_converter->get_slot_len(i) / sizeof(wchar_t);
		wargs[i] = (wchar_t *) uls_malloc((wlen + 1) * sizeof(wchar_t));
		uls_memcopy(wargs[i], wstr, wlen * sizeof(wchar_t));
		wargs[i][wlen] = L'\0';
	}

	delete auw_converter;
	return true;
}

wchar_t*
ArgListW::getWArg(int i)
{
	if (i >= n_wargs) {
		return NULL;
	}

	return wargs[i];
}

wchar_t **
ArgListW::exportWArgs(int *ptr_n_args)
{
	wchar_t **wlist = wargs;

	if (ptr_n_args != NULL) {
		*ptr_n_args = n_wargs;
	}

	wargs = NULL;
	n_wargs = 0;

	return wlist;
}

void
ArgListW::reset()
{
	int i;

	for (i=0; i < n_wargs; i++) {
		uls_mfree(wargs[i]);
	}

	uls_mfree(wargs);
}

wchar_t**
uls::get_warg_list(char **argv, int n_argv)
{
	wchar_t **wargv;

	ArgListW * wlist = new ArgListW();

	wlist->setWArgList(argv, n_argv);
	wargv = wlist->exportWArgs(NULL);

	delete wlist;

	return wargv;
}

void
uls::put_warg_list(wchar_t **wargv, int n_wargv)
{
	int i;

	for (i=0; i < n_wargv; i++) {
		uls_mfree(wargv[i]);
	}

	uls_mfree(wargv);
}
