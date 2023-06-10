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
  <file> uls_lf_sprintf.h </file>
  <brief>
    An upgraded version of varargs routines(sprintf,fprintf,printf, ..).
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, July 2011.
  </author>
*/
#if !defined(ULS_DOTNET) && !defined(__ULS_LF_SPRINTF_H__)
#define __ULS_LF_SPRINTF_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#include <stdarg.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define uls_lf_map_lock(map) uls_lock_mutex(uls_ptr((map)->mtx))
#define uls_lf_map_unlock(map) uls_unlock_mutex(uls_ptr((map)->mtx))

#define uls_lf_lock(uls_lf) uls_lock_mutex(uls_ptr((uls_lf)->mtx))
#define uls_lf_unlock(uls_lf) uls_unlock_mutex(uls_ptr((uls_lf)->mtx))
#endif

#ifdef ULS_DECL_PUBLIC_TYPE

#define ULS_LF_PERCENT_NAMESIZ 3
#define ULS_LF_N_FILLCHS 16
#define ULS_LF_N_FILLSTR 64

#define ULS_LF_LEFT_JUSTIFIED     0x0001  /* left justified */
#define ULS_LF_ZEROPAD            0x0002  /* pad with zero */
#define ULS_LF_PLUS_PREFIX        0x0004  /* show plus */
#define ULS_LF_MINUS_PREFIX       0x0008  /* show minus */

#define ULS_LF_SPECIAL_CHAR       0x0010  /* 0x */
#define ULS_LF_DYNAMIC_WIDTH      0x0020
#define ULS_LF_DYNAMIC_PRECISION  0x0040

#define ULS_LF_BIN_PREFIX         0x0100
#define ULS_LF_HEX_PREFIX         0x0200
#define ULS_LF_PERCENT_E          0x0400
#define ULS_LF_PERCENT_G          0x0800

#define ULS_LF_DFL_FLOAT_PRECISION 6

#define ULS_LF_PERCPROC_object  0

#define ULS_LF_PERCPROC_s       1
#define ULS_LF_PERCPROC_c       2

#define ULS_LF_PERCPROC_d       3
#define ULS_LF_PERCPROC_u       4
#define ULS_LF_PERCPROC_f       5

#define ULS_LF_PERCPROC_ld      6
#define ULS_LF_PERCPROC_lu      7
#define ULS_LF_PERCPROC_lf      8

#define ULS_LF_PERCPROC_lld     9
#define ULS_LF_PERCPROC_llu     10
#define ULS_LF_PERCPROC_llf     11

#define ULS_LF_PERCPROC_Ld      12
#define ULS_LF_PERCPROC_Lu      13
#define ULS_LF_PERCPROC_Lf      14

#define ULS_LF_PERCPROC_p       15

#define ULS_LF_PERCPROC_unknown 128

#define uls_get_percproc_table_ind(len) (len-1)

#define ULS_LF_NO_DEFAULT 0x01

ULS_DECLARE_STRUCT(lf_context);

ULS_DEFINE_DELEGATE_BEGIN(lf_puts, int)(uls_voidptr_t dat, const char* str, int len);
ULS_DEFINE_DELEGATE_END(lf_puts);

