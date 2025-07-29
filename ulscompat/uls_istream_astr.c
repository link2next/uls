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
  <file> uls_istream_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#include "uls/uls_istream_astr.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_alog.h"

ULS_DECL_STATIC int
__set_tmpl_value_astr(uls_tmpl_astr_ptr_t tmpl_ext, const char *name, const char *val)
{
	const char *astr_list[2] = { name, val };
	csz_str_ptr_t csz_ary[2];
	char *ustr;
	int i;
	uls_outparam_t parms1;

	csz_ary[0] = uls_ptr(tmpl_ext->name_ustr);
	csz_ary[1] = uls_ptr(tmpl_ext->val_ustr);

	for (i=0; i<2; i++) {
		if (astr_list[i] == NULL) {
			csz_reset(csz_ary[i]);
		} else {
			if ((ustr = uls_astr2ustr(astr_list[i], -1, csz_ary[i])) == NULL) {
				err_alog("%s: incorrect encoding!", __func__);
				return -1;
			}
		}
	}

	if (val != NULL) {
		astr_num_wchars(val, -1, uls_ptr(parms1));
		tmpl_ext->val_alen = parms1.len;
		tmpl_ext->val_asiz = tmpl_ext->val_alen + 1;
		uls_mfree(tmpl_ext->val_astr);
		tmpl_ext->val_astr = uls_astrdup(val, tmpl_ext->val_alen);
	} else {
		tmpl_ext->val_alen = tmpl_ext->val_asiz = 0;
		uls_mfree(tmpl_ext->val_astr);
	}

	return 0;
}

void
uls_init_tmpl_astr(uls_tmpl_astr_ptr_t tmpl_astr)
{
	csz_init(uls_ptr(tmpl_astr->name_ustr), 64);
	csz_init(uls_ptr(tmpl_astr->val_ustr), 128);

	tmpl_astr->val_astr = NULL;
	tmpl_astr->val_asiz = tmpl_astr->val_alen = 0;
}

void
uls_deinit_tmpl_astr(uls_tmpl_astr_ptr_t tmpl_astr)
{
	csz_deinit(uls_ptr(tmpl_astr->name_ustr));
	csz_deinit(uls_ptr(tmpl_astr->val_ustr));

	uls_mfree(tmpl_astr->val_astr);
	tmpl_astr->val_asiz = tmpl_astr->val_alen = 0;
}

int
uls_init_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags)
{
	uls_tmpl_list_astr_ptr_t ext;

	if (uls_init_tmpls(tmpl_list, n_alloc, flags) < 0) {
		return -1;
	}

	ext = uls_alloc_object(uls_tmpl_list_astr_t);
	uls_init_array_type10(uls_ptr(ext->tmpls_aext), tmpl_astr, n_alloc);
	tmpl_list->shell = ext;

	return 0;
}

int
uls_deinit_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list)
{
	uls_tmpl_list_astr_ptr_t ext = (uls_tmpl_list_astr_ptr_t) tmpl_list->shell;

	uls_deinit_array_type10(uls_ptr(ext->tmpls_aext), tmpl_astr);

	uls_dealloc_object(ext);
	tmpl_list->shell = nilptr;

	return uls_deinit_tmpls(tmpl_list);
}

uls_tmpl_list_ptr_t
uls_create_tmpls_astr(int n_alloc, int flags)
{
	uls_tmpl_list_ptr_t tmpl_list;

	tmpl_list = uls_alloc_object(uls_tmpl_list_t);

	if (uls_init_tmpls_astr(tmpl_list, n_alloc, flags) < 0) {
		uls_dealloc_object(tmpl_list);
		return nilptr;
	}

	tmpl_list->flags &= ~ULS_FL_STATIC;
	return tmpl_list;
}

void
uls_reset_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc)
{
	int theflags = tmpl_list->flags;

	uls_deinit_tmpls_astr(tmpl_list);
	uls_init_tmpls_astr(tmpl_list, n_alloc, 0);

	tmpl_list->flags = theflags;
}

