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
 * uls_istream.h -- the routines that make lexical streams ... --
 *     written by Stanley Hong <link2next@gmail.com>, Aug 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_ISTREAM_H__
#define __ULS_ISTREAM_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_core.h"
#include "uls/uls_stream.h"
#include "uls/uls_fileio.h"
#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif
#include <stdio.h>
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DEF_PUBLIC_TYPE

ULS_DEFINE_STRUCT(istream)
{
	uls_flags_t  flags;
	uls_stream_header_t header;
	int     ref_cnt;

	uls_def_namebuf(filepath, ULS_FILEPATH_MAX);
	int     fd; /* read-only */
#ifdef ULS_FDF_SUPPORT
	fdf_t   *fdf; // fd ---> fdf --->
#endif
	char    *firstline;
	int     len_firstline;
	int     start_off;

	uls_lex_ptr_t uls;
	uls_type_tool(tempfile) uld_file;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_ISTREAM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC void __init_istream(uls_istream_ptr_t istr);
ULS_DECL_STATIC uls_istream_ptr_t __create_istream(int fd);
ULS_DECL_STATIC void __destroy_istream(uls_istream_ptr_t istr);
ULS_DECL_STATIC int make_tokpkt_seqence(uls_lex_ptr_t uls, const char* line, uls_tmpl_pool_ptr_t tmpls_pool);
ULS_DECL_STATIC void make_eoif_lexeme_bin(uls_context_ptr_t ctx, int tok_id, const char *txt, int txtlen);

ULS_DECL_STATIC int uls_readline_buffer(char* buf, int bufsiz);
ULS_DECL_STATIC int parse_uls_hdr(char* line, int fd_in, uls_istream_ptr_t istr);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
int check_istr_compatibility(uls_istream_ptr_t istr, uls_lex_ptr_t uls);
int uls_check_stream_ver(uls_stream_header_ptr_t hdr, uls_lex_ptr_t uls);
int get_rawfile_subtype(char *buff, int n_bytes, uls_ptrtype_tool(outparam) parms);

void uls_ungrab_fd_utf(uls_source_ptr_t isrc);

uls_tmpl_pool_ptr_t uls_import_tmpls(uls_tmpl_list_ptr_t tmpl_list, uls_lex_ptr_t uls);
int uls_bind_tmpls(uls_istream_ptr_t istr, uls_tmpl_list_ptr_t tmpl_list);

int uls_fill_fd_stream(uls_source_ptr_t isrc, char* buf, int buflen, int bufsiz);
void uls_ungrab_fd_stream(uls_source_ptr_t isrc);

int uls_gettok_bin(uls_lex_ptr_t uls);
#endif // ULS_DECL_PROTECTED_PROC

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN void uls_set_istream_tag(uls_istream_ptr_t istr, const char* tag);

ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream(int fd);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_file(const char* fpath);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_fp(FILE *fp);

#ifdef ULS_FDF_SUPPORT
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_filter(fdf_t* fdf, int fd);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_filter_file(fdf_t* fdf, const char* fpath);
ULS_DLL_EXTERN uls_istream_ptr_t uls_open_istream_filter_fp(fdf_t* fdf, FILE *fp);
#endif

ULS_DLL_EXTERN void uls_destroy_istream(uls_istream_ptr_t istr);
#define uls_close_istream uls_destroy_istream

ULS_DLL_EXTERN int uls_rewind_istream(uls_istream_ptr_t istr);
ULS_DLL_EXTERN int uls_bind_istream(uls_istream_ptr_t istr, uls_lex_ptr_t uls);

ULS_DLL_EXTERN int uls_read_tok(uls_istream_ptr_t istr, uls_ptrtype_tool(outparam) parms);

ULS_DLL_EXTERN int _uls_get_raw_input_subtype(FILE* fp);
ULS_DLL_EXTERN int _uls_const_TMPLS_DUP(void);

ULS_DLL_EXTERN uls_istream_ptr_t ulsjava_open_istream_file(const void *filepath, int len_filepath);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_istream_wstr.h"
#endif

#endif // __ULS_ISTREAM_H__
