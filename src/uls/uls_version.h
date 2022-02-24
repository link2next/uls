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

#ifndef __ULS_VERSION_H__
#define __ULS_VERSION_H__

#include "uls/uls_type.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define ULS_VERSION_CODE_LEN     3
#define ULS_VERSION_STR_MAXLEN  11
#define uls_vers_compatible(ver1,ver2) (uls_version_cmp_code(ver1,ver2) >= 2)

ULS_DEFINE_STRUCT(version)
{
	char  major;
	char  minor;
	char  debug;
	char  sequence;
};

ULS_DLL_EXTERN void uls_init_version(uls_version_ptr_t a);
ULS_DLL_EXTERN void uls_deinit_version(uls_version_ptr_t a);

ULS_DLL_EXTERN void uls_version_make(uls_version_ptr_t a,
	char v1, char v2, char v3);
ULS_DLL_EXTERN void uls_version_encode(const uls_version_ptr_t a, char* codstr);
ULS_DLL_EXTERN void uls_version_decode(char* codstr, uls_version_ptr_t a);
ULS_DLL_EXTERN void uls_version_copy(uls_version_ptr_t dst, const uls_version_ptr_t src);
ULS_DLL_EXTERN int uls_version_make_string(const uls_version_ptr_t a, char* ver_str);
ULS_DLL_EXTERN int uls_version_cmp_code(const uls_version_ptr_t a, const uls_version_ptr_t b);
ULS_DLL_EXTERN int uls_version_pars_str(const char *lptr, uls_version_ptr_t a);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_VERSION_H__
