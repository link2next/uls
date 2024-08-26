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
 * uls_file.c -- The file for UTF-16, UTF-32 source code --
 *     written by Stanley Hong <link2next@gmail.com>, Oct 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_UTF_FILE__
#include "uls/utf_file.h"
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#endif

int
ULS_QUALIFIED_METHOD(id_range_comp)(const uls_voidptr_t e, const uls_voidptr_t b)
{
	const uls_ptrtype_tool(uch_range) id_range = (const uls_ptrtype_tool(uch_range)) e;
	const uls_ptrtype_tool(outparam) parms = (const uls_ptrtype_tool(outparam)) b;
	uls_wch_t  val = (uls_wch_t) parms->x1;

	// x2 < UNIT_MAX
	if (id_range->x2 < val) {
		return -1;
	} else if (val < id_range->x1) {
		// 0 < x1
		return 1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(is_utf_id)(uls_wch_t wch)
{
	uls_ptrtype_tool(uch_range) id_range;
	uls_type_tool(outparam) parms1;

	parms1.x1 = (unsigned int) wch;
#ifdef ULS_CLASSIFY_SOURCE
	id_range = (uls_ptrtype_tool(uch_range)) _uls_bisearch_vptr((uls_voidptr_t) uls_ptr(parms1),
		uls_array_slots_type01(uls_ptr(id_range_list1)), id_range_list1.n, id_range_comp);
#else
	id_range = (uls_ptrtype_tool(uch_range)) uls_bi_search((uls_voidptr_t) uls_ptr(parms1),
		(uls_native_vptr_t) uls_array_slots_type01(uls_ptr(id_range_list1)), id_range_list1.n, sizeof(uls_uch_range_t), id_range_comp);
#endif
	if (id_range != nilptr) {
		return 1; // true
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(load_uch_ranges_list)(void)
{
	char linebuff[ULS_LINEBUFF_SIZ+1];
	uls_type_tool(arglst) wrdlst;
	int  len, i, k, n_wrdlst, n_lines, rc, stat=0;

	uls_ptrtype_tool(uch_range) id_range;
	uls_wch_t val1, val2;
	char fpath[ULS_FILEPATH_MAX+1];
	FILE *fin;

	uls_type_tool(wrd) wrdx;
	uls_decl_parray_slots_tool(al_wrds, argstr);

	len = _uls_tool_(strcpy)(fpath, _uls_sysinfo_(etc_dir));
	fpath[len++] = ULS_FILEPATH_DELIM;
	len += _uls_tool_(strcpy)(fpath + len, ULS_ID_RANGES_FNAME);

	if ((fin = _uls_tool_(fp_open)(fpath, ULS_FIO_READ)) == NULL) {
		len = _uls_tool_(strcpy)(fpath, _uls_sysinfo_(etc_dir));
		fpath[len++] = ULS_FILEPATH_DELIM;
		len += _uls_tool_(strcpy)(fpath + len, TMP_ID_RANGES_FNAME);
		if ((fin = _uls_tool_(fp_open)(fpath, ULS_FIO_READ)) == NULL) {
			_uls_log(err_log)("Can't open the file '%s' for uch_ranges!", fpath);
			return -1;
		}
	}

	if ((len=_uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0)) < 1 || linebuff[0] != '#') {
		_uls_log(err_log)("ULS: can't read %s!", fpath);
		_uls_tool_(fp_close)(fin);
		return -1;
	}

	wrdx.lptr = linebuff + 1; // next to '#'
	_uls_tool_(init_arglst)(uls_ptr(wrdlst), 2);

	if ((n_wrdlst = __uls_tool_(explode_str)(uls_ptr(wrdx), ' ', 0, uls_ptr(wrdlst))) < 1) {
		_uls_log(err_log)("%s: incorrect format of the header data!", fpath);
		_uls_tool_(fp_close)(fin);
		_uls_tool_(deinit_arglst)(uls_ptr(wrdlst));
		return -1;
	}

	al_wrds = uls_parray_slots(uls_ptr(wrdlst.args));
	// n_lines: # of records
	n_lines = _uls_tool_(atoi)(al_wrds[0]->str);
	uls_init_array_tool_type01(uls_ptr(id_range_list1), uch_range, n_lines);

	k = 0;
	for (i=1; i <= n_lines; i++) {
		if ((len=_uls_tool_(fp_gets)(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) stat = -1;
			break;
		}

		wrdx.lptr = linebuff;
		if ((rc=__uls_tool_(explode_str)(uls_ptr(wrdx), ' ', 0, uls_ptr(wrdlst))) == 0) {
			continue;
		}

		/* charset range */
		_uls_tool_(get_xrange)(al_wrds[0]->str, &val1, &val2);

		id_range = uls_get_array_slot_type01(uls_ptr(id_range_list1), k);
		id_range->x1 = val1;
		id_range->x2 = val2;
		k++;
	}

	_uls_tool_(deinit_arglst)(uls_ptr(wrdlst));
	_uls_tool_(fp_close)(fin);

	if (k < n_lines || n_lines <= 0) {
		_uls_log(err_log)("%s: internal error to read lines!", fpath);
		stat = -2;
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(unload_uch_ranges_list)(void)
{
	uls_deinit_array_tool_type01(uls_ptr(id_range_list1), uch_range);
}
