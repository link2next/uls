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
  <file> uls_util_wstr.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jan 2015.
  </author>
*/
#include "uls/uls_util_wstr.h"
#include "uls/uls_wlog.h"
#include "uls/uls_fileio.h"


void
uls_wputstr(const wchar_t *wstr)
{
	char *austr;
	int aulen;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

#ifdef ULS_WINDOWS
	austr = uls_wstr2astr(wstr, -1, uls_ptr(csz));
#else
	austr = uls_wstr2ustr(wstr, -1, uls_ptr(csz));
#endif

	if (austr == NULL) {
		err_wlog(L"encoding error!");
	} else {
		aulen = csz_length(uls_ptr(csz));
		uls_put_binstr(austr, aulen, _uls_stdio_fd(1));
	}

	csz_deinit(uls_ptr(csz));
}

int
uls_path_normalize_wstr(const wchar_t* wfpath, wchar_t* wfpath2)
{
	wchar_t *wbuf2;
	int wlen2;
	char *ustr;
	int  ulen;
	csz_str_t csz, csz_wstr2;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		err_wlog(L"encoding error!");
		csz_deinit(uls_ptr(csz));
		return -1;
	}

	if ((ulen = uls_path_normalize(ustr, ustr)) < 0) {
		csz_deinit(uls_ptr(csz));
		return -1;
	}

	csz_init(uls_ptr(csz_wstr2), -1);

	wbuf2 = uls_ustr2wstr(ustr, -ulen, uls_ptr(csz_wstr2));
	csz_deinit(uls_ptr(csz));

	if (wbuf2 == NULL) {
		err_wlog(L"encoding error!");
		csz_deinit(uls_ptr(csz_wstr2));
		return -1;
	}

	wlen2 = auw_csz_wlen(uls_ptr(csz_wstr2));
	uls_memcopy(wfpath2, wbuf2, wlen2*sizeof(wchar_t));
	wfpath2[wlen2] = L'\0';

	csz_deinit(uls_ptr(csz_wstr2));
	return wlen2;
}

int
uls_mkdir_wstr(const wchar_t *wfilepath)
{
	char *ustr;
	int  rval;
	csz_str_t csz;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfilepath, -1, uls_ptr(csz))) == NULL) {
		rval = -1;
	} else {
		rval = uls_mkdir(ustr);
	}

	csz_deinit(uls_ptr(csz));
	return rval;
}

wchar_t*
uls_wstrdup(const wchar_t* wstr, int wlen)
{
	wchar_t *wbuf;
	int i;

	if (wlen < 0) {
		wlen = uls_wcslen(wstr);
	}

	wbuf = uls_malloc((wlen+1) * sizeof(wchar_t));
	for (i=0; i<wlen; i++) wbuf[i] = wstr[i];
	wbuf[wlen] = L'\0';

	return wbuf;
}

wchar_t*
uls_wstrchr(const wchar_t* wlptr, wchar_t wch0)
{
	wchar_t wch;
	int i;

	for (i=0; (wch=wlptr[i]) != L'\0'; i++) {
		if (wch == wch0) return (wchar_t *) (wlptr + i);
	}

	if (wch0 == L'\0') return (wchar_t *) (wlptr + i);
	return NULL;
}

int
_uls_explode_wstr(uls_wrd_ptr_t uw, wchar_t delim_wch, uls_arglst_ptr_t arglst)
{
	wchar_t  *wlptr= (wchar_t *) uw->lptr, *wlptr1, wch;
	uls_decl_parray_slots_init(al, argstr, uls_ptr(arglst->args));
	uls_argstr_ptr_t arg;
	int   k, wlen;

	for (k=0; k < arglst->args.n_alloc; ) {
		if (delim_wch == L' ') {
			for ( ; (wch=*wlptr) == L' ' || wch == L'\t'; wlptr++)
				/* NOTHING */;
		}

		for (wlptr1=wlptr; ; wlptr++) {
			if ((wch=*wlptr) == L'\0') {
				wlen = (int) (wlptr - wlptr1);

				if (wlptr != wlptr1) {
					if ((arg=al[k]) == NULL) {
						al[k] = arg = uls_create_argstr();
					}
					arg->buf = NULL;
					arg->buf_siz = 0;
					arg->str = (char *) wlptr1;
					arg->len = wlen * sizeof(wchar_t);
					++k;
				}

				uw->lptr = (char *) wlptr;
				return k;
			}

			if (wch == delim_wch || (delim_wch == L' ' && wch == L'\t')) {
				wlen = (int) (wlptr - wlptr1);
				*wlptr++ = L'\0';

				if ((arg=al[k]) == NULL) {
					al[k] = arg = uls_create_argstr();
				}
				arg->buf = NULL;
				arg->buf_siz = 0;
				arg->str = (char *) wlptr1;
				arg->len = wlen * sizeof(wchar_t);
				++k;
				break;
			}
		}
	}

	uw->lptr = (char *) wlptr;
	return k;
}

