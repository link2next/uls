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
  <file> uls_lf_swprintf.c </file>
  <brief>
    An wide-char string version of uls_lf_sprintf.c.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#define __ULS_LF_SWPRINTF__
#include "uls/uls_lf_swprintf.h"
#include "uls/uls_util_wstr.h"
#include "uls/uls_lex.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC uls_lf_map_t dfl_convspec_wmap;


void
wlf_shell_init(uls_wlf_shell_ptr_t wlf, uls_lf_ptr_t uls_lf)
{
	csz_init(uls_ptr(wlf->fmtstr), 128*sizeof(wchar_t));
	wlf->uls_lf = uls_lf;
	uls_lf->shell = wlf;
}

void
wlf_shell_deinit(uls_wlf_shell_ptr_t wlf)
{
	csz_deinit(uls_ptr(wlf->fmtstr));
	wlf->uls_lf = nilptr;
}

void
initialize_uls_wlf(void)
{
	uls_lf_init_convspec_wmap(uls_ptr(dfl_convspec_wmap), 0);
	uls_add_default_convspecs(uls_ptr(dfl_convspec_wmap));
}

void
finalize_uls_wlf(void)
{
	uls_lf_deinit_convspec_wmap(uls_ptr(dfl_convspec_wmap));
}

ULS_DLL_EXTERN int
uls_lf_init_convspec_wmap(uls_lf_map_ptr_t lf_map, int flags)
{
	if (uls_lf_init_convspec_map(lf_map, flags) < 0)
		return -1;

	uls_lf_register_convspec(lf_map, "s", fmtproc_ws);
	uls_lf_register_convspec(lf_map, "S", fmtproc_ws);

	return 0;
}

ULS_DLL_EXTERN void
uls_lf_deinit_convspec_wmap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_map(lf_map);
}

ULS_DLL_EXTERN uls_lf_map_ptr_t
uls_lf_create_convspec_wmap(int flags)
{
	uls_lf_map_ptr_t lf_map;

	lf_map = (uls_lf_map_ptr_t ) uls_zalloc(sizeof(uls_lf_map_t));
	if (uls_lf_init_convspec_wmap(lf_map, flags) < 0) {
		return nilptr;
	}

	lf_map->flags &= ~ULS_FL_STATIC;
	return lf_map;
}

ULS_DLL_EXTERN void
uls_lf_destroy_convspec_wmap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_wmap(lf_map);

	if ((lf_map->flags & ULS_FL_STATIC) == 0) {
		uls_mfree(lf_map);
	}
}

ULS_DLL_EXTERN int
uls_wlf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_wlf_shell_ptr_t wlf;

	if (map == nilptr) map = uls_ptr(dfl_convspec_wmap);

	if (uls_lf_init(uls_lf, map, x_dat, puts_proc) < 0)
		return -1;

	wlf = uls_alloc_object(uls_wlf_shell_t);
	wlf_shell_init(wlf, uls_lf);

	return 0;
}

ULS_DLL_EXTERN void
uls_wlf_deinit(uls_lf_ptr_t uls_lf)
{
	uls_wlf_shell_ptr_t wlf = (uls_wlf_shell_ptr_t) uls_lf->shell;

	wlf_shell_deinit(wlf);
	uls_dealloc_object(wlf);
	uls_lf->shell = nilptr;

	uls_lf_deinit(uls_lf);
}

ULS_DLL_EXTERN uls_lf_ptr_t
uls_wlf_create(uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_lf_ptr_t uls_lf;

	uls_lf = uls_alloc_object(uls_lf_t);

	if (uls_wlf_init(uls_lf, map, x_dat, puts_proc) < 0) {
		return nilptr;
	}

	return uls_lf;
}

ULS_DLL_EXTERN void
uls_wlf_destroy(uls_lf_ptr_t uls_lf)
{
	uls_wlf_deinit(uls_lf);
	uls_dealloc_object(uls_lf);
}

ULS_DLL_EXTERN int
__uls_lf_vxwprintf(uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	uls_wlf_shell_ptr_t lf_wctx = (uls_wlf_shell_ptr_t) uls_lf->shell;
	char *ustr;
	int  len;

	if ((ustr = uls_wstr2ustr(wfmt, -1, uls_ptr(lf_wctx->fmtstr))) == nilptr) {
		len = -1;
	} else {
		len = __uls_lf_vxprintf(uls_lf, ustr, args);
		len /= sizeof(wchar_t);
	}

	return len;
}

ULS_DLL_EXTERN int
uls_lf_vxwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxwprintf(uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_xwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = __uls_lf_vxwprintf(uls_lf, wfmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_xwprintf(uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = uls_lf_vxwprintf(uls_lf, wfmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_vxwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args)
{
	uls_voidptr_t old_xdat;
	int len;

	old_xdat = __uls_lf_change_xdat(uls_lf, x_dat);
	len = __uls_lf_vxwprintf(uls_lf, wfmt, args);
	__uls_lf_change_xdat(uls_lf, old_xdat);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_vxwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxwprintf_generic(x_dat, uls_lf, wfmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_xwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = __uls_lf_vxwprintf_generic(x_dat, uls_lf, wfmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_xwprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t* wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = uls_lf_vxwprintf_generic(x_dat, uls_lf, wfmt, args);
	va_end(args);

	return len;
}
