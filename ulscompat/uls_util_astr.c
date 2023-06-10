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
#include "uls/uls_util_astr.h"
#include "uls/uls_alog.h"
#include "uls/ms_mbcs_file.h"


ULS_DLL_EXTERN void
uls_aputstr(const char *astr)
{
#ifdef ULS_WINDOWS
	uls_put_binstr(astr, -1, _uls_stdio_fd(1));
#else
	char *austr;
	int aulen;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((austr = uls_astr2ustr(astr, -1, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
	} else {
		aulen = csz_length(uls_ptr(csz));
		uls_put_binstr(austr, aulen, _uls_stdio_fd(1));
	}

	csz_deinit(uls_ptr(csz));
#endif
}

ULS_DLL_EXTERN int
uls_path_normalize_astr(const char* fpath, char* fpath2)
{
	char *ustr, *abuf2;
	int ulen, alen2;
	csz_str_t csz, csz_astr;

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

	csz_init(uls_ptr(csz_astr), -1);

	abuf2 = uls_ustr2astr(ustr, -ulen, uls_ptr(csz_astr));
	csz_deinit(uls_ptr(csz));

	if (abuf2 == NULL) {
		err_alog("encoding error!");
		csz_deinit(uls_ptr(csz_astr));
		return -1;
	}

	alen2 = csz_length(uls_ptr(csz_astr));
	uls_memcopy(fpath2, abuf2, alen2);
	fpath2[alen2] = '\0';

	csz_deinit(uls_ptr(csz_astr));
	return alen2;
}

ULS_DLL_EXTERN int
uls_mkdir_astr(const char *afilepath)
{
	const char *ustr;
	int rval;
	auw_outparam_t buf_csz;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(afilepath, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = uls_mkdir(ustr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

ULS_DLL_EXTERN char*
uls_astrdup(const char* astr, int alen)
{
	char *abuf;
	int i;
	uls_outparam_t parms1;

	if (alen < 0) {
		astr_num_chars(astr, -1, uls_ptr(parms1));
		alen = parms1.len;
	}

	abuf = (char *) uls_malloc(alen+1);
	for (i=0; i<alen; i++) abuf[i] = astr[i];
	abuf[alen] = '\0';

	return abuf;
}

ULS_DLL_EXTERN char*
uls_astrchr(const char* alptr, char ach0)
{
	char ach;
	int i;

	for (i=0; (ach=alptr[i]) != '\0'; i++) {
		if (ach == ach0) return (char *) (alptr + i);
	}

	if (ach0 == '\0') return (char *) (alptr + i);
	return NULL;
}

ULS_DLL_EXTERN int
_uls_explode_astr(uls_wrd_ptr_t uw, char delim_ach, uls_arglst_ptr_t arglst)
{
	char  *alptr=uw->lptr, *alptr1, ch;
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	uls_argstr_ptr_t arg;
	int   rc, k, alen;

	for (k = 0; k < arglst->args.n_alloc; ) {
		if (delim_ach == ' ') {
			for ( ; (rc=astr_lengthof_char(alptr)) == 1; alptr+=rc) {
				if ((ch=*alptr) != ' ' && ch != '\t')
					break;
			}
		}

		for (alptr1=alptr; ; alptr+=rc) {
			if ((rc=astr_lengthof_char(alptr)) > 1) {
				continue;
			}

			if (rc == 0) {
				if (alptr != alptr1) {
					alen = (int) (alptr - alptr1);
					if ((arg=al[k]) == NULL) {
						al[k] = arg = uls_create_argstr();
					}
					uls_set_argstr(arg, alptr1, alen);
					++k;
				}
				goto end_1;
			}

			if ((ch=*alptr) == delim_ach || (delim_ach == ' ' && ch == '\t')) {
				alen = (int) (alptr - alptr1);
				*alptr++ = '\0';
				if ((arg=al[k]) == NULL) {
					al[k] = arg = uls_create_argstr();
				}
				uls_set_argstr(arg, alptr1, alen);
				++k;
				break;
			}
		}
	}

 end_1:
	uw->lptr = alptr;
	arglst->args.n = k;

	return k;
}

ULS_DLL_EXTERN int
uls_explode_astr(char **ptr_aline, char delim_ach, char** args, int n_args)
{
	uls_wrd_t wrdx;
	uls_arglst_t wrdlst;
	uls_argstr_ptr_t argstr;
	uls_decl_parray_slots(al_wrds, argstr);
	int n_wrd, i;

	wrdx.lptr = *ptr_aline;

	uls_init_arglst(uls_ptr(wrdlst), n_args);

	n_wrd = _uls_explode_astr(uls_ptr(wrdx), delim_ach, uls_ptr(wrdlst));
	al_wrds = uls_parray_slots(uls_ptr(wrdlst.args));
	for (i=0; i<n_wrd; i++) {
		argstr = al_wrds[i];
		args[i] = argstr->str;
	}

	*ptr_aline = wrdx.lptr;
	uls_deinit_arglst(uls_ptr(wrdlst));

	return n_wrd;

}

ULS_DLL_EXTERN int
uls_getopts_astr(int n_args, char* args[], const char* optfmt, uls_optproc_t proc)
{
	return uls_getopts(n_args, args, optfmt, proc);
}

ULS_DLL_EXTERN FILE*
cvt_ms_mbcs_filepath_astr(const char* afilepath, uls_tempfile_ptr_t tmpfile_utf8, int flags)
{
	char *ustr;
	csz_str_t csz;
	FILE *fp;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_astr2ustr(afilepath, -1, uls_ptr(csz))) == NULL) {
		err_alog("encoding error!");
		fp = NULL;
	} else {
		fp = cvt_ms_mbcs_filepath_ustr(ustr, tmpfile_utf8, flags);
	}

	csz_deinit(uls_ptr(csz));

	return fp;
}

ULS_DLL_EXTERN int
uls_lf_aputs_csz(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	csz_str_ptr_t csz = (csz_str_t *) x_dat;
	int alen, rc;
	const char *astr;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		csz_text(csz);
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		rc = alen = buf_csz.outlen;
		csz_append(csz, astr, alen);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return rc;
}

ULS_DLL_EXTERN int
uls_lf_aputs_str(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	uls_buf4str_t *alb = (uls_buf4str_t *) x_dat;
	const char *astr;
	int alen, rc;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		if (alb->bufsiz <= 0) {
			return -1;
		}
		*alb->bufptr = '\0';
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if (wrdlen >= alb->bufsiz ||
		(astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		rc = -1;
	} else {
		rc = alen = buf_csz.outlen;
		uls_memcopy(alb->bufptr, astr, alen);
		alb->bufptr += alen;
		alb->bufsiz -= alen;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rc;
}

ULS_DLL_EXTERN int
uls_lf_aputs_file(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;
	char ch, buff_crlf[2] = { '\r', '\n' };
	int alen, i1, i, rc, n;
	const char *astr;
	auw_outparam_t buf_csz;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(wrdptr, wrdlen, uls_ptr(buf_csz))) == NULL) {
		auw_deinit_outparam(uls_ptr(buf_csz));
		return -1;
	}

	wrdptr = astr;
	wrdlen = alen = buf_csz.outlen;

	for (n = i1 = i = 0; ; i++) {
		if (i >= wrdlen) {
			if ((rc = i - i1) > 0 && fwrite(wrdptr + i1, sizeof(char), rc, fout) != rc) {
				wrdlen = -1;
			}
			n += rc;
			break;
		}

		if ((ch = wrdptr[i]) == '\n') {
			if ((rc = i - i1) > 0 && fwrite(wrdptr + i1, sizeof(char), rc, fout) != rc) {
				wrdlen = -1;
				break;
			}

			if (fwrite(buff_crlf, sizeof(char), 2, fout) != 2) {
				wrdlen = -1;
				break;
			}

			i1 += rc + 1; // +1 == '\n'
			n += rc + 2;  // +2 == '\r' '\n'
		}
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return wrdlen;
}
