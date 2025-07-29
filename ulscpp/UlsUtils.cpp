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
  <file> UlsUtils.cpp </file>
  <brief>
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2018.
  </author>
*/

#include "uls/UlsUtils.h"
#include "uls/UlsAuw.h"

#include <uls/uls_fileio.h>
#include <uls/uls_log.h>

#include <string>
#include <stdlib.h>
#include <memory.h>

using namespace std;

void
uls::memcopy(void *dst, const void *src, int n)
{
	memmove(dst, src, n);
}

int
uls::strLength(const char *str)
{
	const char *ptr;

	for (ptr=str; *ptr != '\0'; ptr++)
		/* NOTHING */;

	return (int) (ptr - str);
}

int
uls::strLength(const wchar_t *wstr)
{
	const wchar_t *ptr;

	for (ptr=wstr; *ptr != L'\0'; ptr++)
		/* NOTHING */;

	return (int) (ptr - wstr);
}

int
uls::strFindIndex(const char *line, char ch0)
{
	int i;

	if (ch0 >= 0x80) return -1;

	for (i=0; line[i] != '\0'; i++) {
		if (line[i] == ch0) return i;
	}

	if (ch0 == '\0') return i;
	return -1;
}

int
uls::strFindIndex(const wchar_t *wline, wchar_t wch0)
{
	wchar_t wch;
	int i;

	for (i=0; (wch=wline[i]) != L'\0'; i++) {
		if (wch == wch0) return i;
	}

	if (wch0 == L'\0') return i;
	return-1;
}

int
uls::strToInt(const char *str)
{
	int i, minus, n = 0;
	char ch;

	if (str[0] == '-') {
		minus = 1;
		i = 1;
	} else {
		minus = 0;
		i = 0;
	}

	for ( ; ; i++) {
		if ((ch=str[i]) > '9' || ch < '0')
			break;
		n = ch - '0' + n * 10;
	}

	if (minus) n = -n;
	return n;
}

int
uls::strToInt(const wchar_t *wstr)
{
	int i, minus, n = 0;
	wchar_t wch;

	if (wstr[0] == L'-') {
		minus = 1;
		i = 1;
	} else {
		minus = 0;
		i = 0;
	}

	for ( ; ; i++) {
		if ((wch=wstr[i]) > L'9' || wch < L'0')
			break;
		n = wch - L'0' + n * 10;
	}

	if (minus) n = -n;
	return n;
}

int
uls::direntExist(const char *fpath)
{
	int stat = 0;
	csz_str_t csz;
	const char *austr;

	if (fpath == NULL) return -1;

	csz_init(uls_ptr(csz), -1);

#ifdef __ULS_WINDOWS__
	austr = uls_astr2ustr(fpath, -1, uls_ptr(csz));
#else
	austr = fpath;
#endif
	if (austr == NULL) {
		stat = -1;
	} else {
		stat = uls_dirent_exist(austr);
	}

	csz_deinit(uls_ptr(csz));
	return stat;
}

int
uls::direntExist(const wchar_t *wfpath)
{
	int  stat = 0;
	csz_str_t csz;
	char *ustr;

	if (wfpath == NULL) return -1;

	csz_init(uls_ptr(csz), -1);

	if ((ustr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz))) == NULL) {
		stat = -1;
	} else {
		stat = uls_dirent_exist(ustr);
	}

	csz_deinit(uls_ptr(csz));
	return stat;
}

FILE*
uls::fileOpenReadolnly(const char *fpath)
{
	FILE *fp;

	if (fpath == NULL) {
		fp = NULL;
	} else {
		fp = fopen(fpath, "rb");
	}

	return fp;
}

FILE*
uls::fileOpenReadolnly(const wchar_t *wfpath)
{
	csz_str_t csz;
	char *austr;
	FILE *fp;

	if (wfpath == NULL) return NULL;

	csz_init(uls_ptr(csz), -1);
#ifdef __ULS_WINDOWS__
	austr = uls_wstr2astr(wfpath, -1, uls_ptr(csz));
#else
	austr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz));
#endif
	if (austr == NULL) {
		fp = NULL;
	} else {
		fp = fileOpenReadolnly(austr);
	}

	csz_deinit(uls_ptr(csz));
	return fp;
}

FILE*
uls::fileOpenWriteolnly(const char *fpath)
{
	FILE *fp;

	if (fpath == NULL) {
		fp = NULL;
	} else {
		fp = fopen(fpath, "wb");
	}

	return fp;
}

