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
 * uls_ostream.c -- the routines that make lexical streams ... --
 *     written by Stanley Hong <link2next@gmail.com>, Aug 2013.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef __ULS_OSTREAM_H__
#define __ULS_OSTREAM_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uld_conf.h"
#include "uls/csz_stream.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
#define ULS_LINE_NUMBERING     0x01
#endif

#ifdef ULS_DEF_PUBLIC_TYPE

ULS_DEFINE_STRUCT(ostream)
{
	uls_flags_t  flags;
	uls_stream_header_t header;
	int     ref_cnt;

	int     fd; /* write-only */

	uls_wr_packet_t pktbuf;
	_uls_type_tool(csz_str) out_fd_csz;
	uls_callback_type_this(make_packet) make_packet_token;
	uls_callback_type_this(make_packet) make_packet_lineno;
	uls_callback_type_this(reorder_bytes) rearrange_packet_bytes;

	uls_lex_ptr_t uls;
};

#endif // ULS_DEF_PUBLIC_TYPE

#if defined(__ULS_OSTREAM__) || defined(ULS_DECL_PRIVATE_PROC)
ULS_DECL_STATIC int uls_make_pkt__null(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) outbuf);
ULS_DECL_STATIC void __make_pkt_to_binstr(uls_wr_packet_ptr_t pkt, char* binstr, int reclen, int txtlen);

ULS_DECL_STATIC int uls_make_pkt__bin(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst);
ULS_DECL_STATIC int uls_make_pkt__bin_lno(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst);

ULS_DECL_STATIC int uls_make_pkt__txt(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst);
ULS_DECL_STATIC int uls_make_pkt__txt_lno(uls_wr_packet_ptr_t pkt, _uls_ptrtype_tool(csz_str) ss_dst);

ULS_DECL_STATIC void __init_ostream(uls_ostream_ptr_t ostr);
ULS_DECL_STATIC void __deinit_ostream(uls_ostream_ptr_t ostr);
ULS_DECL_STATIC uls_ostream_ptr_t __create_ostream(int fd);
ULS_DECL_STATIC void __destroy_ostream(uls_ostream_ptr_t ostr);
ULS_DECL_STATIC void __bind_ostream_callbacks(uls_ostream_ptr_t ostr, int stream_type);
ULS_DECL_STATIC int write_uld_to_ostream(uls_ref_array_type10(lst_names,nam_tok),
	uls_ptrtype_tool(outparam) parms, int fd_out);
ULS_DECL_STATIC int write_ostream_header(uls_ostream_ptr_t ostr, uls_ref_array_type10(lst_names,nam_tok));
ULS_DECL_STATIC int __uls_bind_ostream(uls_ostream_ptr_t ostr, const char *specname,
	uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms);
ULS_DECL_STATIC void __uls_unbind_ostream(uls_ostream_ptr_t ostr);
ULS_DECL_STATIC int __flush_uls_stream_buffer(_uls_ptrtype_tool(csz_str) outbuf, int fd, int force);
ULS_DECL_STATIC int __uls_finalize_ostream(uls_ostream_ptr_t ostr);
ULS_DECL_STATIC int __uls_make_packet_linenum(uls_ostream_ptr_t ostr, int lno, const char* tag, int tag_len);
ULS_DECL_STATIC int __uls_make_packet(uls_ostream_ptr_t ostr, int tokid, const char* tokstr, int l_tokstr);

ULS_DECL_STATIC void fill_uls_redundant_lines(char *buff, int buflen, int len1, const char *mesg2);
ULS_DECL_STATIC int writeline_istr_hdr(char *buf, int bufsiz, int k, const char *line, int linelen);
ULS_DECL_STATIC int format_uls_hdrbuf(char *ulshdr);
ULS_DECL_STATIC int do_end_of_uls_hdr(char *buff, int buflen);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_wr_packet(uls_wr_packet_ptr_t pkt);
void uls_deinit_wr_packet(uls_wr_packet_ptr_t pkt);
int uls_print_tok_eof(uls_ostream_ptr_t ostr);
int uls_print_tok_eoi(uls_ostream_ptr_t ostr);

int save_istr_hdrbuf(char *ulshdr, int buflen, int fd);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
ULS_DLL_EXTERN uls_ostream_ptr_t __uls_create_ostream(int fd_out, uls_lex_ptr_t uls, int stream_type, const char* subname);
ULS_DLL_EXTERN uls_ostream_ptr_t uls_create_ostream(int fd_out, uls_lex_ptr_t uls, const char* subname);
ULS_DLL_EXTERN uls_ostream_ptr_t uls_create_ostream_file(const char* filepath, uls_lex_ptr_t uls, const char* subname);
ULS_DLL_EXTERN int uls_destroy_ostream(uls_ostream_ptr_t ostr);
#define uls_close_ostream uls_destroy_ostream

ULS_DLL_EXTERN int __uls_print_tok(uls_ostream_ptr_t ostr, int tokid, const char* tokstr, int l_tokstr);
ULS_DLL_EXTERN int uls_print_tok(uls_ostream_ptr_t ostr, int tokid, const char* tokstr);

ULS_DLL_EXTERN int __uls_print_tok_linenum(uls_ostream_ptr_t ostr, int lno, const char* tag, int tag_len);
ULS_DLL_EXTERN int uls_print_tok_linenum(uls_ostream_ptr_t ostr, int lno, const char* tag);

ULS_DLL_EXTERN int uls_start_stream(uls_ostream_ptr_t ostr, int flags);

ULS_DLL_EXTERN int _uls_const_LINE_NUMBERING(void);

ULS_DLL_EXTERN uls_ostream_ptr_t ulsjava_create_ostream_file(const void *filepath, int len_filepath,
	uls_lex_ptr_t uls, const void *subname, int len_subname);
ULS_DLL_EXTERN int ulsjava_print_tok(uls_ostream_ptr_t ostr, int tokid, const void *tokstr, int len_tokstr);
ULS_DLL_EXTERN int ulsjava_print_tok_linenum(uls_ostream_ptr_t ostr, int lno, const void *tag, int len_tag);
#endif // ULS_DECL_PUBLIC_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#ifdef ULS_USE_WSTR
#include "uls/uls_ostream_wstr.h"
#endif

#endif // __ULS_OSTREAM_H__
