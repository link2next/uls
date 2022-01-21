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
  <file> uls_fileio_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"
#include "uls/uls_auw.h"

#include "uls/uls_alog.h"

ULS_DLL_EXTERN FILE*
uls_fp_aopen(const char *afpath, int mode)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	FILE *fp;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(afpath, -1, uls_ptr(buf_csz))) == NULL) {
		fp = NULL;
	} else {
		fp = uls_fp_open(ustr, mode);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return fp;
}

ULS_DLL_EXTERN int
uls_fp_gets_astr(FILE* fp, char* abuf, int abuf_siz, int flags)
{
	uls_fio_t fio;
	uls_stdio_box_t fpwrap;
	int rc;

	uls_init_fio(uls_ptr(fio), flags|ULS_FIO_DEL_CR);

	fpwrap.fp_num = -1;
	fpwrap.fp = fp;
	uls_reset_fio(uls_ptr(fio), uls_ptr(fpwrap), consume_ms_mbcs_onechar);

	rc = uls_fio_gets(uls_ptr(fio), abuf, abuf_siz);

	uls_deinit_fio(uls_ptr(fio));

	return rc;
}

ULS_DLL_EXTERN int
uls_close_tempfile_astr(uls_tempfile_ptr_t tmpfile, const char* afilepath)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	if (afilepath == NULL) {
		return uls_close_tempfile(tmpfile, NULL);
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(afilepath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		rval = -1;
	} else {
		rval = uls_close_tempfile(tmpfile, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

ULS_DLL_EXTERN int
uls_fd_open_astr(const char* afpath, int mode)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(afpath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("invalid param!");
		rval = -1;
	} else {
		rval = uls_fd_open(ustr, mode);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}