int
uls_explode_wstr(wchar_t **ptr_wline, wchar_t delim_wch, wchar_t** args, int n_args)
{
	uls_wrd_t wrdx;
	uls_arglst_t arglst;
	uls_argstr_ptr_t arg;
	uls_decl_parray_slots(al_wrds, argstr);
	int n_wrd, i;

	wrdx.lptr = (char *) *ptr_wline;

	uls_init_arglst(uls_ptr(arglst), n_args);

	n_wrd = _uls_explode_wstr(uls_ptr(wrdx), delim_wch, uls_ptr(arglst));
	al_wrds = uls_parray_slots(uls_ptr(arglst.args));
	for (i=0; i<n_wrd; i++) {
		arg = al_wrds[i];
		args[i] = (wchar_t *) arg->str;
	}

	*ptr_wline = (wchar_t *) wrdx.lptr;
	uls_deinit_arglst(uls_ptr(arglst));

	return n_wrd;
}

int
uls_getopts_wstr(int n_args, wchar_t* wargs[], const wchar_t* optwfmt, uls_woptproc_t wproc)
{
	const wchar_t  *cwptr;
	wchar_t        *optwarg, *optwstr;
	int            rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (wargs[i][0] != L'-') break;

		optwstr = uls_ptr(wargs)[i][1];
		for (k=i,j=0; (opt=optwstr[j]) != L'\0'; ) {
			if (opt == L'?') {
				return 0; // call usage();
			}

			if ((cwptr=uls_wstrchr(optwfmt, opt)) == NULL) {
				err_wlog(L"%s: undefined option -%c", __FUNCTION__, opt);
				return -1;
			}

			if (cwptr[1] == L':') { /* the option 'opt' needs a arg-val */
				if (optwstr[j+1] != L'\0') {
					optwarg = optwstr + (j+1);
				} else if (k+1 < n_args && wargs[k+1][0] != L'-') {
					optwarg = wargs[++k];
				} else {
					err_wlog(L"%s: option -%c requires an arg.", __FUNCTION__, opt);
					return -1;
				}

				if ((rc=wproc(opt, optwarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_wlog(L"%s: error in -%c.", __FUNCTION__, opt);
					return rc;
				}
				break;

			} else {
				optwarg = L"";
				if ((rc=wproc(opt, optwarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_wlog(L"%s: error in -%c.", __FUNCTION__, opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}

int
uls_lf_wputs_csz(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	csz_str_ptr_t csz = (csz_str_t *) x_dat;
	wchar_t *wstr;
	int wlen, rc;
	csz_str_t csz_wstr;

	if (wrdptr == NULL) {
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL) {
		rc = -1;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		rc = wlen * sizeof(wchar_t);
		csz_append(csz, (uls_voidptr_t ) wstr, rc);
	}

	csz_deinit(uls_ptr(csz_wstr));

	return rc;
}

int
uls_lf_wputs_str(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	uls_buf4wstr_ptr_t wlb = (uls_buf4wstr_ptr_t) x_dat;
	wchar_t *wstr;
	int wlen, rc;
	csz_str_t csz_wstr;

	if (wrdptr == NULL) {
		if (wlb->wbuflen <= 0) {
			return -1;
		}
		*wlb->wbufptr = L'\0';
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL ||
		(wlen = auw_csz_wlen(uls_ptr(csz_wstr))) >= wlb->wbuflen) {
		rc = -1;
	} else {
		rc = wlen * sizeof(wchar_t);
		uls_memcopy(wlb->wbufptr, wstr, rc);
		wlb->wbufptr += wlen;
		wlb->wbuflen -= wlen;
	}

	csz_deinit(uls_ptr(csz_wstr));

	return rc;
}

int
uls_lf_wputs_file(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen)
{
	FILE *fout = (FILE *) x_dat;
	wchar_t *wstr;
	int wlen, rc;
	csz_str_t csz_wstr;

	if (wrdptr == NULL) {
		fflush(fout);
		return 0;
	}

	csz_init(uls_ptr(csz_wstr), -1);

	if ((wstr = uls_ustr2wstr(wrdptr, wrdlen, uls_ptr(csz_wstr))) == NULL) {
		rc = -1;
	} else {
		wlen = auw_csz_wlen(uls_ptr(csz_wstr));
		rc = wlen * sizeof(wchar_t);
		if (fwrite(wstr, sizeof(wchar_t), wlen, fout) < (size_t) wlen) {
			rc = -1;
		}
	}

	csz_deinit(uls_ptr(csz_wstr));

	return rc;
}

// <brief>
// convert the list of string to wide strings.
// </brief>
void
uls_set_warg_list(uls_warg_list_t *wlist, char **args, int n_args)
{
	csz_str_t csz;
	wchar_t *wstr, *wstr2;
	int i, wlen;

	uls_reset_warg_list(wlist);
	if (n_args <= 0) {
		return;
	}

	wlist->wargs = (uls_warg_t*) uls_malloc(n_args * sizeof(uls_warg_t));
	wlist->n_wargs = n_args;

	csz_init(&csz, -1);

	for (i=0; i < n_args; i++) {
#ifdef ULS_WINDOWS
		wstr = uls_astr2wstr(args[i], -1, &csz);
#else
		wstr = uls_ustr2wstr(args[i], -1, &csz);
#endif
		wlen = csz.len / sizeof(wchar_t);
		wlist->wargs[i].wstr = wstr2 = (wchar_t *) uls_malloc((wlen + 1) * sizeof(wchar_t));
		uls_memcopy(wstr2, wstr, wlen * sizeof(wchar_t));
		wstr2[wlen] = L'\0';
	}

	csz_deinit(&csz);
}

void
uls_init_warg_list(uls_warg_list_t *wlist)
{
	wlist->wargs = NULL;
	wlist->n_wargs = 0;
}

// <brief>
// release the memory of list of wide strings.
// </brief>
void
uls_reset_warg_list(uls_warg_list_t *wlist)
{
	wchar_t *wstr;
	int i;

	for (i = 0; i < wlist->n_wargs; i++) {
		wstr = wlist->wargs[i].wstr;
		uls_mfree(wstr);
	}

	uls_mfree(wlist->wargs);
	wlist->wargs = NULL;
	wlist->n_wargs = 0;
}

void
uls_deinit_warg_list(uls_warg_list_t *wlist)
{
	uls_reset_warg_list(wlist);
}

uls_warg_t*
uls_get_warg(uls_warg_list_t *wlist, int ith)
{
	if (ith >= wlist->n_wargs || ith < 0) {
		return NULL;
	}

	return wlist->wargs + ith;
}

wchar_t**
uls_export_warg_list(uls_warg_list_t *wlist, int *ptr_n_wargs)
{
	wchar_t **wargs2 = (wchar_t **) uls_malloc(wlist->n_wargs * sizeof(wchar_t *));
	int i;

	for (i=0; i<wlist->n_wargs; i++) {
		wargs2[i] = wlist->wargs[i].wstr;
	}

	if (ptr_n_wargs != NULL) {
		*ptr_n_wargs = wlist->n_wargs;
	}

	uls_mfree(wlist->wargs);
	wlist->wargs = NULL;
	wlist->n_wargs = 0;

	return wargs2;
}

wchar_t**
ulscompat_get_warg_list(char **argv, int n_argv)
{
	wchar_t **wargv;

	uls_warg_list_t wlist;

	uls_init_warg_list(&wlist);
	uls_set_warg_list(&wlist, argv, n_argv);

	wargv = uls_export_warg_list(&wlist, NULL);
	uls_deinit_warg_list(&wlist);

	return wargv;

}

void
ulscompat_put_warg_list(wchar_t **wargv, int n_wargv)
{
	int i;

	for (i=0; i < n_wargv; i++) {
		uls_mfree(wargv[i]);
	}

	uls_mfree(wargv);
}
