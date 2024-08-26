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
  <file> uls_sysprops.h </file>
  <brief>
    Managing system properities of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2014.
  </author>
*/

#ifndef __ULS_SYSPROPS_H__
#define __ULS_SYSPROPS_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_prim.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE

#define ULS_PROPPOOL_DFLSIZ 256

ULS_DEFINE_STRUCT(sysprop)
{
	uls_def_namebuf_this(name, ULS_LEXSTR_MAXSIZ);
	int stridx;
};
ULS_DEF_ARRAY_TYPE00(sysprop, SYSPROP_TYPE00_ULS_N_SYSPROPS, ULS_N_SYSPROPS);

ULS_DEFINE_STRUCT(sysinfo)
{
	int initialized;
	int ULS_BYTE_ORDER;

	int LDBL_IEEE754_FMT;
	int LDOUBLE_SIZE_BYTES, LDOUBLE_EXPOSIZE_BITS, LDOUBLE_EXPO_BIAS;
	int LDOUBLE_SIZE_BITS, LDOUBLE_MENTISA_STARTBIT;

	const char *etc_dir, *home_dir, *ulcs_dir;
	char *ULC_SEARCH_PATH;
	int  codepage, multibytes;

	uls_decl_array_type00(properties, sysprop, ULS_N_SYSPROPS);
	int n_properties;

	int  n_strpool, n_alloc_strpool;
	char *strpool;
};
#endif // ULS_DEF_PUBLIC_TYPE

#if defined(ULS_DOTNET) || defined(__ULS_SYSPROPS__)
#define EXTERNAL
#else
#define EXTERNAL extern
#endif

#if !defined(ULS_DOTNET) || defined(ULS_DEF_PUBLIC_DATA)
EXTERNAL uls_sysinfo_ptr_t uls_sysinfo;
#endif

#if defined(__ULS_SYSPROPS__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int __init_system_info(uls_sysinfo_ptr_t sysinfo, int poolsiz);
ULS_DECL_STATIC uls_sysprop_ptr_t __get_system_property(uls_sysinfo_ptr_t sysinfo, const char* name);
ULS_DECL_STATIC char* get_nameval_pair(uls_parm_line_ptr_t parm_ln);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_sysprop(uls_sysprop_ptr_t sys_prop);
void uls_deinit_sysprop(uls_sysprop_ptr_t sys_prop);

void uls_init_sysinfo(uls_sysinfo_ptr_t sysinfo);
void uls_deinit_sysinfo(uls_sysinfo_ptr_t sysinfo);
uls_sysinfo_ptr_t uls_create_sysinfo(void);
void uls_destroy_sysinfo(uls_sysinfo_ptr_t sysinfo);

int initialize_sysprops(const char *fpath);
void finalize_sysprops(void);

int uls_load_system_properties(const char *fpath, uls_sysinfo_ptr_t sysinfo);

void uls_arch2be_array(char* ary, int n);
void uls_be2arch_array(char* ary, int n);

void uls_arch2le_array(char* ary, int n);
void uls_le2arch_array(char* ary, int n);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN const char* uls_add_system_property(const char* name, const char* val);
ULS_DLL_EXTERN const char* uls_get_system_property(const char* name);
#endif

#ifndef ULS_DOTNET
#define _uls_sysinfo_(attr) uls_sysinfo->attr
#endif

#undef EXTERNAL
#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_SYSPROPS_H__
