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
  <file> uls_util_astr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/

#include "uls/uls_lf_saprintf.h"
#include "uls/uls_fileio_astr.h"
#include "uls/uls_util_astr.h"
#include "uls/uls_aprint.h"
#include "uls/uls_alog.h"

#include "uls/ms_mbcs_file.h"

void
uls_aputstr(const char *astr)
{
	int alen = (int) strlen(astr);
	uls_put_binstr(astr, alen, _uls_stdio_fd(1));
}

void
uls_dump_utf8str_astr(const char *astr)
{
	int alen = (int) uls_strlen(astr);
	char *ustr;
	csz_str_t csz;

	csz_init(uls_ptr(csz), alen + 1);

	if ((ustr = uls_astr2ustr(astr, alen, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		uls_dump_utf8str(ustr);
	}

	csz_deinit(uls_ptr(csz));
}

int
uls_astr2int(const char *astr)
{
	int i, minus, n = 0;
	char ch;

	if (astr[0] == '-') {
		minus = 1;
		i = 1;
	} else {
		minus = 0;
		i = 0;
	}

	for ( ; ; i++) {
		if ((ch=astr[i]) > '9' || ch < '0')
			break;
		n = ch - '0' + n * 10;
	}

	if (minus) n = -n;
	return n;
}

int
uls_astrlen(const char *astr)
{
	const char *aptr;

	for (aptr = astr; *aptr != '\0'; aptr++)
		/* NOTHING */;

	return (int) (aptr - astr);
}

int
uls_astrcpy(char *bufptr, const char *astr)
{
	const char *ptr;
	char ch;

	for (ptr=astr; (ch=*ptr) != '\0'; ptr++) {
		*bufptr++ = ch;
	}

	*bufptr = '\0';
	return (int) (ptr - astr);
}

int
uls_astrcmp(const char *astr1, const char *astr2)
{
	const char *ptr1=astr1, *ptr2=astr2;
	char ch1, ch2;
	int i, stat = 0;

	for (i=0; ; i++, ptr1++, ptr2++) {
		ch1 = *ptr1;
		ch2 = *ptr2;

		if (ch1 != ch2) {
			stat = (int) ch1 - (int) ch2;
			break;
		}

		if (ch1 == '\0') break;
	}

	return stat;
}

int
uls_astreql(const char *astr1, const char *astr2)
{
	return !uls_astrcmp(astr1, astr2);
}

int
uls_path_normalize_astr(const char *fpath, char *fpath2)
{
	char *ustr, *abuf2;
	int ulen, alen2;
	csz_str_t csz, csz_astr2;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_astr2ustr(fpath, -1, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
		csz_deinit(uls_ptr(csz));
		return -1;
	}

	if ((ulen = uls_path_normalize(ustr, ustr)) < 0) {
		csz_deinit(uls_ptr(csz));
		return -1;
	}

	csz_init(uls_ptr(csz_astr2), -1);

	abuf2 = uls_ustr2astr(ustr, -ulen, uls_ptr(csz_astr2));
	csz_deinit(uls_ptr(csz));

	if (abuf2 == NULL) {
		err_alog("encoding error!");
		csz_deinit(uls_ptr(csz_astr2));
		return -1;
	}

	alen2 = csz_length(uls_ptr(csz_astr2));
	uls_memcopy(fpath2, abuf2, alen2);
	fpath2[alen2] = '\0';

	csz_deinit(uls_ptr(csz_astr2));
	return alen2;
}

int
uls_mkdir_astr(const char *filepath)
{
	const char *ustr;
	int  rval;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = uls_mkdir(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

char*
uls_astrdup(const char *astr, int alen)
{
	char *abuf;
	int i;
	uls_outparam_t parms1;

	if (alen < 0) {
		astr_num_wchars(astr, -1, uls_ptr(parms1));
		alen = parms1.len;
	}

	abuf = (char *) uls_malloc(alen+1);
	for (i=0; i<alen; i++) abuf[i] = astr[i];
	abuf[alen] = '\0';

	return abuf;
}

char*
uls_astrchr(const char *lptr, char ch0)
{
	char ch;
	int i;

	for (i=0; (ch=lptr[i]) != '\0'; i++) {
		if (ch == ch0) return (char *) (lptr + i);
	}

	if (ch0 == '\0') return (char *) (lptr + i);
	return NULL;
}

int
_uls_explode_astr(uls_wrd_ptr_t uw, char delim_ch, uls_arglst_ptr_t arglst)
{
	char  *lptr = uw->lptr, *lptr1, ch;
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	uls_argstr_ptr_t arg;
	int   rc, k, alen;

	for (k = 0; k < arglst->args.n_alloc; ) {
		if (delim_ch == ' ') {
			for ( ; (rc=astr_lengthof_char(lptr)) == 1; lptr+=rc) {
				if ((ch=*lptr) != ' ' && ch != '\t')
					break;
			}
		}

		for (lptr1=lptr; ; lptr+=rc) {
			if ((rc=astr_lengthof_char(lptr)) > 1) {
				continue;
			}

			if (rc == 0) {
				if (lptr != lptr1) {
					alen = (int) (lptr - lptr1);
					if ((arg=al[k]) == NULL) {
						al[k] = arg = uls_create_argstr();
					}
					uls_set_argstr(arg, lptr1, alen);
					++k;
				}
				goto end_1;
			}

			if ((ch=*lptr) == delim_ch || (delim_ch == ' ' && ch == '\t')) {
				alen = (int) (lptr - lptr1);
				*lptr++ = '\0';
				if ((arg=al[k]) == NULL) {
					al[k] = arg = uls_create_argstr();
				}
				uls_set_argstr(arg, lptr1, alen);
				++k;
				break;
			}
		}
	}

 end_1:
	uw->lptr = lptr;
	arglst->args.n = k;

	return k;
}

int
uls_explode_astr(char **ptr_line, char delim_ch, char **args, int n_args)
{
	uls_wrd_t wrdx;
	uls_arglst_t arglst;
	uls_argstr_ptr_t arg;
	uls_decl_parray_slots(al_wrds, argstr);
	int n_wrd, i;

	wrdx.lptr = *ptr_line;

	uls_init_arglst(uls_ptr(arglst), n_args);

	n_wrd = _uls_explode_astr(uls_ptr(wrdx), delim_ch, uls_ptr(arglst));
	al_wrds = uls_parray_slots(uls_ptr(arglst.args));
	for (i=0; i<n_wrd; i++) {
		arg = al_wrds[i];
		args[i] = arg->str;
	}

	*ptr_line = wrdx.lptr;
	uls_deinit_arglst(uls_ptr(arglst));

	return n_wrd;
}

int
uls_getopts_astr(int n_args, char *args[], const char *optfmt, uls_optproc_t proc)
{
	return uls_getopts(n_args, args, optfmt, proc);
}

FILE*
cvt_ms_mbcs_filepath_astr(const char *filepath, uls_tempfile_ptr_t tmpfile_utf8, int flags)
{
	char *ustr;
	csz_str_t csz;
	FILE *fp;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_astr2ustr(filepath, -1, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
		fp = NULL;
	} else {
		fp = cvt_ms_mbcs_filepath_ustr(ustr, tmpfile_utf8, flags);
	}

	csz_deinit(uls_ptr(csz));

	return fp;
}
