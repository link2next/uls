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
 * ult_utils.c -- read from input file and dump tokens in binary/text stream --
 *     written by Stanley Hong <link2next@gmail.com>, June 2015.
 */
#include "ult_utils.h"

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

char*
ult_strdup(const char* str)
{
	return uls_strdup(str, -1);
}

int
ult_chdir(const char* path)
{
	int rval;

#ifdef ULS_WINDOWS
	rval = _chdir(path);
#else
	rval = chdir(path);
#endif
	if (rval != 0) rval = -1;

	return rval;
}

int
ult_getcwd(char* buf, int buf_siz)
{
	const char *ptr;

	if (buf == NULL) return -1;
#ifdef ULS_WINDOWS
	ptr = _getcwd(buf, buf_siz);
#else
	ptr = getcwd(buf, buf_siz);
#endif

	return ptr == NULL ? -1: (int) strlen(buf);
}

int
ult_is_absolute_path(const char* path)
{
	int stat;

	if (*path == ULS_FILEPATH_DELIM) stat = 1;
#ifdef ULS_WINDOWS
	else if (isalpha(path[0]) && path[1] == ':') stat = 1;
#endif
	else stat = 0;

	return stat;
}

int
ult_guess_specname_from_istream(uls_istream_ptr_t istr, uls_outparam_ptr_t parms)
{
	char* specname = parms->line;

	int ftype, len;

	if ((ftype = istr->header.filetype) == ULS_STREAM_RAW) {
		specname[0] = '\0';
		len = 0;
	} else {
		strcpy(specname, uls_get_namebuf_value(istr->header.specname));
		len = (int) strlen(specname);
		// 'specname' is not nil-string.
	}

	parms->len = len;
	return ftype;
}

int
ult_guess_specname(const char* fpath, uls_outparam_ptr_t parms)
{
	uls_istream_ptr_t istr;
	int ftype;

	if ((istr = uls_open_istream_file(fpath)) == uls_nil) {
		return -1;
	}

	ftype = ult_guess_specname_from_istream(istr, parms);

	if (parms->flags) {
		parms->data = istr;
	} else {
		uls_destroy_istream(istr);
		parms->data = NULL;
	}

	return ftype;
}

int
ult_guess_specname_from_inputfiles(char* specname, FILE *fp_list, int only_first)
{
	char  linebuff[ULS_FILEPATH_MAX+1], *lptr, *fpath;
	int  len, rval, stat = ULS_STREAM_RAW;
	uls_outparam_t parms;

	while (1) {
		if ((len=uls_fp_gets(fp_list, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) stat = -1;
			break;
		}

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		len -= (int) (lptr - linebuff);
		if ((len = str_trim_end(lptr, len)) == 0) continue;
		fpath = lptr;

		parms.line = specname;
		parms.flags = 0;
		if ((rval = ult_guess_specname(fpath, uls_ptr(parms))) < 0) {
			stat = -1;
			break;
		}

		if (rval != ULS_STREAM_RAW) {
			if (stat == ULS_STREAM_RAW) {
				stat = rval;
				if (only_first) break;
			} else if (stat != rval) {
				stat = -1;
				break;
			}
		}
	}

	return stat;
}

int
ult_guess_specname_from_argvlist(char* specname, int n_args, const char** args, int only_first)
{
	const char *fpath;
	int i, rval, stat = ULS_STREAM_RAW;
	uls_outparam_t parms;

	for (i=0; i<n_args; i++) {
		fpath = args[i];

		parms.line = specname;
		parms.flags = 0;
		if ((rval = ult_guess_specname(fpath, uls_ptr(parms))) < 0) {
			stat = -1;
			break;
		}

		if (rval != ULS_STREAM_RAW) {
			if (stat == ULS_STREAM_RAW) {
				stat = rval;
				if (only_first) break;
			} else if (stat != rval) {
				stat = -1;
				break;
			}
		}
	}

	return stat;
}

int
ult_is_inputfiles_raw(const char *filelist)
{
	char  linebuff[ULS_FILEPATH_MAX+1], *lptr, *fpath;
	char specname[ULC_LONGNAME_MAXSIZ+1];
	int  len, rval, stat = 1;
	FILE *fp_list;
	uls_outparam_t parms;

	if ((fp_list = uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
		return -1;
	}

	while (1) {
		if ((len=uls_fp_gets(fp_list, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) stat = -1;
			break;
		}

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		len -= (int) (lptr - linebuff);
		if ((len = str_trim_end(lptr, len)) == 0) continue;
		fpath = lptr;

		parms.line = specname;
		parms.flags = 0;
		if ((rval = ult_guess_specname(fpath, uls_ptr(parms))) < 0) {
			stat = -1;
			break;
		}

		if (rval != ULS_STREAM_RAW) {
			stat = 0;
			break;
		}
	}

	uls_fp_close(fp_list);
	return stat;
}

int
ult_guess_host_byteorder(void)
{
	unsigned int ui;
	unsigned char *ptr;
	int stat;

	ui = 1;
	ptr = (unsigned char *) &ui;
	stat = (ptr[0] == 1) ?  ULS_LITTLE_ENDIAN : ULS_BIG_ENDIAN;

	return stat;
}

int
ult_fd_create_wronly(const char* fpath)
{
	return uls_fd_open(fpath, ULS_FIO_CREAT|ULS_FIO_WRITE);
}

int
ult_fd_open_rdonly(const char* fpath)
{
	return uls_fd_open(fpath, ULS_FIO_READ);
}

void
ult_fd_close(int fd)
{
	uls_fd_close(fd);
}

const char*
ult_get_suffix(const char *filepath)
{
	const char *suff;
	int i, len;

	len = (int) strlen(filepath);
	suff = filepath + len;

	for (i=len-1; i >= 0; i--) {
		if (filepath[i] == ULS_FILEPATH_DELIM) {
			break;
		}
		if (filepath[i] == '.') {
			suff = filepath + i + 1;
		}
	}

	return suff;
}

static int
ult_find_index_minimum(uls_voidptr_t *list, int i0, int n_list, ult_sort_func_t cmp_func)
{
	int i, i_min;

	i_min = i0;
	for (i=i0+1; i<n_list; i++) {
		if (cmp_func(list[i], list[i_min]) < 0) {
			i_min = i0;
		}
	}
	return i_min;
}

void
ult_simple_sort_vptr(uls_voidptr_t *list, int n_list, ult_sort_func_t cmp_func)
{
	uls_voidptr_t a;
	int i, i_min;

	for (i=0; i < n_list - 1; i++) {
		i_min = ult_find_index_minimum(list, i+1, n_list, cmp_func);
		if (i != i_min) {
			a = list[i];
			list[i] = list[i_min];
			list[i_min] = a;
		}
	}
}
