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
 * litesc.c -- mapping esc-char to string --
 *     written by Stanley Hong <link2next@gmail.com>, October 2018.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_LITESC_H__
#define __ULS_LITESC_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_util.h"
#include "uls/csz_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE

#define ULS_ESCCH_START    0x21
#define ULS_ESCCH_END      0x7E
#define ULS_ESCCH_MAPSIZE (ULS_ESCCH_END - ULS_ESCCH_START + 1)

#define ULS_ESCMAP_MODERN_EOS        0x0001
#define ULS_ESCMAP_MODERN_ESC        0x0002
#define ULS_ESCMAP_MODERN_EOL        0x0004
#define ULS_ESCMAP_MODERN_ETC        0x0008

#define ULS_ESCMAP_MODERN_LF         0x0010
#define ULS_ESCMAP_MODERN_TAB        0x0020
#define ULS_ESCMAP_MODERN_U4         0x0040
#define ULS_ESCMAP_MODERN_U8         0x0080

#define ULS_ESCMAP_LEGACY_SQ         0x0100
#define ULS_ESCMAP_LEGACY_DQ         0x0200
#define ULS_ESCMAP_LEGACY_HEX        0x0400
#define ULS_ESCMAP_LEGACY_OCT        0x0800

#define ULS_ESCMAP_LEGACY_CR         0x1000
#define ULS_ESCMAP_LEGACY_BS         0x2000
#define ULS_ESCMAP_LEGACY_FF         0x4000
#define ULS_ESCMAP_LEGACY_VF         0x8000

#define ULS_ESCMAP_LEGACY_BELL      0x10000
#define ULS_ESCMAP_LEGACY_QUES      0x20000

// escstr
#define ULS_FL_ESCSTR_MASK          0x000F
#define ULS_FL_ESCSTR_HEXA          0x0010
#define ULS_FL_ESCSTR_OCTAL         0x0020
#define ULS_FL_ESCSTR_UNICODE       0x0040
#define ULS_FL_ESCSTR_DEL           0x0080
#define ULS_FL_ESCSTR_HEXA_AF       0x0100
#define ULS_FL_ESCSTR_HEXA_af       0x0200
#define ULS_FL_ESCSTR_FIXED_NDIGITS 0x0400

// escmap
#define ULS_ESCMAP_FL_BUILTIN       0x01

ULS_DECLARE_STRUCT(escstr);
ULS_DECLARE_STRUCT(escmap);

ULS_DECLARE_STRUCT(escmap_container);
ULS_DECLARE_STRUCT(escmap_pool);

ULS_DECLARE_STRUCT(litesc_sysinfo);
#endif

#ifdef ULS_DEF_PROTECTED_TYPE
ULS_DEFINE_STRUCT(escstr)
{
	char esc_ch;
	int idx, len;
};
ULS_DEF_PARRAY(escstr);

ULS_DEFINE_STRUCT_BEGIN(escmap_container)
{
	uls_escstr_t escstr;
	uls_escmap_container_ptr_t next;
};
ULS_DEF_PARRAY(escmap_container);

ULS_DEFINE_STRUCT_BEGIN(escmap_pool)
{
	_uls_type_tool(csz_str) strpool;
	uls_decl_parray(escstr_containers, escmap_container);
};

ULS_DEFINE_STRUCT_BEGIN(escmap)
{
	int flags;
	uls_decl_parray(escstr_list, escstr);
	uls_escmap_pool_ptr_t mempool;
};

ULS_DEFINE_STRUCT_BEGIN(litesc_sysinfo)
{
	uls_escmap_pool_t escmap_pool__global;
	uls_escmap_t uls_escstr__legacy;
	uls_escmap_t uls_escstr__verbatim;
	uls_escmap_t uls_escstr__verbatim1;
	uls_escmap_t uls_escstr__modern;
};
#endif // ULS_DEF_PROTECTED_TYPE

