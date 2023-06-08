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
  <file> uls_prim.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2016.
  </author>
*/

#ifndef __ULS_PRIM_H__
#define __ULS_PRIM_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_type.h"
#include <stdio.h>

#if !defined(ULS_DOTNET) && !defined(USE_ULSNETJAVA)
#ifdef ULS_WINDOWS
#include <tchar.h>
#include <io.h>
#include <direct.h>
#ifdef WINDOWS8
#include <stringapiset.h>
#else
#include <winnls.h>
#endif
#endif // ULS_WINDOWS
#endif // !defined(ULS_DOTNET) && !defined(USE_ULSNETJAVA)

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#ifndef ULS_WINDOWS
#include <stdarg.h>
#include <wchar.h>
#endif

#endif // ULS_EXCLUDE_HFILES

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define DECLARE_ULS_MUTEX(name) uls_mutex_struct_t name
#define EXTERN_ULS_MUTEX(name) extern uls_mutex_struct_t name

#define uls_malloc_buffer(n) (char *) _uls_tool_(malloc)(n)
#define uls_malloc_buffer_clear(n) (char *) _uls_tool_(malloc_clear)(n)
#define uls_mfree(a) do { _uls_tool(__uls_mfree)((void *)(a)); (a)=NULL; } while (0)
#endif // ULS_DECL_GLOBAL_TYPES

#ifdef ULS_DECL_PUBLIC_TYPE
#ifdef ULS_DO_ASSERT
#define uls_assert(x) do { \
  if ((x)==0) { \
    _uls_log_primitive(err_log)("assertion failed:%s(#%d)", __FILE__, __LINE__); \
    _uls_log_primitive(err_panic)(#x); \
  } \
} while (0)
#else
#define uls_assert(x)
#endif
#endif // ULS_DECL_PUBLIC_TYPE

#ifdef ULS_DECL_PROTECTED_TYPE
#define ULS_MEMSET_CHUNKSIZE 32

#define BYTE_SIZE_LOG_BITS  3
#define BYTE_SIZE_BITS      (1<<BYTE_SIZE_LOG_BITS)
#define byte_ent_no(i_bit)  ((i_bit) >> BYTE_SIZE_LOG_BITS)
#define byte_offset(i_bit)  ((i_bit) & (BYTE_SIZE_BITS-1))
#define BYTE_LEFT_MOST_1    (1<<(BYTE_SIZE_BITS-1))

#define ULS_INT_MAX \
  ( \
  	((unsigned int) -1) & ~( \
  		(unsigned int) 0x1 << (sizeof(int)*8-1) \
  	) \
  )
#define ULS_INT_MIN (-ULS_INT_MAX-1)

#ifdef ULS_WINDOWS
#define O_BINARY _O_BINARY
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#else // ULS_WINDOWS
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif // ULS_WINDOWS

#ifdef _ULS_IMPLDLL
#define _IS_CH_DIGIT(c) (c>='0' && c<='9')
#define _IS_CH_LOWER(c) (c>='a' && c<='z')
#define _IS_CH_UPPER(c) (c>='A' && c<='Z')
#endif

#define ULS_MAXSZ_NUMSTR    64

#define ULS_LF_SHIFT_BASE_UNKNOWN -2
#define ULS_LF_SHIFT_BASE_OCT  -1
#define ULS_LF_SHIFT_BASE_DEC   0
#define ULS_LF_SHIFT_BASE_BIN   1
#define ULS_LF_SHIFT_BASE_HEX   4

#endif // ULS_DECL_PROTECTED_TYPE

#ifdef ULS_DEF_PUBLIC_TYPE

#if defined(HAVE_PTHREAD)
ULS_DEFINE_STRUCT(mutex_struct)
{
	pthread_mutex_t *mtx_pthr;
};
#elif defined(ULS_WINDOWS)
ULS_DEFINE_STRUCT(mutex_struct)
{
	void *hndl; // HANDLE
};
#else
ULS_DEFINE_STRUCT(mutex_struct)
{
	int hndl;
};
#endif

typedef uls_mutex_struct_ptr_t uls_mutex_t;

ULS_DEFINE_STRUCT(outparam)
{
	int n, n1, n2, len, flags;
	unsigned int x1, x2;

	char *line;
	const char *lptr, *lptr_end;
	uls_uch_t wch;

	uls_native_vptr_t native_data;
	uls_voidptr_t data;
	uls_voidptr_t proc;
};

ULS_DEFINE_STRUCT(parm_line)
{
	char *line;
	const char *lptr, *lptr_end;
	int len;
};

ULS_DEFINE_STRUCT(nambuf)
{
	char *str;
	int len, buf_siz;
};

