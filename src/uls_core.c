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
  <file> uls_lex.c </file>
  <brief>
    Implements the main routines of ULS, such as uls_create(), uls_get_tok(), uls_destroy(), ...
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_CORE__
#include "uls/uls_core.h"
#include "uls/uls_freq.h"
#include "uls/uld_conf.h"
#include "uls/uls_num.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC _ULS_INLINE void
ULS_QUALIFIED_METHOD(__ready_to_use_lexseg)(uls_context_ptr_t ctx)
{
	uls_lexseg_ptr_t lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), ctx->n_lexsegs);

	lexseg->offset2 = -1;
	lexseg->len_text = -1;
	lexseg->n_lfs_raw = 0;

	lexseg->tokdef_vx = nilptr;
	ctx->i_lexsegs = 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(find_prefix_radix)(uls_ptrtype_tool(outparam) parms, uls_lex_ptr_t uls, const char *str)
{
	uls_number_prefix_ptr_t numpfx;
	int i, radix;

	parms->n1 = 0;
	parms->n2 = 10;

	for (i=0; i<uls->n_numcnst_prefixes; i++) {
		numpfx = uls_get_array_slot_type00(uls_ptr(uls->numcnst_prefixes), i);

		if (!_uls_tool_(strncmp)(str, uls_get_namebuf_value(numpfx->prefix), numpfx->l_prefix)) {
			parms->n1 = numpfx->l_prefix;
			parms->n2 = radix = numpfx->radix;
			break;
		}
	}

	return parms->n1;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_number)(uls_lex_ptr_t uls, uls_context_ptr_t ctx, uls_ptrtype_tool(parm_line) parm_ln)
{
	char *cnst_suffixes = uls->numcnst_suffixes;
	uls_wch_t numsep = uls->numcnst_separator;
	uls_ptrtype_tool(outbuf) tokbuf = uls_ptr(ctx->tokbuf);
	const char *cnst_suffix, *srp, *lptr, *lptr0;
	uls_type_tool(outparam) parms1;
	int k = 0, l_tokstr, l_cnst_suffix, n;
	char ch;

	lptr0 = lptr = parm_ln->lptr;
	parms1.flags = 0;
	parms1.wch = numsep;

	for (n = 0; *lptr == '0'; lptr++) ++n;
	if (n > 0) --lptr;

	if (n > 0 && lptr[1] == '.') {
		parms1.n1 = 0;
		parms1.n2 = 10;
	} else {
		lptr = lptr0;
		 find_prefix_radix(uls_ptr(parms1), uls, lptr);
		srp = parms1.lptr_end = _uls_tool_(get_standard_number_prefix)(parms1.n2);
		if (parms1.n2 != 10 && srp == NULL) { // NERVER-REACHED
			_uls_log(err_log)("ERROR: Unsupported radix = %d", parms1.n2);
			return -1;
		}
	}

	parms1.lptr = lptr;
	k = _uls_tool_(extract_number)(uls_ptr(parms1), tokbuf, k);
	if (k < 0) {
		parms1.flags = parms1.len = parms1.n = 0;
		parms1.lptr = lptr0;
		return -1; // not number
	}

	l_tokstr = k;
	str_putc(tokbuf, k++, '\0');

	lptr = parms1.lptr;
	ctx->n_expo = parms1.n;
	ctx->n_digits = parms1.len;

	if ((ch=*lptr) != '\0' && !_uls_tool_(isspace)(ch)) {
		if ((cnst_suffix=is_cnst_suffix_contained(cnst_suffixes, lptr, -1, nilptr)) != NULL) {
			l_cnst_suffix = _uls_tool_(strlen)(cnst_suffix);
			k = _uls_tool(str_append)(tokbuf, k, cnst_suffix, l_cnst_suffix);
			lptr += l_cnst_suffix;
		}
	}
	str_putc(tokbuf, k, '\0');

	parm_ln->lptr = lptr;
	return l_tokstr;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(make_eof_lexeme)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	int lno, k;
	char *tagstr;

	lno = uls_ctx_get_lineno(ctx);
	tagstr = _uls_tool(csz_text)(uls_ptr(ctx->tag));
	k = __uls_make_eoftok_lexeme(uls_ptr(ctx->tokbuf), lno, tagstr);

	ctx->tok = uls->xcontext.toknum_EOF;
	uls->tokdef_vx = slots_rsv[EOF_TOK_IDX];

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = k;
	ctx->s_val_wchars = _uls_tool(ustr_num_wchars)(ctx->s_val, k, nilptr);
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_context_ptr_t)
ULS_QUALIFIED_METHOD(make_eoi_lexeme)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));

	// EOF or EOI
	ctx->tok = uls->xcontext.toknum_EOI;
	uls->tokdef_vx = slots_rsv[EOI_TOK_IDX];

	str_putc(uls_ptr(ctx->tokbuf), 0, '\0');
	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = ctx->s_val_wchars = 0;

	return ctx;
}

ULS_DECL_STATIC _ULS_INLINE int
ULS_QUALIFIED_METHOD(__uls_is_real)(const char *ptr)
{
	int stat;

	if (*ptr == '-' || *ptr == '0') ++ptr;

	if (*ptr == '.') {
		stat = 1;
	} else {
		stat = 0;
	}

	return stat;
}

ULS_DECL_STATIC _ULS_INLINE double
ULS_QUALIFIED_METHOD(__uls_lexeme_unsigned_double)(const char *ptr)
{
	double d_val;

	if (__uls_is_real(ptr)) {
		d_val = atof(ptr);
	} else {
		d_val = (double) strtol(ptr, NULL, 16);
	}

	return d_val;
}

ULS_DECL_STATIC uls_uint32
ULS_QUALIFIED_METHOD(__uls_lexeme_uint32)(const char *ptr)
{
	uls_uint32 u32_val;
	char ch;
	int i;

	if (__uls_is_real(ptr)) {
		u32_val = (uls_uint32) __uls_lexeme_unsigned_double(ptr);

	} else {
		u32_val = 0;

		if (ptr[0] == '0' && ptr[1] == 'x') { // hexa-decimal
			ptr += 2;
			for (i=0; (ch=*ptr) != '\0' && _uls_tool_(isxdigit)(ch); i++, ptr++) {
				u32_val = (u32_val << 4) + (_uls_tool_(isdigit)(ch) ? ch - '0' : 10 + (ch - 'A'));
			}
		} else { // decimal
			for (i=0; (ch=*ptr) != '\0' && _uls_tool_(isdigit)(ch); i++, ptr++) {
				u32_val = u32_val * 10 +  ch - '0';
			}
		}
	}

	return u32_val;
}

ULS_DECL_STATIC uls_uint64
ULS_QUALIFIED_METHOD(__uls_lexeme_uint64)(const char *ptr)
{
	uls_uint64 u64_val;
	char ch;
	int i;

	if (__uls_is_real(ptr)) {
		u64_val = (uls_uint64) __uls_lexeme_unsigned_double(ptr);

	} else {
		u64_val = 0;

		if (ptr[0] == '0' && ptr[1] == 'x') { // hexa-decimal
			ptr += 2;
			for (i=0; (ch=*ptr) != '\0' && _uls_tool_(isxdigit)(ch); i++, ptr++) {
				u64_val = (u64_val << 4) + (_uls_tool_(isdigit)(ch) ? ch - '0' : 10 + (ch - 'A'));
			}
		} else {  // decimal
			for (i=0; (ch=*ptr) != '\0' && _uls_tool_(isxdigit)(ch); i++, ptr++) {
				u64_val = u64_val* 10 + ch - '0' ;
			}
		}
	}

	return u64_val;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_change_line)(uls_lex_ptr_t uls, const char *line, int len, int flags)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	int start_lno = 1;
	uls_ptrtype_tool(outparam) parm;
	char *ptr;

	if (flags & ULS_WANT_EOFTOK) uls_want_eof(uls);
	else uls_unwant_eof(uls);

	uls_input_change_filler_null(inp);
	uls_input_reset(inp, 0, ULS_INP_FL_REFILL_NULL);

	if (xctx->len_prepended_input > 0) {
		parm = uls_alloc_object(uls_type_tool(outparam));

		ptr = (char *) _uls_tool_(malloc)(xctx->len_prepended_input + len + 1);
		_uls_tool_(memcopy)(ptr, xctx->prepended_input, xctx->len_prepended_input);
		_uls_tool_(memcopy)(ptr + xctx->len_prepended_input, line, len);
		if (flags & ULS_MEMFREE_LINE) uls_mfree(line);

		parm->lptr = line = ptr;
		parm->len = len = xctx->len_prepended_input + len;
		ptr[len] = '\0';

		inp->isource.usrc = (uls_voidptr_t) parm;
		inp->isource.usrc_ungrab = uls_ref_callback_this(uls_ungrab_linecheck);

		start_lno -= xctx->lfs_prepended_input;

	} else if (flags & (ULS_DO_DUP|ULS_MEMFREE_LINE)) {
		parm = uls_alloc_object(uls_type_tool(outparam));

		if (flags & ULS_DO_DUP) {
			line = _uls_tool_(strdup)(line, len);
		}

		parm->lptr = line;
		parm->len = len;

		inp->isource.usrc = (uls_voidptr_t) parm;
		inp->isource.usrc_ungrab = uls_ref_callback_this(uls_ungrab_linecheck);
	}

	inp->line_num = start_lno;
	inp->rawbuf_ptr = line;
	inp->rawbuf_bytes = len;
	inp->isource.flags |= ULS_ISRC_FL_EOF;

	ctx->line = ctx->line_end = inp->rawbuf_ptr;
	ctx->lptr = ctx->line;
	__uls_ctx_set_lineno(ctx, start_lno);
}

