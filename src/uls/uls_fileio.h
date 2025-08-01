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
  <file> uls_fileio.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2015.
  </author>
*/

#ifndef __ULS_FILEIO_H__
#define __ULS_FILEIO_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define ULS_EOF  -1

#define ULS_FIO_READ       0x01
#define ULS_FIO_WRITE      0x02
#define ULS_FIO_EXCL       0x04
#define ULS_FIO_NO_UTF8BOM 0x08

#define ULS_FIO_MULTLINE 0x0100
#define ULS_FIO_KEEP_LF  0x0200
#define ULS_FIO_DEL_CR   0x0400

#define ULS_FIO_MS_MBCS  0x4000
#define ULS_FIO_UTF8     0x8000

#define uls_export_tempfile(tmp) uls_close_tempfile(tmp, (tmp)->filepath)
#define uls_tempfile_fp(tmp) ((tmp)->fp)
#define uls_tempfile_fd(tmp) ((tmp)->fd)
#define uls_tempfile_is_open(tmp) ((tmp)->len_filepath>0)
#define uls_tempfile_path(tmpfile) uls_get_namebuf_value((tmpfile)->filepath)
#define uls_tempfile_pathlen(tmpfile) ((tmpfile)->len_filepath)

#endif // ULS_DECL_GLOBAL_TYPES

#ifdef ULS_DECL_PUBLIC_TYPE

#define ST_MODE_NOENT    0
#define ST_MODE_REG      1
#define ST_MODE_DIR      2
#define ST_MODE_SYMLNK   3
#define ST_MODE_CHR      4
#define ST_MODE_BLK      5
#define ST_MODE_FIFO     6
#define ST_MODE_SOCK     7
#define ST_MODE_SPECIAL  8
#define ST_MODE_FILE     ST_MODE_REG

#ifdef __ULS_WINDOWS__
#define uls_fd_seek      _lseek
#define uls_fd_read      _read
#define uls_fd_write     _write
#define uls_fp_fdopen    _fdopen
#define uls_fp_fileno    _fileno
#define uls_fd_dup       _dup
#define uls_popen        _popen
#else
#define uls_fd_seek      lseek
#define uls_fd_read      read
#define uls_fd_write     write
#define uls_fp_fdopen    fdopen
#define uls_fp_fileno    fileno
#define uls_fd_dup       dup
#define uls_popen        popen
#endif

#define ULS_TEMP_FILEPATH_MAXSIZ  127

ULS_DEFINE_DELEGATE_BEGIN(fgetch, int)(uls_voidptr_t dat, char *buf, int buf_siz);
ULS_DEFINE_DELEGATE_END(fgetch);
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(fio)
{
	uls_flags_t flags;
	uls_callback_type_this(fgetch) fgetch;
	uls_voidptr_t dat;
};

ULS_DEFINE_STRUCT(stdio_box)
{
	int fp_num;
	FILE *fp;
};
ULS_DEF_ARRAY_TYPE00(stdio_box, STDIO_BOX_TYPE00_ULS_N_BOXLST, ULS_N_BOXLST);

ULS_DEFINE_STRUCT(file)
{
	uls_fio_t fio;
	uls_stdio_box_t fpwrap;
};

