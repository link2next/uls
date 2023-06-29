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
  <file> uls_fileio.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_FILEIO__
#include "uls/uls_fileio.h"
#include "uls/uls_misc.h"
#include "uls/uls_auw.h"
#include "uls/uls_log.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#ifndef ULS_WINDOWS
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_create_check)(const char* fpath, uls_outparam_ptr_t parms)
{
	int mode = parms->n1;
	int ftype, mode2, perm2;

	if ((mode & ULS_FIO_RDWR) == 0) {
		mode |= ULS_FIO_WRITE;
	}

	if ((ftype=uls_dirent_exist(fpath)) < 0) {
		return -1;
	}

	if (ftype > 0) {
		if (ftype == ST_MODE_FILE) {
			if (mode & ULS_FIO_EXCL) {
				return -2;
			}
		} else {
			return -3;
		}
	}

#ifdef ULS_WINDOWS
	mode2 = _O_CREAT | _O_BINARY | _O_TRUNC | _O_NOINHERIT;
	if (mode & ULS_FIO_READ) {
		mode2 |= _O_RDWR;
		perm2 = _S_IWRITE | _S_IREAD;
	} else {
		mode2 |= _O_WRONLY;
		perm2 = _S_IWRITE;
	}
#else
	mode2 = O_CREAT | O_TRUNC;
	if (mode & ULS_FIO_READ) {
		mode2 |= O_RDWR;
	} else {
		mode2 |= O_WRONLY;
	}
	perm2 = 0644;
#endif

	parms->n1 = mode;
	parms->n2 = mode2;
	parms->n = perm2;

	return ftype;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_open_check)(uls_outparam_ptr_t parms)
{
	int mode = parms->n1, mode2;

	if ((mode & ULS_FIO_RDWR) == 0) {
		mode |= ULS_FIO_READ;
	}

#ifdef ULS_WINDOWS
	mode2 = _O_BINARY;
	if (mode & ULS_FIO_WRITE) {
		mode2 |=  _O_RDWR;
	} else {
		mode2 |= _O_RDONLY;
	}
#else
	mode2 = 0;
	if (mode & ULS_FIO_WRITE) {
		mode2 |=  O_RDWR;
	} else {
		mode2 |= O_RDONLY;
	}
#endif

	parms->n1 = mode;
	parms->n2 = mode2;

	return 0;
}

#ifdef ULS_WINDOWS
ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_create)(const char* fpath, int mode)
{
	const char *astr;
	int fd, mode2, perm2;
	auw_outparam_t buf_csz;
	uls_outparam_t parms;

	parms.n1 = mode;
	if (__uls_fd_create_check(fpath, uls_ptr(parms)) < 0) {
		return -1;
	}
	mode = parms.n1;
	mode2 = parms.n2;
	perm2 = parms.n;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(fpath, -1, uls_ptr(buf_csz))) == NULL) {
		fd = -1;
	} else {
		fd = _open(astr, mode2, perm2);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));

	return fd;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_open)(const char* fpath, int mode)
{
	const char *astr;
	int fd, mode2;
	auw_outparam_t buf_csz;
	uls_outparam_t parms;

	parms.n1 = mode;
	__uls_fd_open_check(uls_ptr(parms));
	mode = parms.n1;
	mode2 = parms.n2;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(fpath, -1, uls_ptr(buf_csz))) == NULL) {
		fd = -1;
	} else {
		fd = _open(astr, mode2);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return fd;
}

#else

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_create)(const char* fpath, int mode)
{
	int fd, mode2, perm2=0644;
	uls_outparam_t parms;

	parms.n1 = mode;
	if (__uls_fd_create_check(fpath, uls_ptr(parms)) < 0) {
		return -1;
	}
	mode = parms.n1;
	mode2 = parms.n2;
	perm2 = parms.n;

	fd = open(fpath, mode2, perm2);
	return fd;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_fd_open)(const char* fpath, int mode)
{
	int fd, mode2;
	uls_outparam_t parms;

	parms.n1 = mode;
	__uls_fd_open_check(uls_ptr(parms));
	mode = parms.n1;
	mode2 = parms.n2;

	if ((fd = open(fpath, mode2)) < 0) {
	 	_uls_log(err_log)("can't open '%s'", fpath);
	 	return -1;
	}

	return fd;
}