const char*
ULS_QUALIFIED_METHOD(skip_white_spaces)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	const char  *lptr, *ch_ctx = uls->ch_context;
	char ch;

	for (lptr = ctx->lptr; (ch=*lptr) != '\0'; lptr++) {
		if (!uls_canbe_ch_space(ch_ctx, ch)) {
			break;
		}
		if (ch == '\n') {
			__uls_ctx_inc_lineno(ctx, 1);
		}
	}

	ctx->lptr = lptr;
	return lptr;
}

void
ULS_QUALIFIED_METHOD(realloc_tokdef_array)(uls_lex_ptr_t uls, int n1, int n2)
{
	int siz;

	/* allocate the slot for new uls_tokdef_vx_t. */
	if (uls->tokdef_vx_array.n_alloc < (siz = uls->tokdef_vx_array.n + n1)) {
		siz = uls_roundup(siz, TOKDEF_LINES_DELTA);
		uls_resize_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, siz);
		// Need it to zerofy the newly allocated space?
	}

	/* allocate the slot for new uls_tokdef_ptr_t. */
	if (uls->tokdef_array.n_alloc < (siz = uls->tokdef_array.n + n2)) {
		siz = uls_roundup(siz, TOKDEF_LINES_DELTA);
		uls_resize_parray(uls_ptr(uls->tokdef_array), tokdef, siz);
	}
}

void
ULS_QUALIFIED_METHOD(free_tokdef_array)(uls_lex_ptr_t uls)
{
	uls_tokdef_ptr_t e;
	uls_tokdef_vx_ptr_t e_vx;

	uls_decl_parray_slots(slots_keyw, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int i;

	slots_keyw = uls_parray_slots(uls_ptr(uls->tokdef_array));
	for (i=0; i<uls->tokdef_array.n; i++) {
		e = slots_keyw[i];
		uls_destroy_tokdef(e);
	}
	uls_deinit_parray(uls_ptr(uls->tokdef_array));

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		if (!(e_vx->flags & (ULS_VX_CHRMAP|ULS_VX_REFERRED))) {
			e_vx->base = nilptr;
			uls_destroy_tokdef_vx(e_vx);
		}
//		slots_vx[i] = nilptr;
	}

	uls_deinit_parray(uls_ptr(uls->tokdef_vx_array));
	uls_deinit_parray(uls_ptr(uls->tokdef_vx_rsvd));
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(get_idtok_list)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	uls_decl_parray_slots(slots_keyw, tokdef);
	uls_tokdef_ptr_t idtok_list;
	uls_tokdef_ptr_t e;
	int i, n = 0;

	slots_keyw = uls_parray_slots(uls_ptr(uls->tokdef_array));
	idtok_list = nilptr;

	for (i=0; i < uls->tokdef_array.n; i++) {
		e = slots_keyw[i];

		if (e->keyw_type == ULS_KEYW_TYPE_IDSTR) {
			e->link = idtok_list;
			idtok_list = e;
			++n;
		}
	}

	parms->n = n;
	return idtok_list;
}

int
ULS_QUALIFIED_METHOD(ulc_load)(ulc_header_ptr_t uls_config, FILE *fin_ulc,
	FILE *fin_uld, int siz_uld_filebuff, FILE *fin_ulf)
{
	uls_lex_ptr_t uls = uls_config->uls;
	ulc_fpitem_ptr_t  ulc_fp_stack;
	char     linebuff[ULS_LINEBUFF_SIZ__ULC+1], *lptr;
	int      i, linelen, rc, stat = 0;
	int      n_idtok_list, tok_id;

	uls_tokdef_ptr_t e, e_link, idtok_list;
	uls_tokdef_vx_ptr_t e_vx;

	uls_keyw_stat_list_ptr_t keyw_stat_list;
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	int n_lst;

	ulf_header_t ulf_hdr;
	uls_type_tool(outparam) parms;
	uls_kwtable_ptr_t kw_tbl;

	ulc_fp_stack = ulc_fp_push(nilptr, fin_ulc, uls_get_namebuf_value(uls->ulc_name));

	uls_config->fp_stack = ulc_fp_stack;
	rc = ulc_read_header(fin_ulc, uls_config);
	ulc_fp_stack = (ulc_fpitem_ptr_t) uls_config->fp_stack;
	if (rc < 0) {
		_uls_log(err_log)("fail to read the header of uls-spec.");
		release_ulc_fp_stack(ulc_fp_stack);
		_uls_tool_(fp_close)(fin_uld);
		_uls_tool_(fp_close)(fin_ulf);
		return -1;
	}

	reset_tokid_seed(uls, uls_config);

	parms.line = uls_get_namebuf_value(uls_config->tagbuf);
	parms.x1 = 1;
	ulc_fp_stack = ulc_fp_pop(ulc_fp_stack, uls_ptr(parms));
	fin_ulc = (FILE *) parms.native_data;

	uls_config->lno = parms.n;
	while (1) {
		if ((linelen = _uls_tool_(fp_gets)(fin_ulc, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				_uls_log(err_log)("%s: ulc file i/o error at %d", __func__, parms.n);
				stat = -1;
				break;
			}

			_uls_tool_(fp_close)(fin_ulc);
			if (ulc_fp_stack == nilptr) break;

			parms.line = uls_get_namebuf_value(uls_config->tagbuf);
			parms.x1 = 1;
			ulc_fp_stack = ulc_fp_pop(ulc_fp_stack, uls_ptr(parms));
			fin_ulc = (FILE *) parms.native_data;

			uls_config->lno = parms.n;
			continue;
		}
		++uls_config->lno;

		if (*(lptr = _uls_tool(skip_blanks)(linebuff)) == '\0' ||
			*lptr == '#' || (lptr[0]=='/' && lptr[1]=='/'))
			continue;

		if ((e_vx = ulc_proc_line(uls_config, lptr, uls_ptr(parms))) == nilptr) {
			stat = -1;
			break;
		}

//		e = (uls_tokdef_ptr_t) parms.data;
		tok_id = e_vx->tok_id;

		if (tok_id < uls_config->tok_id_min)
			uls_config->tok_id_min = tok_id;

		if (tok_id > uls_config->tok_id_max)
			uls_config->tok_id_max = tok_id;
	}

	release_ulc_fp_stack(ulc_fp_stack);
	if (stat < 0) {
		free_tokdef_array(uls);
		_uls_tool_(fp_close)(fin_uld);
		_uls_tool_(fp_close)(fin_ulf);
		return -1;
	}

	if (fin_uld != NULL) {
		rc = uld_load_fp(uls, fin_uld, siz_uld_filebuff);
		_uls_tool_(fp_close)(fin_uld);
		if (rc < 0) {
			_uls_log(err_log)("Failed to read uld-file!");
			return -1;
		}
	}

	kw_tbl = uls_ptr(uls->idkeyw_table);
	uls_reset_kwtable(kw_tbl, uls->flags & ULS_FL_CASE_INSENSITIVE);

	idtok_list = get_idtok_list(uls, uls_ptr(parms));
	// a list of type 'tokdef_t*'
	n_idtok_list = parms.n;

	if (fin_ulf != NULL &&
		(keyw_stat_list = ulf_load(idtok_list, n_idtok_list, fin_ulf, uls_ptr(ulf_hdr))) != nilptr) {
		for (i = 0; i < 3; i++) {
			kw_tbl->hash_stat.weight[i] = ulf_hdr.weights[i];
		}

		lst = uls_ptr(keyw_stat_list->lst);
		n_lst = keyw_stat_list->lst.n;

		slots_lst = uls_parray_slots(lst);
		_uls_quicksort_vptr(slots_lst, n_lst, keyw_stat_comp_by_freq);

		for (i=0; i < n_lst; i++) {
			e = slots_lst[i]->keyw_info;
			// Enter the items having low frequency first!
			uls_add_kw(kw_tbl, e);
		}

		ulc_free_kwstat_list(keyw_stat_list);
		keyw_stat_list = nilptr;

	} else {
		ulf_init_header(uls_ptr(ulf_hdr));

		// consume tok_info_lst upto n_lst
		for (e = idtok_list; e != nilptr; e = e_link) {
			e_link = e->link;
			uls_add_kw(kw_tbl, e);
		}
	}

	ulf_deinit_header(uls_ptr(ulf_hdr));
	_uls_tool_(fp_close)(fin_ulf);

	return stat;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_init_fp)(uls_lex_ptr_t uls)
{
	int  n;

	uls_initial_zerofy_object(uls);
	uls->flags = ULS_FL_STATIC;

	uls_init_namebuf(uls->ulc_name, ULC_LONGNAME_MAXSIZ);
	uls_set_namebuf_value(uls->ulc_name, "none");

	uls->numcnst_separator = ULS_DECIMAL_SEPARATOR_DFL;

	uls_init_array_type00(uls_ptr(uls->numcnst_prefixes), number_prefix, ULS_N_MAX_NUMBER_PREFIXES);
	uls->n_numcnst_prefixes = 0;

	uls_init_bytespool(uls->numcnst_suffixes, ULS_CNST_SUFFIXES_MAXSIZ + 1, 1);
	uls->numcnst_suffixes[0] = '\0';

	uls_init_array_tool_type01(uls_ptr(uls->idfirst_charset), uch_range, 0);
	uls_init_array_tool_type01(uls_ptr(uls->id_charset), uch_range, 0);

	uls_init_array_type01a(uls_ptr(uls->commtypes), commtype, ULS_N_MAX_COMMTYPES);
	uls->n_commtypes = 0;

	uls_init_parray(uls_ptr(uls->quotetypes), quotetype, ULS_N_MAX_QUOTETYPES);

	_uls_tool_(version_make)(uls_ptr(uls->ulc_ver), 0, 0, 0);
	_uls_tool_(version_make)(uls_ptr(uls->config_filever),
		ULC_VERSION_MAJOR, ULC_VERSION_MINOR, ULC_VERSION_DEBUG);
	_uls_tool_(version_make)(uls_ptr(uls->stream_filever),
		ULS_VERSION_STREAM_MAJOR, ULS_VERSION_STREAM_MINOR,
		ULS_VERSION_STREAM_DEBUG);

	uls_init_bytespool(uls->ch_context, ULS_SYNTAX_TABLE_SIZE, 0);
	uls->id_max_bytes = uls->id_max_uchars = ULS_INT_MAX;

	n = N_RESERVED_TOKS + TOKDEF_LINES_DELTA;
	uls_init_parray(uls_ptr(uls->tokdef_array), tokdef, n);
	uls_init_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, n);
	uls_init_parray(uls_ptr(uls->tokdef_vx_rsvd), tokdef_vx, N_RESERVED_TOKS);
	uls->tokdef_vx = nilptr;

	uls_init_kwtable(uls_ptr(uls->idkeyw_table));
	uls_init_1char_table(uls_ptr(uls->onechar_table));
	uls_init_2char_table(uls_ptr(uls->twoplus_table));

	uls_init_escmap_pool(uls_ptr(uls->escstr_pool));

	uls_xcontext_init(uls_ptr(uls->xcontext), uls_ref_callback_this(uls_gettok_raw));
	uls->xcontext.context->flags |= ULS_CTX_FL_EOF | ULS_CTX_FL_GETTOK_RAW;

	init_reserved_toks(uls);
}

