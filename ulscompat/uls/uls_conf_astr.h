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
  <file> uls_conf_astr.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/
#ifndef __ULS_CONF_ASTR_H__
#define __ULS_CONF_ASTR_H__

#include "uls/uls_conf.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

ULS_DLL_EXTERN int uls_is_valid_specpath_astr(const char* confname);
ULS_DLL_EXTERN int uls_get_spectype_astr(const char *fpath, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN void ulc_list_searchpath_astr(const char* confname);
ULS_DLL_EXTERN void ulc_set_searchpath_astr(const char *pathlist);
ULS_DLL_EXTERN int ulc_add_searchpath_astr(const char *pathlist, int front);

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#ifdef ULS_USE_ASTR
#define uls_is_valid_specpath uls_is_valid_specpath_astr
#define uls_get_spectype uls_get_spectype_astr

#define ulc_list_searchpath ulc_list_searchpath_astr
#define ulc_set_searchpath ulc_set_searchpath_astr
#define ulc_add_searchpath ulc_add_searchpath_astr
#endif
#endif

#endif // __ULS_CONF_ASTR_H__