ULS_DEFINE_STRUCT(wrd)
{
	char *wrd, *lptr;
	int len, siz;
};

ULS_DEFINE_STRUCT(argstr)
{
	char *buf, *str;
	int len, buf_siz;
};
ULS_DEF_PARRAY(argstr);

ULS_DEFINE_STRUCT(arglst)
{
	uls_decl_parray(args, argstr);
};

ULS_DEFINE_STRUCT(uch_range)
{
	uls_uch_t x1;
	uls_uch_t x2;
};
ULS_DEF_ARRAY_TYPE01(uch_range);
#ifndef ULS_CLASSIFY_SOURCE
ULS_DEF_PARRAY(uch_range);
#endif

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_PRIM__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_mutex_struct_t uls_global_mtx;
#endif

#if defined(__ULS_PRIM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC _ULS_INLINE int __find_first_1bit(char ch, int j1, int j2);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int uls_lf_number_u(char *numstr, unsigned int num, int base_shift);
int uls_lf_number_lu(char *numstr, unsigned long num, int base_shift);
int uls_lf_number_Lu(char *numstr, unsigned long long num, int base_shift);

#ifndef ULS_DOTNET
void err_log_puts(const char* mesg, int len);
int uls_vsnprintf_primitive(char *buf,  int bufsiz, const char* fmt, va_list args);
int uls_snprintf_primitive(char *buf,  int bufsiz, const char* fmt, ...);
int err_vlog_primitive(const char* fmt, va_list args);
void err_log_primitive(const char* fmt, ...);
void err_panic_primitive(const char* fmt, ...);
#endif

int is_octal_char(char ch);
int is_hexa_char(char ch);
int is_num_radix(uls_uch_t ch, int radix);
char num2char_radix(int val);
char read_hexa_char(char* ptr);

int uls_index_range(uls_outparam_ptr_t parms, int i2_limit);
void uls_clear_bits(char* srcptr, uls_uint32 start_bit, uls_uint32 end_bit);
int uls_find_first_1bit(char* srcptr,
	uls_uint32 start_bit, uls_uint32 end_bit, uls_uint32* found_bit);

int uls_check_longdouble_fmt(int endian);

ULS_DLL_EXTERN void uls_init_argstr(uls_argstr_ptr_t arg, int siz);
ULS_DLL_EXTERN void uls_deinit_argstr(uls_argstr_ptr_t arg);

int initialize_primitives(void);
void finalize_primitives(void);

#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC

const char* uls_get_standard_number_prefix(int radix);
int uls_find_standard_prefix_radix(const char *line, int *ptr_radix);

int uls_isgraph(int c);
int uls_isprint(int c);
int uls_iscntrl(int c);
int uls_isspace(int c);
int uls_isdigit(int c);
int uls_islower(int c);
int uls_isupper(int c);
int uls_isalpha(int c);
int uls_isalnum(int c);
int uls_isxdigit(int c);
char uls_toupper(int c);
char uls_tolower(int c);

int is_pure_integer(const char* lptr, uls_outparam_ptr_t parms);
int is_pure_word(const char* lptr, int must_id);
int uls_atoi(const char *str);

void uls_get_xrange(const char* wrd, uls_uint32* ptr_x1, uls_uint32* ptr_x2);
int get_range_aton(uls_outparam_ptr_t parms);
int uls_range_of_bits(uls_uint32 n);

int uls_host_byteorder(void);
void uls_reverse_bytes(char* ary, int n);

int uls_strcmp(const char* str1, const char* str2);
int uls_strncmp(const char* str1, const char* str2, int n);
int uls_strcasecmp(const char* str1, const char* str2);

char* uls_strchr(const char* lptr, char ch0);
char* uls_strchr_r(const char* lptr, char ch);
const char* uls_strstr(const char *str, const char* substr);

int uls_str_tolower(const char* src, char *dst, int len);
int uls_str_toupper(const char* src, char *dst, int len);
int uls_memcmp(const void *src1, const void *src2, int n);

unsigned int uls_skip_atou(uls_outparam_ptr_t parms);
unsigned int uls_skip_atox(uls_outparam_ptr_t parms);

char* split_clause(uls_outparam_ptr_t parms);
char* split_litstr(char *str, char qch);

int uls_fp_getline(FILE* fp, char* buf, int buf_siz);
void uls_appl_exit(int exit_code);

void uls_init_nambuf(uls_nambuf_ptr_t arg, int siz);
void uls_deinit_nambuf(uls_nambuf_ptr_t arg);
int uls_set_nambuf(uls_nambuf_ptr_t arg, const char *name, int name_len);
int uls_set_nambuf_raw(char *argbuf, int argbuf_siz, const char *name, int name_len);