void
ULS_QUALIFIED_METHOD(uls_dealloc_lex)(uls_lex_ptr_t uls)
{
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_number_prefix_ptr_t numpfx;
	int i;

	uls_xcontext_deinit(uls_ptr(uls->xcontext));

	uls_deinit_2char_table(uls_ptr(uls->twoplus_table));
	uls_deinit_kwtable(uls_ptr(uls->idkeyw_table));
	free_tokdef_array(uls);

	// 1char tokens
	uls_deinit_1char_table(uls_ptr(uls->onechar_table));

	// idfirst_charsets
	uls_deinit_array_tool_type01(uls_ptr(uls->idfirst_charset), uch_range);

	// id_charsets
	uls_deinit_array_tool_type01(uls_ptr(uls->id_charset), uch_range);

	// commtypes
	uls->n_commtypes = 0;
	uls_deinit_array_type01a(uls_ptr(uls->commtypes), commtype);

	// quotetypes
	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		uls_destroy_quotetype(qmt);
	}
	uls_deinit_parray(uls_ptr(uls->quotetypes));
	uls_deinit_escmap_pool(uls_ptr(uls->escstr_pool));

	uls_deinit_namebuf(uls->ulc_name);
	uls_deinit_bytespool(uls->ch_context);

	for (i=0; i<uls->n_numcnst_prefixes; i++) {
		numpfx = uls_get_array_slot_type00(uls_ptr(uls->numcnst_prefixes), i);
		uls_deinit_namebuf(numpfx->prefix);
		numpfx->l_prefix = 0;
		numpfx->radix = 0;
	}
	uls_deinit_array_type00(uls_ptr(uls->numcnst_prefixes), number_prefix);
	uls->n_numcnst_prefixes = 0;

	uls_deinit_bytespool(uls->numcnst_suffixes);

	if (uls->flags & ULS_FL_STATIC) {
		uls->shell = nilptr;
	} else {
		uls_dealloc_object(uls);
	}
}

int
ULS_QUALIFIED_METHOD(uls_spec_compatible)(uls_lex_ptr_t uls, const char *specname, uls_ptrtype_tool(version) filever)
{
	int  stat=1;
	char  ver_str1[ULS_VERSION_STR_MAXLEN+1];
	char  ver_str2[ULS_VERSION_STR_MAXLEN+1];

	if (!uls_streql(uls_get_namebuf_value(uls->ulc_name), specname) ||
		!uls_vers_compatible(uls_ptr(uls->stream_filever), filever)) {
		_uls_tool_(version_make_string)(uls_ptr(uls->stream_filever), ver_str1);
		_uls_tool_(version_make_string)(filever, ver_str2);
		_uls_log(err_log)("%s: Unsupported or not compatible:", __func__);
		_uls_log(err_log)("\t'%s'(%s)", uls_get_namebuf_value(uls->ulc_name), ver_str1);
		_uls_log(err_log)("\t'%s'(%s)", specname, ver_str2);
		stat = 0;
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_fillbuff)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	const char *line;
	int rc;

	if ((ctx->flags & ULS_CTX_FL_ERR) || (inp->isource.flags & ULS_ISRC_FL_ERR)) {
		_uls_log(err_log)("%s: called again after I/O failed!", __func__);
		return -1;
	}

	if (ctx->flags & ULS_CTX_FL_EOF) return 0;

	rc = ctx->fill_proc(uls_ptr(uls->xcontext));
	if (rc < 0 || (rc == 0 && ctx->n_lexsegs == 0)) {
		if (rc < 0) {
			uls->tokdef_vx = set_err_tok(uls, "failed to fill buffer!");
		}

		ctx->lptr = ctx->line = ctx->cnst_nilstr;
		ctx->line_end = ctx->line;

		return rc;
	}

	ctx->lptr = ctx->line = line = _uls_tool(csz_text)(uls_ptr(ctx->zbuf1));
	if ((ctx->line_end=line+rc) < line) {
		_uls_log(err_panic)("%s: invalid string length, %d.", __func__, rc);
		return -1;
	}

	__ready_to_use_lexseg(ctx);

	return 1;
}

int
ULS_QUALIFIED_METHOD(uls_clear_and_fillbuff)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	_uls_tool(csz_reset)(uls_ptr(ctx->zbuf1));
	_uls_tool(csz_reset)(uls_ptr(ctx->zbuf2));

	return uls_fillbuff(uls);
}

int
ULS_QUALIFIED_METHOD(uls_fillbuff_and_reset)(uls_lex_ptr_t uls, const char *str0)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	int rc;

	_uls_tool(csz_reset)(uls_ptr(ctx->zbuf1));
	_uls_tool(csz_reset)(uls_ptr(ctx->zbuf2));

	if (str0 != NULL) {
		_uls_tool(csz_puts)(uls_ptr(ctx->zbuf1), str0);
	}

	if ((rc = uls_fillbuff(uls)) > 0) {
		ctx = uls->xcontext.context;
		ctx->tok = uls->xcontext.toknum_NONE;
		ctx->tokbuf.buf[0] = '\0';
		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = 0;
	}

	return rc;
}


const char*
ULS_QUALIFIED_METHOD(__uls_eof_tag)(const char *ptr, uls_ptrtype_tool(outparam) parms)
{
	parms->lptr = ptr;
	_uls_tool_(skip_atou)(parms); // linenum

	++parms->lptr;
	parms->len = _uls_tool_(skip_atou)(parms); // the length of eof-tag

	return ++parms->lptr;
}

int
ULS_QUALIFIED_METHOD(__uls_make_eoftok_lexeme)(uls_ptrtype_tool(outbuf) tokbuf, int lno, const char *tagstr)
{
	int k, rc;

	if (tagstr == NULL) {
		tagstr = ""; rc = 0;
	} else {
		rc = _uls_tool_(strlen)(tagstr);
	}

	k = ((1+N_MAX_DIGITS_INT)<<1) + 2 + rc; // 1+ == '-'

	_uls_tool(str_append)(tokbuf, 0, NULL, k);
	k = _uls_log_(snprintf)(tokbuf->buf, k+1, "%d %d", lno, rc);

	if (rc > 0) {
		str_putc(tokbuf, k++, ' ');
		k = _uls_tool(str_append)(tokbuf, k, tagstr, rc);
	}

	return k;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(set_err_tok)(uls_lex_ptr_t uls, const char *errmsg)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	const char *lptr;
	char numbuf[32];
	int k, k1;

	ctx->tok = uls->xcontext.toknum_ERR;

	lptr = uls_ctx_get_tag(ctx);
	k = k1 = _uls_tool(str_puts)(uls_ptr(ctx->tokbuf), 0, lptr);
	ctx->s_val_wchars = _uls_tool(ustr_num_wchars)(lptr, k, nilptr);

	_uls_log_(snprintf)(numbuf, sizeof(numbuf), "<%d> ", uls_ctx_get_lineno(ctx));
	k = _uls_tool(str_puts)(uls_ptr(ctx->tokbuf), k, numbuf);
	k = _uls_tool(str_puts)(uls_ptr(ctx->tokbuf), k, errmsg);

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = k;
	ctx->s_val_wchars += k - k1;
	ctx->flags |= ULS_CTX_FL_ERR;

	return slots_rsv[ERR_TOK_IDX];
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_onechar_lexeme_vx)(uls_lex_ptr_t uls, uls_tokdef_vx_ptr_t e_vx,
	const char *lptr, int len)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	ctx->tok = e_vx->tok_id;
	_uls_tool(str_append)(uls_ptr(ctx->tokbuf), 0, lptr, len);

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = len;
	ctx->s_val_wchars = 1;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(__uls_onechar_lexeme)(uls_lex_ptr_t uls, uls_wch_t wch, const char *lptr, int len)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), wch, nilptr)) == nilptr) {
		e_vx = ctx->anonymous_uchar_vx;
		e_vx->tok_id = (int) wch;
	}

	__uls_onechar_lexeme_vx(uls, e_vx, lptr, len);
	return e_vx;
}

