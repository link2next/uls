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
  <file> ms_mbcs_file.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jun 2015.
  </author>
*/
#include "uls/uls_istream.h"
#include "uls/ms_mbcs_file.h"
#include "uls/uls_fileio_astr.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_auw.h"
#include "uls/uls_alog.h"

#define MBCS2UTF8_LINESIZE ((1<<14)-2)

ULS_DECL_STATIC int
conv_mbcs2utf8(FILE *fp, FILE *fpout)
{
	int linebuff_len, stat=0;
	char *linebuff;
	int ulen;
	const char *ustr;
	auw_outparam_t buf_csz;

	linebuff = (char *) uls_malloc(MBCS2UTF8_LINESIZE+2);
	auw_init_outparam(uls_ptr(buf_csz));

	while (1) {
		if ((linebuff_len=uls_fp_gets_astr(fp, linebuff, MBCS2UTF8_LINESIZE, 0)) <= ULS_EOF) {
			if (linebuff_len < ULS_EOF) {
				err_alog("%s: error to read a line", __FUNCTION__);
				stat =-1;
			}
			break;
		}

		linebuff[linebuff_len++] = '\n';
		linebuff[linebuff_len] = '\0';

		// linebuff_len > 0
		if ((ustr = uls_astr2ustr_ptr(linebuff, -1, uls_ptr(buf_csz))) == NULL) {
			stat = -3;
			break;
		}

		ulen = buf_csz.outlen;

		if (fwrite(ustr, sizeof(char), ulen, fpout) < (unsigned long) ulen) {
			stat = -4;
			break;
		}
	}

	rewind(fpout);

	auw_deinit_outparam(uls_ptr(buf_csz));
	uls_mfree(linebuff);

	return stat;
}

void
ms_mbcs_tmpf_ungrabber(uls_voidptr_t data)
{
	uls_tempfile_ptr_t tmpfile = (uls_tempfile_ptr_t ) data;
	uls_destroy_tempfile(tmpfile);
}

void
ms_mbcs_fp_ungrabber(uls_voidptr_t data)
{
	uls_outparam_ptr_t parm = (uls_outparam_ptr_t) data;
	FILE *fp = (FILE *) parm->native_data;

	uls_dealloc_object(parm);
	uls_fp_close(fp);
}

void
ms_mbcs_str_ungrabber(uls_voidptr_t data)
{
	uls_outparam_ptr_t parm = (uls_outparam_ptr_t) data;
	char *str = (char *) parm->line;

	uls_dealloc_object(parm);
	uls_mfree(str);
}

ULS_DLL_EXTERN FILE*
cvt_ms_mbcs_fp(FILE *fp, uls_tempfile_ptr_t tmpfile_utf8, int flags)
{
	FILE *fp2;

	if ((flags & (ULS_FILE_MS_MBCS|ULS_FILE_UTF8)) == 0) {
#ifdef ULS_WINDOWS
		flags |= ULS_FILE_MS_MBCS;
#else
		flags |= ULS_FILE_UTF8;
#endif
	}

	if (_uls_get_raw_input_subtype(fp) < 0 && (flags & ULS_FILE_MS_MBCS)) {
		if ((fp2 = uls_fopen_tempfile(tmpfile_utf8)) == NULL) {
			err_alog("%s: can't create a temp-file!", __FUNCTION__);
			return NULL;
		}

		if (conv_mbcs2utf8(fp, fp2) < 0) {
			err_alog("%s: can't convert input to mbcs!", __FUNCTION__);
			return NULL;
		}
		fp = fp2;
	}

	return fp;
}

ULS_DLL_EXTERN FILE*
cvt_ms_mbcs_filepath_ustr(const char* filepath, uls_tempfile_ptr_t tmpfile_utf8, int flags)
{
	FILE  *fp, *fp2;

	if ((fp = uls_fp_open(filepath, ULS_FIO_READ)) == NULL) { // filepath : UTF-8
		err_alog("%s: open error!", __FUNCTION__);
		return NULL;
	}

	if ((fp2 = cvt_ms_mbcs_fp(fp, tmpfile_utf8, flags)) != fp) {
		uls_fp_close(fp);
	}

	return fp2;
}

ULS_DLL_EXTERN FILE*
cvt_ms_mbcs_filepath_wstr(const wchar_t* wfilepath, uls_tempfile_ptr_t tmpfile_utf8, int flags)
{
	char *ustr;
	csz_str_t csz;
	FILE *fp;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfilepath, -1, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
		fp = NULL;
	} else {
		fp = cvt_ms_mbcs_filepath_ustr(ustr, tmpfile_utf8, flags);
	}

	csz_deinit(uls_ptr(csz));
	return fp;
}

