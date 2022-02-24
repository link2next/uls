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
  <file> uls_stream.c </file>
  <brief>
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, March 2013.
  </author>
*/

#ifndef __ULS_STREAM_H__
#define __ULS_STREAM_H__

#include "uls/uls_version.h"
#include "uls/uls_input.h"

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#define ULS_STREAM_BIN_LE     0
#define ULS_STREAM_BIN_BE     1
#define ULS_STREAM_TXT        2

#define ULS_STREAM_ERR         0x01
#define ULS_STREAM_FDCLOSE     0x02
#define ULS_STREAM_REWINDABLE  0x04

#define ULS_RSVD_TOKS_MAXSIZ (63+(2+N_MAX_DIGITS_INT)*N_RESERVED_TOKS)
#define ULS_TMPLS_DUP 0x01
#define ULS_RDPKT_SIZE 24

#define ULS_STREAM_RAW        0
#define ULS_STREAM_ULS        1

ULS_DECLARE_STRUCT(wr_packet);

ULS_DEFINE_DELEGATE_BEGIN(reorder_bytes,void)(char *binpkt, int len_binpkt);
ULS_DEFINE_DELEGATE_END(reorder_bytes);

ULS_DEFINE_DELEGATE_BEGIN(make_packet,int)(uls_wr_packet_ptr_t pkt, csz_str_ptr_t outbuf);
ULS_DEFINE_DELEGATE_END(make_packet);

ULS_DEFINE_STRUCT(stream_header)
{
	uls_version_t filever;

	uls_def_namebuf(specname, ULC_LONGNAME_MAXSIZ);
	int     filetype, subtype, reverse;

	uls_def_namebuf(subname, ULS_STREAM_SUBNAME_SIZE);
	uls_def_namebuf(ctime, ULS_STREAM_CTIME_SIZE);
};

ULS_DEFINE_STRUCT(rd_packet)
{
	int tok_id;
	int len_tokstr;
	const char *tokstr;
};
ULS_DEF_ARRAY_TYPE10(rd_packet);

ULS_DEFINE_STRUCT_BEGIN(wr_packet)
{
	uls_rd_packet_t pkt;
	int lineno;
	uls_reorder_bytes_t reorder_bytes;
};

ULS_DEFINE_STRUCT(tmpl)
{
	int idx;
	const char *name;
	const char *sval;
	char *name_buff, *sval_buff;
	int idx_name, idx_sval;
};
ULS_DEF_ARRAY_TYPE10(tmpl);

ULS_DEFINE_STRUCT(tmpl_list)
{
	int flags;
	uls_decl_array_type10(tmpls, tmpl);
	uls_voidptr_t shell;
};

ULS_DEFINE_STRUCT(tmplvar)
{
	const char *name;
	const char *sval;
	int i0_pkt_ary, n_pkt_ary;
};
ULS_DEF_ARRAY_TYPE10(tmplvar);

ULS_DEFINE_STRUCT(tmpl_pool)
{
	uls_decl_array_type10(tmplvars, tmplvar);
	uls_decl_array_type10(pkt_ary, rd_packet);
	int n_pkt_ary_delta;
	csz_str_t str_pool;
};

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_tmpl(uls_tmpl_ptr_t tmpl);
void uls_deinit_tmpl(uls_tmpl_ptr_t tmpl);

void uls_init_stream_header(uls_stream_header_ptr_t hdr);
void uls_deinit_stream_header(uls_stream_header_ptr_t hdr);

void uls_reorder_bytes_null(char *binpkt, int len_binpkt);
void uls_reorder_bytes_binle(char *binpkt, int len_binpkt);
void uls_reorder_bytes_binbe(char *binpkt, int len_binpkt);

uls_tmplvar_ptr_t uls_search_tmpls(uls_ref_array_type10(tmpls, tmplvar), const char* name);

void uls_init_tmpl_pool(uls_tmpl_pool_ptr_t tmpls_pool, int n_tmpls, int n_alloc);
void uls_deinit_tmpl_pool(uls_tmpl_pool_ptr_t tmpls_pool);
uls_tmpl_pool_ptr_t uls_create_tmpl_pool(int n_tmpls, int n_alloc);
void uls_destroy_tmpl_pool(uls_tmpl_pool_ptr_t tmpls_pool);

void uls_init_rd_packet(uls_rd_packet_ptr_t pkt);
void uls_deinit_rd_packet(uls_rd_packet_ptr_t pkt);
void uls_set_rd_packet(uls_rd_packet_ptr_t pkt, int tok_id, int txtlen, const char* txtptr);

void add_rd_packet_to_tmpls_pool(int tok_id, int txtlen, const char* txtptr, uls_tmpl_pool_ptr_t tmpls_pool);

void uls_init_tmplvar(uls_tmplvar_ptr_t tvar);
void uls_deinit_tmplvar(uls_tmplvar_ptr_t tvar);
#endif

ULS_DLL_EXTERN uls_tmpl_ptr_t uls_find_tmpl(uls_tmpl_list_ptr_t tmpl_list, const char* name);

ULS_DLL_EXTERN int _uls_const_STREAM_BIN_LE(void);
ULS_DLL_EXTERN int _uls_const_STREAM_BIN_BE(void);

ULS_DLL_EXTERN int uls_init_tmpls(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags);
ULS_DLL_EXTERN int uls_deinit_tmpls(uls_tmpl_list_ptr_t tmpl_list);

ULS_DLL_EXTERN void uls_reset_tmpls(uls_tmpl_list_ptr_t tmpl_list, int n_alloc);
ULS_DLL_EXTERN uls_tmpl_list_ptr_t uls_create_tmpls(int n_alloc, int flags);
ULS_DLL_EXTERN int uls_destroy_tmpls(uls_tmpl_list_ptr_t tmpl_list);
ULS_DLL_EXTERN void uls_dump_tmpls(uls_tmpl_list_ptr_t tmpl_list);

ULS_DLL_EXTERN const char* uls_get_tmpl_value(uls_tmpl_list_ptr_t tmpl_list, const char* name);
ULS_DLL_EXTERN int uls_set_tmpl_value(uls_tmpl_list_ptr_t tmpl_list, const char* name, const char *val);
ULS_DLL_EXTERN int uls_add_tmpl(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val);

ULS_DLL_EXTERN int ulsjava_add_tmpl(uls_tmpl_list_ptr_t tmpl_list,
	const void *name, int len_name, const void *val, int len_val);

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_STREAM_H__