int
ULS_QUALIFIED_METHOD(uls_gettok_raw)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t  ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));

	const char      *lptr, *ch_ctx = uls->ch_context;
	char            ch, ch_grp;
	uls_lexseg_ptr_t  lexseg, lexseg_next;
	int             j, k, rc, n_wchars;
	uls_wch_t       wch;
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	uls_type_tool(parm_line) parm_ln;

	if (ctx->delta_lineno != 0) {
		__uls_ctx_inc_lineno(ctx, ctx->delta_lineno);
		ctx->delta_lineno = 0;
	}

 next_loop:
	lptr = skip_white_spaces(uls);

	if ((ch=*lptr) < ULS_SYNTAX_TABLE_SIZE) {
		ch_grp = ch_ctx[ch];
	} else {
		ch_grp = ULS_CH_2PLUS;
	}

	wch = ch;
	rc = 1;

	if ((_uls_tool_(isdigit)(ch) || ch == '.') &&
		(parm_ln.lptr=lptr, k=get_number(uls, ctx, uls_ptr(parm_ln)), lptr=parm_ln.lptr, k > 0)) {
		e_vx = slots_rsv[NUM_TOK_IDX];
		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = ctx->s_val_wchars = k;
		ctx->tok = e_vx->tok_id;

	} else if ((ch_grp & ULS_CH_IDFIRST) || (rc = uls_is_char_idfirst(uls, lptr, &wch)) > 0) {
		for (n_wchars = k = 0; ; ) {
			for (j=0; j<rc; j++) {
				str_putc(uls_ptr(ctx->tokbuf), k++, *lptr++);
			}
			++n_wchars;

			if ((ch=*lptr) < ULS_SYNTAX_TABLE_SIZE) {
				if (!(ch_ctx[ch] & ULS_CH_ID)) break;
				wch = ch;
				rc = 1;
			} else {
				rc = _uls_tool_(decode_utf8)(lptr, -1, &wch);
				if (!uls_is_char_id(uls, wch)) break;
			}
		}
		str_putc(uls_ptr(ctx->tokbuf), k, '\0');

		if ((e = is_keyword_idstr(uls_ptr(uls->idkeyw_table), ctx->tokbuf.buf, k)) != nilptr) {
			e_vx = e->view;
			ctx->tok = e_vx->tok_id;
			ctx->s_val = ctx->tokbuf.buf;
			ctx->s_val_len = k;
			ctx->s_val_wchars = n_wchars;
		} else if (n_wchars > uls->id_max_uchars || k > uls->id_max_bytes ) {
			e_vx = set_err_tok(uls, "Too long identifier!");
		} else {
			str_putc(uls_ptr(ctx->tokbuf), k, '\0');

			e_vx = slots_rsv[ID_TOK_IDX];
			ctx->s_val = ctx->tokbuf.buf;
			ctx->s_val_len = k;
			ctx->s_val_wchars = n_wchars;
			ctx->tok = e_vx->tok_id;
		}

	} else if ((ch_grp & ULS_CH_2PLUS) &&
		(e_vx = is_keyword_twoplus(uls_ptr(uls->twoplus_table), ch_ctx, lptr)) != nilptr) {
		/* FOUND */
		e = e_vx->base;
		ctx->tok = e_vx->tok_id;

		rc = e->ulen_keyword;
		_uls_tool(str_append)(uls_ptr(ctx->tokbuf), 0, lptr, rc);

		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = rc;
		ctx->s_val_wchars = _uls_tool(ustr_num_wchars)(ctx->s_val, rc, nilptr);
		lptr += rc;

	} else if (ch == '\0') {
		if (ctx->i_lexsegs >= ctx->n_lexsegs) {
			if ((rc = uls_clear_and_fillbuff(uls)) < 0) {
				return 0;
			}

			if (rc > 0) {
				// actually, rc == 1, means than it's been actually filled from IO-buffer.
				goto next_loop; // normal case
			}

			// EOF
			make_eof_lexeme(uls);
			return 1;
		}

		// QUOTE-STRING
		lexseg = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs);
		++ctx->i_lexsegs;
		lexseg_next = uls_get_array_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs);

		ctx->line = ctx->lptr = csz_data_ptr(uls_ptr(ctx->zbuf1)) + lexseg_next->offset1;
		ctx->line_end = ctx->line + lexseg_next->len1;
		lptr = ctx->lptr;

		if (lexseg->offset2 < 0) {
			goto next_loop;
		}
		e_vx = lexseg->tokdef_vx;

		// lexseg: offset1~'\0'(len1) and offset2~len_text
		ctx->flags |= ULS_CTX_FL_QTOK | ULS_CTX_FL_EXTERN_TOKBUF;
		ctx->s_val = csz_data_ptr(uls_ptr(ctx->zbuf2)) + lexseg->offset2;

		ctx->tok = e_vx->tok_id;
		if (ctx->tok == uls->xcontext.toknum_NUMBER) {
			ctx->s_val_len = _uls_tool_(strlen)(ctx->s_val);
		} else {
			ctx->s_val_len = lexseg->len_text;
		}

		ctx->s_val_wchars = _uls_tool(ustr_num_wchars)(ctx->s_val, ctx->s_val_len, nilptr);
		ctx->delta_lineno = lexseg->n_lfs_raw;

	} else {
		if (ch < ULS_SYNTAX_TABLE_SIZE) {
			wch = ch;
			rc = 1;
		} else {
			rc = _uls_tool_(decode_utf8)(lptr, -1, &wch);
		}

		if ((e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), wch, nilptr)) != nilptr) {
			if (wch == '\n') {
				if (ctx->lineno <= 0) {
					__uls_ctx_inc_lineno(ctx, 1);
					ctx->lptr = ++lptr;
					goto next_loop;
				}

				ctx->delta_lineno =  1;
			}
			__uls_onechar_lexeme_vx(uls, e_vx, lptr, rc);

		} else if (_uls_tool_(isgraph)(wch) || (uls->flags & ULS_FL_MULTIBYTES_CHRTOK)) {
			e_vx = __uls_onechar_lexeme(uls, wch, lptr, rc);

		} else {
			if (_uls_tool_(isgraph)(wch)) {
				e_vx = set_err_tok(uls, "Unknown unicode char!");
			} else { // skipping the ASCII control-char
				ctx->lptr = ++lptr;
				goto next_loop;
			}
		}

		lptr += rc;
	}

	uls->tokdef_vx = e_vx;
	ctx->lptr = lptr;

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__uls_change_isrc)(uls_lex_ptr_t uls, uls_voidptr_t usrc,
	uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	const char *str0;
	int start_lno;

	uls_input_reset(inp, ULS_INPUT_BUFSIZ, -1);
	uls_input_change_filler(inp, usrc, fill_rawbuf, ungrab_proc);

	start_lno = 1;
	if (xctx->len_prepended_input > 0) {
		str0 = xctx->prepended_input;
		start_lno -= xctx->lfs_prepended_input;
	} else {
		str0 = NULL;
	}

	if (uls_fillbuff_and_reset(uls, str0) < 0) {
		_uls_log(err_log)("%s: fail to fill the initial buff", __func__);
		return -1;
	}

	__uls_ctx_set_lineno(ctx, start_lno);
	return 0;
}

ULS_QUALIFIED_RETTYP(uls_context_ptr_t)
ULS_QUALIFIED_METHOD(uls_push)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_flags_t  mask_want_eof = ctx->flags & ULS_CTX_FL_WANT_EOFTOK;
	uls_context_ptr_t ctx_new;

	ctx_new = uls_alloc_object(uls_context_t);
	uls_init_context(ctx_new, uls_ref_callback_this(uls_gettok_raw), uls->xcontext.toknum_NONE);

	ctx_new->flags |= ULS_CTX_FL_GETTOK_RAW;
	if (mask_want_eof)
		ctx_new->flags |= ULS_CTX_FL_WANT_EOFTOK;

	uls_input_reset(ctx_new->input, -1, 0);

	ctx_new->prev = ctx;
	uls->xcontext.context = ctx_new;

	return ctx_new;
}

const char*
ULS_QUALIFIED_METHOD(uls_tokstr)(uls_lex_ptr_t uls)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	const char *tokstr = __uls_lexeme(uls);

	if (ctx->tok == xctx->toknum_NUMBER) {
		if (ctx->l_tokbuf_aux < 0) {
			ctx->l_tokbuf_aux = _uls_tool_(number_gexpr)(tokstr, ctx->s_val_len,
				ctx->n_expo, uls_ptr(ctx->tokbuf_aux));
		}

		if (ctx->l_tokbuf_aux > 0) {
			tokstr = ctx->tokbuf_aux.buf;
		}
	}

	return tokstr;
}

int
ULS_QUALIFIED_METHOD(uls_tokstr_len)(uls_lex_ptr_t uls)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	int rc, len = uls_lexeme_len(uls);

	if (ctx->tok == xctx->toknum_NUMBER) {
		uls_tokstr(uls);
		if ((rc = ctx->l_tokbuf_aux) > 0) {
			len = rc;
		}
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(uls_tokstr_wlen)(uls_lex_ptr_t uls)
{
	const char *tokstr;
	int len;

	tokstr = uls_tokstr(uls);
	len = uls_tokstr_len(uls);

	return _uls_tool(ustr_num_wchars)(tokstr, len, nilptr);
}

const char*
ULS_QUALIFIED_METHOD(uls_tok2keyw)(uls_lex_ptr_t uls, int t)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	const char *str;

	if ((e_vx = uls_find_tokdef_vx(uls, t)) == nilptr) {
		str = NULL;
	} else {
		if ((e = e_vx->base) == nilptr) str = "";
		else str = uls_get_namebuf_value(e->keyword);
	}

	return str;
}

const char*
ULS_QUALIFIED_METHOD(uls_tok2name)(uls_lex_ptr_t uls, int t)
{
	uls_tokdef_vx_ptr_t e_vx;
	const char *str;

	if ((e_vx = uls_find_tokdef_vx(uls, t)) == nilptr) {
		str = NULL;
	} else {
		str = uls_get_namebuf_value(e_vx->name);
	}

	return str;
}

