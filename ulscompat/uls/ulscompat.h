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
  <file> ulscompat.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/
#ifndef __ULSCOMPAT_H__
#define __ULSCOMPAT_H__

#include "uls/uls_type.h"
#include <wchar.h>

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef __GNUC__
void __attribute__((constructor)) _initialize_ulscompat(void);
void __attribute__((destructor)) _finalize_ulscompat(void);
#else
void _initialize_ulscompat(void);
void _finalize_ulscompat(void);
#endif

ULS_DLL_EXTERN void initialize_ulscompat(void);
ULS_DLL_EXTERN void finalize_ulscompat(void);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULSCOMPAT_H__
