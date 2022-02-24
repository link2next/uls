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
  <file> uls_istream_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#include "uls/uls_lex.h"
#include "uls/uls_auw.h"
#include "uls/uls_util.h"

#include "uls/uls_istream_wstr.h"
#include "uls/uls_util_wstr.h"
#include "uls/uls_wlog.h"

ULS_DECL_STATIC int
__set_tmpl_value_wstr(uls_tmpl_wstr_ptr_t tmpl_ext, const wchar_t *name, const wchar_t *val)
{
	const wchar_t *wstr_list[2] = { name, val };
	csz_str_ptr_t csz_ary[2];
	char *ustr;
	int i;

	csz_ary[0] = uls_ptr(tmpl_ext->name_ustr);
	csz_ary[1] = uls_ptr(tmpl_ext->val_ustr);

	for (i=0; i<2; i++) {
		if (wstr_list[i] == NULL) {
			csz_reset(csz_ary[i]);

		} else {
			if ((ustr = uls_wstr2ustr(wstr_list[i], -1, csz_ary[i])) == NULL) {
				err_wlog(L"%s: incorrect encoding!", __FUNCTION__);
				return -1;
			}
		}
	}

	if (val != NULL) {
		tmpl_ext->val_wlen = uls_wcslen(val);
		tmpl_ext->val_wsiz = tmpl_ext->val_wlen + 1;
		uls_mfree(tmpl_ext->val_wstr);
		tmpl_ext->val_wstr = uls_wstrdup(val, tmpl_ext->val_wlen);
	} else {
		tmpl_ext->val_wlen = tmpl_ext->val_wsiz = 0;
		uls_mfree(tmpl_ext->val_wstr);
	}

	return 0;
}

void
__init_tmpls_wstr(uls_tmpl_wstr_ptr_t tmpl_ary, int n)
{
	uls_tmpl_wstr_ptr_t tmpl;
	int i;

	for (i=0; i<n; i++) {
		tmpl = tmpl_ary + i;

		csz_init(uls_ptr(tmpl->name_ustr), 64);
		csz_init(uls_ptr(tmpl->val_ustr), 128);

		tmpl->val_wstr = NULL;
		tmpl->val_wsiz = tmpl->val_wlen = 0;
	}
}

void
uls_init_tmpl_wstr(uls_tmpl_wstr_ptr_t tmpl_wstr)
{
	csz_init(uls_ptr(tmpl_wstr->name_ustr), 64);
	csz_init(uls_ptr(tmpl_wstr->val_ustr), 128);

	tmpl_wstr->val_wstr = NULL;
	tmpl_wstr->val_wsiz = tmpl_wstr->val_wlen = 0;
}

void
uls_deinit_tmpl_wstr(uls_tmpl_wstr_ptr_t tmpl_wstr)
{
	csz_deinit(uls_ptr(tmpl_wstr->name_ustr));
	csz_deinit(uls_ptr(tmpl_wstr->val_ustr));

	uls_mfree(tmpl_wstr->val_wstr);
	tmpl_wstr->val_wsiz = tmpl_wstr->val_wlen = 0;
}

int
uls_init_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags)
{
	uls_tmpl_list_wstr_ptr_t ext;

	if (uls_init_tmpls(tmpl_list, n_alloc, flags) < 0) {
		return -1;
	}

	ext = uls_alloc_object(uls_tmpl_list_wstr_t);
	uls_init_array_type10(uls_ptr(ext->tmpls_wext), tmpl_wstr, n_alloc);
	tmpl_list->shell = ext;

	return 0;
}

int
uls_deinit_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list)
{
	uls_tmpl_list_wstr_ptr_t ext = (uls_tmpl_list_wstr_ptr_t) tmpl_list->shell;

	uls_deinit_array_type10(uls_ptr(ext->tmpls_wext), tmpl_wstr);

	uls_dealloc_object(ext);
	tmpl_list->shell = nilptr;

	return uls_deinit_tmpls(tmpl_list);
}

uls_tmpl_list_ptr_t
uls_create_tmpls_wstr(int n_alloc, int flags)
{
	uls_tmpl_list_ptr_t tmpl_list;

	tmpl_list = uls_alloc_object(uls_tmpl_list_t);

	if (uls_init_tmpls_wstr(tmpl_list, n_alloc, flags) < 0) {
		uls_dealloc_object(tmpl_list);
		return nilptr;
	}

	tmpl_list->flags &= ~ULS_FL_STATIC;
	return tmpl_list;
}

void
uls_reset_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list, int n_alloc)
{
	int theflags = tmpl_list->flags;

	uls_deinit_tmpls_wstr(tmpl_list);
	uls_init_tmpls_wstr(tmpl_list, n_alloc, 0);

	tmpl_list->flags = theflags;
}

