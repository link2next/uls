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
  <file> uls_emit_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jun 2015.
  </author>
*/
#include "uls/uls_lex.h"
#include "uls/uls_util.h"

#include "uls/uls_emit.h"
#include "uls/uls_emit_wstr.h"
#include "uls/uls_auw.h"
#include "uls/uls_wlog.h"


ULS_DLL_EXTERN int
uls_init_parms_emit_wstr(uls_parms_emit_ptr_t emit_parm,
	const wchar_t *out_wdpath, const wchar_t *out_wfname, const wchar_t *wfpath_uld,
	const wchar_t *ulc_wname, const wchar_t *class_wname, const wchar_t *enum_wname,
	const wchar_t *tok_wpfx, uls_flags_t flags)
{
	uls_emit_wshell_ptr_t wext = uls_alloc_object(uls_emit_wshell_t);
	int i, stat = 0;

	wext->wrdlst[0] = out_wdpath;
	wext->wrdlst[1] = out_wfname;
	wext->wrdlst[2] = wfpath_uld;
	wext->wrdlst[3] = ulc_wname;
	wext->wrdlst[4] = class_wname;
	wext->wrdlst[5] = enum_wname;
	wext->wrdlst[6] = tok_wpfx;

	for (i=0; i < N_PARMS_EMIT_WSTR; i++) {
		if (wext->wrdlst[i] == NULL) {
			wext->ustr[i] = NULL;

		} else {
			csz_init(wext->csz_wrd_ary + i, -1);

			if ((wext->ustr[i] = uls_wstr2ustr(wext->wrdlst[i], -1, wext->csz_wrd_ary + i)) == NULL) {
				err_wlog(L"encoding error!");
				stat = -1;
				break;
			}
		}
	}

	if (stat < 0) {
		csz_deinit(wext->csz_wrd_ary + i);
		for (--i; i>=0; i--) {
			if (wext->ustr[i] != NULL) {
				csz_deinit(wext->csz_wrd_ary + i);
			}
		}
		uls_dealloc_object(wext);
	} else {
		emit_parm->ext_data = (uls_voidptr_t) wext;
		uls_init_parms_emit(emit_parm,
			wext->ustr[0], wext->ustr[1], wext->ustr[2],
			wext->ustr[3], wext->ustr[4], wext->ustr[5],
			wext->ustr[6], flags);
	}

	return stat;
}

ULS_DLL_EXTERN int
uls_deinit_parms_emit_wstr(uls_parms_emit_ptr_t emit_parm)
{
	uls_emit_wshell_ptr_t wext = (uls_emit_wshell_ptr_t) emit_parm->ext_data;
	int i, stat;

	stat = uls_deinit_parms_emit(emit_parm);

	for (i=0; i<N_PARMS_EMIT_WSTR; i++) {
		if (wext->ustr[i] != NULL) {
			csz_deinit(wext->csz_wrd_ary + i);
		}
	}

	uls_dealloc_object(wext);
	return stat;
}