const char*
ULS_QUALIFIED_METHOD(uls_tok2keyw_2)(uls_lex_ptr_t uls, int t, uls_ptrtype_tool(outparam) parms)
{
	const char *str;
	int len;

	if ((str = uls_tok2keyw(uls, t)) != NULL) {
		len = _uls_tool_(strlen)(str);
	} else {
		len = 0;
	}

	if (parms != nilptr) {
		parms->lptr = str;
		parms->len = len;
	}

	return str;
}

const char*
ULS_QUALIFIED_METHOD(uls_tok2name_2)(uls_lex_ptr_t uls, int t, uls_ptrtype_tool(outparam) parms)
{
	const char *str;
	int len;

	if ((str = uls_tok2name(uls, t)) != NULL) {
		len = _uls_tool_(strlen)(str);
	} else {
		len = 0;
	}

	if (parms != nilptr) {
		parms->lptr = str;
		parms->len = len;
	}

	return str;
}

int
ULS_QUALIFIED_METHOD(_uls_get_tokid_list)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	uls_onechar_table_ptr_t tbl = uls_ptr(uls->onechar_table);
	int k, n_alloc, n1, n2, n3, ch, ch0;
	int *outbuf;

	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	uls_onechar_tokdef_etc_ptr_t  e_etc;
	uls_tokdef_vx_ptr_t e_vx;
	int i, j;

	n1 = uls->tokdef_vx_array.n;
	for (n2 = ch = 0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (uls_find_1char_tokdef_vx(tbl, ch, nilptr) != nilptr) {
			++n2;
		}
	}

	n3 = 0;
	for (e_etc = uls->onechar_table.tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		++n3;
	}

	n_alloc = n1 + n2 + n3;
	outbuf = (int *) _uls_tool_(malloc)(n_alloc * sizeof(int));

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (k=0; k<n1; k++) {
		e_vx = slots_vx[k];
		outbuf[k] = e_vx->tok_id;
	}

	for (e_etc = uls->onechar_table.tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		ch = e_etc->wch;
		if (!__is_in_ilist(outbuf, k, ch)) {
			outbuf[k++] = ch;
		}
	}

	for (i = 0; i < ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(uls->onechar_table.tokgrps), i);
		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));

		ch0 = tokgrp->ch0;
		for (j = 0; j < tokgrp->tokdef_vx_1char.n; j++) {
			ch = ch0 + j;
			if ((e_vx = slots_vx[j]) != nilptr) {
				if (k >= n_alloc) {
					_uls_log(err_log)("%s: InternalError, Out of Buffer!", __func__);
					uls_mfree(outbuf);
					parms->native_data = NULL;
					return 0;
				}
				if (!__is_in_ilist(outbuf, k, ch)) {
					outbuf[k++] = ch;
				}
			}
		}
	}

	for (ch = 0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (uls_find_1char_tokdef_vx(tbl, ch, nilptr) != nilptr && !__is_in_ilist(outbuf, k, ch)) {
			outbuf[k++] = ch;
		}
	}

	if (k != n_alloc) {
		outbuf = (int *) _uls_tool_(mrealloc)(outbuf, k * sizeof(int));
	}

	parms->native_data = outbuf;
	return k;
}

void
ULS_QUALIFIED_METHOD(_uls_put_tokid_list)(uls_ptrtype_tool(outparam) parms)
{
	int *outbuf = (int *) parms->native_data;
	uls_mfree(outbuf);
}

ULS_QUALIFIED_RETTYP(uls_tokid_simple_list_ptr_t)
ULS_QUALIFIED_METHOD(_uls_get_tokid_list_2)(uls_lex_ptr_t uls)
{
	uls_tokid_simple_list_ptr_t lst;
	uls_type_tool(outparam) parms;

	lst = uls_alloc_object(uls_tokid_simple_list_t);

	lst->n_tokid_list = _uls_get_tokid_list(uls, uls_ptr(parms));
	lst->tokid_list = (int *) parms.native_data;
	lst->uls = uls;

	return lst;
}

void
ULS_QUALIFIED_METHOD(_uls_put_tokid_list_2)(uls_tokid_simple_list_ptr_t lst)
{
	uls_type_tool(outparam) parms;

	parms.native_data = lst->tokid_list;
	_uls_put_tokid_list(uls_ptr(parms));

	lst->tokid_list = nilptr;
	lst->n_tokid_list = 0;

	uls_dealloc_object(lst);
}

int
ULS_QUALIFIED_METHOD(uls_get_1char_charset)(char *buff)
{
	int ch, n = 0;

	for (ch = 1; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (_uls_tool_(isgraph)(ch) && !_uls_tool_(isalnum)(ch)) {
			buff[n++] = (char) ch;
		}
	}

	buff[n] = '\0';
	return n;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__filter_1char_toks)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	char *char_tokens = parms->line; // ULS_SYNTAX_TABLE_SIZE
	const char *not_chrtoks = parms->lptr;
	char not_ch_toks[ULS_CHRTOKS_MAXSIZ + 1];
	char  ch, *wrdptr;
	int i, k, n;

	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_commtype_ptr_t cmt;
	uls_quotetype_ptr_t qmt;

	k = _uls_tool_(strcpy)(not_ch_toks, not_chrtoks);

	for (i = 0; i < uls->n_commtypes; i++) {
		cmt = uls_get_array_slot_type01(uls_ptr(uls->commtypes), i);
		ch = uls_get_namebuf_value(cmt->start_mark)[0];
		if (cmt->len_start_mark == 1 &&
				_uls_tool_(isgraph)(ch) && _uls_tool_(strchr)(not_ch_toks, ch) == NULL) {
			not_ch_toks[k++] = ch;
			not_ch_toks[k] = '\0';
		}
	}

	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i = 0; i < uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		ch = uls_get_namebuf_value(qmt->start_mark)[0];
		if (qmt->len_start_mark == 1 &&
				_uls_tool_(isgraph)(ch) && _uls_tool_(strchr)(not_ch_toks, ch) == NULL) {
			not_ch_toks[k++] = ch;
			not_ch_toks[k] = '\0';
		}
	}

	n = uls_get_1char_charset(char_tokens);

	for (wrdptr = not_ch_toks; (ch = *wrdptr) != '\0'; wrdptr++) {
		for (i = 0; i < n; i++) { // linear-search
			if (char_tokens[i] == ch) {
				char_tokens[i] = ' ';
				break;
			}
		}
	}

	for (i = 0; i < n; ) {
		if (char_tokens[i] == ' ') {
			char_tokens[i] = char_tokens[--n];
			char_tokens[n] = '\0';
		} else {
			++i;
		}
	}
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__rearrange_1char_toks)(uls_lex_ptr_t uls, const char *char_tokens)
{
	uls_onechar_table_ptr_t tbl = uls_ptr(uls->onechar_table);
	uls_decl_parray_slots(slots_vx, tokdef_vx);

	uls_tokdef_outparam_t parms2;
	uls_tokdef_vx_ptr_t e_vx;
	const char *cptr;
	int i, j, ch;

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (i = N_RESERVED_TOKS; i < uls->tokdef_vx_array.n; ) {
		e_vx = slots_vx[i];
		if (e_vx->flags & ULS_VX_CHRMAP) {
			// delete e_vx from tokdef_vx_array[]
			if ((j = uls->tokdef_vx_array.n - 1) != i) {
				slots_vx[i] = slots_vx[j];
			}
			--uls->tokdef_vx_array.n;
		} else {
			++i;
		}
	}

	for (cptr = char_tokens; (ch = *cptr) != '\0'; cptr++) {
		// ch not in { '\n', '\t' }
		if (find_1char_tokdef_map(tbl, ch, uls_ptr(parms2)) == nilptr && parms2.tokgrp != nilptr) {
			e_vx = uls_create_tokdef_vx(ch, NULL, nilptr);
			e_vx->flags |= ULS_VX_CHRMAP;
			insert_1char_tokdef_map(parms2.tokgrp, ch, e_vx);
		}
	}
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__load_ulc_from_config_files)(ulc_header_ptr_t hdr, const char *confname)
{
	uls_lex_ptr_t uls = hdr->uls;
	char   specname[ULC_LONGNAME_MAXSIZ+1];
	char   linebuff[ULS_LINEBUFF_SIZ__ULD+1];
	char char_tokens[ULS_SYNTAX_TABLE_SIZE];

	FILE   *fin_ulc, *fin_ulf, *fin_uld = NULL;
	int    rc, len_basedir, len_specname, typ_fpath;
	int    siz_uld_filebuff = 0;
	uls_type_tool(outparam) parms1;

	parms1.line = specname;
	typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
	len_basedir = parms1.n;
	len_specname = parms1.len;

	if (typ_fpath < 0) return -1;

	if (typ_fpath == ULS_NAME_FILEPATH_ULD) {
		if ((fin_uld=_uls_tool_(fp_open)(confname, ULS_FIO_READ)) == NULL) {
			_uls_log(err_log)("can't open file %s", confname);
			return -1;
		}

		if ((rc = _uls_tool_(fp_filesize)(fin_uld)) < 0) {
			_uls_log(err_log)("invalid file: %s", confname);
			return -1;
		}
		siz_uld_filebuff = uld_calc_filebuff_size(rc);

		rc = _uls_tool_(fp_gets)(fin_uld, linebuff, sizeof(linebuff), 0);
		if ( rc < 3 || !(linebuff[0] == '#' && linebuff[1] == '@')) {
			_uls_log(err_log)("Invalid spec-name in %s.", confname);
			_uls_tool_(fp_close)(fin_uld);
			return -1;
		}

		// Get the base specname in the line.
		rc = _uls_tool(str_trim_end)(linebuff, rc);
		confname = linebuff + 2;

		parms1.line = specname;
		typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
		len_basedir = parms1.n;
		len_specname = parms1.len;

		if (typ_fpath != ULS_NAME_SPECNAME) {
			_uls_log(err_log)("Invalid spec-name in %s.", confname);
			_uls_tool_(fp_close)(fin_uld);
			return -1;
		}
	}

	uls_set_namebuf_value(uls->ulc_name, specname);

	if ((fin_ulc = uls_get_ulc_path(typ_fpath, confname, len_basedir,
		specname, len_specname, uls_ptr(parms1))) == NULL) {
		_uls_tool_(fp_close)(fin_uld);
		return -1;
	}
	fin_ulf = (FILE *) parms1.native_data;

	if ((rc = ulc_load(hdr, fin_ulc, fin_uld, siz_uld_filebuff, fin_ulf)) < 0) {
		return -1;
	}
	// fin_ulc, fin_ulf, fin_uld consumed

	if (check_if_keyw(uls) < 0) {
		return -1;
	}

	// 1char
	parms1.line = char_tokens;
	parms1.lptr = hdr->not_chrtoks;
	__filter_1char_toks(uls, uls_ptr(parms1));
	__rearrange_1char_toks(uls, char_tokens);

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_init)(uls_lex_ptr_t uls, const char *confname)
{
	ulc_header_t  uls_config;
	int stat = 0;

	if (uls == nilptr || confname == NULL) {
		_uls_log(err_log)("%s: invalid parameter!", __func__);
		return -1;
	}

	__uls_init_fp(uls);
	ulc_init_header(uls_ptr(uls_config), uls);

	if (__load_ulc_from_config_files(uls_ptr(uls_config), confname) < 0) {
		uls_dealloc_lex(uls);
		stat = -1;
	} else if (uls_classify_tok_group(uls) < 0) {
		_uls_log(err_log)("%s: lex-conf file not consistent!", __func__);
		stat = -1;
	} else {
		uls_grab(uls);
	}

	ulc_deinit_header(uls_ptr(uls_config));
	return stat;
}

