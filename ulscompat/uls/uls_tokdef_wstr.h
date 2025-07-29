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
  <file> uls_tokdef_wstr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#ifndef __ULS_TOKDEF_WSTR_H__
#define __ULS_TOKDEF_WSTR_H__

#include "uls/uls_auw.h"
#ifdef _ULSCOMPAT_INTERNALLY_USES
#include "uls/uls_tokdef.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DEFINE_STRUCT(id2wstr_pair)
{
	int     tok_id;
	int     n_wchars;
	wchar_t *wstr;
	int     wlen;
};

uls_id2wstr_pair_ptr_t __find_widename_in_list(uls_id2wstr_pair_ptr_t pair_list, int pair_list_len, int t);
uls_id2wstr_pair_ptr_t __add_widename_in_list(uls_outparam_ptr_t parms,	int t, const char *sval, int n_bytes);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_TOKDEF_WSTR_H__
