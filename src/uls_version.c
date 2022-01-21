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
#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_version.h"
#include "uls/uls_print.h"
#endif

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_version_make)(uls_version_ptr_t a, char v1, char v2, char v3)
{
	a->major = v1;
	a->minor = v2;
	a->debug = v3;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_version_encode)(const uls_version_ptr_t a, char* codstr)
{
	codstr[0] = a->major;
	codstr[1] = a->minor;
	codstr[2] = a->debug;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_version_decode)(char* codstr, uls_version_ptr_t a)
{
	a->major = codstr[0];
	a->minor = codstr[1];
	a->debug = codstr[2];
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_version_copy)(uls_version_ptr_t dst, const uls_version_ptr_t src)
{
	dst->major = src->major;
	dst->minor = src->minor;
	dst->debug = src->debug;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_version_make_string)(const uls_version_ptr_t a, char* ver_str)
{
	char codstr[ULS_VERSION_CODE_LEN];
	int i, k=0, n=0;

	uls_version_encode(a, codstr);

	for (i=0; i<ULS_VERSION_CODE_LEN; i++) {
		if (codstr[i] != 0) n = i + 1;
	}

	if (n==0) {
		ver_str[k++] = '0';
		ver_str[k] = '\0';
	} else {
		for (i=0; i<n; i++) {
			if (i != 0) ver_str[k++] = '.';

			k += _uls_log_(snprintf)(ver_str+k, ULS_VERSION_STR_MAXLEN+1-k, "%d", codstr[i]);
		}
	}

	return k;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_version_cmp_code)(const uls_version_ptr_t a, const uls_version_ptr_t b)
{
	char codstr1[ULS_VERSION_CODE_LEN];
	char codstr2[ULS_VERSION_CODE_LEN];
	int i;

	uls_version_encode(a, codstr1);
	uls_version_encode(b, codstr2);

	for (i=0; i<ULS_VERSION_CODE_LEN; i++) {
		if (codstr1[i] != codstr2[i]) break;
	}

	return i;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_version_pars_str)(const char *lptr, uls_version_ptr_t a)
{
	char codstr[ULS_VERSION_CODE_LEN];
	uls_outparam_t parms;
	int i, n;

	for (i=0; i<ULS_VERSION_CODE_LEN; ) {
		if (!uls_isdigit(*lptr)) break;

		parms.lptr = lptr;
		codstr[i++] = (char) uls_skip_atou(uls_ptr(parms));
		lptr = parms.lptr;

		if (*lptr != '.' || !uls_isdigit(*++lptr))
			break;
	}

	for (n=i; i<ULS_VERSION_CODE_LEN; i++)
		codstr[i] = 0;

	uls_version_decode(codstr, a);

	return n;
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_init_version)(uls_version_ptr_t a)
{
	uls_version_make(a, 0, 0, 0);
}

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_deinit_version)(uls_version_ptr_t a)
{
	uls_version_make(a, 0, 0, 0);
}