ULS_QUALIFIED_RETTYP(uls_lex_ptr_t)
ULS_QUALIFIED_METHOD(uls_create)(const char *confname)
{
	uls_lex_ptr_t uls;

	uls = uls_alloc_object(uls_lex_t);
	if (uls_init(uls, confname) < 0) {
		uls_dealloc_object(uls);
		return nilptr;
	}

	uls->flags &= ~ULS_FL_STATIC;
	return uls;
}

void
ULS_QUALIFIED_METHOD(uls_reset)(uls_lex_ptr_t uls)
{
	uls_pop_all(uls);
}

int
ULS_QUALIFIED_METHOD(uls_destroy)(uls_lex_ptr_t uls)
{
	if (uls == nilptr || uls->ref_cnt <= 0) {
		_uls_log(err_log)("%s: called on an invalid object!", __func__);
		return -1;
	}

	if (--uls->ref_cnt > 0) return uls->ref_cnt;

	uls_pop_all(uls);
	uls_dealloc_lex(uls);

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_skip_blanks)(uls_lex_ptr_t uls)
{
	int stat = 0;
	uls_context_ptr_t ctx = uls->xcontext.context;
	const char *lptr;

	if (*(lptr = skip_white_spaces(uls)) == '\0') {
		if (ctx->i_lexsegs >= ctx->n_lexsegs) {
			if (uls_clear_and_fillbuff(uls) < 0) {
				stat = -1;
			} else {
				lptr = skip_white_spaces(uls);
			}
		}
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_is_quote_tok)(uls_lex_ptr_t uls, int tok_id)
{
	uls_quotetype_ptr_t qmt;
	qmt = uls_find_quotetype_by_tokid(uls_ptr(uls->quotetypes), uls->quotetypes.n, tok_id);
	return qmt != nilptr ? 1 : 0;
}

uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_get_extra_tokdef)(uls_lex_ptr_t uls, int tok_id)
{
	uls_tokdef_vx_ptr_t e_vx;

	if (__uls_tok(uls) == tok_id) {
		return uls_get_current_extra_tokdef(uls);
	}

	if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) == nilptr)
		return nilptr;

	return e_vx->extra_tokdef;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_set_extra_tokdef_vx)(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_tokdef_vx_ptr_t e_vx;
	char buff[ULS_UTF8_CH_MAXLEN];
	uls_wch_t wch;
	int rc;

	if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) == nilptr) {
		if ((uls->flags & ULS_FL_MULTIBYTES_CHRTOK) && tok_id > 0 && xctx->num_unregst_wch_tokens < 100) {
			wch = tok_id;

			if ((rc = _uls_tool_(encode_utf8)(wch, buff, -1)) > 0) {
				e_vx = uls_create_tokdef_vx(wch, NULL, nilptr);
				insert_1char_tokdef_etc(uls_ptr(uls->onechar_table), wch, e_vx);
				__uls_onechar_lexeme_vx(uls, e_vx, buff, rc);
				++xctx->num_unregst_wch_tokens;
			}
		}
	}

	if (e_vx != nilptr) {
		e_vx->extra_tokdef = extra_tokdef;
	} else {
		_uls_log(err_log)("%s: unregistered (wchar) token(%d)", __func__, tok_id);
	}

	return e_vx;
}

int
ULS_QUALIFIED_METHOD(uls_set_extra_tokdef)(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
{
	int rval;

	if (uls_set_extra_tokdef_vx(uls, tok_id, extra_tokdef) != nilptr) {
		rval = 0;
	} else {
		rval = -1;
	}

	return rval;
}

uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_get_current_extra_tokdef)(uls_lex_ptr_t uls)
{
	return uls->tokdef_vx->extra_tokdef;
}

void
ULS_QUALIFIED_METHOD(uls_set_current_extra_tokdef)(uls_lex_ptr_t uls, uls_voidptr_t extra_tokdef)
{
	uls_tokdef_vx_ptr_t e_vx = uls->tokdef_vx;

	if (e_vx->flags & ULS_VX_ANONYMOUS) {
		e_vx = uls_set_extra_tokdef_vx(uls, __uls_tok(uls), extra_tokdef);
	} else {
		e_vx->extra_tokdef = extra_tokdef;
	}
}

int
ULS_QUALIFIED_METHOD(uls_get_tok)(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	if (ctx->tok == uls->xcontext.toknum_EOI || ctx->tok == uls->xcontext.toknum_ERR) {
		return ctx->tok;
	}

	ctx->l_tokbuf_aux = -1;

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		return ctx->tok;
	}

	ctx->flags &= ~(ULS_CTX_FL_QTOK | ULS_CTX_FL_EXTERN_TOKBUF);

	if (ctx->tok == uls->xcontext.toknum_EOF) {
		if ((ctx = uls_pop(uls)) == nilptr) {
			ctx = make_eoi_lexeme(uls);
			return ctx->tok;
		}
	}

	while (1) {
		if (ctx->gettok(uls) == 0) {
			if (ctx->tok == uls->xcontext.toknum_NONE) {
				continue;
			}
			break;
		}

		if (ctx->tok == uls->xcontext.toknum_EOF) {
			if (ctx->flags & ULS_CTX_FL_WANT_EOFTOK) {
				break;
			}
		}

		if ((ctx = uls_pop(uls)) == nilptr) {
			ctx = make_eoi_lexeme(uls);
			break;
		}
	}

	return ctx->tok;
}

void
ULS_QUALIFIED_METHOD(uls_set_tok)(uls_lex_ptr_t uls, int tokid, const char *lexeme, int l_lexeme)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	ctx->tok = tokid;

	if (lexeme == NULL) {
		lexeme = "";
		l_lexeme = 0;
	} else if (l_lexeme < 0) {
		l_lexeme = _uls_tool_(strlen)(lexeme);
	}

	_uls_tool(str_append)(uls_ptr(ctx->tokbuf), 0, lexeme, l_lexeme);
	ctx->s_val = ctx->tokbuf.buf;

	ctx->s_val_len = l_lexeme;
	ctx->s_val_wchars = _uls_tool(ustr_num_wchars)(ctx->s_val, l_lexeme, nilptr);
	ctx->l_tokbuf_aux = -1;
}

void
ULS_QUALIFIED_METHOD(uls_expect)(uls_lex_ptr_t uls, int value)
{
	int val0 = __uls_tok(uls);

	if (val0 != value) {
		if (_uls_tool_(isprint)(val0))
			_uls_log(err_log)("tok-'%c'", val0);
		else
			_uls_log(err_log)("tok-%d", val0);

		if (_uls_tool_(isprint)(value))
			_uls_log(err_panic)("tok-'%c' expected!!", value);
		else
			_uls_log(err_panic)("tok-%d expected!!", value);
	}
}