FILE*
uls::fileOpenWriteolnly(const wchar_t *wfpath)
{
	csz_str_t csz;
	char *austr;
	FILE *fp;

	if (wfpath == NULL) return NULL;

	csz_init(uls_ptr(csz), -1);
#ifdef __ULS_WINDOWS__
	austr = uls_wstr2astr(wfpath, -1, uls_ptr(csz));
#else
	austr = uls_wstr2ustr(wfpath, -1, uls_ptr(csz));
#endif
	if (austr == NULL) {
		fp = NULL;
	} else {
		fp = fileOpenWriteolnly(austr);
	}

	csz_deinit(uls_ptr(csz));
	return fp;
}

char*
uls::getFilename(const char *filepath, int* len_fname)
{
	const char *ptr, *ptr0;
	const char *filename;
	int  len_filename, len, i;
	char  ch;

	if (filepath == NULL) return NULL;
	for (ptr0 = NULL, ptr = filepath; (ch=*ptr) != '\0'; ptr++) {
		if (ch == ULS_FILEPATH_DELIM) ptr0 = ptr;
	}

	if (ptr0 != NULL) {
		filename = ptr0 + 1;
	} else {
		filename = filepath;
	}

	len_filename = len = strlen(filename);
	for (i = len_filename - 1; i >= 0; i--) {
		if (filename[i] == '.') {
			len = i;
			break;
		}
	}

	if (len_fname != NULL) {
		*len_fname = len;
	}

	return (char *) filename;
}

wchar_t*
uls::getFilename(const wchar_t *wfilepath, int* len_fname)
{
	const wchar_t *ptr, *ptr0;
	const wchar_t *wfilename;
	int  len_wfilename, len, i;
	wchar_t  wch;

	if (wfilepath == NULL) return NULL;
	for (ptr0 = NULL, ptr = wfilepath; (wch=*ptr) != L'\0'; ptr++) {
		if (wch == ULS_FILEPATH_DELIM) ptr0 = ptr;
	}

	if (ptr0 != NULL) {
		wfilename = ptr0 + 1;
	} else {
		wfilename = wfilepath;
	}

	len_wfilename = len = strLength(wfilename);
	for (i = len_wfilename - 1; i >= 0; i--) {
		if (wfilename[i] == '.') {
			len = i;
			break;
		}
	}

	if (len_fname != NULL) {
		*len_fname = len;
	}

	return (wchar_t *) wfilename;
}

int
uls::parseCommandOptions(int n_args, char *args[], const char *optfmt, uls::optproc_t proc)
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

			if ((rc = strFindIndex(optfmt, (char) opt)) < 0) {
				err_log("parseCommandOptions: undefined option -%c", opt);
				return -1;
			}

			cptr = optfmt + rc;
			if (cptr[1] == ':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					err_log("parseCommandOptions: option -%c requires an arg.", opt);
					return -1;
				}

				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_log("An error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = nullbuff;
				if ((rc = proc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_log("parseCommandOptions: error in -%c.", opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}

int
uls::parseCommandOptions(int n_args, wchar_t *args[], const wchar_t *optfmt, woptproc_t wproc)
{
	const wchar_t  *cptr;
	wchar_t *optarg, *optstr, nullbuff[4] = { L'\0', };
	int         rc, opt, i, j, k;

	for (i=1; i<n_args; i=k+1) {
		if (args[i][0] != L'-') break;

		optstr = uls_ptr(args[i][1]);
		for (k=i,j=0; (opt=optstr[j]) != '\0'; ) {
			if (opt == L'?') {
				return 0; // call usage();
			}

			if ((rc = strFindIndex(optfmt, (wchar_t) opt)) < 0) {
				err_log("parseCommandOptions: undefined option -%c", opt);
				return -1;
			}

			cptr = optfmt + rc;
			if (cptr[1] == L':') { /* the option 'opt' needs a arg-val */
				if (optstr[j+1]!='\0') {
					optarg = optstr + (j+1);
				} else if (k+1 < n_args && args[k+1][0] != '-') {
					optarg = args[++k];
				} else {
					err_log("parseCommandOptions: option -%c requires an arg.", opt);
					return -1;
				}

				if ((rc = wproc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_log("Error in processing the option -%c, %s.", opt, optarg);
					return rc;
				}
				break;

			} else {
				optarg = nullbuff;
				if ((rc = wproc(opt, optarg)) != 0) {
					if (rc > 0) rc = 0;
					else err_log("parseCommandOptions: error in -%c.", opt);
					return rc;
				}
				j++;
			}
		}
	}

	return i;
}