ULS_DEFINE_STRUCT(tempfile)
{
	uls_flags_t flags;

	uls_def_namebuf_this(filepath, ULS_TEMP_FILEPATH_MAXSIZ);
	int  len_filepath;

	int  fd;
	FILE *fp;
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_FILEIO__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_decl_array_type00(stdio_boxlst, stdio_box, ULS_N_BOXLST);
#endif

#if defined(__ULS_FILEIO__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __uls_fd_create_check(const char *fpath, uls_outparam_ptr_t parms);
ULS_DECL_STATIC int __uls_fd_open_check(uls_outparam_ptr_t parms);
ULS_DECL_STATIC int __uls_fd_create(const char *fpath, int mode);
ULS_DECL_STATIC int __uls_fd_open(const char *fpath, int mode);
ULS_DECL_STATIC int __consume_ms_mbcs_char_getbyte(FILE *fp, char *bufptr);
ULS_DECL_STATIC int __open_tempfile(uls_tempfile_ptr_t tmpfile);
ULS_DECL_STATIC int __uls_close_tempfile(uls_tempfile_ptr_t tmpfile, const char *filepath);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void initialize_uls_fileio(void);
void finalize_uls_fileio(void);

ULS_DLL_EXTERN void uls_init_fio(uls_fio_ptr_t fio, int flags);
ULS_DLL_EXTERN void uls_deinit_fio(uls_fio_ptr_t fio);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
int uls_readn(int fd, uls_native_vptr_t ptr, int n);
int uls_writen(int fd, uls_native_vptr_t ptr, int n);
int uls_readline(int fd, char *ptr, int n);

ULS_DLL_EXTERN int uls_chdir(const char *path);
ULS_DLL_EXTERN int uls_getcwd(char *buf, int buf_siz);
ULS_DLL_EXTERN int uls_unlink(const char *filepath);

int uls_copyfile_fd(int fd_in, int fd_out);
ULS_DLL_EXTERN int uls_copyfile(const char *filepath1, const char *filepath2);
ULS_DLL_EXTERN int uls_movefile(const char *filepath1, const char *filepath2);

ULS_DLL_EXTERN int _uls_stdio_fd(int fp_num);
ULS_DLL_EXTERN FILE* _uls_stdio_fp(int fp_num);
ULS_DLL_EXTERN uls_stdio_box_ptr_t _uls_stdio_fp_box(int fp_num);

ULS_DLL_EXTERN int uls_dirent_exist(const char *path);
ULS_DLL_EXTERN int uls_fd_open(const char *fpath, int mode);
ULS_DLL_EXTERN void uls_fd_close(int fd);
ULS_DLL_EXTERN void uls_put_binstr(const char *str, int len, int fd);

ULS_DLL_EXTERN int uls_fgetc_ascii_str(uls_voidptr_t dat, char *buf, int buf_siz);
ULS_DLL_EXTERN int consume_ms_mbcs_onechar(uls_voidptr_t dat, char *buf, int buf_siz);

ULS_DLL_EXTERN void uls_reset_fio(uls_fio_ptr_t fio, uls_voidptr_t dat, uls_fgetch_t consumer);
ULS_DLL_EXTERN uls_fio_ptr_t uls_create_fio(uls_voidptr_t dat, uls_fgetch_t consumer, int flags);
ULS_DLL_EXTERN void uls_destroy_fio(uls_fio_ptr_t fio);

ULS_DLL_EXTERN int uls_fio_gets(uls_fio_ptr_t fio, char *buf, int buf_siz);

ULS_DLL_EXTERN uls_file_ptr_t uls_open_filp(const char *filepath, int flags);
ULS_DLL_EXTERN void uls_close_filp(uls_file_ptr_t filp);
ULS_DLL_EXTERN int uls_filp_gets(uls_file_ptr_t filp, char *buf, int buf_siz);

ULS_DLL_EXTERN FILE* uls_fp_open(const char *filepath, int mode);
ULS_DLL_EXTERN void uls_fp_close(FILE *fp);
ULS_DLL_EXTERN int uls_fp_gets(FILE *fp, char *buf, int buf_siz, int flags);
ULS_DLL_EXTERN int uls_fp_getc(FILE *fp);
ULS_DLL_EXTERN void uls_fp_putc(FILE *fp, char ch);
ULS_DLL_EXTERN int uls_fp_filesize(FILE *fin);

ULS_DLL_EXTERN void uls_init_tempfile(uls_tempfile_ptr_t tmpfile);
ULS_DLL_EXTERN void uls_deinit_tempfile(uls_tempfile_ptr_t tmpfile);
ULS_DLL_EXTERN uls_tempfile_ptr_t uls_create_tempfile(void);
ULS_DLL_EXTERN int uls_open_tempfile(uls_tempfile_ptr_t tmpfile);
ULS_DLL_EXTERN FILE* uls_fopen_tempfile(uls_tempfile_ptr_t tmpfile);
ULS_DLL_EXTERN int uls_close_tempfile(uls_tempfile_ptr_t tmpfile, const char *filepath);
ULS_DLL_EXTERN void uls_destroy_tempfile(uls_tempfile_ptr_t tmpfile);

ULS_DLL_EXTERN void fp_ungrabber_wstr(uls_voidptr_t data);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef _ULS_USE_ULSCOMPAT
#if defined(ULS_USE_WSTR)
#include "uls/uls_fileio_wstr.h"
#elif defined(ULS_USE_ASTR)
#include "uls/uls_fileio_astr.h"
#endif
#endif

#endif // __ULS_FILEIO_H__
