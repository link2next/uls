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
 * uls_emit.h -- generating the output file of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, March 2014.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_EMIT_H__
#define __ULS_EMIT_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_tokdef.h"
#include "uls/uld_conf.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define ULS_FL_WANT_REGULAR_TOKS  0x01
#define ULS_FL_WANT_QUOTE_TOKS    0x02
#define ULS_FL_WANT_RESERVED_TOKS 0x04
#define ULS_FL_WANT_WRAPPER       0x08
#define ULS_FL_ULD_FILE           0x10

#define ULS_FL_C_GEN              0x0100
#define ULS_FL_CPP_GEN            0x0200
#define ULS_FL_CPPCLI_GEN         0x0400
#define ULS_FL_CS_GEN             0x0800
#define ULS_FL_JAVA_GEN           0x1000
#define ULS_FL_LANG_GEN_MASK      0xFF00
#endif

#ifdef ULS_DEF_PUBLIC_TYPE
#define ULS_CLASS_DEPTH          32
ULS_DEFINE_DELEGATE_BEGIN(proc_uld_line, int)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr);
ULS_DEFINE_DELEGATE_END(proc_uld_line);

ULS_DEFINE_STRUCT(parms_emit)
{
	uls_flags_t flags;

	const char *ulc_name, *filepath_conf;
	const char *enum_name, *tok_pfx;
	const char *class_name, *class_path;

	char *out_filepath, *ulc_filepath_enc;
	const char *out_dpath, *out_fname;
	int len_out_filepath;

	uls_type_tool(arglst) name_components;
	int n_name_components;

	uls_voidptr_t ext_data;

	char *fname_buff; // ULS_FILENAME_MAX
	char *ename_buff; // ULS_FILENAME_MAX
};
#endif

#if defined(__ULS_EMIT__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC void emit_source_head(const char *name);
ULS_DECL_STATIC int comp_by_tokid_vx(const uls_voidptr_t a, const uls_voidptr_t b);

ULS_DECL_STATIC int filter_to_print_tokdef(uls_lex_ptr_t uls, uls_tokdef_ptr_t e, int flags);
ULS_DECL_STATIC void print_tokdef_constants(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char* tok_pfx, int flags);
ULS_DECL_STATIC void print_tokdef_enum_constants(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char* enum_name, const char* tok_pfx, int flags);

ULS_DECL_STATIC int __print_uld_lineproc_2(uld_line_ptr_t tok_names, int n_tabs, const char *lptr);

ULS_DECL_STATIC int __print_uld_lineproc_3cpp(uld_line_ptr_t tok_names, int n_tabs, const char *lptr);
ULS_DECL_STATIC int __print_uld_lineproc_3cs(uld_line_ptr_t tok_names, int n_tabs, const char *lptr);
ULS_DECL_STATIC int __print_uld_lineproc_3java(uld_line_ptr_t tok_names, int n_tabs, const char *lptr);

ULS_DECL_STATIC int __print_uld_c_source_2_fp(int n_tabs, FILE *fin_uld);
ULS_DECL_STATIC int collect_printable_list_of_tokdef_vx(uls_lex_ptr_t uls,
	uls_parms_emit_ptr_t emit_parm, uls_ref_parray(tokdef_ary_prn,tokdef_vx));
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int print_uld_source(FILE *fin_uld, int n_tabs, uls_proc_uld_line_t lineproc);

void print_tokdef_c_header(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn, uls_parms_emit_ptr_t emit_parm);

int __print_tokdef_c_source_fp(FILE *fin_uld);
int __print_tokdef_c_source_file(const char *filepath);
int print_tokdef_c_source(uls_parms_emit_ptr_t emit_parm, const char *base_ulc);

int print_tokdef_cpp_header(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc);
int print_tokdef_cpp_source(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc);

int print_tokdef_cs(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc);

int print_tokdef_java(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN int uls_init_parms_emit(uls_parms_emit_ptr_t emit_parm,
	const char *out_dpath, const char *out_fname,
	const char *filepath_cfg, const char* ulc_name,
	const char* class_path, const char *enum_name,
	const char *tok_pfx, const char *ulc_filepath, int flags);
ULS_DLL_EXTERN void uls_deinit_parms_emit(uls_parms_emit_ptr_t emit_parm);
ULS_DLL_EXTERN int uls_generate_tokdef_file(uls_lex_ptr_t uls, uls_parms_emit_ptr_t emit_parm);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_emit_wstr.h"
#endif

#endif // __ULS_EMIT_H__
