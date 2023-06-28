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
  <file> ulscompat.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/

#include "uls.h"
#define __ULSCOMPAT__
#include "uls/ulscompat.h"

#include "uls/uls_util_wstr.h"
#include "uls/uls_lf_swprintf.h"
#include "uls/uls_wprint.h"
#include "uls/uls_wlog.h"

#include <stdlib.h>

ULS_DECL_STATIC int ulscompat_inited;

void
#ifdef __GNUC__
__attribute__((constructor))
#endif
_initialize_ulscompat(void)
{
	if (ulscompat_inited) return;
	initialize_uls_wlf();
	initialize_uls_wprint();
	initialize_uls_wlog();
	ulscompat_inited = 1;
}

void
#ifdef __GNUC__
__attribute__((destructor))
#endif
_finalize_ulscompat(void)
{
	if (!ulscompat_inited) return;
	finalize_uls_wlog();
	finalize_uls_wprint();
	finalize_uls_wlf();
	ulscompat_inited = 0;
}

void
initialize_ulscompat(void)
{
	initialize_uls();
	_initialize_ulscompat();
}

void
finalize_ulscompat(void)
{
	_finalize_ulscompat();
	finalize_uls();
}

#if defined(ULS_WINDOWS)
BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	BOOL rval = TRUE;

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		initialize_ulscompat();
		if (!ulscompat_inited) rval = FALSE;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		finalize_ulscompat();
		if (ulscompat_inited) rval = FALSE;
		break;
	}

	return rval;
}
#endif