#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__open_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	int fd_out, len, i=0;
	const char *basedir;
	char filepath_buf[ULS_TEMP_FILEPATH_MAXSIZ + 1];
#ifdef ULS_WINDOWS
	auw_outparam_t buf_csz;

	if ((basedir=getenv("TEMP")) == NULL || uls_dirent_exist_astr(basedir) != ST_MODE_DIR) {
		if (uls_dirent_exist(ULS_OS_TEMP_DIR) != ST_MODE_DIR)
			_uls_log(err_panic)("ULS: can't make temporary directory, '%s'!", ULS_OS_TEMP_DIR);
		basedir = ULS_OS_TEMP_DIR;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((basedir = uls_astr2ustr_ptr(basedir, -1, uls_ptr(buf_csz))) == NULL) {
		auw_deinit_outparam(uls_ptr(buf_csz));
		return -1;
	}
#else
	basedir = ULS_OS_TEMP_DIR;
#endif

	while (1) {
		len = _uls_log_(snprintf)(filepath_buf, ULS_TEMP_FILEPATH_MAXSIZ,
			"%s%c_ULStmpfile0x%x", basedir, ULS_FILEPATH_DELIM, rand());

		uls_set_namebuf_value_this(tmpfile->filepath, filepath_buf);

		if ((fd_out = uls_fd_open(uls_get_namebuf_value_this(tmpfile->filepath),
			ULS_FIO_CREAT|ULS_FIO_RDWR|ULS_FIO_EXCL)) >= 0) {
			tmpfile->len_filepath = len;
			tmpfile->fd = fd_out;
			break;
		}

		if (++i >= 500) {
			fd_out = -1;
			break;
		}
	}

	tmpfile->fp = NULL;
#ifdef ULS_WINDOWS
	auw_deinit_outparam(uls_ptr(buf_csz));
#endif
	return fd_out;
}

int
ULS_QUALIFIED_METHOD(uls_readn)(int fd, uls_native_vptr_t vptr, int n)
{
	char *ptr = (char *) vptr;
	int	nleft, rc;

	if (n <= 0) {
		_uls_log(err_log)("%s: invalid parameter n=%d!", __func__, n);
		return -3;
	}

	for (nleft=n; nleft > 0; ) {
		if ((rc=uls_fd_read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)  continue;
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				uls_msleep(10);
				continue;
			} else {
				_uls_log(err_log)("%s: %s", __func__, strerror(errno));
			}
			return -1;
		} else if (rc==0) {
			break;
		}

		nleft -= rc;
		ptr   += rc;
	}

	return n - nleft;
}

int
ULS_QUALIFIED_METHOD(uls_writen)(int fd, uls_native_vptr_t vptr, int n)
{
	char *ptr = (char *) vptr;
	int	 nleft, rc;

	if (n < 0) {
		_uls_log(err_log)("%s: invalid parameter n=%d!", __func__, n);
		return -3;
	}

	for (nleft=n; nleft > 0; ) {
		if ((rc=uls_fd_write(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)  continue;
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				uls_msleep(10);
				continue;
			} else {
				_uls_log(err_log)("%s: %s", __func__, strerror(errno));
			}
			return -1;
		} else if (rc==0) {
			break;
		}

		nleft -= rc;
		ptr   += rc;
	}

	return n - nleft;
}

