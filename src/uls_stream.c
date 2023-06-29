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
 * uls_stream.c -- the routines that make lexical streams ... --
 *     written by Stanley Hong <link2next@gmail.com>, March 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_stream.h"
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_log.h"
#endif

void
ULS_QUALIFIED_METHOD(uls_init_tmpl)(uls_tmpl_ptr_t tmpl)
{
	tmpl->name = tmpl->sval = NULL;
	tmpl->name_buff = tmpl->sval_buff = NULL;
	tmpl->idx_name = tmpl->idx_sval = -1;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_tmpl)(uls_tmpl_ptr_t tmpl)
{
	uls_mfree(tmpl->name_buff);
	uls_mfree(tmpl->sval_buff);
}

void
ULS_QUALIFIED_METHOD(uls_init_stream_header)(uls_stream_header_ptr_t hdr)
{
	_uls_tool_(init_version)(uls_ptr(hdr->filever));
	uls_init_namebuf(hdr->specname, ULC_LONGNAME_MAXSIZ);

	hdr->filetype = ULS_STREAM_RAW;
	hdr->subtype = hdr->reverse = 0;

	uls_init_namebuf(hdr->subname, ULS_STREAM_SUBNAME_SIZE);
	uls_init_namebuf(hdr->ctime, ULS_STREAM_CTIME_SIZE);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_stream_header)(uls_stream_header_ptr_t hdr)
{
	_uls_tool_(deinit_version)(uls_ptr(hdr->filever));
	uls_deinit_namebuf(hdr->specname);
	uls_deinit_namebuf(hdr->subname);
	uls_deinit_namebuf(hdr->ctime);
}

void
ULS_QUALIFIED_METHOD(uls_reorder_bytes_null)(char *binpkt, int len_binpkt)
{
	/* NOTHING */
}

void
ULS_QUALIFIED_METHOD(uls_reorder_bytes_binle)(char *binpkt, int len_binpkt)
{
	char *hdrbuf = binpkt;
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_BIG_ENDIAN) {
		_uls_tool_(reverse_bytes)(hdrbuf, sizeof(uls_int32));
		_uls_tool_(reverse_bytes)(hdrbuf + sizeof(uls_int32), sizeof(uls_int32));
	}
}

void
ULS_QUALIFIED_METHOD(uls_reorder_bytes_binbe)(char *binpkt, int len_binpkt)
{
	char *hdrbuf = binpkt;

	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
		_uls_tool_(reverse_bytes)(hdrbuf, sizeof(uls_int32));
		_uls_tool_(reverse_bytes)(hdrbuf + sizeof(uls_int32), sizeof(uls_int32));
	}
}

void
ULS_QUALIFIED_METHOD(uls_dump_tmpls)(uls_tmpl_list_ptr_t tmpl_list)
{
	uls_tmpl_ptr_t tmpl;
	int i;

	printf("%s:\n", __func__);

	if (tmpl_list == nilptr) {
		printf("%s: tmpl_list == NULL!\n", __func__);
		return;
	}

	for (i=0; i<tmpl_list->tmpls.n; i++) {
		tmpl = uls_get_array_slot_type10(uls_ptr(tmpl_list->tmpls), i);
		printf("\t%d] %s  '%s'\n", i, tmpl->name, tmpl->sval);
	}
	printf("\n");
}

ULS_QUALIFIED_RETTYP(uls_tmpl_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_tmpl)(uls_tmpl_list_ptr_t tmpl_list, const char* name)
{
	uls_tmpl_ptr_t tmpl;
	int i;

	if (name == NULL) return nilptr;

	for (i=0; i<tmpl_list->tmpls.n; i++) {
		tmpl = uls_get_array_slot_type10(uls_ptr(tmpl_list->tmpls), i);
		if (uls_streql(tmpl->name, name)) {
			return tmpl;
		}
	}

	return nilptr;
}

