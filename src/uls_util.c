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

#ifndef __ULS_WINDOWS__
#include <unistd.h>
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
		ch += 'A' - 10;

	return ch;
}

void
ULS_QUALIFIED_METHOD(uls_dump_utf8str)(const char *str)
{
	unsigned char ch, ch1;
	unsigned char buff[4];
	int i, k;

	for (i = 0; (ch = str[i]) != '\0'; i++) {
		k = 0;

		if ((ch1 = ch >> 4) != 0)
			buff[k++] = uls_nibble2ascii(ch1);
		else
			buff[k++] = '0';

		buff[k++] = uls_nibble2ascii(ch & 0x0F);
		buff[k] = '\0';

		_uls_log_(printf)(" %s", (char *) buff);
	}
}

void
ULS_QUALIFIED_METHOD(uls_putstr)(const char *str)
{
	uls_put_binstr(str, -1, _uls_stdio_fd(1));
}

int
ULS_QUALIFIED_METHOD(uls_str2int)(const char *str)
{
	return _uls_tool_(atoi)(str);
}

int
ULS_QUALIFIED_METHOD(uls_str_length)(const char *str)
{
	return _uls_tool_(strlen)(str);
}

int
ULS_QUALIFIED_METHOD(uls_str_equal)(const char *str1, const char *str2)
{
	return uls_streql(str1, str2);
}

int
ULS_QUALIFIED_METHOD(uls_str_copy)(char *bufptr, const char *str)
{
	return uls_strcpy(bufptr, str);
}

int
ULS_QUALIFIED_METHOD(uls_str_compare)(const char *str1, const char *str2)
{
	return uls_strcmp(str1, str2);
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

#ifdef __ULS_WINDOWS__
int
ULS_QUALIFIED_METHOD(get_volume_name)(const char *filepath)
{
	const char *ptr = filepath;
	int len;

	if (ptr[0] == ULS_FILEPATH_DELIM && ptr[1] == ULS_FILEPATH_DELIM) {
		if ((ptr = _uls_tool_(strchr)(ptr+2, ULS_FILEPATH_DELIM)) != NULL) {
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

char*
ULS_QUALIFIED_METHOD(path_skip_uplevel)(char *fpath)
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
ULS_QUALIFIED_METHOD(__os_mkdir_ustr)(const char *dirname)
{
	int rval;
#ifdef __ULS_WINDOWS__
	const char *astr;
	_uls_type_tool(auw_outparam) buf_csz;

	_uls_tool(auw_init_outparam)(uls_ptr(buf_csz));

	if ((astr = _uls_tool_(ustr2astr_ptr)(dirname, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = _mkdir(astr);
		if (rval != 0) rval = -1;
	}

	_uls_tool(auw_deinit_outparam)(uls_ptr(buf_csz));
#else
	rval = mkdir(dirname, 0755);
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

#ifdef __ULS_WINDOWS__
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

int
ULS_QUALIFIED_METHOD(__uls_path_normalize_ustr)(const char *fpath, char *outbuf)
{
	const char *ptr, *ptr1 = fpath, *ptr2;
	char ch, *outptr = outbuf;
	int len, bAdded;

	for (ptr = ptr1; ; ptr++) {
		if ((ch=*ptr) == '\0' || is_filepath_delim(ch)) {
			ptr2 = ptr;

			if ((len = (int) (ptr2 - ptr1)) > 0 && !(len == 1 && *ptr1 == '.')) {
				for (ptr = ptr1; ptr < ptr2; ptr++) {
					*outptr++ = *ptr;
				}
				bAdded = 1;

			} else {
				bAdded = 0;
			}

			for (ptr = ptr2; (ch=*ptr) != '\0'; ptr++) {
				if (!is_filepath_delim(ch)) break;
			}

			if (ch == '\0') break;
			if (bAdded) *outptr++ = ULS_FILEPATH_DELIM;
			ptr1 = ptr;
		}
	}

	if ((len = (int) (outptr - outbuf)) > 1 && outbuf[len - 1] == ULS_FILEPATH_DELIM) {
		--outptr;
		--len;
	}

	*outptr = '\0';
	return len;
}

int
ULS_QUALIFIED_METHOD(uls_path_normalize)(const char *fpath, char *fpath2)
{
	const char *ptr, *ptr1 = fpath;
	char ch, *outbuf = fpath2;
	int n;

	if (fpath == NULL || fpath2 == NULL) return -1;

#ifdef __ULS_WINDOWS__
	if (is_filepath_delim(fpath[0]) && is_filepath_delim(fpath[1])) {
		// network location
		*outbuf++ = ULS_FILEPATH_DELIM;
		*outbuf++ = ULS_FILEPATH_DELIM;
		n = 2;
#else
	if (fpath[0] == ULS_FILEPATH_DELIM) {
		*outbuf++ = ULS_FILEPATH_DELIM;
		n = 1;
#endif
		// Skip the subsequent path delimeters
		for (ptr = fpath + n; (ch=*ptr) != '\0'; ptr++) {
			if (!is_filepath_delim(ch)) break;
		}
		ptr1 = ptr;

	} else {
		n = 0;
	}

	n += __uls_path_normalize_ustr(ptr1, outbuf);
	if (n <= 0) {
		*fpath2 = '.';
		n = 1;
	}

	return n;
}

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

#ifndef ULS_DOTNET
/**
 * Parsing command line arguments.
 * e.g. i0 = uls_getopts(argc, argv, "lqf:s:o:v", options);
 *     i0 < 0 if there're errors.
 *     use argv[i0], argv[i0+1] ... as strings(void of '-') arguments if i0 > 0
 */
int
ULS_QUALIFIED_METHOD(uls_getopts)(int n_args, char *args[], const char *optfmt, uls_optproc_t proc)
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

			if ((cptr=_uls_tool_(strchr)(optfmt, (char) opt)) == NULL) {
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
					else _uls_log(err_log)("An error in processing the option -%c, %s.", opt, optarg);
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
#endif // ~ULS_DOTNET

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

#if defined(__ULS_WINDOWS__) && !defined(ULS_DOTNET)
char*
ULS_QUALIFIED_METHOD(uls_win32_lookup_regval)(wchar_t *reg_dir, uls_ptrtype_tool(outparam) parms)
{
	wchar_t *reg_name = (wchar_t*) parms->line;
	int	n_wchars;
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

#ifdef __ULS_WINDOWS__
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
#ifdef __ULS_WINDOWS__
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
#if defined(__ULS_WINDOWS__) && !defined(ULS_DOTNET)
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
