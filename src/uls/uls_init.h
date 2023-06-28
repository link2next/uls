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
  <file> uls_init.h </file>
  <brief>
    The main header file for ULS user.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/

#ifndef __ULS_INIT_H__
#define __ULS_INIT_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls_type.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#if defined(__ULS_INIT__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __initialize_uls(void);
ULS_DECL_STATIC void __finalize_uls(void);
#ifndef ULS_WINDOWS
ULS_DECL_STATIC int set_uls_locale(void);
#endif
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void _initialize_uls(void);
void _finalize_uls(void);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void initialize_uls(void);
ULS_DLL_EXTERN void finalize_uls(void);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_INIT_H__
