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
  <file> uls_util.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_UTIL__
#include "uls/uls_util.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#include "uls/uls_auw.h"

#include <stdlib.h>
#include <string.h>
#ifndef ULS_WINDOWS
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__os_mkdir_ustr)(const char *filepath)
{
	int rval;
#ifdef ULS_WINDOWS
	const char *astr;
	_uls_tool_type(auw_outparam) buf_csz;

	_uls_tool(auw_init_outparam)(uls_ptr(buf_csz));

	if ((astr = _uls_tool_(ustr2astr_ptr)(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = _mkdir(astr);
		if (rval != 0) rval = -1;
	}

	_uls_tool(auw_deinit_outparam)(uls_ptr(buf_csz));
#else
	rval = mkdir(filepath, 0755);
#endif
	return rval;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_mkdir_ustr)(const char *filepath0)
{
	char fpath_buf[ULS_FILEPATH_MAX+3];
	char *filepath, *fpath, *ptr, ch_bak;
	int  n, i, rc, len, len1;
	_uls_tool_type_(outparam) parms;

	len = _uls_tool_(strcpy)(fpath_buf, filepath0);
	filepath = fpath_buf;
	ptr = filepath + len;

#ifdef ULS_WINDOWS
	if ((len1 = get_volume_name(filepath)) == len) {
		return -1;
	}
#else
	len1 = 0;
#endif
	if (len1 > 0) {
		fpath = fpath_buf + len1 + 1; // +1 : next to '/'

		ch_bak = *fpath;
		*fpath = '\0';

		if ((rc=_uls_tool_(dirent_exist)(fpath_buf)) < 0 || rc != ST_MODE_DIR)
			return -1;
		*fpath = ch_bak;
	} else {
		fpath = path_skip_uplevel(fpath_buf);
	}

	if ((rc=_uls_tool_(dirent_exist)(filepath)) < 0 || rc == ST_MODE_FILE) {
		return -1;
	} else if (rc == ST_MODE_DIR) {
		return 0;
	}

	for (n=1; ; n++) {
		parms.lptr = fpath;
		ptr = __uls_tool_(filename)(uls_ptr(parms));

		if (fpath < ptr) {
			ptr[-1] = '\0';
		} else {
			break;
		}

		if ((rc=_uls_tool_(dirent_exist)(filepath)) < 0) {
			return -1;
		} else if (rc == ST_MODE_DIR) {
			ptr[-1] = ULS_FILEPATH_DELIM;
			break;
		}
	}

	for (i=0; i<n; i++) {
		len = _uls_tool_(strlen)(ptr);
		if (__os_mkdir_ustr(filepath) < 0) {
			_uls_log(err_log)("mkdir('%s') of %s failed", filepath);
			return -1;
		}

		ptr += len;
		*ptr++ = ULS_FILEPATH_DELIM;
	}

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_get_exeloc_by_pathenv)(const char *progname, char *fpath_buf)
{
	const char *pathlist, *dirptr;
	int dirlen, fpath_buflen=-1;
	_uls_tool_type_(outparam) parms;

	if (progname == NULL || *progname == '\0') return -1;

	if ((pathlist=getenv("PATH")) != NULL) {
		parms.lptr = pathlist;
		dirptr = uls_get_dirpath(progname, uls_ptr(parms));
		dirlen = parms.len;

		if (dirptr != NULL) {
			_uls_tool_(memcopy)(fpath_buf, dirptr, dirlen);
			fpath_buflen = dirlen;

			fpath_buf[fpath_buflen++] = ULS_FILEPATH_DELIM;
			_uls_tool_(strcpy)(fpath_buf+fpath_buflen, progname);
			fpath_buflen += _uls_tool_(strlen)(progname);
		}
	}

	return fpath_buflen;
}

int
ULS_QUALIFIED_METHOD(is_filepath_delim)(char ch)
{
	int rc = 0;
#ifdef ULS_WINDOWS
	if (ch == ULS_FILEPATH_DELIM || ch == '/') rc = 1;
#else
	if (ch == ULS_FILEPATH_DELIM) rc = 1;
#endif
	return rc;
}

int
ULS_QUALIFIED_METHOD(is_absolute_path)(const char* path)
{
	int stat;

	if (*path == ULS_FILEPATH_DELIM) stat = 1;
#ifdef ULS_WINDOWS
	else if (_uls_tool_(isalpha)(path[0]) && path[1] == ':') stat = 1;
#endif
	else stat = 0;

	return stat;
}

int
ULS_QUALIFIED_METHOD(is_path_prefix)(const char *filepath)
{
	int i, stat=0;
	char ch;

	if (filepath == NULL || *filepath == '\0') {
		return -1;
	}

	if (is_absolute_path(filepath))
		return 1;

	for (i=0; ; i++) {
		if ((ch=filepath[i]) == '\0' || ch == ULS_FILEPATH_DELIM) {
			if (i == 1 || (i == 2 && filepath[1] == '.')) {
				if (filepath[0] == '.') stat = 1;
			}
			break;
		}
	}

	return stat;
}

FILE*
ULS_QUALIFIED_METHOD(uls_get_spec_fp)(const char* dirpath_list, const char* fpath, _uls_tool_ptrtype_(outparam) parms)
{
	char filepath_buff[ULS_FILEPATH_MAX+1];
	const char *fptr, *lptr0, *lptr;
	int len, len_fptr;
	FILE *fp_in;
	_uls_tool_type_(outparam) parms1;

	if (fpath == NULL) return NULL;

	if (dirpath_list == NULL || is_absolute_path(fpath)) {
		fp_in = _uls_tool_(fp_open)(fpath, ULS_FIO_READ);

		parms1.lptr = fpath;
		lptr = __uls_tool_(filename)(uls_ptr(parms1));

		if (parms != nilptr) {
			parms->lptr = fpath;
			parms->len = (int) (lptr - fpath);
		}

		return fp_in;
	}

	fp_in = NULL;
	for (lptr0 = dirpath_list; lptr0 != NULL; ) {
		if ((lptr = _uls_tool_(strchr)(lptr0, ULS_DIRLIST_DELIM)) != NULL) {
			len_fptr = (int) (lptr - lptr0);
			fptr = lptr0;
			lptr0 = ++lptr;
		} else {
			len_fptr = _uls_tool_(strlen)(lptr0);
			fptr = lptr0;
			lptr0 = NULL;
		}

		if (len_fptr > 0) {
			len = _uls_tool_(strncpy)(filepath_buff, fptr, len_fptr);
			filepath_buff[len++] = ULS_FILEPATH_DELIM;
			_uls_tool_(strcpy)(filepath_buff+len, fpath);
		} else {
			_uls_tool_(strcpy)(filepath_buff, fpath);
		}

		if ((fp_in=_uls_tool_(fp_open)(filepath_buff, ULS_FIO_READ)) != NULL) {
			if (parms != nilptr) {
				parms->lptr = fptr;
				parms->len = len_fptr;
			}
			break;
		}
	}

	return fp_in;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_dirpath)(const char* fname, _uls_tool_ptrtype_(outparam) parms)
{
	const char* dirpath_list = parms->lptr;
	char fpath_buff[ULS_FILEPATH_MAX+1];
	const char *fptr, *lptr0, *lptr, *dirpath_ret=NULL;
	int   len, len_fptr;

	if (dirpath_list == NULL) {
		return NULL;
	}

	for (lptr0=dirpath_list; lptr0 != NULL; ) {
		if ((lptr = _uls_tool_(strchr)(lptr0, ULS_DIRLIST_DELIM)) != NULL) {
			len_fptr = (int) (lptr - lptr0);
			fptr = lptr0;
			lptr0 = ++lptr;
		} else {
			len_fptr = _uls_tool_(strlen)(lptr0);
			fptr = lptr0;
			lptr0 = NULL;
		}

		if (len_fptr==0) continue;

		_uls_tool_(strncpy)(fpath_buff, fptr, len_fptr);
		len = len_fptr;
		fpath_buff[len++] = ULS_FILEPATH_DELIM;
		_uls_tool_(strcpy)(fpath_buff+len, fname);

		if (_uls_tool_(dirent_exist)(fpath_buff) == ST_MODE_FILE) {
			dirpath_ret = fptr;
			parms->len = len_fptr;
			break;
		}
	}

	return dirpath_ret;
}

char*
ULS_QUALIFIED_METHOD(path_skip_uplevel)(char* fpath)
{
	char *lptr;

	if (fpath[0] == ULS_FILEPATH_DELIM) {
		lptr = fpath + 1;
	} else {
		for (lptr=fpath; lptr[0] == '.' && lptr[1] == '.'; ) {
			if (lptr[2] == '\0') {
				lptr += 2;
				break;
			}

			if (lptr[2] == ULS_FILEPATH_DELIM) {
				lptr += 3;
			} else {
				// ..more-chars-exist!
				break;
			}
		}
	}

	return lptr;
}

#ifdef ULS_WINDOWS

int
ULS_QUALIFIED_METHOD(get_volume_name)(const char* filepath)
{
	const char *ptr = filepath;
	char delim_ch;
	int len;

	if (is_filepath_delim(ptr[0]) && is_filepath_delim(ptr[1])) {
		delim_ch = ptr[0];
		if ((ptr = _uls_tool_(strchr)(ptr+2, delim_ch)) != NULL) {
			len = (int) (ptr - filepath);
		} else {
			len = _uls_tool_(strlen)(filepath);
		}

	} else if (_uls_tool_(isalpha)(ptr[0]) && (ptr[1]==':' || (_uls_tool_(isalpha)(ptr[1]) && ptr[2]==':'))) {
		++ptr;
		ptr = _uls_tool_(strchr)(ptr, ':') + 1;
		len = (int) (ptr - filepath);

	} else {
		len = 0;
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(__uls_path_normalize_ustr)(const char* fpath, char* fpath2)
#else
int
ULS_QUALIFIED_METHOD(uls_path_normalize)(const char* fpath, char* fpath2)
#endif
{
	const char *ptr0=fpath, *ptr1, *ptr;
	char ch, *out_ptr=fpath2;

#ifndef ULS_WINDOWS
	if (fpath == NULL || fpath2 == NULL) return -1;
#endif

	for (ptr=ptr0; (ch=*ptr) != '\0'; ) {
		if (is_filepath_delim(ch)) {
			ptr1 = ptr;
			// Skip the redundant ULS_FILEPATH_DELIMs.
			for (++ptr; (ch=*ptr)!='\0'; ptr++) {
				if (!is_filepath_delim(ch)) break;
			}

			if (ptr0+1 == ptr1 && *ptr0 == '.') {
				--out_ptr;
			} else if (ch != '\0') {
				*out_ptr++ = ULS_FILEPATH_DELIM;
			}

			ptr0 = ptr;

		} else {
			++ptr;
			*out_ptr++ = ch;
		}
	}

	*out_ptr = '\0';
	return (int) (out_ptr - fpath2);
}

int
ULS_QUALIFIED_METHOD(uls_get_exeloc_dir)(const char* argv0, char *fpath_buf)
{
	int len;
	char *ptr;
#ifdef ULS_WINDOWS
	char *ustr;
	_uls_tool_type(csz_str) ustr_csz;
#endif

	if (fpath_buf == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

#ifdef ULS_WINDOWS
	if ((len = GetModuleFileNameA(NULL, fpath_buf, ULS_FILEPATH_MAX)) >= ULS_FILEPATH_MAX) {
		_uls_log(err_log)("%s: internal error!", __FUNCTION__);
		return -1;
	}
	fpath_buf[len] = '\0';

	_uls_tool(csz_init)(uls_ptr(ustr_csz), -1);

	if ((ustr = _uls_tool_(astr2ustr)(fpath_buf, len, uls_ptr(ustr_csz))) == NULL ||
		(ptr = (char *) _uls_tool_(strchr_r)(ustr, ULS_FILEPATH_DELIM)) == NULL) {
		len = -1;
	} else {
		len = (int) (ptr - ustr);
		_uls_tool_(memcopy)(fpath_buf, ustr, len);
		fpath_buf[len] = '\0';
	}

	_uls_tool(csz_deinit)(uls_ptr(ustr_csz));
#else
	if (argv0 == NULL) {
		if ((len = (int) readlink("/proc/self/exe", fpath_buf, ULS_FILEPATH_MAX+1)) < 0) {
			return -1;
		}
		fpath_buf[len] = '\0';
	} else {
		if (argv0[0] == ULS_FILEPATH_DELIM) {
			len = _uls_tool_(strcpy)(fpath_buf, argv0);
		} else if (_uls_tool_(strchr)(argv0, ULS_FILEPATH_DELIM) != NULL &&
			(len=_uls_tool_(getcwd)(fpath_buf, ULS_FILEPATH_MAX+1)) > 0) {
			fpath_buf[len++] = ULS_FILEPATH_DELIM;
			len += _uls_tool_(strcpy)(fpath_buf+len, argv0);
		} else if ((len = uls_get_exeloc_by_pathenv(argv0, fpath_buf)) < 0) {
			return -2;
		}
	}

	if ((ptr=realpath(fpath_buf, NULL)) != NULL) {
		len = _uls_tool_(strcpy)(fpath_buf, ptr);
		free(ptr);

		ptr = (char *) _uls_tool_(strchr_r)(fpath_buf, ULS_FILEPATH_DELIM);
		uls_assert(ptr != NULL);
		if (fpath_buf < ptr) {
			*ptr = '\0';
			len = (int) (ptr - fpath_buf);
		} else {
			*++ptr = '\0';
			len = 1;
		}
	} else {
		len = -1;
	}
#endif
	return len;
}

void
ULS_QUALIFIED_METHOD(uls_print_bytes)(const char* srcptr, int n_bytes)
{
	char ch;
	int i, j;

	if (n_bytes==0) return;

	for (i=0; ; ) {
		ch = srcptr[i];

		for (j=0; j<8; j++) {
			if (ch & BYTE_LEFT_MOST_1) _uls_log_(printf)("1");
			else _uls_log_(printf)(".");
			ch <<= 1;
		}
		if (++i>=n_bytes) break;
		_uls_log_(printf)(" ");
	}
}

void
ULS_QUALIFIED_METHOD(initialize_uls_util)(void)
{
	unsigned int a;
#ifdef ULS_WINDOWS
	SYSTEMTIME sys_time;
	FILETIME now;
	GetLocalTime(&sys_time);
	SystemTimeToFileTime(&sys_time, &now);
	a = (unsigned int) now.dwHighDateTime;
#else
	a = (unsigned int) time(NULL);
#endif
	srand(a);
}

void
ULS_QUALIFIED_METHOD(finalize_uls_util)(void)
{
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(skip_c_comment_file)(FILE* fin)
{
	int  ch, prev_ch;
	int  n_lines=0;

/* ret-val <  0 : error */
/* ret-val == n_lines : c-style comment completed */

	for (prev_ch=-1; (ch=fgetc(fin))!=EOF; prev_ch=ch) {
		if (ch =='\n') {
			++n_lines;
		} else if (ch=='/' && prev_ch=='*') {
			return n_lines;
		}
	}

	return -1;
}

#ifdef ULS_WINDOWS
ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_path_normalize)(const char* fpath, char* fpath2)
{
	const char *ptr0, *ptr;
	char *out_ptr, ch;
	int n;

	if (fpath == NULL || fpath2 == NULL) return -1;
	ptr0 = fpath;
	out_ptr = fpath2;

	if (is_filepath_delim(fpath[0]) && is_filepath_delim(fpath[1])) {
		// network location
		*out_ptr++ = ULS_FILEPATH_DELIM;
		*out_ptr++ = ULS_FILEPATH_DELIM;

		// Skip the redundant ULS_FILEPATH_DELIMs.
		for (ptr=fpath+2; (ch=*ptr)!='\0'; ptr++) {
			if (!is_filepath_delim(ch)) break;
		}
		ptr0 = ptr;
		n = (int) (out_ptr - fpath2);
	} else {
		n = 0;
	}

	n += __uls_path_normalize_ustr(ptr0, out_ptr);

	return n;
}
#endif

int
ULS_QUALIFIED_METHOD(uls_mkdir)(const char *filepath0)
{
	int rval;
	char *fpath;

	if (filepath0 == NULL || *filepath0 == '\0') return -1;
	fpath = _uls_tool_(strdup)(filepath0, -1);

	if (uls_path_normalize(fpath, fpath) < 0) {
		uls_mfree(fpath);
		return -1;
	}

	if (*fpath == '\0') {
		uls_mfree(fpath);
		return 0;
	}

	rval = __uls_mkdir_ustr(fpath);
	uls_mfree(fpath);

	return rval;
}

void
ULS_QUALIFIED_METHOD(uls_get_current_time_yyyymmdd_hhmm)(char* buf, int buf_siz)
{
#ifdef ULS_WINDOWS
	SYSTEMTIME tm;
	int len;

	GetLocalTime(&tm);

	len = _uls_log_(snprintf)(buf, buf_siz, "%4d-", tm.wYear);
	len += _uls_log_(snprintf)(buf+len, buf_siz-len, "%02d-%02d", tm.wMonth, tm.wDay);
	len += _uls_log_(snprintf)(buf+len, buf_siz-len, " %02d:%02d", tm.wHour, tm.wMinute);
#else
	struct tm  *tm;
	time_t  curtim;

	curtim = time(NULL);
	tm = localtime(uls_ptr(curtim));

	_uls_log_(snprintf)(buf, buf_siz, "%4d-%02d-%02d %02d:%02d",
		1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
#endif
}

#ifndef ULS_DOTNET
/**
 * Parsing command line arguments.
 * e.g. i0 = uls_getopts(argc, argv, "lqf:s:o:v", options);
 *     i0 < 0 if there're errors.
 *     use argv[i0], argv[i0+1] ... as strings(void of '-') arguments if i0 > 0
 */
ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_getopts)(int n_args, char* args[], const char* optfmt, uls_optproc_t proc)
{
	const char  *ptr;
	char        *optarg, *optstr;
	int         rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (args[i][0] != '-') break;

		optstr = uls_ptr(args[i][1]);
		for (k=i,j=0; (opt=optstr[j]) != '\0'; ) {
			if (opt=='?') {
				return 0; // call usage();
			}

			if ((ptr=_uls_tool_(strchr)(optfmt, opt)) == NULL) {
				_uls_log(err_log)("%s: undefined option -%c", __FUNCTION__, opt);
				return -1;
			}

			if (ptr[1] == ':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					_uls_log(err_log)("%s: option -%c requires an arg.", __FUNCTION__, opt);
					return -1;
				}

				if ((rc=proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = "";
				if ((rc=proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("%s: error in -%c.", __FUNCTION__, opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}
#endif // ULS_DOTNET
