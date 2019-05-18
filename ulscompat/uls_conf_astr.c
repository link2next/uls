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
  <file> uls_conf_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/
#include "uls/uls_conf.h"
#include "uls/uls_auw.h"
#include "uls/uls_util.h"
#include "uls/uls_alog.h"


ULS_DLL_EXTERN int
uls_is_valid_specpath_astr(const char* confname)
{
	int rval;
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(confname, -1, uls_ptr(buf_csz)))  == NULL) {
		err_alog("encoding error!");
		rval = -1;
	} else {
		rval = uls_is_valid_specpath(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

ULS_DLL_EXTERN int
uls_get_spectype_astr(const char *afpath, uls_outparam_ptr_t parms)
{
	char* specname = parms->line;
	const char *ufpath, *astr;
	int alen;
	int stat, len_dpath_ustr, l_specname_ustr;
	char specname_ustr[ULC_LONGNAME_MAXSIZ+1];
	auw_outparam_t ufpath_csz, abuf_csz;
	uls_outparam_t parms1;

	auw_init_outparam(uls_ptr(ufpath_csz));
	if ((ufpath = uls_astr2ustr_ptr(afpath, -1, uls_ptr(ufpath_csz))) == NULL) {
		err_alog("encoding error!");
		auw_deinit_outparam(uls_ptr(ufpath_csz));
		return -1;
	}

	parms1.line = specname_ustr;
	stat = uls_get_spectype(ufpath, uls_ptr(parms1));
	len_dpath_ustr = parms1.n;
	l_specname_ustr = parms1.len;

	auw_init_outparam(uls_ptr(abuf_csz));
	if ((astr = uls_ustr2astr_ptr(specname_ustr, -l_specname_ustr, uls_ptr(abuf_csz))) == NULL) {
		err_alog("encoding error!");
		auw_deinit_outparam(uls_ptr(ufpath_csz));
		auw_deinit_outparam(uls_ptr(abuf_csz));
		return -1;
	}

	alen = abuf_csz.outlen;

	uls_memcopy(specname, astr, alen);
	specname[alen] = '\0';
	parms->len = alen;

	if ((astr=uls_ustr2astr(ufpath, len_dpath_ustr, uls_ptr(abuf_csz.csz))) == NULL) {
		err_alog("encoding error!");
		stat = -1;
	} else {
		parms->n = csz_length(uls_ptr(abuf_csz.csz));
	}

	auw_deinit_outparam(uls_ptr(ufpath_csz));
	auw_deinit_outparam(uls_ptr(abuf_csz));

	return stat;
}

ULS_DLL_EXTERN void
ulc_list_searchpath_astr(const char* confname)
{
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(confname, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		ulc_list_searchpath(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

ULS_DLL_EXTERN void
ulc_set_searchpath_astr(const char *pathlist)
{
	const char *ustr;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(pathlist, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		ulc_set_searchpath(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
}

ULS_DLL_EXTERN int
ulc_add_searchpath_astr(const char *pathlist, int front)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(pathlist, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		rval = -1;
	} else {
		rval = ulc_add_searchpath(ustr, front);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}
