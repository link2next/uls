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
  <file> uls_input.h </file>
  <brief>
    Input manipulating procedures of linux/posix file descriptor or memory ranges.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, June 2011.
  </author>
*/

#ifndef __ULS_INPUT_H__
#define __ULS_INPUT_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/csz_stream.h"
#include "uls/litstr.h"
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_GLOBAL_TYPES
// the flags of 'uls_source'
#define ULS_ISRC_FL_EOF           0x01
#define ULS_ISRC_FL_ERR           0x02
#define ULS_ISRC_FL_DUPD          0x04

// the flags of 'commtype'
#define ULS_COMM_ONELINE     0x01
#define ULS_COMM_COLUMN0     0x02
#define ULS_COMM_NESTED      0x04
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
ULS_DECLARE_STRUCT(source);
ULS_DECLARE_STRUCT(input);
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(input_refill, int)(uls_input_ptr_t inp, int n_bytes);
ULS_DEFINE_DELEGATE_END(input_refill);

ULS_DEFINE_DELEGATE_BEGIN(fill_isource, int)(uls_source_ptr_t isrc, char*buf, int buflen, int bufsiz);
ULS_DEFINE_DELEGATE_END(fill_isource);

ULS_DEFINE_DELEGATE_BEGIN(ungrab_isource, void)(uls_source_ptr_t isrc);
ULS_DEFINE_DELEGATE_END(ungrab_isource);
#endif

#ifdef ULS_DEF_PROTECTED_TYPE
ULS_DEFINE_STRUCT(commtype)
{
	int flags;

	uls_def_namebuf(start_mark, 1 + ULS_COMM_MARK_MAXSIZ); // +1 for '\n'
	int  len_start_mark;

	uls_def_namebuf(end_mark, ULS_COMM_MARK_MAXSIZ);
	int  len_end_mark;

	int  n_lfs;
};
ULS_DEF_ARRAY_TYPE01(commtype);

ULS_DEFINE_STRUCT_BEGIN(source)
{
	uls_flags_t flags;

	uls_voidptr_t usrc; // input-stream
	uls_callback_type_this(fill_isource) usrc_fillbuff;
	uls_callback_type_this(ungrab_isource) usrc_ungrab;
};

#define ULS_INP_FL_REFILL_NULL 0x0100
ULS_DEFINE_STRUCT_BEGIN(input)
{
	uls_flags_t flags;
	uls_source_t isource;

	uls_type_tool(outbuf) rawbuf;
	const char *rawbuf_ptr;
	int  rawbuf_bytes;
	int line_num;

	uls_callback_type_this(input_refill) refill;
};
#endif // ULS_DEF_PUBLIC_TYPE

#ifdef ULS_DECL_PRIVATE_PROC
ULS_DECL_STATIC _ULS_INLINE int __input_space_proc(const char* ch_ctx, _uls_ptrtype_tool(csz_str) ss_dst, uls_ptrtype_tool(outparam) parms);
#endif

#ifdef ULS_DECL_PROTECTED_PROC
void uls_init_commtype(uls_commtype_ptr_t qmt);
void uls_deinit_commtype(uls_commtype_ptr_t qmt);

int input_skip_comment(uls_commtype_ptr_t cmt, uls_input_ptr_t inp, uls_ptrtype_tool(outparam) parms);
int input_quote_proc(uls_input_ptr_t inp, uls_quotetype_ptr_t qmt, _uls_ptrtype_tool(csz_str) ss_dst, uls_ptrtype_tool(outparam) parms);
int input_space_proc(const char* ch_ctx, uls_input_ptr_t inp, _uls_ptrtype_tool(csz_str) ss_dst, int len_surplus, uls_ptrtype_tool(outparam) parms0);

void uls_init_isource(uls_source_ptr_t isrc);
void uls_deinit_isource(uls_source_ptr_t isrc);

#define uls_input_isset_fl(inp,fl) ((inp)->flags & (fl))
#define uls_input_set_fl(inp,fl) ((inp)->flags |= (fl))
#define uls_input_clear_fl(inp,fl) ((inp)->flags &= ~(fl))
void uls_init_input(uls_input_ptr_t inp);
void uls_deinit_input(uls_input_ptr_t inp);
uls_input_ptr_t uls_create_input(void);
void uls_destroy_input(uls_input_ptr_t inp);

void uls_input_reset_cursor(uls_input_ptr_t inp);
void uls_input_reset(uls_input_ptr_t inp, int bufsiz, int flags);

void uls_input_change_filler_null(uls_input_ptr_t inp);
void uls_input_change_filler(uls_input_ptr_t inp,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc);

int uls_input_read(uls_source_ptr_t isrc, char *buf, int buflen0, int bufsiz);
void uls_regulate_rawbuf(uls_input_ptr_t inp);
int uls_resize_rawbuf(uls_input_ptr_t inp, int delta);

int uls_input_refill_null(uls_input_ptr_t inp, int n_bytes);
int uls_input_refill_buffer(uls_input_ptr_t inp, int n_bytes);

int uls_init_line_in_input(uls_input_ptr_t inp, const char* line, int n_bytes, int ipos);
void uls_ungrab_linecheck(uls_source_ptr_t isrc);

int uls_fill_null_source(uls_source_ptr_t isrc, char* buf, int buflen, int bufsiz);
void uls_ungrab_null_source(uls_source_ptr_t isrc);

int uls_fill_fd_source_utf8(uls_source_ptr_t isrc, char *buf, int buflen, int bufsiz);
int uls_fill_fd_source_utf16(uls_source_ptr_t isrc, char *buf, int buflen, int bufsiz);
int uls_fill_fd_source_utf32(uls_source_ptr_t isrc, char *buf, int buflen, int bufsiz);

#endif // ULS_DECL_PROTECTED_PROC

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_INPUT_H__
