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
  <file> uls_lf_saprintf.h </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..) for MS-MBCS encoding.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/
#define __ULS_LF_SAPRINTF__
#include "uls/uls_lf_saprintf.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_lex.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC uls_lf_map_t dfl_convspec_amap;


ULS_DECL_STATIC int
fmtproc_as(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_t* lf_ctx)
{
	int ulen, rc;
	const char *astr, *ustr;
	auw_outparam_t buf_csz;

	astr = (char *) va_arg(lf_ctx->args, char *);
	if (astr == NULL) astr = "<NULL>";

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(astr, -1, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		ulen = buf_csz.outlen;
		rc = ustr_num_chars(ustr, ulen, nilptr);
		rc = uls_lf_fill_mbstr(x_dat, puts_proc, uls_ptr(lf_ctx->perfmt), ustr, ulen, rc);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}

void
alf_shell_init(uls_alf_shell_ptr_t alf, uls_lf_ptr_t uls_lf)
{
	csz_init(uls_ptr(alf->fmtstr), 128);
	alf->uls_lf = uls_lf;
	uls_lf->shell = alf;
}

void
alf_shell_deinit(uls_alf_shell_ptr_t alf)
{
	csz_deinit(uls_ptr(alf->fmtstr));
	alf->uls_lf = nilptr;
}

void
initialize_uls_alf(void)
{
	uls_lf_init_convspec_amap(uls_ptr(dfl_convspec_amap), 0);
	uls_add_default_convspecs(uls_ptr(dfl_convspec_amap));
}

void
finalize_uls_alf(void)
{
	uls_lf_deinit_convspec_amap(uls_ptr(dfl_convspec_amap));
}

ULS_DLL_EXTERN int
uls_lf_init_convspec_amap(uls_lf_map_ptr_t lf_map, int flags)
{
	if (uls_lf_init_convspec_map(lf_map, flags) < 0)
		return -1;

	uls_lf_register_convspec(lf_map, "s", fmtproc_as);
	uls_lf_register_convspec(lf_map, "S", fmtproc_ws);

	return 0;
}

ULS_DLL_EXTERN void
uls_lf_deinit_convspec_amap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_map(lf_map);
}

ULS_DLL_EXTERN uls_lf_map_ptr_t
uls_lf_create_convspec_amap(int flags)
{
	uls_lf_map_ptr_t lf_map;

	lf_map = (uls_lf_map_ptr_t ) uls_zalloc(sizeof(uls_lf_map_t));
	if (uls_lf_init_convspec_amap(lf_map, flags) < 0) {
		return nilptr;
	}

	lf_map->flags &= ~ULS_FL_STATIC;
	return lf_map;
}

ULS_DLL_EXTERN void
uls_lf_destroy_convspec_amap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_amap(lf_map);

	if ((lf_map->flags & ULS_FL_STATIC) == 0) {
		uls_mfree(lf_map);
	}
}

ULS_DLL_EXTERN int
uls_alf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_alf_shell_ptr_t alf;

	if (map == nilptr) map = uls_ptr(dfl_convspec_amap);

	if (uls_lf_init(uls_lf, map, x_dat, puts_proc) < 0)
		return -1;

	alf = uls_alloc_object(uls_alf_shell_t);
	alf_shell_init(alf, uls_lf);

	return 0;
}

ULS_DLL_EXTERN void
uls_alf_deinit(uls_lf_ptr_t uls_lf)
{
	uls_alf_shell_ptr_t alf = (uls_alf_shell_ptr_t) uls_lf->shell;

	alf_shell_deinit(alf);
	uls_dealloc_object(alf);
	uls_lf->shell = nilptr;

	uls_lf_deinit(uls_lf);
}

ULS_DLL_EXTERN uls_lf_ptr_t
uls_alf_create(uls_lf_map_ptr_t map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc)
{
	uls_lf_ptr_t uls_lf;

	uls_lf = uls_alloc_object(uls_lf_t);

	if (uls_alf_init(uls_lf, map, x_dat, puts_proc) < 0) {
		return nilptr;
	}

	return uls_lf;
}

ULS_DLL_EXTERN void
uls_alf_destroy(uls_lf_ptr_t uls_lf)
{
	uls_alf_deinit(uls_lf);
	uls_dealloc_object(uls_lf);
}

ULS_DLL_EXTERN int
__uls_lf_vxaprintf(uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	uls_alf_shell_ptr_t lf_actx = (uls_alf_shell_ptr_t) uls_lf->shell;
	char *ustr;
	int len;

	if ((ustr=uls_astr2ustr(afmt, -1, uls_ptr(lf_actx->fmtstr))) == NULL) {
		len = -1;
	} else {
		len = __uls_lf_vxprintf(uls_lf, ustr, args);
	}

	return len;
}

ULS_DLL_EXTERN int
uls_lf_vxaprintf(uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxaprintf(uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_xaprintf(uls_lf_ptr_t uls_lf, const char* afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = __uls_lf_vxaprintf(uls_lf, afmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_xaprintf(uls_lf_ptr_t uls_lf, const char* afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = uls_lf_vxaprintf(uls_lf, afmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_vxaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	uls_voidptr_t old_xdat;
	int len;

	old_xdat = __uls_lf_change_xdat(uls_lf, x_dat);
	len = __uls_lf_vxaprintf(uls_lf, afmt, args);
	__uls_lf_change_xdat(uls_lf, old_xdat);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_vxaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxaprintf_generic(x_dat, uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

ULS_DLL_EXTERN int
__uls_lf_xaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = __uls_lf_vxaprintf_generic(x_dat, uls_lf, afmt, args);
	va_end(args);

	return len;
}

ULS_DLL_EXTERN int
uls_lf_xaprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = uls_lf_vxaprintf_generic(x_dat, uls_lf, afmt, args);
	va_end(args);

	return len;
}