int
uls_destroy_tmpls_astr(uls_tmpl_list_ptr_t tmpl_list)
{
	if (uls_deinit_tmpls_astr(tmpl_list) < 0) {
		return -1;
	}

	if (!(tmpl_list->flags & ULS_FL_STATIC)) {
		uls_dealloc_object(tmpl_list);
	}

	return 0;
}

uls_tmpl_t*
uls_find_tmpl_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	uls_tmpl_t *tmpl;

	if (name == NULL) return nilptr;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(name, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("%s: incorrect encoding!", __func__);
		tmpl = nilptr;
	} else {
		tmpl = uls_find_tmpl(tmpl_list, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return tmpl;
}

const char*
uls_get_tmpl_value_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name)
{
	uls_tmpl_list_astr_ptr_t ext = (uls_tmpl_list_astr_ptr_t) tmpl_list->shell;
	uls_tmpl_astr_ptr_t tmpl_ext;
	uls_tmpl_t *tmpl;

	if ((tmpl = uls_find_tmpl_astr(tmpl_list, name)) == nilptr) {
		return NULL;
	}

	tmpl_ext = uls_get_array_slot_type10(uls_ptr(ext->tmpls_aext), tmpl->idx);
	return tmpl_ext->val_astr;
}

int
uls_set_tmpl_value_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val)
{
	uls_tmpl_list_astr_ptr_t ext = (uls_tmpl_list_astr_ptr_t) tmpl_list->shell;
	uls_tmpl_astr_ptr_t tmpl_ext;
	uls_tmpl_t *tmpl;

	if ((tmpl = uls_find_tmpl_astr(tmpl_list, name)) == nilptr) {
		return -1;
	}

	tmpl_ext = uls_get_array_slot_type10(uls_ptr(ext->tmpls_aext), tmpl->idx);
	if (__set_tmpl_value_astr(tmpl_ext, name, val) < 0) {
		return -1;
	}

	return 0;
}

int
uls_add_tmpl_astr(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val)
{
	uls_tmpl_list_astr_ptr_t ext = (uls_tmpl_list_astr_ptr_t) tmpl_list->shell;
	uls_tmpl_astr_ptr_t tmpl_ext;
	char *nam_ustr, *val_ustr;
	int k;

	if ((k = tmpl_list->tmpls.n) >= tmpl_list->tmpls.n_alloc) {
		err_alog("Full of tmpl-array.");
		return -1;
	}

	uls_alloc_array_slot_type10(uls_ptr(ext->tmpls_aext), tmpl_astr, k);
	tmpl_ext = uls_get_array_slot_type10(uls_ptr(ext->tmpls_aext), k);

	if (__set_tmpl_value_astr(tmpl_ext, name, val) < 0) {
		return -1;
	}
	ext->tmpls_aext.n = k + 1;

	nam_ustr = csz_text(uls_ptr(tmpl_ext->name_ustr));
	if (*nam_ustr == '\0') nam_ustr = NULL;
	val_ustr = csz_text(uls_ptr(tmpl_ext->val_ustr));

	if (uls_add_tmpl(tmpl_list, nam_ustr, val_ustr) < 0) {
		return -1;
	}

	return 0;
}

void
uls_set_istream_atag(uls_istream_ptr_t istr, const char *atag)
{
	const char *ustr;
	auw_outparam_t buf_csz;

	if (atag == NULL) return;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(atag, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		uls_set_istream_tag(istr, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

uls_istream_ptr_t
uls_open_istream_file_astr(const char *filepath)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	uls_istream_ptr_t hdr;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_open_istream_file(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return hdr;
}

#ifdef ULS_FDF_SUPPORT
uls_istream_ptr_t
uls_open_istream_filter_file_astr(fdf_t* fdf, const char *filepath)
{
	uls_istream_ptr_t hdr;
	auw_outparam_t buf_csz;
	const char *ustr;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_open_istream_filter_file(fdf, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return hdr;
}
#endif