int
ULS_QUALIFIED_METHOD(uls_push_isrc)(uls_lex_ptr_t uls,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_push(uls);

	if (__uls_change_isrc(uls, isrc, fill_rawbuf, ungrab_proc) < 0) {
		uls_pop(uls);
		return -1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_isrc)(uls_lex_ptr_t uls,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_pop(uls);
	return uls_push_isrc(uls, isrc, fill_rawbuf, ungrab_proc);
}

ULS_QUALIFIED_RETTYP(uls_context_ptr_t)
ULS_QUALIFIED_METHOD(uls_pop)(uls_lex_ptr_t uls)
{
	uls_source_ptr_t isrc;
	uls_context_ptr_t ctx = uls->xcontext.context, ctx_prev;
	uls_userdata_ptr_t ud, ud_inner;

	if (ctx == nilptr || (ctx_prev=ctx->prev) == nilptr) {
		return nilptr;
	}

	isrc = uls_ptr(ctx->input->isource);
	isrc->usrc_ungrab(isrc);

	for (ud=ctx->user_data; ud != nilptr; ud=ud_inner) {
		ud_inner = ud->inner;
		if (ud->proc != nilptr) ud->proc(ud->data);
		uls_dealloc_object(ud);
	}
	ctx->user_data = nilptr;

	uls_deinit_context(ctx);
	uls_dealloc_object(ctx);

	uls->xcontext.context = ctx_prev;

	return ctx_prev;
}

void
ULS_QUALIFIED_METHOD(uls_pop_all)(uls_lex_ptr_t uls)
{
	while (!uls_is_context_initial(uls)) {
		uls_pop(uls);
	}
}

int
ULS_QUALIFIED_METHOD(uls_push_line)(uls_lex_ptr_t uls, const char *line, int len, int flags)
{
	if (line == NULL) {
		line = "";
		len = 0;
	} else if (len < 0) {
		len = _uls_tool_(strlen)(line);
	}

	uls_push(uls);
	__uls_change_line(uls, line, len, flags);

	if (uls_fillbuff_and_reset(uls, NULL) < 0) {
		_uls_log(err_log)("%s: fail to fill the initial buff", __func__);
		return -1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_set_line)(uls_lex_ptr_t uls, const char *line, int len, int flags)
{
	uls_pop(uls);
	return uls_push_line(uls, line, len, flags);
}

int
ULS_QUALIFIED_METHOD(uls_cardinal_toknam)(char *toknam, uls_lex_ptr_t uls, int tok_id, const char *tokstr)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_tokdef_vx_ptr_t e_vx;
	uls_quotetype_ptr_t qmt;
	const char *cptr;
	int rc, is_one_wch, has_lxm = 1;
	uls_wch_t wch;

	if (tok_id == xctx->toknum_ID) {
		cptr = "ID";

	} else if (tok_id == xctx->toknum_NUMBER) {
		cptr = "NUMBER";

	} else if (tok_id == xctx->toknum_TMPL) {
		cptr = "TMPL";

	} else if (tok_id == xctx->toknum_EOI) {
		cptr = "EOI";
		has_lxm = 0;

	} else if (tok_id == xctx->toknum_EOF) {
		cptr = "EOF";

	} else if (tok_id == xctx->toknum_LINK) {
		cptr = "LINK";
		has_lxm = 0;

	} else if (tok_id == '\n') {
		cptr = "LF";
		has_lxm = 0;

	} else if (tok_id == '\t') {
		cptr = "TAB";
		has_lxm = 0;

	} else if ((qmt=uls_find_quotetype_by_tokid(uls_ptr(uls->quotetypes), uls->quotetypes.n, tok_id)) != nilptr) {
		if (qmt->tok_id == xctx->toknum_NONE) {
			cptr = "NONE";
			has_lxm = 0;
		} else {
			cptr = "LITSTR";
		}
	} else {
		rc = _uls_tool_(decode_utf8)(tokstr, -1, &wch);
		if (*tokstr != '\0' && rc < _uls_tool_(strlen)(tokstr)) {
			is_one_wch = 0;
		} else {
			is_one_wch = 1;
		}

		if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) != nilptr) {
			cptr = uls_get_namebuf_value(e_vx->name);
		} else {
			cptr = NULL;
			toknam[0] = '\0';
			if (!is_one_wch) {
				_uls_log(err_log)("InternalError: token(%d) lexeme('%s')", tok_id, tokstr);
			} else if ((int) wch != tok_id) {
				_uls_log_(snprintf)(toknam, ULS_CARDINAL_TOKNAM_SIZ, "%5d", tok_id);
			}
		}
	}

	if (cptr != NULL) {
		_uls_tool_(strcpy)(toknam, cptr);
	}

	return has_lxm;
}

int
ULS_QUALIFIED_METHOD(uls_get_number_prefix)(uls_ptrtype_tool(outparam) parms, char *prefix)
{
	const char *tok_str = parms->lptr;
	int i = 0;

	if (*tok_str == '-') {
		prefix[i++] = '-';
		++tok_str;
	}

	if (*tok_str == '.') {
		prefix[i++] = '0';
	}

	prefix[i] = '\0';
	parms->lptr = tok_str;

	return i;
}

int
ULS_QUALIFIED_METHOD(uls_cardinal_toknam_deco)(char *toknam_buff, const char *toknam)
{
	int len;
	len = _uls_log_(snprintf)(toknam_buff, ULS_CARDINAL_TOKNAM_SIZ+1, "[%7s]", toknam);
	return len;
}

int
ULS_QUALIFIED_METHOD(uls_cardinal_toknam_deco_lxmpfx)(char *toknam_buff, char *lxmpfx, uls_lex_ptr_t uls,
	int tok_id, uls_ptrtype_tool(outparam) parms)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	char toknam[ULS_CARDINAL_TOKNAM_SIZ+1];
	int  has_lxm;

	has_lxm = uls_cardinal_toknam(toknam, uls, tok_id, parms->lptr);
	uls_cardinal_toknam_deco(toknam_buff, toknam);

	if (tok_id == xctx->toknum_NUMBER) {
		uls_get_number_prefix(parms, lxmpfx);
	} else {
		lxmpfx[0] = '\0';
	}

	return has_lxm;
}

void
ULS_QUALIFIED_METHOD(uls_dump_tok)(uls_lex_ptr_t uls, const char *pfx, const char *suff)
{
	int tok_id = __uls_tok(uls), has_lxm;
	const char *numsuff, *tok_str = uls_tokstr(uls);
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];
	char lxmpfx[ULS_CARDINAL_LXMPFX_MAXSIZ+1];
	uls_type_tool(outparam) parms;

	if (pfx == NULL) pfx = "";
	if (suff == NULL) suff = "";

	parms.lptr = tok_str;
	has_lxm = uls_cardinal_toknam_deco_lxmpfx(toknam_buff, lxmpfx, uls, tok_id, uls_ptr(parms));
	tok_str = parms.lptr;

	_uls_log_(printf)("%s%s", pfx, toknam_buff);
	if (has_lxm) {
		_uls_log_(printf)(" %s%s", lxmpfx, tok_str);
		if (tok_id == uls->xcontext.toknum_NUMBER) {
			if ((numsuff = uls_number_suffix(uls)) != NULL && *numsuff != '\0') {
				_uls_log_(printf)(" %s", numsuff);
			}
		}
	}
	_uls_log_(printf)("%s", suff);
}

void
ULS_QUALIFIED_METHOD(_uls_dump_tok_2)(const char *pfx, const char *id_str,
	const char *tok_str, const char *suff)
{
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];

	if (pfx == NULL) pfx = "";
	if (suff == NULL) suff = "";

	uls_cardinal_toknam_deco(toknam_buff, id_str);
	_uls_log_(printf)("%s%s", pfx, toknam_buff);
	_uls_log_(printf)(" %s%s", tok_str, suff);
}

void
ULS_QUALIFIED_METHOD(uls_dumpln_tok)(uls_lex_ptr_t uls)
{
	uls_dump_tok(uls, "\t", "\n");
}

int
ULS_QUALIFIED_METHOD(uls_is_real)(uls_lex_ptr_t uls)
{
	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		return __uls_is_real(__uls_lexeme(uls));
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_is_int)(uls_lex_ptr_t uls)
{
	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		return !__uls_is_real(__uls_lexeme(uls));
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_is_zero)(uls_lex_ptr_t uls)
{
	const char *ptr;
	int len;

	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		ptr = __uls_lexeme(uls);
		len = __uls_lexeme_len(uls);
		if (ptr[0] == '0' && (len == 1 || (len == 2 && ptr[1] == '.')) ) {
			return 1;
		}
	}

	return 0;
}

const char*
ULS_QUALIFIED_METHOD(uls_number_suffix)(uls_lex_ptr_t uls)
{
	const char *cptr;

	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		cptr = __uls_lexeme(uls) + __uls_lexeme_len(uls) + 1;
	} else {
		cptr = "";
	}

	return cptr;
}

const char*
ULS_QUALIFIED_METHOD(uls_eof_tag)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_EOF)
		return NULL;

	ptr = __uls_eof_tag(__uls_lexeme(uls), parms);
	return ptr;
}

double
ULS_QUALIFIED_METHOD(uls_lexeme_double)(uls_lex_ptr_t uls)
{
	const char *ptr;
	double d_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		_uls_log(err_log)("The current token is NOT number!");
		return 0.;
	}

	if (*(ptr = __uls_lexeme(uls)) == '-') {
		minus = 1;
		++ptr;
	}

	d_val = __uls_lexeme_unsigned_double(ptr);
	if (minus) d_val = -d_val;

	return d_val;
}

uls_uint32
ULS_QUALIFIED_METHOD(uls_lexeme_uint32)(uls_lex_ptr_t uls)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER || *(ptr = uls_tokstr(uls)) == '-') {
		_uls_log(err_log)("The current token is NOT unsigned-int-type!");
		return 0;
	}

	return __uls_lexeme_uint32(ptr);
}

uls_int32
ULS_QUALIFIED_METHOD(uls_lexeme_int32)(uls_lex_ptr_t uls)
{
	const char *ptr;
	uls_int32 u32_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		_uls_log(err_log)("The current token is NOT signed-int-type!");
		return 0;
	}

	if (*(ptr = uls_tokstr(uls)) == '-') {
		minus = 1;
		++ptr;
	}

	u32_val = __uls_lexeme_uint32(ptr);

	return minus ? - (uls_int32) u32_val : u32_val;
}

uls_uint64
ULS_QUALIFIED_METHOD(uls_lexeme_uint64)(uls_lex_ptr_t uls)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER || *(ptr = uls_tokstr(uls)) == '-') {
		_uls_log(err_log)("The current token is NOT unsigned-int-type!");
		return 0;
	}

	return __uls_lexeme_uint64(ptr);
}

uls_int64
ULS_QUALIFIED_METHOD(uls_lexeme_int64)(uls_lex_ptr_t uls)
{
	const char *ptr;
	uls_int64 i64_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		_uls_log(err_log)("The current token is NOT signed-int-type!");
		return 0;
	}

	if (*(ptr = uls_tokstr(uls)) == '-') {
		minus = 1;
		++ptr;
	}

	i64_val = __uls_lexeme_uint64(ptr);
	return minus ? - (uls_int64) i64_val : i64_val;
}

