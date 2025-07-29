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
  <file> uls_file.h </file>
  <brief>
    The file for UTF-16, UTF-32 source code
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/

#ifndef __ULS_UTF_FILE_H__
#define __ULS_UTF_FILE_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#if defined(__ULS_UTF_FILE__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_decl_array_tool_type01(id_range_list1, uch_range);
#endif

#if defined(__ULS_UTF_FILE__) || defined(ULS_DECL_PROTECTED_PROC)
int id_range_comp(uls_const_voidptr_t e, uls_const_voidptr_t b);
int load_uch_ranges_list(void);
void unload_uch_ranges_list(void);
int is_utf_id(uls_wch_t wch);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_UTF_FILE_H__
