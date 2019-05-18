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
#include "uls/uls_type.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PROTECTED_TYPE

ULS_DEFINE_STRUCT(uls_uch_id_range)
{
  int  ent_no;
  uls_uch_t  x1, x2;
};
ULS_DEF_PARRAY_THIS(uch_id_range);

#ifndef ULS_CLASSIFY_SOURCE
ULS_DEF_ARRAY_THIS_TYPE10(uch_id_range);
#endif
#endif

#if defined(__ULS_UTF_FILE__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_decl_array_this_type10(id_range_list1, uch_id_range);
ULS_DECL_STATIC uls_decl_parray(id_range_list2, uch_id_range);
ULS_DECL_STATIC int   n_id_range_list1, n_id_range_list2;
#endif

#if defined(__ULS_UTF_FILE__) || defined(ULS_DECL_PROTECTED_PROC)
int id_range_comp(const uls_voidptr_t e, const uls_voidptr_t b);
void uls_init_uch_id_range(uls_uch_id_range_ptr_t range);
void uls_deinit_uch_id_range(uls_uch_id_range_ptr_t range);

int load_uch_ranges_list(int max_pref);
void unload_uch_ranges_list(void);
int is_utf_id(uls_uch_t uch);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_UTF_FILE_H__