unsigned int
ULS_QUALIFIED_METHOD(uls_lexeme_u)(uls_lex_ptr_t uls)
{
	unsigned int u_val;

	if (sizeof(unsigned int) == sizeof(uls_uint32)) {
		u_val = (unsigned int) uls_lexeme_uint32(uls);
	} else if (sizeof(unsigned int) == sizeof(uls_uint64)) {
		u_val = (unsigned int) uls_lexeme_uint64(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(unsigned-int) size!");
		u_val = 0;
	}

	return u_val;
}

int
ULS_QUALIFIED_METHOD(uls_lexeme_d)(uls_lex_ptr_t uls)
{
	int i_val;

	if (sizeof(int) == sizeof(uls_int32)) {
		i_val = (int) uls_lexeme_int32(uls);
	} else if (sizeof(int) == sizeof(uls_int64)) {
		i_val = (int) uls_lexeme_int64(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(int) size!");
		i_val = 0;
	}

	return i_val;
}

unsigned long
ULS_QUALIFIED_METHOD(uls_lexeme_lu)(uls_lex_ptr_t uls)
{
	unsigned long lu_val;

	if (sizeof(unsigned long) == sizeof(uls_uint32)) {
		lu_val = (unsigned long) uls_lexeme_uint32(uls);
	} else if (sizeof(unsigned long) == sizeof(uls_uint64)) {
		lu_val = (unsigned long) uls_lexeme_uint64(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(unsigned-long) size!");
		lu_val = 0;
	}

	return lu_val;
}

long
ULS_QUALIFIED_METHOD(uls_lexeme_ld)(uls_lex_ptr_t uls)
{
	long ld_val;

	if (sizeof(long) == sizeof(uls_int32)) {
		ld_val = (long) uls_lexeme_int32(uls);
	} else if (sizeof(long) == sizeof(uls_int64)) {
		ld_val = (long) uls_lexeme_int64(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(long) size!");
		ld_val = 0;
	}

	return ld_val;
}

unsigned long long
ULS_QUALIFIED_METHOD(uls_lexeme_llu)(uls_lex_ptr_t uls)
{
	unsigned long long llu_val;

	if (sizeof(unsigned long long) == sizeof(uls_uint64)) {
		llu_val = (unsigned long long) uls_lexeme_uint64(uls);
	} else if (sizeof(unsigned long long) == sizeof(uls_uint32)) {
		llu_val = (unsigned long long) uls_lexeme_uint32(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(unsigned-long-long) size!");
		llu_val = 0;
	}

	return llu_val;
}

long long
ULS_QUALIFIED_METHOD(uls_lexeme_lld)(uls_lex_ptr_t uls)
{
	long long lld_val;

	if (sizeof(long long) == sizeof(uls_int64)) {
		lld_val = (long long) uls_lexeme_int64(uls);
	} else if (sizeof(long long) == sizeof(uls_int32)) {
		lld_val = (long long) uls_lexeme_int32(uls);
	} else {
		_uls_log(err_log)("Unsupported integer(long long) size!");
		lld_val = 0;
	}

	return lld_val;
}

int
ULS_QUALIFIED_METHOD(_uls_const_WANT_EOFTOK)(void)
{
	return ULS_WANT_EOFTOK;
}

int
ULS_QUALIFIED_METHOD(_uls_const_DO_DUP)(void)
{
	return ULS_DO_DUP;
}

uls_wch_t
ULS_QUALIFIED_METHOD(_uls_const_NEXTCH_NONE)(void)
{
	return ULS_WCH_NONE;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_EOI)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_EOI;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_EOF)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_EOF;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_ERR)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_ERR;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_NONE)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_NONE;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_ID)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_ID;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_NUMBER)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_NUMBER;
}

int
ULS_QUALIFIED_METHOD(_uls_toknum_TMPL)(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_TMPL;
}

int
ULS_QUALIFIED_METHOD(_uls_get_lineno)(uls_lex_ptr_t uls)
{
	return uls_ctx_get_lineno(uls->xcontext.context);
}

void
ULS_QUALIFIED_METHOD(_uls_set_lineno)(uls_lex_ptr_t uls, int lineno)
{
	uls_ctx_set_tag(uls->xcontext.context, NULL, lineno);
}

void
ULS_QUALIFIED_METHOD(_uls_inc_lineno)(uls_lex_ptr_t uls, int delta)
{
	uls_ctx_inc_lineno(uls->xcontext.context, delta);
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_space)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	return uls_canbe_ch_space(uls->ch_context, wch);
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_idfirst)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	return uls_canbe_ch_idfirst(uls->ch_context, wch);
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_id)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	return uls_canbe_ch_id(uls->ch_context, wch);
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_quote)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	return uls_canbe_ch_quote(uls->ch_context, wch);
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_1ch_token)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	int stat;
	if (uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), wch, nilptr) != nilptr) {
		stat = 1;
	} else {
		stat = 0;
	}
	return stat;
}

int
ULS_QUALIFIED_METHOD(_uls_is_wch_2ch_token)(uls_lex_ptr_t uls, uls_wch_t wch)
{
	return uls_canbe_ch_2ch_token(uls->ch_context, wch);
}

int
ULS_QUALIFIED_METHOD(_uls_tok_id)(uls_lex_ptr_t uls)
{
	return __uls_tok(uls);
}

const char*
ULS_QUALIFIED_METHOD(_uls_lexeme)(uls_lex_ptr_t uls)
{
	return __uls_lexeme(uls);
}

int
ULS_QUALIFIED_METHOD(_uls_lexeme_len)(uls_lex_ptr_t uls)
{
	return __uls_lexeme_len(uls);
}

int
ULS_QUALIFIED_METHOD(_uls_lexeme_wlen)(uls_lex_ptr_t uls)
{
	return __uls_lexeme_wlen(uls);
}

const char*
ULS_QUALIFIED_METHOD(_uls_get_tag)(uls_lex_ptr_t uls)
{
	return __uls_get_tag(uls);
}

int
ULS_QUALIFIED_METHOD(_uls_get_taglen)(uls_lex_ptr_t uls)
{
	return __uls_get_taglen(uls);
}

const char*
ULS_QUALIFIED_METHOD(_uls_get_tag2)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	const char *tagstr = uls_ctx_get_tag(ctx);

	if (parms != nilptr) {
		parms->lptr = tagstr;
		parms->len = uls_ctx_get_taglen(ctx);
	}

	return tagstr;
}

void
ULS_QUALIFIED_METHOD(uls_set_tag)(uls_lex_ptr_t uls, const char *tag, int lno)
{
	uls_ctx_set_tag(uls->xcontext.context, tag, lno);
}

const char*
ULS_QUALIFIED_METHOD(_uls_number_suffix)(uls_lex_ptr_t uls, uls_ptrtype_tool(outparam) parms)
{
	const char *ptr;

	parms->lptr = ptr = uls_number_suffix(uls);
	parms->len = _uls_tool_(strlen)(ptr);

	return ptr;
}

ULS_QUALIFIED_RETTYP(uls_lex_ptr_t)
ULS_QUALIFIED_METHOD(ulsjava_create)(uls_native_vptr_t confname, int len_confname)
{
	const char *ustr = _uls_tool_(strdup)((const char *)confname, len_confname);
	uls_lex_ptr_t  uls;

	uls = uls_create(ustr);
	uls_mfree(ustr);

	return uls;
}

void
ULS_QUALIFIED_METHOD(ulsjava_dump_tok)(uls_lex_ptr_t uls, const void *pfx, int len_pfx, const void *suff, int len_suff)
{
	const char *ustr1 = _uls_tool_(strdup)((const char *)pfx, len_pfx);
	const char *ustr2 = _uls_tool_(strdup)((const char *)suff, len_suff);

	uls_dump_tok(uls, ustr1, ustr2);

	uls_mfree(ustr1);
	uls_mfree(ustr2);
}

void
ULS_QUALIFIED_METHOD(ulsjava_set_tag)(uls_lex_ptr_t uls, const void *tag, int len_tag, int lineno)
{
	const char *ustr = _uls_tool_(strdup)((const char *)tag, len_tag);

	uls_set_tag(uls, ustr, lineno);
	uls_mfree(ustr);
}

#ifndef ULS_DOTNET

const char*
ULS_QUALIFIED_METHOD(uls_get_tag2)(uls_lex_ptr_t uls, int* ptr_n_bytes)
{
	const char *tagstr;
	uls_type_tool(outparam) parms;

	tagstr = _uls_get_tag2(uls, uls_ptr(parms));
	if (ptr_n_bytes != NULL) {
		*ptr_n_bytes = parms.len;
	}

	return tagstr;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_eoftag)(uls_lex_ptr_t uls, int *ptr_len_tag)
{
	uls_type_tool(outparam) parms1;
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_EOF)
		return NULL;

	ptr = __uls_eof_tag(__uls_lexeme(uls), uls_ptr(parms1));
	if (ptr_len_tag != NULL) {
		*ptr_len_tag = parms1.len;
	}

	return ptr;
}

int
ulsjava_get_tokid_list(uls_lex_ptr_t uls, int **ptr_outbuf)
{
	uls_type_tool(outparam) parms;
	int n;

	n = _uls_get_tokid_list(uls, uls_ptr(parms));
	*ptr_outbuf = parms.native_data;

	return n;
}

void
ulsjava_put_tokid_list(uls_lex_ptr_t uls, int **ptr_outbuf)
{
	uls_type_tool(outparam) parms;

	parms.native_data = *ptr_outbuf;
	_uls_put_tokid_list(uls_ptr(parms));
}

#endif