int
ULS_QUALIFIED_METHOD(uls_readline)(int fd, char* ptr, int n)
{
	int  i, rc;

	if (n < 1 || ptr==NULL || fd < 0) {
		_uls_log(err_log)(" invalid param, n=%d", n);
		return -1;
	}

	--n;

	for (i=0; ; i++) {
		if (i >= n) {
//			_uls_log(err_log)("too long line, truncating, ...");
			break;
		}

		if ((rc=uls_fd_read(fd, ptr, 1)) < 0) {
			if (errno==EINTR)  continue;
			return -1;
		} else if (rc==0) {
			break;
		}

		if (*ptr++ == '\n') {
			++i;
			break;
		}
	}

	*ptr = '\0';
	return i;
}

#ifdef ULS_WINDOWS

int
ULS_QUALIFIED_METHOD(uls_chdir)(const char* path)
{
	const char *astr;
	int rval;
	auw_outparam_t buf_csz;

	if (path == NULL) return -1;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(path, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = _chdir(astr);
		if (rval != 0) rval = -1;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

int
ULS_QUALIFIED_METHOD(uls_getcwd)(char* buf, int buf_siz)
{
	char  *ptr;
	const char *ustr;
	int ulen;
	auw_outparam_t buf_csz;

	if ((ptr = _getcwd(buf, buf_siz)) == NULL) {
		return -1;
	}

	auw_init_outparam(uls_ptr(buf_csz));

	if ((ustr = uls_astr2ustr_ptr(buf, -1, uls_ptr(buf_csz))) == NULL ||
		(ulen = buf_csz.outlen) >= buf_siz) {
		ulen = -1;
	} else {
		uls_memcopy(buf, ustr, ulen);
		buf[ulen] = '\0';
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return ulen;
}

int
ULS_QUALIFIED_METHOD(uls_unlink)(const char *filepath)
{
	const char *astr;
	int rval;
	auw_outparam_t buf_csz;

	if (filepath == NULL) return -1;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(filepath, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -1;
	} else {
		rval = _unlink(astr);
		if (rval != 0) rval = -1;
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

#else // ~ULS_WINDOWS

int
ULS_QUALIFIED_METHOD(uls_chdir)(const char* path)
{
	if (path == NULL) return -1;
	return chdir(path);
}

int
ULS_QUALIFIED_METHOD(uls_getcwd)(char* buf, int buf_siz)
{
	const char *ptr;

	if (buf == NULL) return -1;
	ptr = getcwd(buf, buf_siz);
	return ptr == NULL ? -1: uls_strlen(buf);
}

int
ULS_QUALIFIED_METHOD(uls_unlink)(const char *filepath)
{
	if (filepath == NULL) return -1;
	return unlink(filepath);
}

#endif // ULS_WINDOWS

int
ULS_QUALIFIED_METHOD(uls_copyfile_fd)(int fd_in, int fd_out)
{
	int rc, stat = 1;
	char buf[512];

	while (stat > 0) {
		if ((rc=uls_readn(fd_in, buf, sizeof(buf))) < 0) {
			_uls_log(err_log)("%s: error in reading ..", __func__);
			stat = -1;
			break;
		} else if (rc < sizeof(buf)) {
			stat = 0;
			if (rc == 0) break;
		}

		if (uls_writen(fd_out, buf, rc) < rc) {
			_uls_log(err_log)("%s: error in writing after writing %d ..", __func__, rc);
			stat = -2;
		}
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_copyfile)(const char* filepath1, const char* filepath2)
{
	int  fd_in, fd_out;
	int  rc;

	if ((fd_in=uls_fd_open(filepath1, 0)) < 0) {
		return -1;
	}

	if ((fd_out = uls_fd_open(filepath2, ULS_FIO_CREAT)) < 0) {
		uls_fd_close(fd_in);
		return -1;
	}

	rc = uls_copyfile_fd(fd_in, fd_out);
	uls_fd_close(fd_out);
	uls_fd_close(fd_in);

	return rc;
}

int
ULS_QUALIFIED_METHOD(uls_movefile)(const char* fpath1, const char* fpath2)
{
	if (fpath1 == NULL || *fpath1 == '\0') return -1;
	if (fpath2 == NULL || *fpath2 == '\0') return -1;

	if (fpath1 == fpath2 || uls_streql(fpath1, fpath2)) {
		return 0;
	}

	if (uls_copyfile(fpath1, fpath2) < 0) {
		_uls_log(err_log)("%s:(copy-file) error", __func__);
		return -1;
	}

	if (uls_unlink(fpath1) < 0) {
		_uls_log(err_log)("%s:(unlink) error", __func__);
		return -1;
	}
	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_close_tempfile)(uls_tempfile_ptr_t tmpfile, const char* filepath)
{
	int stat = 1;

	if (uls_get_namebuf_value_this(tmpfile->filepath) == filepath) {
		_uls_log(err_log)("%s: invalid filepath!", __func__);
		return -4;
	}

	if (tmpfile->fp != NULL) {
		uls_fp_close(tmpfile->fp);
	} else if (tmpfile->fd >= 0) {
		uls_fd_close(tmpfile->fd);
	}

	if (uls_dirent_exist(uls_get_namebuf_value_this(tmpfile->filepath)) != ST_MODE_FILE) {
		_uls_log(err_log)("%s: can' find the tempfile %s!", __func__, uls_get_namebuf_value_this(tmpfile->filepath));
		stat = -3;

	} else if (filepath == NULL) {
		if (uls_unlink(uls_get_namebuf_value_this(tmpfile->filepath)) < 0) {
			_uls_log(err_log)("%s: unlink error!", __func__);
			stat = -2;
		} else {
			stat = 2;
		}

	} else if (uls_movefile(uls_get_namebuf_value_this(tmpfile->filepath), filepath) < 0) {
		_uls_log(err_log)("can't move the temp-file '%s' to '%s'.", uls_get_namebuf_value_this(tmpfile->filepath), filepath);
		stat = -1;
	}

	tmpfile->fp = NULL;
	tmpfile->fd = -1;

	return stat;
}

void
ULS_QUALIFIED_METHOD(initialize_uls_fileio)(void)
{
	uls_stdio_box_ptr_t fpwrap;
	int i;

	uls_init_array_type00(uls_ptr(stdio_boxlst), stdio_box, ULS_N_BOXLST);
	for (i=0; i<ULS_N_BOXLST; i++) {
		fpwrap = uls_get_array_slot_type00(uls_ptr(stdio_boxlst), i);
		fpwrap->fp_num = i;
		fpwrap->fp = _uls_stdio_fp(i);
	}
}

void
ULS_QUALIFIED_METHOD(finalize_uls_fileio)(void)
{
	uls_deinit_array_type00(uls_ptr(stdio_boxlst), stdio_box);
}

int
ULS_QUALIFIED_METHOD(_uls_stdio_fd)(int fp_num)
{
	int fd;

	if (fp_num == 0) {
		fd = STDIN_FILENO;
	} else if (fp_num == 1) {
		fd = STDOUT_FILENO;
	} else if (fp_num == 2) {
		fd = STDERR_FILENO;
	} else {
		fd = -1;
	}

	return fd;
}

FILE*
ULS_QUALIFIED_METHOD(_uls_stdio_fp)(int fp_num)
{
	FILE *fp;

	if (fp_num == 0) {
		fp = stdin;
	} else if (fp_num == 1) {
		fp = stdout;
	} else if (fp_num == 2) {
		fp = stderr;
	} else {
		fp = NULL;
	}

	return fp;
}

ULS_QUALIFIED_RETTYP(uls_stdio_box_ptr_t)
ULS_QUALIFIED_METHOD(_uls_stdio_fp_box)(int fp_num)
{
	uls_stdio_box_ptr_t fpwrap;

	if (fp_num < 0 || fp_num >= ULS_N_BOXLST) {
		return nilptr;
	}

	fpwrap = uls_get_array_slot_type00(uls_ptr(stdio_boxlst), fp_num);
	return fpwrap;
}

#ifdef ULS_WINDOWS

int
ULS_QUALIFIED_METHOD(uls_dirent_exist_astr)(const char* afpath)
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

int
ULS_QUALIFIED_METHOD(uls_dirent_exist)(const char* path)
{
	int rval;
	const char *astr;
	auw_outparam_t buf_csz;

	if (path == NULL || *path == '\0')
		return -1;

	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(path, -1, uls_ptr(buf_csz))) == NULL) {
		rval = -2;
	} else {
		rval = uls_dirent_exist_astr(astr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
	return rval;
}

#else

int
ULS_QUALIFIED_METHOD(uls_dirent_exist)(const char* path)
{
	struct stat statbuff;
	int rval;

	if (path == NULL || *path == '\0')
		return -1;

	while (stat(path, uls_ptr(statbuff)) < 0) {
		if (errno == EINTR) continue;
		if (errno == ENOENT) return ST_MODE_NOENT;
		/* ~ENOENT */
		return -1;
	}

	if (S_ISREG(statbuff.st_mode)) rval = ST_MODE_REG;
	else if (S_ISDIR(statbuff.st_mode)) rval = ST_MODE_DIR;
	else if (S_ISLNK(statbuff.st_mode)) rval = ST_MODE_SYMLNK;
	else if (S_ISCHR(statbuff.st_mode)) rval = ST_MODE_CHR;
	else if (S_ISBLK(statbuff.st_mode)) rval = ST_MODE_BLK;
	else if (S_ISFIFO(statbuff.st_mode)) rval = ST_MODE_FIFO;
	else if (S_ISSOCK(statbuff.st_mode)) rval = ST_MODE_SOCK;
	else rval = ST_MODE_SPECIAL;

	return rval;
}

#endif

#ifdef ULS_WINDOWS
void
ULS_QUALIFIED_METHOD(uls_fd_close)(int fd)
{
	if (fd >= 0) {
		_close(fd);
	}
}

#else
void
ULS_QUALIFIED_METHOD(uls_fd_close)(int fd)
{
	if (fd >= 0) {
		close(fd);
	}
}
#endif

int
ULS_QUALIFIED_METHOD(uls_fd_open)(const char* fpath, int mode)
{
	int fd;

	if (fpath == NULL) return -1;

	if (mode & ULS_FIO_CREAT) {
		fd = __uls_fd_create(fpath, mode);
	} else {
		fd = __uls_fd_open(fpath, mode);
	}

	return fd;
}

void
ULS_QUALIFIED_METHOD(uls_put_binstr)(const char* str, int len, int fd)
{
	if (len < 0) len = uls_strlen(str);

	if (uls_fd_write(fd, str, len) < 0) {
		_uls_log_primitive(err_panic)("put_bin_str: write error!");
	}
}

int
ULS_QUALIFIED_METHOD(uls_fgetc_ascii_str)(uls_voidptr_t dat, char *buf, int buf_siz)
{
	uls_stdio_box_ptr_t fpwrap = (uls_stdio_box_ptr_t) dat;
	FILE *fp = fpwrap->fp;
	int ch;

	if ((ch=fgetc(fp)) == EOF) {
		if (ferror(fp) || !feof(fp)) {
			return -1;
		}
		return 0;
	}

	*buf = (char) ch;
	return 1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__consume_ms_mbcs_char_getbyte)(FILE *fp, char *bufptr)
{
	int ch;

	if ((ch=fgetc(fp)) == EOF) {
		if (ferror(fp) || !feof(fp)) {
			return -1;
		}
		return 0;
	}

	*bufptr = (char) ch;
	return 1;
}

int
ULS_QUALIFIED_METHOD(consume_ms_mbcs_onechar)(uls_voidptr_t dat, char *buf, int buf_siz)
{
	uls_stdio_box_ptr_t fpwrap = (uls_stdio_box_ptr_t) dat;
	FILE *fp = fpwrap->fp;
	int n, i, rc;

	if ((rc=__consume_ms_mbcs_char_getbyte(fp, buf)) <= 0) {
		return rc;
	}

	if ((n = astr_lengthof_char(buf)) >= buf_siz) {
		return -1;
	}

	for (i=1; i<n; i++) {
		if (__consume_ms_mbcs_char_getbyte(fp, buf+i) <= 0) {
			return -1;
		}
	}

	return i; // not n but i
}

void
ULS_QUALIFIED_METHOD(uls_init_fio)(uls_fio_ptr_t fio, int flags)
{
	fio->flags = ULS_FL_STATIC | flags;
	fio->fgetch = uls_ref_callback_this(uls_fgetc_ascii_str);
	fio->dat = (uls_voidptr_t) _uls_stdio_fp_box(0);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_fio)(uls_fio_ptr_t fio)
{
	fio->flags &= ULS_FL_STATIC;
	fio->dat = nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_reset_fio)(uls_fio_ptr_t fio, uls_voidptr_t dat, uls_fgetch_t consumer)
{
	if (consumer == nilptr) {
		consumer = uls_ref_callback_this(uls_fgetc_ascii_str);
	}

	fio->dat = dat;
	fio->fgetch = consumer;
}

ULS_QUALIFIED_RETTYP(uls_fio_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_fio)(uls_voidptr_t dat, uls_fgetch_t consumer, int flags)
{
	uls_fio_ptr_t fio;

	fio = uls_alloc_object(uls_fio_t);
	uls_init_fio(fio, flags);
	fio->flags &= ~ULS_FL_STATIC;

	uls_reset_fio(fio, dat, consumer);

	return fio;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_fio)(uls_fio_ptr_t fio)
{
	if (fio == nilptr) return;

	uls_deinit_fio(fio);
	if ((fio->flags & ULS_FL_STATIC) == 0) {
		uls_dealloc_object(fio);
	}
}

int
ULS_QUALIFIED_METHOD(uls_fio_gets)(uls_fio_ptr_t fio, char* buf, int buf_siz)
{
	char *bufptr, *buf_end;
	int len, rc, esc=0;
	int ch, flags=fio->flags;

	if (buf == NULL || buf_siz < 1) {
		_uls_log(err_log)("%s: invalid parameter buf or buf_siz=%d", __func__, buf_siz);
		return ULS_EOF - 3;
	}

	buf_end = buf + buf_siz;

	for (bufptr=buf,rc=0; ; ) {
		if ((bufptr+=rc) >= buf_end) {
			_uls_log(err_log)("%s: Too long line(>%d)! return error!", __func__, buf_siz);
			len = ULS_EOF - 2;
			break;
		}

		if ((rc=fio->fgetch(fio->dat, bufptr, (int)(buf_end-bufptr))) <= 0) {
			*bufptr = '\0';
			if (rc < 0) {
				len = ULS_EOF - 1;
			} else { // rc == 0
				if ((len = (int) (bufptr - buf)) <= 0) {
					len = ULS_EOF;
				}
			}
			break;
		} else if (rc > 1) {
			continue;
		}

		if ((ch = *bufptr) == '\n') {
			if ((flags & ULS_FIO_MULTLINE) && esc) {
				--bufptr; // invalidating the previous char '\\'
				if (flags & ULS_FIO_KEEP_LF) {
					*bufptr = '\n';
				} else {
					rc = 0;
				}

				esc = 0;
				continue;
			}

			if (buf < bufptr && (flags & ULS_FIO_DEL_CR)) {
				if (bufptr[-1] == '\r') {
					--bufptr;
				} else if (buf[0] == '\r') {
					len = (int) (bufptr - buf);
					uls_memmove(buf, buf+1, len-1);
					--bufptr;
				}
			}

			*bufptr = '\0';
			len = (int) (bufptr - buf);
			break;
		}

		if (ch == '\r' && (flags & ULS_FIO_DEL_CR)) {
			rc = 0;
		} else {
			esc = (ch == '\\') ? 1 : 0;
		}
	}

	return len;
}

ULS_QUALIFIED_RETTYP(uls_file_ptr_t)
ULS_QUALIFIED_METHOD(uls_open_filp)(const char *filepath, int mode)
{
	uls_callback_type_this(fgetch) consumer;
	uls_stdio_box_ptr_t p_fpwrap;
	uls_file_ptr_t filp;
	FILE *fp;

	if (filepath == NULL) return nilptr;
	filp = uls_alloc_object(uls_file_t);

	if ((fp = uls_fp_open(filepath, mode)) == NULL) {
		uls_dealloc_object(filp);
		return nilptr;
	}

	if (mode & ULS_FIO_MS_MBCS) {
		consumer = uls_ref_callback_this(consume_ms_mbcs_onechar);
	} else {
		consumer = uls_ref_callback_this(uls_fgetc_ascii_str);
	}

	uls_init_fio(uls_ptr(filp->fio), mode);

	p_fpwrap = uls_ptr(filp->fpwrap);
	p_fpwrap->fp_num = -1;
	p_fpwrap->fp = fp;

	uls_reset_fio(uls_ptr(filp->fio), p_fpwrap, consumer);

	return filp;
}

void
ULS_QUALIFIED_METHOD(uls_close_filp)(uls_file_ptr_t filp)
{
	uls_stdio_box_ptr_t p_fpwrap;

	if (filp == nilptr) return;

	p_fpwrap = uls_ptr(filp->fpwrap);
	uls_fp_close(p_fpwrap->fp);

	uls_deinit_fio(uls_ptr(filp->fio));
	uls_dealloc_object(filp);
}

int
ULS_QUALIFIED_METHOD(uls_filp_gets)(uls_file_ptr_t filp, char* buf, int buf_siz)
{
	uls_stdio_box_ptr_t p_fpwrap = uls_ptr(filp->fpwrap);
	FILE *fp = p_fpwrap->fp;
	long fpos;
	int len;

	if (ferror(fp)) return ULS_EOF-1;
	fpos = ftell(fp);

	if ((len = uls_fio_gets(uls_ptr(filp->fio), buf, buf_siz)) < ULS_EOF) {
		fseek(fp, fpos, SEEK_SET);
	}

	return len;
}

FILE*
ULS_QUALIFIED_METHOD(uls_fp_open)(const char *filepath, int mode)
{
	FILE* fp;
	char modestr[8];
	int j, rc;
#ifdef ULS_WINDOWS
	const char *astr;
	auw_outparam_t buf_csz;
#endif
	uls_outparam_t parms;

	if (filepath == NULL) return NULL;

	parms.n1 = mode;
	if (mode & ULS_FIO_CREAT) {
		rc = __uls_fd_create_check(filepath, uls_ptr(parms));
	} else {
		rc = __uls_fd_open_check(uls_ptr(parms));
	}
	mode = parms.n1;

	if (rc < 0) {
		_uls_log(err_log)("%s: invalid mode", __func__);
		return NULL;
	}

	j = 0;
	if (mode & ULS_FIO_CREAT) {
		modestr[j++] = 'w';
#ifdef ULS_WINDOWS
		modestr[j++] = 'b';
#endif
		if (mode & ULS_FIO_READ) {
			modestr[j++] = '+';
		}
	} else {
		modestr[j++] = 'r';
#ifdef ULS_WINDOWS
		modestr[j++] = 'b';
#endif
		if (mode & ULS_FIO_WRITE) {
			modestr[j++] = '+';
		}
	}
	modestr[j] = '\0';

#ifdef ULS_WINDOWS
	auw_init_outparam(uls_ptr(buf_csz));

	if ((astr = uls_ustr2astr_ptr(filepath, -1,  uls_ptr(buf_csz))) == NULL) {
		_uls_log(err_log)("%s: encoding error!", __func__);
		fp = NULL;
	} else {
		fp = fopen(astr, modestr);
	}

	auw_deinit_outparam(uls_ptr(buf_csz));
#else
	fp = fopen(filepath, modestr);
#endif

	return fp;
}

int
ULS_QUALIFIED_METHOD(uls_fp_gets)(FILE *fp, char* buf, int buf_siz, int flags)
{
	uls_fio_t fio;
	uls_stdio_box_t fpwrap;
	int rc;

	uls_init_fio(uls_ptr(fio), flags | ULS_FIO_DEL_CR);

	fpwrap.fp_num = -1;
	fpwrap.fp = fp;
	uls_reset_fio(uls_ptr(fio), uls_ptr(fpwrap), nilptr);

	rc = uls_fio_gets(uls_ptr(fio), buf, buf_siz);

	uls_deinit_fio(uls_ptr(fio));

	return rc;
}

void
ULS_QUALIFIED_METHOD(uls_fp_close)(FILE *fp)
{
	if (fp != NULL) {
		if (fp != _uls_stdio_fp(2) && fp != _uls_stdio_fp(1) && fp != _uls_stdio_fp(0)) {
			fclose(fp);
		}
	}
}

int
ULS_QUALIFIED_METHOD(uls_fp_getc)(FILE *fp)
{
	return getc(fp);
}

void
ULS_QUALIFIED_METHOD(uls_fp_putc)(FILE *fp, char ch)
{
	putc(ch, fp);
}

void
ULS_QUALIFIED_METHOD(uls_init_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	tmpfile->flags = ULS_FL_STATIC;
	uls_init_namebuf_this(tmpfile->filepath, ULS_TEMP_FILEPATH_MAXSIZ);
	tmpfile->len_filepath = 0;
	tmpfile->fd = -1;
	tmpfile->fp = NULL;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	uls_close_tempfile(tmpfile, NULL);
}

ULS_QUALIFIED_RETTYP(uls_tempfile_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_tempfile)(void)
{
	uls_tempfile_ptr_t tmpfile;

	tmpfile = uls_alloc_object(uls_tempfile_t);
	uls_init_tempfile(tmpfile);
	tmpfile->flags &= ~ULS_FL_STATIC;
	return tmpfile;
}

int
ULS_QUALIFIED_METHOD(uls_open_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	int fd;

	uls_sys_lock();

	if (tmpfile->len_filepath > 0) {
		_uls_log(err_log)("The tempfile already used!");
		fd = -1;
	} else {
		fd = __open_tempfile(tmpfile);
	}

	uls_sys_unlock();

	return fd;
}

FILE*
ULS_QUALIFIED_METHOD(uls_fopen_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	int fd_out;
	FILE *fp_out;
#ifdef ULS_WINDOWS
	const char *modstr = "wb+";
#else
	const char *modstr = "w+";
#endif

	uls_sys_lock();
	if (tmpfile->len_filepath > 0) {
		uls_sys_unlock();
		_uls_log(err_log)("The tempfile already used!");
		return NULL;
	}

	if ((fd_out = __open_tempfile(tmpfile)) < 0 ||
		(tmpfile->fp = uls_fp_fdopen(fd_out, modstr)) == NULL) {
		uls_fd_close(fd_out);
		fp_out = NULL;
	} else {
		fp_out = tmpfile->fp;
		tmpfile->fd = -1;
	}

	uls_sys_unlock();
	return fp_out;
}

int
ULS_QUALIFIED_METHOD(uls_close_tempfile)(uls_tempfile_ptr_t tmpfile, const char* filepath)
{
	int stat = 0;
	uls_sys_lock();

	if (tmpfile->len_filepath > 0) {
		if (__uls_close_tempfile(tmpfile, filepath) < 0) {
			_uls_log(err_log)("%s: failed to deinit tempfile!", __func__);
			stat = -1;
		}
		tmpfile->len_filepath = 0;
	}

	if (tmpfile->len_filepath >= 0) {
		uls_deinit_namebuf_this(tmpfile->filepath);
		tmpfile->len_filepath = -1;
	}

	uls_sys_unlock();
	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_tempfile)(uls_tempfile_ptr_t tmpfile)
{
	if (tmpfile == nilptr) return;
	uls_deinit_tempfile(tmpfile);

	if (!(tmpfile->flags & ULS_FL_STATIC)) {
		uls_dealloc_object(tmpfile);
	}
}
