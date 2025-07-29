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
  <file> uls_lf_saprintf.c </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..) for MS-MBCS encoding.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/

#include "uls/uls_lf_saprintf.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_alog.h"

ULS_DECL_STATIC uls_lf_map_t dfl_convspec_amap;

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

int
uls_lf_init_convspec_amap(uls_lf_map_ptr_t lf_map, int flags)
{
	if (uls_lf_init_convspec_map(lf_map, flags) < 0)
		return -1;

	uls_lf_register_convspec(lf_map, "s", fmtproc_as);
	uls_lf_register_convspec(lf_map, "S", fmtproc_ws);

	return 0;
}

void
uls_lf_deinit_convspec_amap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_map(lf_map);
}

uls_lf_map_ptr_t
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

void
uls_lf_destroy_convspec_amap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_amap(lf_map);

	if ((lf_map->flags & ULS_FL_STATIC) == 0) {
		uls_mfree(lf_map);
	}
}

int
uls_alf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t map, uls_lf_puts_t puts_proc)
{
	uls_alf_shell_ptr_t alf;

	if (map == nilptr) map = uls_ptr(dfl_convspec_amap);

	if (uls_lf_init(uls_lf, map, puts_proc) < 0)
		return -1;

	alf = uls_alloc_object(uls_alf_shell_t);
	alf_shell_init(alf, uls_lf);

	return 0;
}

void
uls_alf_deinit(uls_lf_ptr_t uls_lf)
{
	uls_alf_shell_ptr_t alf = (uls_alf_shell_ptr_t) uls_lf->shell;

	alf_shell_deinit(alf);
	uls_dealloc_object(alf);
	uls_lf->shell = nilptr;

	uls_lf_deinit(uls_lf);
}

uls_lf_ptr_t
uls_alf_create(uls_lf_map_ptr_t map, uls_lf_puts_t puts_proc)
{
	uls_lf_ptr_t uls_lf;

	uls_lf = uls_alloc_object(uls_lf_t);

	if (uls_alf_init(uls_lf, map, puts_proc) < 0) {
		return nilptr;
	}

	return uls_lf;
}

void
uls_alf_destroy(uls_lf_ptr_t uls_lf)
{
	uls_alf_deinit(uls_lf);
	uls_dealloc_object(uls_lf);
}

int
__uls_lf_vxaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	uls_alf_shell_ptr_t lf_actx = (uls_alf_shell_ptr_t) uls_lf->shell;
	char *ustr;
	int len;

	if ((ustr = uls_astr2ustr(afmt, -1, uls_ptr(lf_actx->fmtstr))) == NULL) {
		len = -1;
	} else {
		len = __uls_lf_vxprintf(x_dat, uls_lf, ustr, args);
	}

	return len;
}

int
uls_lf_vxaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	int len;

	uls_lf_lock(uls_lf);
	len = __uls_lf_vxaprintf(x_dat, uls_lf, afmt, args);
	uls_lf_unlock(uls_lf);

	return len;
}

int
__uls_lf_xaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = __uls_lf_vxaprintf(x_dat, uls_lf, afmt, args);
	va_end(args);

	return len;
}

int
uls_lf_xaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = uls_lf_vxaprintf(x_dat, uls_lf, afmt, args);
	va_end(args);

	return len;
}
