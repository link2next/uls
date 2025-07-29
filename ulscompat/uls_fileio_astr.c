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

#include "uls/uls_fileio_astr.h"
#include "uls/uls_alog.h"

int
uls_dirent_exist_astr(const char *afpath)
{
	DWORD dwAttrib;
	int rval;

	if ((dwAttrib = GetFileAttributesA(afpath)) == INVALID_FILE_ATTRIBUTES) {
		return ST_MODE_NOENT;
	}

	if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) rval = ST_MODE_DIR;
	else rval = ST_MODE_FILE;

	return rval;
}

FILE*
uls_fp_aopen(const char *fpath, int mode)
{
	const char *ustr;
	auw_outparam_t buf_csz;
	FILE *fp;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(fpath, -1, uls_ptr(buf_csz))) == NULL) {
		fp = NULL;
	} else {
		fp = uls_fp_open(ustr, mode);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return fp;
}

int
uls_fp_gets_astr(FILE *fp, char *abuf, int abuf_siz, int flags)
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

int
uls_close_tempfile_astr(uls_tempfile_ptr_t tmpfile, const char *filepath)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	if (filepath == NULL) {
		return uls_close_tempfile(tmpfile, NULL);
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("encoding error!");
		rval = -1;
	} else {
		rval = uls_close_tempfile(tmpfile, ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

int
uls_fd_open_astr(const char *fpath, int mode)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(fpath, -1, uls_ptr(buf_csz))) == NULL) {
		err_alog("invalid param!");
		rval = -1;
	} else {
		rval = uls_fd_open(ustr, mode);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}
