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
  <file> uls_util.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2020.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_UTIL__
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"
#include "uls/uls_auw.h"

#include <stdlib.h>
#include <string.h>

#ifndef ULS_WINDOWS
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#endif

unsigned char
ULS_QUALIFIED_METHOD(uls_nibble2ascii)(unsigned char ch)
{
	if (ch >= 0 && ch <= 9)
		ch += '0';
	else
		ch += 'a' - 10;

	return ch;
}

void
ULS_QUALIFIED_METHOD(uls_putstr)(const char* str)
{
	uls_put_binstr(str, -1, _uls_stdio_fd(1));
}

int
ULS_QUALIFIED_METHOD(ult_str2int)(const char *str)
{
	return _uls_tool_(atoi)(str);
}

int
ULS_QUALIFIED_METHOD(ult_str_length)(const char *str)
{
	return _uls_tool_(strlen)(str);
}

int
ULS_QUALIFIED_METHOD(ult_str_equal)(const char *str1, const char *str2)
{
	return uls_streql(str1, str2);
}

int
ULS_QUALIFIED_METHOD(ult_str_copy)(char *bufptr, const char *str)
{
	return uls_strcpy(bufptr, str);
}

int
ULS_QUALIFIED_METHOD(ult_str_compare)(const char* str1, const char* str2)
{
	return uls_strcmp(str1, str2);
}

char*
ULS_QUALIFIED_METHOD(ult_skip_blanks)(const char* lptr)
{
	return skip_blanks(lptr);
}

char*
ULS_QUALIFIED_METHOD(ult_splitstr)(char** p_str, int *p_len)
{
	uls_wrd_t wrdx;
	char *wrd;

	wrdx.lptr = *p_str;
	wrd = _uls_splitstr(uls_ptr(wrdx));
	*p_str = wrdx.lptr;

	if (p_len != NULL) *p_len = wrdx.len;
	return wrd;
}

char*
ULS_QUALIFIED_METHOD(ult_split_litstr)(char *str, char qch)
{
	return split_litstr(str, qch);
}

