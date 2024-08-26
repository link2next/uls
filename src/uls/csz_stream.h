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
  <file> csz_stream.h </file>
  <brief>
  	csz: C String terminated by Zero.
    Routines for dynamically manipulating infinite c-string.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef __CSZ_STREAM_H__
#define __CSZ_STREAM_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define N_LINES_IN_POOL 32
#define CSZ_STREAM_DELTA_DFL 64

/* Character String Zero */

#define str_deinit(a,b) _uls_tool(str_free)(a,b)
#define str_copy _uls_tool(str_modify)
#define str_putc(outbuf,k,ch) \
	_uls_tool(__str_putc)(outbuf, CSZ_STREAM_DELTA_DFL, k, ch)
#define str_putc_nosafe(outbuf, k, ch) ((outbuf)->buf[k]=(ch))
#define str_dataptr_k(outbuf,k) ((outbuf)->buf+(k))
#define str_dataptr(outbuf) ((outbuf)->buf)

#define csz_copy _uls_tool(csz_modify)
#define csz_putc_nosafe(csz,ch) do { \
		(csz)->pool[(csz)->len] = (ch); \
		++(csz)->len; \
	} while (0)

#define csz_truncate(csz, len2) \
	do { \
		if ((len2) < (csz)->len) (csz)->len = (len2); \
	} while (0)

#define csz_length(csz) ((csz)->len)
#define csz_size(csz) ((csz)->pool.siz)
#define csz_data_ptr(csz) ((csz)->pool.buf)

#endif // ULS_DECL_GLOBAL_TYPES

#if defined(ULS_DEF_PROTECTED_TYPE)
_ULS_DECLARE_STRUCT(csz_buf_line);

_ULS_DEFINE_STRUCT_BEGIN(csz_buf_line)
{
	char *line;
	int  size;
	csz_buf_line_ptr_t next;
};
#endif

#if defined(ULS_DEF_PUBLIC_TYPE)
_ULS_DECLARE_STRUCT(csz_global_data);
_ULS_DEFINE_STRUCT_BEGIN(csz_global_data)
{
	uls_mutex_struct_t  mtx;
	csz_buf_line_ptr_t  inactive_list;
	csz_buf_line_ptr_t  active_list;
};

ULS_DEFINE_STRUCT(outbuf)
{
	char   *buf;
	int    siz, siz_delta;
};

_ULS_DEFINE_STRUCT(csz_str)
{
	uls_outbuf_t pool;
	int    alloc_delta;
	int    len;
};

#endif

#if defined(__CSZ_STREAM__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC csz_global_data_ptr_t csz_global;
#endif

#if defined(__CSZ_STREAM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC char* __find_in_pool(uls_outbuf_ptr_t tmp_buf, int siz);
ULS_DECL_STATIC int __release_in_pool(char* ptr, int siz);
ULS_DECL_STATIC void __init_csz_pool(void);
ULS_DECL_STATIC void __reset_csz_pool(void);
ULS_DECL_STATIC void __deinit_csz_pool(void);
ULS_DECL_STATIC _ULS_INLINE void __str_modify(uls_outbuf_ptr_t outbuf, int n_delta, int k, const char* str, int len);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void initialize_csz(void);
void reset_csz(void);
void finalize_csz(void);
#endif

#ifdef ULS_DECL_PUBLIC_PROC

ULS_DLL_EXTERN void str_init(uls_outbuf_ptr_t outbuf, int siz);
ULS_DLL_EXTERN void str_free(uls_outbuf_ptr_t outbuf);

ULS_DLL_EXTERN void str_modify(uls_outbuf_ptr_t outbuf, int k, const char* str, int len);
ULS_DLL_EXTERN int str_append(uls_outbuf_ptr_t outbuf, int k, const char* str, int len);
ULS_DLL_EXTERN int  str_puts(uls_outbuf_ptr_t outbuf, int k, const char* str);
ULS_DLL_EXTERN void __str_putc(uls_outbuf_ptr_t outbuf, int n_delta, int k, char ch);

ULS_DLL_EXTERN void csz_init(csz_str_ptr_t csz, int n_delta);
ULS_DLL_EXTERN void csz_deinit(csz_str_ptr_t csz);
ULS_DLL_EXTERN csz_str_ptr_t csz_create(int n_delta);
ULS_DLL_EXTERN void csz_destroy(csz_str_ptr_t csz);

ULS_DLL_EXTERN void csz_reset(csz_str_ptr_t csz);

ULS_DLL_EXTERN char* csz_modify(csz_str_ptr_t csz, int k, const char* str, int len);
ULS_DLL_EXTERN char* csz_append(csz_str_ptr_t csz, const char* str, int len);
ULS_DLL_EXTERN void csz_puts(csz_str_ptr_t csz, const char* str);
ULS_DLL_EXTERN void csz_putc(csz_str_ptr_t csz, char ch);

ULS_DLL_EXTERN void csz_add_ch(csz_str_ptr_t csz, char ch);
ULS_DLL_EXTERN void csz_add_eos(csz_str_ptr_t csz);
ULS_DLL_EXTERN char* csz_text(csz_str_ptr_t csz);
ULS_DLL_EXTERN wchar_t* uls_get_csz_wstr(csz_str_ptr_t csz);
ULS_DLL_EXTERN char* csz_export(csz_str_ptr_t csz);

#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __CSZ_STREAM_H__
