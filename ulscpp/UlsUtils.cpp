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