void
ULS_QUALIFIED_METHOD(ult_dump_bin)(const char *str)
{
	unsigned char ch, ch1;
	unsigned char buff[4];
	int i, k;

	for (i = 0; (ch = str[i]) != '\0'; i++) {
		k = 0;
		if ((ch1 = ch >> 4) != 0)
			buff[k++] = uls_nibble2ascii(ch1);;
		buff[k++] = uls_nibble2ascii(ch & 0x0F);
		buff[k] = '\0';

		_uls_log_(printf)(" %s", (char *) buff);
	}
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

int
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

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_get_exeloc_by_pathenv)(const char *progname, char *fpath_buf)
{
	const char *pathlist, *dirptr;
	int dirlen, fpath_buflen=-1;
	uls_type_tool(outparam) parms;

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
ULS_QUALIFIED_METHOD(uls_get_exeloc_dir)(const char* argv0, char *fpath_buf)
{
	int len;
	char *ptr;
#ifdef ULS_WINDOWS
	char *ustr;
	_uls_type_tool(csz_str) ustr_csz;
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
#ifdef HAVE_READLINK
                len = (int) readlink("/proc/self/exe", fpath_buf, ULS_FILEPATH_MAX+1);
#else
                len = -1;
#endif
                if (len < 0) return -1;
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

#ifdef HAVE_REALPATH
	if ((ptr=realpath(fpath_buf, NULL)) != NULL) {
		len = _uls_tool_(strcpy)(fpath_buf, ptr);
		free(ptr);
	}
#endif
	ptr = (char *) _uls_tool_(strchr_r)(fpath_buf, ULS_FILEPATH_DELIM);
	if (ptr == NULL) return -3;

	if (fpath_buf < ptr) {
		*ptr = '\0';
		len = (int) (ptr - fpath_buf);
	} else {
		*++ptr = '\0';
		len = 1;
	}
#endif // ULS_WINDOWS
	return len;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_dirpath)(const char* fname, uls_ptrtype_tool(outparam) parms)
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

		if (uls_dirent_exist(fpath_buff) == ST_MODE_FILE) {
			dirpath_ret = fptr;
			parms->len = len_fptr;
			break;
		}
	}

	return dirpath_ret;
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

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__os_mkdir_ustr)(const char *filepath)
{
	int rval;
#ifdef ULS_WINDOWS
	const char *astr;
	_uls_type_tool(auw_outparam) buf_csz;

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
	uls_type_tool(outparam) parms;

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
#endif

#ifdef ULS_WINDOWS
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

#ifdef ULS_WINDOWS
int
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
ULS_QUALIFIED_METHOD(isp_init)(uls_isp_ptr_t isp, int init_size)
{
	if (init_size < 0)
		init_size = 512;

	isp->buff = (char *) uls_malloc(init_size);
	isp->siz_strpool = init_size;
	isp->len_strpool = 0;
}

void
ULS_QUALIFIED_METHOD(isp_reset)(uls_isp_ptr_t isp)
{
	isp->len_strpool = 1;
}

void
ULS_QUALIFIED_METHOD(isp_deinit)(uls_isp_ptr_t isp)
{
	isp->len_strpool = 0;

	if (isp->siz_strpool > 0) {
		uls_mfree(isp->buff);
		isp->buff = NULL;
	}
}

char*
ULS_QUALIFIED_METHOD(isp_find)(uls_isp_ptr_t isp, const char* str, int len)
{
	char *ptr;
	int l, ind;

	if (len < 0) {
		len = uls_strlen(str);
	}

	for (ind=0; ind < isp->len_strpool; ind += l+1) {
		ptr = isp->buff + ind;

		l = uls_strlen(ptr);
		if (l == len && uls_streql(str, ptr)) {
			return ptr;
		}
	}

	return NULL;
}

char*
ULS_QUALIFIED_METHOD(isp_insert)(uls_isp_ptr_t isp, const char* str, int len)
{
	char *ptr;
	int i, l;

	if (len < 0) len = uls_strlen(str);

	l = isp->siz_strpool - isp->len_strpool;
	if (len + 1 > l) {
		_uls_log(err_log)("%s: isp full!", __FUNCTION__);
		return NULL;
	}

	ptr = isp->buff + isp->len_strpool;
	for (i=0; i<len; i++) *ptr++ = str[i];
	*ptr = '\0';

	l = isp->len_strpool;
	isp->len_strpool += len + 1;

	return isp->buff + l;
}

#ifndef ULS_DOTNET
char*
ULS_QUALIFIED_METHOD(uls_split_filepath)(const char* filepath, int* len_fname)
{

	return uls_filename(filepath, len_fname);
}

/**
 * Parsing command line arguments.
 * e.g. i0 = uls_getopts(argc, argv, "lqf:s:o:v", options);
 *     i0 < 0 if there're errors.
 *     use argv[i0], argv[i0+1] ... as strings(void of '-') arguments if i0 > 0
 */
int
ULS_QUALIFIED_METHOD(uls_getopts)(int n_args, char* args[], const char* optfmt, uls_optproc_t proc)
{
	const char  *cptr;
	char *optarg, *optstr, nullbuff[4] = { '\0', };
	int         rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (args[i][0] != '-') break;

		optstr = uls_ptr(args[i][1]);
		for (k=i,j=0; (opt=optstr[j]) != '\0'; ) {
			if (opt == '?') {
				return 0; // call usage();
			}

			if ((cptr=_uls_tool_(strchr)(optfmt, opt)) == NULL) {
				_uls_log(err_log)("%s: undefined option -%c", __func__, opt);
				return -1;
			}

			if (cptr[1] == ':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					_uls_log(err_log)("%s: option -%c requires an arg.", __func__, opt);
					return -1;
				}

				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("An Error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = nullbuff;
				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else _uls_log(err_log)("%s: error in -%c.", __func__, opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}
#endif // ULS_DOTNET

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ms_codepage)(uls_ptrtype_tool(outparam) parms)
{
	const char *name = parms->lptr;
	uls_type_tool(outparam) parms1;
	const char *cptr;
	int n, mbs;

	if (name[0] != 'c' || name[1] != 'p' || !_uls_tool_(isdigit)(name[2]))
		return -1;

	parms1.lptr = name + 2;
	n = (int)_uls_tool_(skip_atou)(uls_ptr(parms1));
	cptr = parms1.lptr;
	if (*cptr != '\0') return -1;

	if (n == 932 || n == 936 || n == 949 || n == 950 ||
		n == 20932 || n == 20936 || n == 20949 ||
		n == 51932 || n == 51936 || n == 51949 || n == 51950) {
		mbs = 2;
	} else {
		mbs = 1;
	}

	parms->n = mbs;
	return n;
}

#if defined(ULS_WINDOWS) && !defined(ULS_DOTNET)
char*
ULS_QUALIFIED_METHOD(uls_win32_lookup_regval)(wchar_t* reg_dir, uls_ptrtype_tool(outparam) parms)
{
	wchar_t* reg_name = (wchar_t*) parms->line;
	int	n_wchars, stat = -1;
	HKEY   hKeyRoot, hRegKey;
	DWORD  value_type, bufsize;
	wchar_t  *lpKeyStr, keyRootBuff[8];
	LONG   rval;

	csz_str_t csz;
	char *ustr;

	if (reg_dir == NULL) return NULL;

	if ((lpKeyStr = wcschr(reg_dir, L':')) == NULL || reg_dir == lpKeyStr ||
		(rval=(LONG)(lpKeyStr-reg_dir)) >= sizeof(keyRootBuff)/sizeof(wchar_t)) {
		_uls_log(err_log)("incorrect format of reg-dir!");
		return NULL;
	}

	wcsncpy(keyRootBuff, reg_dir, rval);
	keyRootBuff[rval] = L'\0';
	reg_dir = ++lpKeyStr;

	if (!wcscmp(keyRootBuff, L"HKLM")) {
		hKeyRoot = HKEY_LOCAL_MACHINE;
	} else if (!wcscmp(keyRootBuff, L"HKCU")) {
		hKeyRoot = HKEY_CURRENT_USER;
	} else if (!wcscmp(keyRootBuff, L"HKCC")) {
		hKeyRoot = HKEY_CURRENT_CONFIG;
	} else if (!wcscmp(keyRootBuff, L"HKCR")) {
		hKeyRoot = HKEY_CLASSES_ROOT;
	} else if  (!wcscmp(keyRootBuff, L"HKU")) {
		hKeyRoot = HKEY_USERS;
	} else {
		_uls_log(err_log)("unknown reg key!");
		return NULL;
	}

	rval = RegOpenKeyExW(hKeyRoot, reg_dir, 0, KEY_READ, &hRegKey);
	if (rval != ERROR_SUCCESS) {
		_uls_log(err_log)("Can't find the home directory of ULS.");
		return NULL;
	}

	value_type = REG_SZ;
	rval = RegQueryValueExW(hRegKey, reg_name, 0, &value_type, NULL, &bufsize);
	if (rval != ERROR_SUCCESS) {
		if (rval == ERROR_FILE_NOT_FOUND)
			_uls_log(err_log)("Error: RegQueryValueEx: 'UlsHome' Not found");
		RegCloseKey(hRegKey);
		return NULL;
	}

	if ((lpKeyStr = (wchar_t *) uls_malloc(bufsize)) == NULL ||
		(rval = RegQueryValueExW(hRegKey, reg_name, 0,
			&value_type, (LPBYTE) lpKeyStr, &bufsize)) != ERROR_SUCCESS) {
		_uls_log(err_log)("RegQueryValueEx failed");
		ustr = NULL;

	} else {
		n_wchars = bufsize/sizeof(wchar_t) - 1;
		csz_init(uls_ptr(csz), (n_wchars+1) * 2);

		if ((ustr = uls_wstr2ustr(lpKeyStr, n_wchars, uls_ptr(csz))) == NULL) {
			parms->n = -1;
		} else {
			parms->n = csz_length(uls_ptr(csz));
			ustr = csz_export(uls_ptr(csz));
		}

		csz_deinit(uls_ptr(csz));
	}

	RegCloseKey(hRegKey);
	uls_mfree(lpKeyStr);

	return ustr;
}
#endif

int
ULS_QUALIFIED_METHOD(initialize_uls_util)(void)
{
	char pathbuff[ULS_FILEPATH_MAX+1];
	const char *fpath, *cptr;

	int rc, len;
	uls_type_tool(outparam) parms;
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

	initialize_primitives();
	initialize_csz();

#ifndef ULS_DOTNET
	initialize_uls_lf();
	uls_add_default_convspecs(uls_lf_get_default());
	initialize_uls_sysprn();
	initialize_uls_syserr();
#endif
	if ((fpath = getenv("ULS_SYSPROPS")) == NULL || _uls_tool_(dirent_exist)(fpath) != ST_MODE_REG) {
		fpath = ULS_SYSPROPS_FPATH;
		if ((rc = _uls_tool_(dirent_exist)(fpath)) <= 0 || rc != ST_MODE_REG) {
#ifdef ULS_WINDOWS
			len = _uls_tool_(strcpy)(pathbuff, ULS_SHARE_DFLDIR);
#else
			len = uls_strcpy(pathbuff, ULS_OS_TEMP_DIR);
#endif
			pathbuff[len++] = ULS_FILEPATH_DELIM;
			len += _uls_tool_(strcpy)(pathbuff + len, TMP_SYSPROPS_FNAME);

			fpath = pathbuff;
			if ((rc = _uls_tool_(dirent_exist)(fpath)) <= 0 || rc != ST_MODE_REG) {
				_uls_log(err_log)("ULS: can't find the system property file in %s.", ULS_SYSPROPS_FPATH);
				return -1;
			}
		}
	}

	if ((rc=initialize_sysprops(fpath)) < 0) {
		_uls_log(err_log)("ULS: can't load the system property file in %s(err=%d).", fpath, rc);
		return -1;
	}

	initialize_uls_fileio();

	if ((_uls_sysinfo_(home_dir) = uls_get_system_property("ULS_HOME")) == NULL) {
#if defined(ULS_WINDOWS) && !defined(ULS_DOTNET)
		char *homedir;

		parms.line = (char *) ULS_REG_INSTDIR_NAME;
		homedir = uls_win32_lookup_regval(ULS_REG_HOME, uls_ptr(parms));
		rc = parms.n;

		if (homedir == NULL) {
			_uls_log(err_log)("ULS: don't know about the installation information.");
			return -1;
		}

		_uls_sysinfo_(home_dir) = uls_add_system_property("ULS_HOME", homedir);
		uls_mfree(homedir);
#else
		_uls_log(err_log)("ULS: don't know the installed directory.");
		return -1;
#endif
	}

	if ((_uls_sysinfo_(etc_dir) = uls_get_system_property("ULS_ETC")) == NULL) {
		_uls_log(err_log)("ULS: can't find etc dir for uls!");
		return -1;
	}

	if ((_uls_sysinfo_(ulcs_dir) = uls_get_system_property("ULS_ULCS")) == NULL) {
		_uls_sysinfo_(ulcs_dir) = ULS_SHARE_DFLDIR;
	}

	if ((cptr = uls_get_system_property("ULS_MBCS")) == NULL) {
		_uls_log(err_log)("ULS: can't find the encoding of the system!");
		return -1;
	}

	if (uls_streql(cptr, "utf8")) {
		_uls_sysinfo_(codepage) = 0; // UTF-8
		_uls_sysinfo_(multibytes) = 0;
	} else {
		parms.lptr = cptr;
		if ((rc = get_ms_codepage(uls_ptr(parms))) <= 0) {
			_uls_log(err_log)("%s: unknown file-encoding %s", cptr);
			return -1;
		}
		_uls_sysinfo_(codepage) = rc;
		_uls_sysinfo_(multibytes) = parms.n;
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_uls_util)(void)
{
	finalize_uls_fileio();
	finalize_sysprops();
#ifndef ULS_DOTNET
	finalize_uls_syserr();
	finalize_uls_sysprn();
	finalize_uls_lf();
#endif
	finalize_csz();
	finalize_primitives();
}