ULS_DEFINE_DELEGATE_BEGIN(lf_convspec, int)(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DEFINE_DELEGATE_END(lf_convspec);
#endif // ULS_DECL_PUBLIC_TYPE

#ifdef ULS_DEF_PROTECTED_TYPE

#ifdef _ULS_IMPLDLL
ULS_DEFINE_STRUCT(buf4str)
{
	unsigned int flags;
	char *buf, *bufptr;
	int bufsiz;
};
#endif

ULS_DEFINE_STRUCT(lf_convflag)
{
	uls_flags_t  flags;
	int    width, precision;
};

ULS_DEFINE_STRUCT(lf_name2proc)
{
	char name[ULS_LF_PERCENT_NAMESIZ+1];
	int l_name;
	uls_lf_convspec_t proc;
	uls_voidptr_t user_data;
};

ULS_DEFINE_STRUCT(lf_convspec_table)
{
	uls_lf_name2proc_ptr_t proc_tab;
	int n_used, n_allocd;
};

ULS_DEFINE_STRUCT(lf_map)
{
	uls_mutex_struct_t mtx;
	int flags, ref_cnt;

	uls_lf_convspec_table_t linear;
	uls_lf_convspec_table_t sorted[ULS_LF_PERCENT_NAMESIZ];
};
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
ULS_DEFINE_STRUCT(lf)
{
	void           *x_dat;
	uls_lf_puts_t  uls_lf_puts;

	uls_mutex_struct_t mtx;
	uls_lf_map_ptr_t convspec_map;

	uls_voidptr_t g_dat;
	csz_str_t wbuf1;
	csz_str_t wbuf2;

	uls_voidptr_t shell;
};

ULS_DEFINE_STRUCT_BEGIN(lf_context)
{
	void           *g_dat;
	uls_lf_convflag_t perfmt;
	csz_str_ptr_t   numbuf1, numbuf2;
	void           *u_dat;
	va_list        args;
};

ULS_DEFINE_STRUCT(lf_delegate)
{
	uls_voidptr_t xdat;
	uls_lf_puts_t puts;
};

#endif

#if defined(__ULS_LF_SPRINTF__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_lf_map_t dfl_convspec_map;
#endif

#if defined(__ULS_LF_SPRINTF__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC void __uls_lf_sysputs(const char* msg);
ULS_DECL_STATIC int __puts_proc_str(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, const char* str, int len);

ULS_DECL_STATIC int fmtproc_d(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_u(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_lld(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_llu(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);

ULS_DECL_STATIC int fmtproc_LX(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_Lx(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_X(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_x(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);

ULS_DECL_STATIC int fmtproc_o(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_p(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_ld(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_lu(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_lX(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_lx(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);

ULS_DECL_STATIC int __fmtproc_f_e_g(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx, double num_f);
ULS_DECL_STATIC int fmtproc_f(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_e(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_g(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);

ULS_DECL_STATIC int __fmtproc_lf_le_lg(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx, long double num_lf);
ULS_DECL_STATIC int fmtproc_lf(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_le(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_lg(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC int fmtproc_null(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);

ULS_DECL_STATIC uls_lf_name2proc_ptr_t uls_lf_bi_search(const char* keyw, int len, uls_lf_name2proc_ptr_t ary, int n_ary);
ULS_DECL_STATIC uls_lf_convspec_t __find_convspec(const char** p_fmtptr, uls_lf_map_ptr_t map, uls_lf_context_ptr_t lf_ctx);
ULS_DECL_STATIC uls_lf_name2proc_ptr_t __realloc_convspec_table(uls_lf_convspec_table_ptr_t tbl, int n);
ULS_DECL_STATIC int __add_convspec_sorted(uls_lf_convspec_table_ptr_t tbl,
	const char* percent_name, uls_lf_convspec_t proc, uls_voidptr_t user_data);
ULS_DECL_STATIC int __replace_convspec_linear(uls_lf_convspec_table_ptr_t tbl,
	const char* percent_name, uls_lf_convspec_t proc, uls_voidptr_t user_data);
ULS_DECL_STATIC void load_default_convspec_map(uls_lf_map_ptr_t lf_map);
ULS_DECL_STATIC int __uls_lf_skip_atou(const char ** p_ptr);
#endif // ULS_DECL_PRIVATE_PROC

#ifdef ULS_DECL_PROTECTED_PROC
int uls_lf_puts_prefix(char* str, int flags);
int uls_lf_fill_ch(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, char ch_fill, int n);
int uls_lf_fill_numstr(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc,
	uls_lf_convflag_ptr_t p, const char* numstr, int l_numstr);
int uls_lf_fill_mbstr(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc,
	uls_lf_convflag_ptr_t p, const char* numstr, int l_numstr, int lw_numstr);

int fmtproc_s(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
int fmtproc_ws(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
int fmtproc_c(uls_voidptr_t x_dat, uls_lf_puts_t puts_proc, uls_lf_context_ptr_t lf_ctx);
void __add_convspec_linear(uls_lf_convspec_table_ptr_t tbl,
	char* percent_name, uls_lf_convspec_t proc, int tbl_ind);

void initialize_uls_lf(void);
void finalize_uls_lf(void);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN uls_lf_map_ptr_t uls_lf_get_default(void);

ULS_DLL_EXTERN int uls_lf_init_convspec_map(uls_lf_map_ptr_t lf_map, int flags);
ULS_DLL_EXTERN int uls_lf_deinit_convspec_map(uls_lf_map_ptr_t lf_map);
ULS_DLL_EXTERN uls_lf_map_ptr_t uls_lf_create_convspec_map(int flags);
ULS_DLL_EXTERN int uls_lf_destroy_convspec_map(uls_lf_map_ptr_t lf_map);

ULS_DLL_EXTERN void uls_lf_grab_convspec_map(uls_lf_map_ptr_t lf_map);
ULS_DLL_EXTERN void uls_lf_ungrab_convspec_map(uls_lf_map_ptr_t lf_map);

ULS_DLL_EXTERN int uls_lf_register_convspec(uls_lf_map_ptr_t lf_map,
	const char* percent_name, uls_lf_convspec_t proc);

ULS_DLL_EXTERN int uls_lf_init(uls_lf_ptr_t uls_lf, uls_lf_map_ptr_t dst_map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_lf_deinit(uls_lf_ptr_t uls_lf);
ULS_DLL_EXTERN uls_lf_ptr_t uls_lf_create(uls_lf_map_ptr_t dst_map, uls_voidptr_t x_dat, uls_lf_puts_t puts_proc);
ULS_DLL_EXTERN void uls_lf_destroy(uls_lf_ptr_t uls_lf);

ULS_DLL_EXTERN uls_voidptr_t __uls_lf_change_gdat(uls_lf_ptr_t uls_lf, uls_voidptr_t gdat);
ULS_DLL_EXTERN uls_voidptr_t uls_lf_change_gdat(uls_lf_ptr_t uls_lf, uls_voidptr_t gdat);

ULS_DLL_EXTERN uls_voidptr_t __uls_lf_change_xdat(uls_lf_ptr_t uls_lf, uls_voidptr_t xdat);
ULS_DLL_EXTERN uls_voidptr_t uls_lf_change_xdat(uls_lf_ptr_t uls_lf, uls_voidptr_t xdat);

ULS_DLL_EXTERN void __uls_lf_change_puts(uls_lf_ptr_t uls_lf, uls_lf_delegate_ptr_t delegate);
ULS_DLL_EXTERN void uls_lf_change_puts(uls_lf_ptr_t uls_lf, uls_lf_delegate_ptr_t delegate);

ULS_DLL_EXTERN int __uls_lf_vxprintf(uls_lf_ptr_t uls_lf, const char* fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxprintf(uls_lf_ptr_t uls_lf, const char* fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xprintf(uls_lf_ptr_t uls_lf, const char* fmt, ...);
ULS_DLL_EXTERN int uls_lf_xprintf(uls_lf_ptr_t uls_lf, const char* fmt, ...);

ULS_DLL_EXTERN int __uls_lf_vxprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, va_list args);
ULS_DLL_EXTERN int uls_lf_vxprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, va_list args);
ULS_DLL_EXTERN int __uls_lf_xprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, ...);
ULS_DLL_EXTERN int uls_lf_xprintf_generic(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* fmt, ...);

ULS_DLL_EXTERN int uls_lf_puts_csz(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_str(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_file(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
ULS_DLL_EXTERN int uls_lf_puts_null(uls_voidptr_t x_dat, const char* wrdptr, int wrdlen);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_LF_SPRINTF_H__