int
uls_destroy_tmpls_wstr(uls_tmpl_list_ptr_t tmpl_list)
{
	if (uls_deinit_tmpls_wstr(tmpl_list) < 0) {
		return -1;
	}

	if (!(tmpl_list->flags & ULS_FL_STATIC)) {
		uls_dealloc_object(tmpl_list);
	}

	return 0;
}

uls_tmpl_t*
uls_find_tmpl_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t *name)
{
	char *ustr;
	csz_str_t csz;
	uls_tmpl_t *tmpl;

	if (name == NULL) return nilptr;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(name, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"%s: incorrect encoding!", __FUNCTION__);
		tmpl = nilptr;
	} else {
		tmpl = uls_find_tmpl(tmpl_list, ustr);
	}

	csz_deinit(uls_ptr(csz));
	return tmpl;
}

const wchar_t*
uls_get_tmpl_value_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t *name)
{
	uls_tmpl_list_wstr_ptr_t ext = (uls_tmpl_list_wstr_ptr_t) tmpl_list->shell;
	uls_tmpl_wstr_ptr_t tmpl_ext;
	uls_tmpl_t *tmpl;

	if ((tmpl = uls_find_tmpl_wstr(tmpl_list, name)) == nilptr) {
		return NULL;
	}

	tmpl_ext = uls_array_get_slot_type10(uls_ptr(ext->tmpls_wext), tmpl->idx);
	return tmpl_ext->val_wstr;
}

int
uls_set_tmpl_value_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t* name, const wchar_t *val)
{
	uls_tmpl_list_wstr_ptr_t ext = (uls_tmpl_list_wstr_ptr_t) tmpl_list->shell;
	uls_tmpl_wstr_ptr_t tmpl_ext;
	uls_tmpl_t *tmpl;

	if ((tmpl = uls_find_tmpl_wstr(tmpl_list, name)) == nilptr) {
		return -1;
	}

	tmpl_ext = uls_array_get_slot_type10(uls_ptr(ext->tmpls_wext), tmpl->idx);
	if (__set_tmpl_value_wstr(tmpl_ext, name, val) < 0) {
		return -1;
	}

	return 0;
}

int
uls_add_tmpl_wstr(uls_tmpl_list_ptr_t tmpl_list, const wchar_t *name, const wchar_t *val)
{
	uls_tmpl_list_wstr_ptr_t ext = (uls_tmpl_list_wstr_ptr_t) tmpl_list->shell;
	uls_tmpl_wstr_ptr_t tmpl_ext;
	char *nam_ustr, *val_ustr;
	int k;

	if ((k = tmpl_list->tmpls.n) >= tmpl_list->tmpls.n_alloc) {
		err_wlog(L"Full of tmpl-array.");
		return -1;
	}

	uls_array_alloc_slot_type10(uls_ptr(ext->tmpls_wext), tmpl_wstr, k);
	tmpl_ext = uls_array_get_slot_type10(uls_ptr(ext->tmpls_wext), k);

	if (__set_tmpl_value_wstr(tmpl_ext, name, val) < 0) {
		return -1;
	}
	ext->tmpls_wext.n = k + 1;

	nam_ustr = csz_text(uls_ptr(tmpl_ext->name_ustr));
	if (*nam_ustr == '\0') nam_ustr = NULL;
	val_ustr = csz_text(uls_ptr(tmpl_ext->val_ustr));

	if (uls_add_tmpl(tmpl_list, nam_ustr, val_ustr) < 0) {
		return -1;
	}

	return 0;
}

void
uls_set_istream_wtag(uls_istream_ptr_t istr, const wchar_t* wtag)
{
	char *ustr;
	csz_str_t csz;

	if (wtag == NULL) return;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wtag, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
	} else {
		uls_set_istream_tag(istr, ustr);
	}

	csz_deinit(uls_ptr(csz));
}

uls_istream_ptr_t
uls_open_istream_file_wstr(const wchar_t* filepath)
{
	char *ustr;
	csz_str_t csz;
	uls_istream_ptr_t hdr;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(filepath, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_open_istream_file(ustr);
	}

	csz_deinit(uls_ptr(csz));
	return hdr;
}

#ifdef ULS_FDF_SUPPORT
uls_istream_ptr_t
uls_open_istream_filter_file_wstr(fdf_t* fdf, const wchar_t* wfilepath)
{
	char *ustr;
	uls_istream_ptr_t hdr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfilepath, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		hdr = nilptr;
	} else {
		hdr = uls_open_istream_filter_file(fdf, ustr);
	}

	csz_deinit(uls_ptr(csz));
	return hdr;
}
#endif