ULS_QUALIFIED_RETTYP(uls_tmplvar_ptr_t)
ULS_QUALIFIED_METHOD(uls_search_tmpls)(uls_ref_array_type10(tmpls,tmplvar), const char* name)
{
	uls_tmplvar_ptr_t tmpl;
	int i;

	if (tmpls == nilptr) return nilptr;

	for (i=0; i<tmpls->n; i++) {
		tmpl = uls_get_array_slot_type10(tmpls, i);
		// tmpl != NULL

		if (uls_streql(tmpl->name, name))
			return tmpl;
	}

	return nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_init_tmpl_pool)(uls_tmpl_pool_ptr_t tmpls_pool, int n_tmpls, int n_alloc)
{
	int i;

	uls_init_array_type10(uls_ptr(tmpls_pool->tmplvars), tmplvar, n_tmpls);
	for (i=0; i<n_tmpls; i++) {
		uls_alloc_array_slot_type10(uls_ptr(tmpls_pool->tmplvars), tmplvar, i);
	}
	tmpls_pool->tmplvars.n = n_tmpls;

	uls_init_array_type10(uls_ptr(tmpls_pool->pkt_ary), rd_packet, n_alloc);
	tmpls_pool->n_pkt_ary_delta =  n_alloc;

	_uls_tool(csz_init)(uls_ptr(tmpls_pool->str_pool), 512);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_tmpl_pool)(uls_tmpl_pool_ptr_t tmpls_pool)
{
	if (tmpls_pool == nilptr) return;

	uls_deinit_array_type10(uls_ptr(tmpls_pool->tmplvars), tmplvar);
	uls_deinit_array_type10(uls_ptr(tmpls_pool->pkt_ary), rd_packet);
	_uls_tool(csz_deinit)(uls_ptr(tmpls_pool->str_pool));
}

ULS_QUALIFIED_RETTYP(uls_tmpl_pool_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_tmpl_pool)(int n_tmpls, int n_alloc)
{
	uls_tmpl_pool_ptr_t tmpls_pool;

	tmpls_pool = uls_alloc_object(uls_tmpl_pool_t);
	uls_init_tmpl_pool(tmpls_pool, n_tmpls, n_alloc);

	return tmpls_pool;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_tmpl_pool)(uls_tmpl_pool_ptr_t tmpls_pool)
{
	if (tmpls_pool == nilptr) return;
	uls_deinit_tmpl_pool(tmpls_pool);
	uls_dealloc_object(tmpls_pool);
}

void
ULS_QUALIFIED_METHOD(uls_init_rd_packet)(uls_rd_packet_ptr_t pkt)
{
	pkt->tok_id = 0;
	pkt->len_tokstr = 0;
	pkt->tokstr = NULL;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_rd_packet)(uls_rd_packet_ptr_t pkt)
{
}

void
ULS_QUALIFIED_METHOD(uls_set_rd_packet)(uls_rd_packet_ptr_t pkt, int tok_id, int txtlen, const char* txtptr)
{
	pkt->tok_id = tok_id;
	pkt->len_tokstr = txtlen;
	pkt->tokstr = txtptr;
}

void
ULS_QUALIFIED_METHOD(add_rd_packet_to_tmpls_pool)(int tok_id, int txtlen, const char* txtptr, uls_tmpl_pool_ptr_t tmpls_pool)
{
	uls_rd_packet_ptr_t pkt;
	int siz;

	if (tmpls_pool->pkt_ary.n >= tmpls_pool->pkt_ary.n_alloc) {
		siz = tmpls_pool->pkt_ary.n_alloc + 1;
		siz = uls_roundup(siz, tmpls_pool->n_pkt_ary_delta);
		uls_resize_array_type10(uls_ptr(tmpls_pool->pkt_ary), rd_packet, siz);
	}

	uls_alloc_array_slot_type10(uls_ptr(tmpls_pool->pkt_ary), rd_packet, tmpls_pool->pkt_ary.n);
	pkt = uls_get_array_slot_type10(uls_ptr(tmpls_pool->pkt_ary), tmpls_pool->pkt_ary.n);

	uls_set_rd_packet(pkt, tok_id, txtlen, txtptr);
	++tmpls_pool->pkt_ary.n;
}

void
ULS_QUALIFIED_METHOD(uls_init_tmplvar)(uls_tmplvar_ptr_t tvar)
{
	tvar->name = NULL;
	tvar->sval = NULL;
	tvar->i0_pkt_ary = -1;
	tvar->n_pkt_ary = -1;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_tmplvar)(uls_tmplvar_ptr_t tvar)
{
}

int
ULS_QUALIFIED_METHOD(_uls_const_STREAM_BIN_LE)(void)
{
	return ULS_STREAM_BIN_LE;
}

int
ULS_QUALIFIED_METHOD(_uls_const_STREAM_BIN_BE)(void)
{
	return ULS_STREAM_BIN_BE;
}