void uls_msleep(int msecs);

ULS_DLL_EXTERN void uls_memset(void *dstbuf, char ch, int n);
ULS_DLL_EXTERN void uls_bzero(void *dstbuf, int n);

ULS_DLL_EXTERN void *uls_malloc(unsigned int n_bytes);
ULS_DLL_EXTERN void *uls_malloc_clear(unsigned int n_bytes);
ULS_DLL_EXTERN void *uls_mrealloc(void *ptr, unsigned int n_bytes);
ULS_DLL_EXTERN void *__uls_mfree(void *ptr);

ULS_DLL_EXTERN char* uls_strdup(const char* str, int len);
ULS_DLL_EXTERN void* uls_memcopy(void *dst, const void* src, int n);
ULS_DLL_EXTERN void* uls_memmove(void *dst, const void* src, int n);

ULS_DLL_EXTERN int uls_strlen(const char* str);
ULS_DLL_EXTERN int uls_strcpy(char* bufptr, const char* str);
ULS_DLL_EXTERN int uls_strncpy(char* bufptr, const char* ptr, int n);

ULS_DLL_EXTERN char* skip_blanks(const char* lptr);
ULS_DLL_EXTERN int str_trim_end(char* str, int len);
ULS_DLL_EXTERN char* _uls_splitstr(uls_wrd_ptr_t uw);
ULS_DLL_EXTERN int _uls_explode_str(uls_wrd_ptr_t uw, char delim_ch, int dupstr, uls_arglst_ptr_t arglst);
ULS_DLL_EXTERN char* _uls_filename(uls_outparam_ptr_t parms);

ULS_DLL_EXTERN uls_argstr_ptr_t uls_create_argstr(void);
ULS_DLL_EXTERN void uls_destroy_argstr(uls_argstr_ptr_t arg);
ULS_DLL_EXTERN void uls_set_argstr(uls_argstr_ptr_t arg, char *name, int name_len);
ULS_DLL_EXTERN char *uls_copy_argstr(uls_argstr_ptr_t arg, const char *name, int name_len);

ULS_DLL_EXTERN void uls_init_arglst(uls_arglst_ptr_t arglst, int siz);
ULS_DLL_EXTERN void uls_deinit_arglst(uls_arglst_ptr_t arglst);
ULS_DLL_EXTERN void uls_reset_arglst(uls_arglst_ptr_t arglst);
ULS_DLL_EXTERN void uls_resize_arglst(uls_arglst_ptr_t arglst, int n1_alloc);
ULS_DLL_EXTERN int uls_append_arglst(uls_arglst_ptr_t arglst, uls_argstr_ptr_t arg);

ULS_DLL_EXTERN int ustr_num_chars(const char *str, int len, uls_outparam_ptr_t parms);

ULS_DLL_EXTERN int uls_encode_utf8(uls_uch_t uch, char* utf8buf);
ULS_DLL_EXTERN int uls_decode_utf8(const char *utf8buf, int siz_utf8buf, uls_uch_t *p_val);
ULS_DLL_EXTERN int uls_encode_utf16(uls_uch_t uch, uls_uint16 *utf16buf);
ULS_DLL_EXTERN int uls_decode_utf16(const uls_uint16 *buf, int buf_len, uls_uch_t *p_uch);
ULS_DLL_EXTERN int uls_encode_utf32(uls_uch_t uch, uls_uint32 *buf);
ULS_DLL_EXTERN int uls_decode_utf32(const uls_uint32 buf, uls_uch_t *p_uch);

ULS_DLL_EXTERN ULS_DECL_EXTERN_STATIC void uls_init_mutex(uls_mutex_t mtx);
ULS_DLL_EXTERN ULS_DECL_EXTERN_STATIC void uls_deinit_mutex(uls_mutex_t mtx);
ULS_DLL_EXTERN ULS_DECL_EXTERN_STATIC void uls_lock_mutex(uls_mutex_t mtx);
ULS_DLL_EXTERN ULS_DECL_EXTERN_STATIC void uls_unlock_mutex(uls_mutex_t mtx);

#ifndef ULS_DOTNET
ULS_DLL_EXTERN void *uls_zalloc(unsigned int n_bytes);
ULS_DLL_EXTERN char *uls_splitstr(char** p_str);
ULS_DLL_EXTERN int uls_explode_str(char **ptr_line, char delim_ch, char** args, int n_args);
ULS_DLL_EXTERN char *uls_filename(const char *filepath, int* len_fname);
#endif

ULS_DLL_EXTERN void uls_sys_lock(void);
ULS_DLL_EXTERN void uls_sys_unlock(void);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_PRIM_H__