#if defined(__ULS_LITESC__) || defined(ULS_DEF_PRIVATE_DATA)
ULS_DECL_STATIC uls_litesc_sysinfo_ptr_t uls_litesc;
#endif

#if defined(__ULS_LITESC__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int get_escstr_bin_opts(uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int extract_escstr_mapexpr(char *line, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC int __parse_escmap_optgrp(char *line);
ULS_DECL_STATIC int parse_escmap_optgrp(uls_escmap_ptr_t esc_map, uls_ptrtype_tool(outparam) parms);

#endif // ULS_DECL_PRIVATE_PROC

#ifdef ULS_DECL_PROTECTED_PROC
int uls_escmap_canbe_escch(uls_wch_t wch);
void uls_init_escstr(uls_escstr_ptr_t escstr, char ch, int idx, int len);
void uls_deinit_escstr(uls_escstr_ptr_t escstr);

void uls_init_escmap(uls_escmap_ptr_t map, uls_escmap_pool_ptr_t mempool);
void __uls_deinit_escmap(uls_escmap_ptr_t map);
void uls_deinit_escmap(uls_escmap_ptr_t map);
uls_escmap_ptr_t uls_alloc_escmap(uls_escmap_pool_ptr_t mempool);
void uls_dealloc_escmap(uls_escmap_ptr_t map);
uls_escstr_ptr_t uls_find_escstr_nosafe(uls_escmap_ptr_t map, int ind, char ch);
uls_escstr_ptr_t uls_find_escstr(uls_escmap_ptr_t map, char ch);

uls_escmap_container_ptr_t uls_alloc_escmap_container(char esc_ch, int idx, int len);
void uls_dealloc_escmap_container(uls_escmap_container_ptr_t wrap);

void uls_init_escmap_pool(uls_escmap_pool_ptr_t escmap_pool);
void uls_deinit_escmap_pool(uls_escmap_pool_ptr_t escmap_pool);
uls_escstr_ptr_t uls_search_escmap_pool(uls_escmap_pool_ptr_t escmap_pool, char esc_ch, const char *str, int len);
uls_escstr_ptr_t __uls_add_escmap_pool(uls_escmap_pool_ptr_t escmap_pool, int ind, char esc_ch, int idx, int len);

int uls_add_escstr(uls_escmap_pool_ptr_t escmap_pool, uls_escmap_ptr_t map, char esc_ch, const char *str, int len);
void __uls_add_escstr(uls_escmap_pool_ptr_t escmap_pool, uls_escmap_ptr_t map, int ind, char esc_ch, const char *str, int len);
int uls_del_escstr(uls_escmap_ptr_t map, char esc_ch);
void __uls_register_escstr(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, int ind, char esc_ch, const char *str, int len);
void uls_register_escstr_nosafe(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len);
int uls_register_escstr(uls_escmap_pool_ptr_t escmap_pool, uls_escmap_ptr_t map, char esc_ch, const char *str, int len);

void __uls_clone_escmap(uls_escmap_ptr_t src_map, uls_escmap_ptr_t dst_map, uls_escmap_pool_ptr_t dst_escmap_pool);
void __uls_set_escmap(uls_escmap_ptr_t dst_map, int flags);
uls_escmap_ptr_t uls_dup_escmap(uls_escmap_ptr_t src_map, uls_escmap_pool_ptr_t escmap_pool, int flags);

uls_escmap_ptr_t uls_parse_escmap_feature(uls_ptrtype_tool(outparam) parms);
int uls_parse_escmap_mapping(uls_escmap_ptr_t esc_map, uls_escmap_pool_ptr_t escmap_pool, char *line);

int initialize_uls_litesc();
void finalize_uls_litesc();

#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
int uls_dec_escaped_char(uls_escstr_ptr_t escstr, uls_escmap_ptr_t map, uls_ptrtype_tool(outparam) parms, _uls_ptrtype_tool(csz_str) cszbuf);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_LITESC_H__