int
ULS_QUALIFIED_METHOD(uls_init_tmpls)(uls_tmpl_list_ptr_t tmpl_list, int n_alloc, int flags)
{
	if (n_alloc < 0) return -1;

	tmpl_list->flags = ULS_FL_STATIC;
	if (flags & ULS_TMPLS_DUP) {
		tmpl_list->flags |= ULS_TMPLS_DUP;
	}

	uls_init_array_type10(uls_ptr(tmpl_list->tmpls), tmpl, n_alloc);
	tmpl_list->shell = nilptr;

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_deinit_tmpls)(uls_tmpl_list_ptr_t tmpl_list)
{
	uls_deinit_array_type10(uls_ptr(tmpl_list->tmpls), tmpl);

	tmpl_list->shell = nilptr;
	tmpl_list->flags &= ULS_FL_STATIC;

	return 0;
}

ULS_QUALIFIED_RETTYP(uls_tmpl_list_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_tmpls)(int n_alloc, int flags)
{
	uls_tmpl_list_ptr_t tmpl_list;

	tmpl_list = uls_alloc_object(uls_tmpl_list_t);

	if (uls_init_tmpls(tmpl_list, n_alloc, flags) < 0) {
		uls_dealloc_object(tmpl_list);
		return nilptr;
	}

	tmpl_list->flags &= ~ULS_FL_STATIC;
	return tmpl_list;
}

void
ULS_QUALIFIED_METHOD(uls_reset_tmpls)(uls_tmpl_list_ptr_t tmpl_list, int n_alloc)
{
	uls_resize_array_type10(uls_ptr(tmpl_list->tmpls), tmpl, n_alloc);
}

int
ULS_QUALIFIED_METHOD(uls_destroy_tmpls)(uls_tmpl_list_ptr_t tmpl_list)
{
	if (uls_deinit_tmpls(tmpl_list) < 0) {
		return -1;
	}

	if (!(tmpl_list->flags & ULS_FL_STATIC)) {
		uls_dealloc_object(tmpl_list);
	}
	return 0;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_tmpl_value)(uls_tmpl_list_ptr_t tmpl_list, const char* name)
{
	uls_tmpl_ptr_t tmpl;

	if ((tmpl = uls_find_tmpl(tmpl_list, name)) == nilptr) {
		return NULL;
	}

	return tmpl->sval;
}

int
ULS_QUALIFIED_METHOD(uls_set_tmpl_value)(uls_tmpl_list_ptr_t tmpl_list, const char* name, const char *val)
{
	uls_tmpl_ptr_t tmpl;

	if (name == NULL) return -1;

	if ((tmpl = uls_find_tmpl(tmpl_list, name)) == nilptr) {
		return -1;
	}

	if (tmpl_list->flags & ULS_TMPLS_DUP) {
		uls_mfree(tmpl->sval_buff);
		if (val != NULL) {
			tmpl->sval_buff = _uls_tool_(strdup)(val,-1);
		}
		tmpl->sval = tmpl->sval_buff;
	} else {
		tmpl->sval = val;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_add_tmpl)(uls_tmpl_list_ptr_t tmpl_list, const char *name, const char *val)
{
	uls_tmpl_ptr_t tmpl;
	int k;

	if (name == NULL) return -1;

	if ((tmpl = uls_find_tmpl(tmpl_list, name)) != nilptr) {
		return -1;
	}

	if ((k = tmpl_list->tmpls.n) >= tmpl_list->tmpls.n_alloc) {
		_uls_log(err_log)("Full of tmpl array[%d]", tmpl_list->tmpls.n_alloc);
		return -1;
	}

	uls_alloc_array_slot_type10(uls_ptr(tmpl_list->tmpls), tmpl, k);
	tmpl = uls_get_array_slot_type10(uls_ptr(tmpl_list->tmpls), k);
	tmpl->idx = k;

	if (tmpl_list->flags & ULS_TMPLS_DUP) {
		tmpl->name = tmpl->name_buff = _uls_tool_(strdup)(name, -1);
		if (val != NULL) {
			tmpl->sval = tmpl->sval_buff = _uls_tool_(strdup)(val,-1);
		} else {
			tmpl->sval = tmpl->sval_buff = NULL;
		}
	} else {
		tmpl->name = name;
		tmpl->sval = val;
	}

	tmpl_list->tmpls.n = k + 1;
	return k;
}

int
ULS_QUALIFIED_METHOD(ulsjava_add_tmpl)(uls_tmpl_list_ptr_t tmpl_list,
	const void *name, int len_name,  const void *val, int len_val)
{
	char *ustr1 = _uls_tool_(strdup)((const char *)name, len_name);
	char *ustr2 = _uls_tool_(strdup)((const char *)val, len_val);
	int rc;

	rc = uls_add_tmpl(tmpl_list, ustr1, ustr2);

	uls_mfree(ustr1);
	uls_mfree(ustr2);

	return rc;
}
