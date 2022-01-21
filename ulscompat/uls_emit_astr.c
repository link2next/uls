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
  <file> uls_emit_astr.c </file>
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
#include "uls/uls_emit_astr.h"
#include "uls/uls_alog.h"


ULS_DLL_EXTERN int
uls_init_parms_emit_astr(uls_parms_emit_ptr_t emit_parm,
	const char *out_dpath, const char *out_fname, const char *fpath_uld,
	const char *ulc_name, const char *class_name, const char *enum_name,
	const char *tok_pfx, int flags)
{
	uls_emit_ashell_ptr_t aext = uls_alloc_object(uls_emit_ashell_t);
	int i, stat = 0;

	aext->wrdlst[0] = out_dpath;
	aext->wrdlst[1] = out_fname;
	aext->wrdlst[2] = fpath_uld;
	aext->wrdlst[3] = ulc_name;
	aext->wrdlst[4] = class_name;
	aext->wrdlst[5] = enum_name;
	aext->wrdlst[6] = tok_pfx;

	for (i=0; i < N_PARMS_EMIT_ASTR; i++) {
		if (aext->wrdlst[i] == NULL) {
			aext->ustr[i] = NULL;
		} else {
			auw_init_outparam(aext->buf_csz + i);

			if ((aext->ustr[i] = uls_astr2ustr_ptr(aext->wrdlst[i], -1, aext->buf_csz + i)) == NULL) {
				err_alog("encoding error!");
				stat = -1;
				break;
			}
		}
	}

	if (stat < 0) {
		auw_deinit_outparam(aext->buf_csz + i);
		for (--i; i>=0; i--) {
			if (aext->ustr[i] != NULL) {
				auw_deinit_outparam(aext->buf_csz + i);
			}
		}
		uls_dealloc_object(aext);
	} else {
		emit_parm->ext_data = (uls_voidptr_t) aext;
		uls_init_parms_emit(emit_parm,
			aext->ustr[0], aext->ustr[1], aext->ustr[2],
			aext->ustr[3], aext->ustr[4], aext->ustr[5],
			aext->ustr[6], flags);
	}

	return stat;
}

ULS_DLL_EXTERN int
uls_deinit_parms_emit_astr(uls_parms_emit_ptr_t emit_parm)
{
	uls_emit_ashell_ptr_t aext = (uls_emit_ashell_ptr_t) emit_parm->ext_data;
	int i, stat;

	stat = uls_deinit_parms_emit(emit_parm);

	for (i=0; i<N_PARMS_EMIT_ASTR; i++) {
		if (aext->ustr[i] != NULL) {
			auw_deinit_outparam(aext->buf_csz + i);
		}
	}

	uls_dealloc_object(aext);
	return stat;
}
