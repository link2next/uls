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
#define __ULS_CORE__
#include "uls/uls_core.h"
#include "uls/uls_freq.h"
#include "uls/uld_conf.h"
#include "uls/uls_num.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC _ULS_INLINE void
__ready_to_use_lexseg(uls_context_ptr_t ctx)
{
	uls_lexseg_ptr_t lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->n_lexsegs);

	lexseg->offset2 = -1;
	lexseg->len_text = -1;
	lexseg->n_lfs_raw = 0;

	lexseg->tokdef_vx = nilptr;
	ctx->i_lexsegs = 0;
}

ULS_DECL_STATIC int
find_prefix_radix(uls_outparam_ptr_t parms, uls_lex_ptr_t uls, const char *str)
{
	uls_number_prefix_ptr_t numpfx;
	int i, radix = -1;

	for (i=0; i<uls->n_numcnst_prefixes; i++) {
		numpfx = uls_array_get_slot_type00(uls_ptr(uls->numcnst_prefixes), i);

		if (!uls_strncmp(str, numpfx->prefix, numpfx->l_prefix)) {
			parms->n1 = numpfx->l_prefix;
			parms->n2 = radix = numpfx->radix;
			break;
		}
	}

	return radix;
}

ULS_DECL_STATIC int
get_number(uls_lex_ptr_t uls, uls_context_ptr_t ctx, uls_parm_line_ptr_t parm_ln)
{
	char *cnst_suffixes = uls->numcnst_suffixes;
	uls_uch_t numsep = uls->numcnst_separator;
	uls_outbuf_ptr_t tokbuf = uls_ptr(ctx->tokbuf);
	const char *cnst_suffix, *lptr =  parm_ln->lptr;
	uls_outparam_t parms1, parms2;
	int k0, k=0, l_cnst_suffix;
	char ch;

	parms1.flags = 0;
	parms1.uch = numsep;

 	if (*lptr == '-') {
		parms1.flags |= ULS_NUM_FL_MINUS;
		str_putc(tokbuf, k++, '-');
		++lptr;
	}

	k0 = k;
	parms1.lptr = lptr;

	if ((ch=lptr[0]) == '0' && lptr[1] != '.') {
		if (find_prefix_radix(uls_ptr(parms2), uls, lptr) < 0) {
			// set tokbuf to zero
			str_putc(tokbuf, 0, '0');
			str_putc(tokbuf, 1, '\0');
			k = 1;

			parms1.flags |= ULS_NUM_FL_ZERO;
			parms1.flags &= ~ULS_NUM_FL_MINUS;
			parms1.n = parms1.len = 1;
			parms1.lptr = ++lptr;
		} else {
			parms1.n1 = parms2.n1;
			parms1.n2 = parms2.n2; // radix
			k = num2stdfmt_0(uls_ptr(parms1), tokbuf, k0);
		}
	} else if (uls_isdigit(ch) || ch == '.') {
		k = num2stdfmt(uls_ptr(parms1), tokbuf, k0);
	} else {
		k = -1;
	}

	if (k <= 0) {
		return -1; // not number
	}

	lptr = parms1.lptr;
	ctx->expo = parms1.n;
	ctx->n_digits = parms1.len;

	if ((ch=*lptr) != '\0' && !uls_isspace(ch)) {
		if ((cnst_suffix=is_cnst_suffix_contained(cnst_suffixes, lptr, -1, nilptr)) != NULL) {
			l_cnst_suffix = uls_strlen(cnst_suffix);
			str_putc(tokbuf, k++, ' ');
			k = str_append(tokbuf, k, cnst_suffix, l_cnst_suffix);
			lptr += l_cnst_suffix;
		}
	}

	parm_ln->lptr = lptr;
	return k;
}

ULS_DECL_STATIC void
make_eof_lexeme(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	int lno, k;
	char *tagstr;

	lno = __uls_get_lineno(uls);
	tagstr = csz_text(uls_ptr(ctx->tag));
	k = __uls_make_eoftok_lexeme(uls_ptr(ctx->tokbuf), lno, tagstr);

	ctx->tok = uls->xcontext.toknum_EOF;
	uls->tokdef_vx = slots_rsv[EOF_TOK_IDX];

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = k;
	ctx->s_val_uchars = ustr_num_chars(ctx->s_val, k, nilptr);
}

ULS_DECL_STATIC uls_context_ptr_t 
make_eoi_lexeme(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));

	// EOF or EOI
	ctx->tok = uls->xcontext.toknum_EOI;
	uls->tokdef_vx = slots_rsv[EOI_TOK_IDX];

	str_putc(uls_ptr(ctx->tokbuf), 0, '\0');
	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = ctx->s_val_uchars = 0;

	return ctx;
}

ULS_DECL_STATIC _ULS_INLINE int
__uls_is_real(const char *ptr)
{
	if (*ptr == '-') ++ptr;
	if (*ptr == '.') return 1;
	return 0;
}

ULS_DECL_STATIC _ULS_INLINE double
__uls_lexeme_unsigned_double(const char *ptr)
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
__uls_lexeme_uint32(const char *ptr)
{
	uls_uint32 u32_val;
	char ch;
	int i;

	if (__uls_is_real(ptr)) {
		u32_val = (uls_uint32) __uls_lexeme_unsigned_double(ptr);
	} else {
		u32_val = 0;

		for (i=0; (ch=*ptr) != '\0' && uls_isxdigit(ch); i++, ptr++) {
			if (i >= sizeof(uls_uint32) * 2) {
				err_log("The number %s is too big for 32-bits integer.", ptr);
				err_log("Anyway trucating it, ...");
				break;
			}

			u32_val = (u32_val << 4) + (uls_isdigit(ch) ? ch - '0' : 10 + (ch - 'A'));
		}
	}

	return u32_val;
}

ULS_DECL_STATIC uls_uint64
__uls_lexeme_uint64(const char *ptr)
{
	uls_uint64 u64_val;
	char ch;
	int i;

	if (__uls_is_real(ptr)) {
		u64_val = (uls_uint64) __uls_lexeme_unsigned_double(ptr);

	} else {
		u64_val = 0;

		for (i=0; (ch=*ptr) != '\0' && uls_isxdigit(ch); i++, ptr++) {
			if (i >= sizeof(uls_uint64) * 2) {
				err_log("The number %s is too big for 64-bits integer.", ptr);
				err_log("Truncating it, ...");
				break;
			}

			u64_val = (u64_val << 4) + (uls_isdigit(ch) ? ch - '0' : 10 + (ch - 'A'));
		}
	}

	return u64_val;
}

ULS_DECL_STATIC int
__uls_change_line(uls_lex_ptr_t uls, const char* line, int len, int flags)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	int start_lno=1;
	uls_outparam_ptr_t parm;
	char *ptr;

	if (flags & ULS_WANT_EOFTOK) uls_want_eof(uls);
	else uls_unwant_eof(uls);

	uls_input_change_filler_null(inp);

	if (xctx->len_prepended_input > 0) {
		parm = uls_alloc_object(uls_outparam_t);

		ptr = uls_malloc_buffer(xctx->len_prepended_input + len + 1);
		uls_memcopy(ptr, xctx->prepended_input, xctx->len_prepended_input);
		uls_memcopy(ptr + xctx->len_prepended_input, line, len);
		if (flags & ULS_MEMFREE_LINE) line = uls_mfree((char *) line);

		parm->lptr = line = ptr;
		parm->len = len = xctx->len_prepended_input + len;
		ptr[len] = '\0';

		inp->isource.usrc = (uls_voidptr_t) parm;
		inp->isource.usrc_ungrab = uls_ref_callback(uls_ungrab_linecheck);

		start_lno -= xctx->lfs_prepended_input;

	} else if (flags & (ULS_DO_DUP|ULS_MEMFREE_LINE)) {
		parm = uls_alloc_object(uls_outparam_t);

		if (flags & ULS_DO_DUP) {
			line = uls_strdup(line, len);
		}

		parm->lptr = line;
		parm->len = len;

		inp->isource.usrc = (uls_voidptr_t) parm;
		inp->isource.usrc_ungrab = uls_ref_callback(uls_ungrab_linecheck);

	} else {
		uls_input_set_fl(inp, ULS_INP_FL_READONLY);
	}

	uls_input_reset(inp, 0, ULS_INP_FL_REFILL_NULL);

	inp->line_num = start_lno;
	inp->rawbuf_ptr = line;
	inp->rawbuf_bytes = len;
	inp->isource.flags |= ULS_ISRC_FL_EOF;

	ctx->line = ctx->line_end = inp->rawbuf_ptr;
	ctx->lptr = ctx->line;
	ctx->lineno = start_lno;

	return 0;
}

const char*
skip_white_spaces(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	const char  *lptr, *ch_ctx = uls->ch_context;
	char ch;

	for (lptr = ctx->lptr; (ch=*lptr) != '\0'; lptr++) {
		if (ch >= ULS_SYNTAX_TABLE_SIZE || ch_ctx[ch] != 0) {
			break;
		}
		if (ch == '\n') {
			uls_context_inc_lineno(ctx, 1);
		}
	}

	ctx->lptr = lptr;
	return lptr;
}

void
free_tokdef_array(uls_lex_ptr_t uls)
{
	uls_tokdef_ptr_t e;
	uls_tokdef_vx_ptr_t e_vx;

	uls_decl_parray_slots(slots_tok, tokdef);
	uls_decl_parray_slots(slots_vx, tokdef_vx);
	int i;

	slots_tok = uls_parray_slots(uls_ptr(uls->tokdef_array));
	for (i=0; i<uls->tokdef_array.n; i++) {
		e = slots_tok[i];
		uls_destroy_tokdef(e);
	}
	uls_deinit_parray(uls_ptr(uls->tokdef_array));

	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));
	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		e_vx->base = nilptr;
		uls_destroy_tokdef_vx(e_vx);
	}

	uls_deinit_parray(uls_ptr(uls->tokdef_vx_array));
	uls_deinit_parray(uls_ptr(uls->tokdef_vx_rsvd));
}

int
ulc_load(uls_lex_ptr_t uls, FILE *fin_ulc, FILE *fin_ulf)
{
	ulc_header_t  uls_config;
	fp_list_ptr_t  ulc_fp_stack;
	char     linebuff[ULS_LINEBUFF_SIZ__ULC+1], *lptr;
	char     *tagstr;
	int      linelen, lno;
	int      i, rc, stat = 0;
	int      n, n_idtok_list, tok_id;

	uls_tokdef_ptr_t e, e_link, idtok_list;
	uls_tokdef_vx_ptr_t e_vx;

	uls_keyw_stat_list_ptr_t keyw_stat_list;
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	int n_lst;

	ulf_header_t ulf_hdr;
	uls_outparam_t parms, parms2;

	uls_version_make(uls_ptr(uls_config.filever), 0, 0, 0);

	uls_config.n_keys_twoplus = uls_config.n_keys_idstr = 0;
	uls_config.tok_id_seed = 0;
	uls_config.tok_id_min = uls_config.tok_id_max = 0;

	n = N_RESERVED_TOKS + TOKDEF_LINES_DELTA;
	uls_resize_parray(uls_ptr(uls->tokdef_array), tokdef, n);
	uls->tokdef_array.n = 0;

	uls_resize_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, n);
	uls->tokdef_vx_array.n = 0;

	init_reserved_toks(uls);

	ulc_fp_stack = ulc_fp_push(nilptr, fin_ulc, uls->ulc_name);

	parms.data = ulc_fp_stack;
	rc = ulc_read_header(uls, fin_ulc, uls_ptr(uls_config), uls_ptr(parms));
	ulc_fp_stack = (fp_list_ptr_t) parms.data;

	if (rc < 0) {
		err_log("fail to read the header of uls-spec.");
		release_ulc_fp_stack(ulc_fp_stack);
		uls_fp_close(fin_ulf);
		return -1;
	}

	if (classify_char_group(uls, uls_ptr(uls_config)) < 0) {
		err_log("%s: lex-conf file not consistent!", __func__);
		release_ulc_fp_stack(ulc_fp_stack);
		uls_fp_close(fin_ulf);
		return -1;
	}

	parms.data = ulc_fp_stack;
	fin_ulc = ulc_fp_pop(uls_ptr(parms), 1);
	ulc_fp_stack = (fp_list_ptr_t) parms.data;
	tagstr = parms.line;
	lno = parms.n;

	while (1) {
		if ((linelen=uls_fp_gets(fin_ulc, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			uls_mfree(tagstr);
			if (linelen < ULS_EOF) {
				err_log("%s: ulc file i/o error at %d", __func__, lno);
				stat = -1;
				break;
			}

			uls_fp_close(fin_ulc);
			if (ulc_fp_stack == nilptr) break;

			parms.data = ulc_fp_stack;
			fin_ulc = ulc_fp_pop(uls_ptr(parms), 1);
			ulc_fp_stack = (fp_list_ptr_t) parms.data;
			tagstr = parms.line;
			lno = parms.n;
			continue;
		}
		++lno;

		if (*(lptr = skip_blanks(linebuff)) == '\0' ||
			*lptr == '#' || (lptr[0]=='/' && lptr[1]=='/'))
			continue;

		if ((e_vx=ulc_proc_line(uls->ulc_name,
			lno, lptr, uls, uls_ptr(uls_config), uls_ptr(parms2))) == nilptr) {
			stat = -1;
			break;
		}

		e = (uls_tokdef_ptr_t) parms2.data;

		tok_id = e_vx->tok_id;

		if (tok_id < uls_config.tok_id_min)
			uls_config.tok_id_min = tok_id;

		if (tok_id > uls_config.tok_id_max)
			uls_config.tok_id_max = tok_id;
	}

	if (stat < 0 || check_rsvd_toks(uls) < 0) {
		release_ulc_fp_stack(ulc_fp_stack);
		free_tokdef_array(uls);
		uls_fp_close(fin_ulf);
		return -1;
	}

	// uls.commtypes, uls.quotetypes is used as matching the prefices of comments, quote-strings
	//     as the defined order

	uls->xcontext.len_surplus = calc_len_surplus_recommended(uls);

	if ((i=uls->tokdef_array.n) < uls->tokdef_array.n_alloc) {
		// shrink the size of uls->tokdef_array to the compact size.
		uls_resize_parray(uls_ptr(uls->tokdef_array), tokdef, i);
	}

	if ((i=uls->tokdef_vx_array.n) < uls->tokdef_vx_array.n_alloc) {
		// shrink the size of uls->tokdef_vx_array to the compact size.
		uls_resize_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, i);
	}

	if (classify_tok_group(uls) < 0) {
		err_log("%s: lex-conf file not consistent!", __func__);
		uls_fp_close(fin_ulf);
		return -1;
	}

	idtok_list = rearrange_tokname_of_quotetypes(uls, uls_config.n_keys_twoplus, uls_ptr(parms));
	n_idtok_list = parms.n;

	uls_init_kwtable(uls_ptr(uls->idkeyw_table), 0, uls->flags & ULS_FL_CASE_INSENSITIVE);

	if (fin_ulf != NULL &&
		(keyw_stat_list=ulf_load(idtok_list, n_idtok_list, fin_ulf, uls_ptr(ulf_hdr), uls_ptr(uls->idkeyw_table))) != nilptr) {
		lst = uls_ptr(keyw_stat_list->lst);
		n_lst = keyw_stat_list->lst.n;

		slots_lst = uls_parray_slots(lst);
		_uls_quicksort_vptr(slots_lst, n_lst, keyw_stat_comp_by_freq);

		for (i=0; i < n_lst; i++) {
			e = slots_lst[i]->keyw_info;
			uls_add_kw(uls_ptr(uls->idkeyw_table), e);
		}

		ulc_free_kwstat_list(keyw_stat_list);
		keyw_stat_list = nilptr;

	} else {
		ulf_init_header(uls_ptr(ulf_hdr));
		uls_reset_kwtable(uls_ptr(uls->idkeyw_table), ULF_HASH_TABLE_SIZE,
			nilptr, uls_ptr(uls->idkeyw_table.dflhash_stat));

		// consume tok_info_lst upto n_lst
		for (e=idtok_list; e!=nilptr; e=e_link) {
			e_link = e->link;
			uls_add_kw(uls_ptr(uls->idkeyw_table), e);
		}
	}

	ulf_deinit_header(uls_ptr(ulf_hdr));
	uls_fp_close(fin_ulf);

	distribute_2char_toks(uls_ptr(uls->twoplus_table), uls->idkeyw_table.str_ncmp);

	return stat;
}

int
uls_init_fp(uls_lex_ptr_t uls, const char *specname,
	FILE *fin_ulc, FILE *fin_ulf)
{
	uls_context_ptr_t ctx;
	int  ch, rc;
	char *ch_ctx;

	uls_set_namebuf_value(uls->ulc_name, specname);
	uls->ref_cnt = 0;

	uls_version_make(uls_ptr(uls->ulc_ver), 0, 0, 0);
	uls_version_make(uls_ptr(uls->config_filever),
		ULC_VERSION_MAJOR, ULC_VERSION_MINOR, ULC_VERSION_DEBUG);
	uls_version_make(uls_ptr(uls->stream_filever),
		ULS_VERSION_STREAM_MAJOR, ULS_VERSION_STREAM_MINOR,
		ULS_VERSION_STREAM_DEBUG);

	ch_ctx = uls->ch_context;
	for (ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (uls_isdigit(ch)) {
			ch_ctx[ch] = ULS_CH_DIGIT;
		} else if (uls_isgraph(ch)) {
			ch_ctx[ch] = ULS_CH_1;
		} else { // ctrl-chars
			ch_ctx[ch] = 0;
		}
	}

	ch_ctx['-'] |= ULS_CH_DIGIT;
	ch_ctx['.'] |= ULS_CH_DIGIT;

	uls->id_max_bytes = uls->id_max_uchars = INT_MAX;

	uls_init_parray(uls_ptr(uls->tokdef_vx_array), tokdef_vx, 0);
	uls_init_parray(uls_ptr(uls->tokdef_vx_rsvd), tokdef_vx, 0);

	uls_init_parray(uls_ptr(uls->tokdef_array), tokdef, 0);
	uls->tokdef_vx = nilptr;

	__init_kwtable(uls_ptr(uls->idkeyw_table));
	uls_init_1char_table(uls_ptr(uls->onechar_table));
	uls_init_2char_table(uls_ptr(uls->twoplus_table));

	uls_init_escmap_pool(uls_ptr(uls->escstr_pool));

	uls_xcontext_init(uls_ptr(uls->xcontext), uls_ref_callback(uls_gettok_raw));
	uls->xcontext.context->flags |= ULS_CTX_FL_EOF | ULS_CTX_FL_GETTOK_RAW;

	if ((rc = ulc_load(uls, fin_ulc, fin_ulf)) < 0) {
		// fin_ulc, fin_ulf consumed
		uls_xcontext_deinit(uls_ptr(uls->xcontext));
		uls_deinit_1char_table(uls_ptr(uls->onechar_table));
		uls_deinit_2char_table(uls_ptr(uls->twoplus_table));
		return -1;
	}

	uls->xcontext.ch_context = uls->ch_context;

	uls->xcontext.commtypes = uls_ptr(uls->commtypes);
	uls->xcontext.n2_commtypes = uls->n1_commtypes;

	uls->xcontext.quotetypes = uls_ptr(uls->quotetypes);

	ctx = uls->xcontext.context;
	ctx->tok = uls->xcontext.toknum_EOI;

	ctx->tokbuf.buf[0] = '\0';
	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = ctx->s_val_uchars = 0;

	return 0;
}

void
uls_dealloc_lex(uls_lex_ptr_t uls)
{
	uls_quotetype_ptr_t qmt;
	uls_decl_parray_slots(slots_qmt, quotetype);
	uls_number_prefix_ptr_t numpfx;
	int i;

	uls_xcontext_deinit(uls_ptr(uls->xcontext));

	uls_deinit_escmap_pool(uls_ptr(uls->escstr_pool));

	uls_deinit_2char_table(uls_ptr(uls->twoplus_table));
	uls_deinit_1char_table(uls_ptr(uls->onechar_table));
	uls_deinit_kwtable(uls_ptr(uls->idkeyw_table));

	free_tokdef_array(uls);

	// idfirst_charsets
	uls_deinit_array_type01(uls_ptr(uls->idfirst_charset), uch_range);

	// id_charsets
	uls_deinit_array_type01(uls_ptr(uls->id_charset), uch_range);

	// commtypes
	uls->n1_commtypes = 0;
	uls_deinit_array_type01a(uls_ptr(uls->commtypes), commtype);

	// quotetypes
	slots_qmt = uls_parray_slots(uls_ptr(uls->quotetypes));
	for (i=0; i<uls->quotetypes.n; i++) {
		qmt = slots_qmt[i];
		uls_destroy_quotetype(qmt);
	}

	uls_deinit_parray(uls_ptr(uls->quotetypes));

	uls_deinit_namebuf(uls->ulc_name);
	uls_deinit_bytespool(uls->ch_context);

	for (i=0; i<uls->n_numcnst_prefixes; i++) {
		numpfx = uls_array_get_slot_type00(uls_ptr(uls->numcnst_prefixes), i);
		uls_deinit_namebuf(numpfx->prefix);
		numpfx->l_prefix = 0;
		numpfx->radix = 0;
	}
	uls_deinit_array_type00(uls_ptr(uls->numcnst_prefixes), number_prefix);
	uls->n_numcnst_prefixes = 0;

	uls_deinit_bytespool(uls->numcnst_suffixes);

	if (uls->flags & ULS_FL_STATIC) {
		uls->onechar_table.tokdefs_etc_list = nilptr;
	} else {
		uls_dealloc_object(uls);
	}
}

int
uls_spec_compatible(uls_lex_ptr_t uls, const char* specname, uls_version_ptr_t filever)
{
	int  stat=1;
	char  ver_str1[ULS_VERSION_STR_MAXLEN+1];
	char  ver_str2[ULS_VERSION_STR_MAXLEN+1];

	if (!uls_streql(uls->ulc_name, specname) ||
		!uls_vers_compatible(uls_ptr(uls->stream_filever), filever)) {
		uls_version_make_string(uls_ptr(uls->stream_filever), ver_str1);
		uls_version_make_string(filever, ver_str2);
		err_log("%s: Unsupported or not compatible:", __func__);
		err_log("\t'%s'(%s)", uls->ulc_name, ver_str1);
		err_log("\t'%s'(%s)", specname, ver_str2);
		stat = 0;
	}

	return stat;
}

int
uls_fillbuff(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	const char *line;
	int rc;

	if ((ctx->flags & ULS_CTX_FL_ERR) || inp->isource.flags & ULS_ISRC_FL_ERR) {
		err_log("%s: called again after I/O failed!", __func__);
		return -1;
	} else if (ctx->flags & ULS_CTX_FL_EOF) {
		return 0;
	}

	rc = ctx->fill_proc(uls_ptr(uls->xcontext));
	if (rc < 0 || (rc == 0 && ctx->n_lexsegs == 0)) {
		if (rc < 0) {
			uls->tokdef_vx = set_err_tok(uls, "failed to fill buffer!");
		}

		ctx->lptr = ctx->line = ctx->cnst_nilstr;
		ctx->line_end = ctx->line;

		return rc;
	}

	ctx->lptr = ctx->line = line =csz_text(uls_ptr(ctx->zbuf1));
	if ((ctx->line_end=line+rc) < line) {
		err_panic("%s: invalid string length, %d.", __func__, rc);
		return -1;
	}

	__ready_to_use_lexseg(ctx);

	return 1;
}

int
uls_clear_and_fillbuff(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	csz_reset(uls_ptr(ctx->zbuf1));
	csz_reset(uls_ptr(ctx->zbuf2));

	return uls_fillbuff(uls);
}

int
uls_fillbuff_and_reset(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx;
	int rc;

	if ((rc=uls_clear_and_fillbuff(uls)) > 0) {
		ctx = uls->xcontext.context;
		ctx->tok = uls->xcontext.toknum_NONE;
		ctx->tokbuf.buf[0] = '\0';
		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = 0;
	}

	return rc;
}

const char*
__uls_eof_tag(const char *ptr, uls_outparam_ptr_t parms)
{
	parms->lptr = ptr;
	uls_skip_atou(parms); // linenum

	++parms->lptr;
	parms->len = uls_skip_atou(parms); // the length of eof-tag

	return ++parms->lptr;
}

int
__uls_make_eoftok_lexeme(uls_outbuf_ptr_t tokbuf, int lno, const char *tagstr)
{
	int k, rc;

	if (tagstr == NULL) {
		tagstr = ""; rc = 0;
	} else {
		rc = uls_strlen(tagstr);
	}

	k = ((1+N_MAX_DIGITS_INT)<<1) + 2 + rc; // 1+ == '-'

	str_append(tokbuf, 0, NULL, k);
	k = uls_snprintf(tokbuf->buf, k+1, "%d %d", lno, rc);

	if (rc > 0) {
		str_putc(tokbuf, k++, ' ');
		k = str_append(tokbuf, k, tagstr, rc);
	}

	return k;
}

uls_tokdef_vx_ptr_t
set_err_tok(uls_lex_ptr_t uls, const char* errmsg)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));
	const char *lptr;
	char numbuf[32];
	int k, k1;

	ctx->tok = uls->xcontext.toknum_ERR;

	lptr = __uls_get_tag(uls);
	k = k1 = str_puts(uls_ptr(ctx->tokbuf), 0, lptr);
	ctx->s_val_uchars = ustr_num_chars(lptr, k, nilptr);

	uls_snprintf(numbuf, sizeof(numbuf), "<%d> ", __uls_get_lineno(uls));
	k = str_puts(uls_ptr(ctx->tokbuf), k, numbuf);
	k = str_puts(uls_ptr(ctx->tokbuf), k, errmsg);

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = k;
	ctx->s_val_uchars += k - k1;
	ctx->flags |= ULS_CTX_FL_ERR;

	return slots_rsv[ERR_TOK_IDX];
}


ULS_DECL_STATIC uls_tokdef_vx_ptr_t
__uls_onechar_lexeme(uls_lex_ptr_t uls, uls_uch_t uch, const char *lptr, int len)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = uls_find_1char_tokdef_vx(uls_ptr(uls->onechar_table), uch, nilptr)) == nilptr) {
		if (uch < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[uch] & ULS_CH_1)) {
			e_vx = ctx->anonymous_uchar_vx;
			e_vx->tok_id = (int) uch;
		} else {
			e_vx = set_err_tok(uls, "Unknown unicode char!");
			return e_vx;
		}
	}

	ctx->tok = e_vx->tok_id;
	str_append(uls_ptr(ctx->tokbuf), 0, lptr, len);

	ctx->s_val = ctx->tokbuf.buf;
	ctx->s_val_len = len;
	ctx->s_val_uchars = 1;

	return e_vx;
}
int
uls_gettok_raw(uls_lex_ptr_t uls)
{
	uls_context_ptr_t  ctx = uls->xcontext.context;
	const char      *ch_ctx = uls->ch_context;
	uls_decl_parray_slots_init(slots_rsv, tokdef_vx, uls_ptr(uls->tokdef_vx_rsvd));

	const char      *lptr;
	char            ch, ch_grp;
	uls_lexseg_ptr_t  lexseg, lexseg_next;
	int             j, k, rc0, rc, n_uchars;
	uls_uch_t       uch;
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	uls_parm_line_t parm_ln;

	if (ctx->delta_lineno != 0) {
		ctx->lineno += ctx->delta_lineno;
		ctx->delta_lineno = 0;
	}

 next_loop:
	lptr = skip_white_spaces(uls);
	if (uls_decode_utf8(lptr, -1, &uch) <= 0) {
		e_vx = set_err_tok(uls, "Incorrect utf-8 format!");
		uls->tokdef_vx = e_vx;
		ctx->lptr = lptr;
		return 0;
	}

	if ((ch=*lptr) < ULS_SYNTAX_TABLE_SIZE) {
		ch_grp = ch_ctx[ch];
	} else {
		ch_grp = 0; // N/A
	}

	if (ch == '\0') {
		if (ctx->i_lexsegs >= ctx->n_lexsegs) {
			if ((rc=uls_clear_and_fillbuff(uls)) < 0) {
				return 0;

			} else if (rc > 0) {
				// actually, rc == 1, means than it's been actually filled from IO-buffer.
				goto next_loop; // normal case
			}

			// EOF
			make_eof_lexeme(uls);
			return 1;
		}

		// QUOTE-STRING
		lexseg = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ctx->i_lexsegs);
		lexseg_next = uls_array_get_slot_type10(uls_ptr(ctx->lexsegs), ++ctx->i_lexsegs);

		ctx->line = ctx->lptr = csz_data_ptr(uls_ptr(ctx->zbuf1)) + lexseg_next->offset1;
		ctx->line_end = ctx->line + lexseg_next->len1;
		lptr = ctx->lptr;

		if (lexseg->offset2 < 0) {
			goto next_loop;
		}
		e_vx = lexseg->tokdef_vx;

		// lexseg: offset1~'\0'(len1) and offset2~len_text
		ctx->flags |= ULS_CTX_FL_QTOK;
		ctx->s_val = csz_data_ptr(uls_ptr(ctx->zbuf2)) + lexseg->offset2;
		ctx->flags |= ULS_CTX_FL_EXTERN_TOKBUF;

		ctx->s_val_len = lexseg->len_text;
		ctx->s_val_uchars = ustr_num_chars(ctx->s_val, ctx->s_val_len, nilptr);
		ctx->tok = e_vx->tok_id;

		ctx->delta_lineno = lexseg->n_lfs_raw;

	} else if ((ch_grp & ULS_CH_DIGIT) &&
		(parm_ln.lptr=lptr, k=get_number(uls, ctx, uls_ptr(parm_ln)), lptr=parm_ln.lptr, k > 0)) {
		ctx->tok = uls->xcontext.toknum_NUMBER;
		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = ctx->s_val_uchars = k;
		e_vx = slots_rsv[NUM_TOK_IDX];

	} else if ((rc0=uls_is_char_idfirst(uls, lptr, &uch)) > 0) {
		k = 0;

		for (rc = rc0; rc == 1; ) {
			str_putc(uls_ptr(ctx->tokbuf), k++, *lptr++);
			rc = uls_is_char_id(uls, lptr, &uch);
		}
		str_putc(uls_ptr(ctx->tokbuf), k, '\0');

		if (k > 0 && (e=is_keyword_idstr(uls_ptr(uls->idkeyw_table), ctx->tokbuf.buf, k)) != nilptr) {
			e_vx = e->view;
			ctx->tok = e_vx->tok_id;
			ctx->s_val = ctx->tokbuf.buf;
			ctx->s_val_len = ctx->s_val_uchars = k;

		} else {
			n_uchars = k;
			while (rc >= 1) {
				for (j=0; j<rc; j++) {
					str_putc(uls_ptr(ctx->tokbuf), k++, *lptr++);
				}
				++n_uchars;
				rc = uls_is_char_id(uls, lptr, &uch);
			}

			if (n_uchars > uls->id_max_uchars || k > uls->id_max_bytes ) {
				e_vx = set_err_tok(uls, "Too long identifier!");
			} else {
				str_putc(uls_ptr(ctx->tokbuf), k, '\0');
				ctx->s_val = ctx->tokbuf.buf;
				ctx->s_val_len = k;
				ctx->s_val_uchars = n_uchars;
				ctx->tok = uls->xcontext.toknum_ID;
				e_vx = slots_rsv[ID_TOK_IDX];
			}
		}

	} else if (((ch_grp & ULS_CH_2PLUS) || ch >= ULS_SYNTAX_TABLE_SIZE) &&
		(e=is_keyword_twoplus(uls_ptr(uls->twoplus_table), ch_ctx, lptr)) != nilptr) {
		/* FOUND */
		e_vx = e->view;
		ctx->tok = e_vx->tok_id;

		rc = e->l_keyword;

		str_append(uls_ptr(ctx->tokbuf), 0, lptr, rc);
		ctx->tokbuf.buf[rc] = '\0';

		ctx->s_val = ctx->tokbuf.buf;
		ctx->s_val_len = ctx->s_val_uchars = rc;
		lptr += rc;

	} else if (rc0 < -1) {
		rc = -rc0;
		e_vx = __uls_onechar_lexeme(uls, uch, lptr, rc);
		lptr += rc;

	} else if (ch_grp & ULS_CH_1) {
		if (ch == '\n') {
			if (ctx->lineno <= 0 && (uls->xcontext.flags & ULS_XCTX_FL_IGNORE_LF)) {
				++ctx->lineno;
				ctx->lptr = ++lptr;
				goto next_loop;
			}
			ctx->delta_lineno =  1;
		}

		e_vx = __uls_onechar_lexeme(uls, ch, lptr, 1);
		++lptr;

	} else {
		e_vx = set_err_tok(uls, "Unknown char!");
	}

	uls->tokdef_vx = e_vx;
	ctx->lptr = lptr;

	return 0;
}

int
__uls_change_isrc(uls_lex_ptr_t uls, int bufsiz, uls_voidptr_t isrc,
	uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_input_ptr_t   inp = ctx->input;
	int start_lno;

	if (bufsiz < 0) bufsiz = ULS_FDBUF_INITSIZE;

	uls_input_reset(inp, bufsiz, -1);
	uls_input_reset_cursor(inp);
	uls_input_change_filler(inp, isrc, fill_rawbuf, ungrab_proc);

	start_lno = 1;
	if (xctx->len_prepended_input > 0) {
		csz_append(uls_ptr(ctx->zbuf1), xctx->prepended_input, xctx->len_prepended_input);
		start_lno -= xctx->lfs_prepended_input;
	}

	uls_context_set_tag(ctx, NULL, start_lno);

	if (uls_fillbuff_and_reset(uls) < 0) {
		err_log("%s: fail to fill the initial buff", __func__);
		return -1;
	}

	return 0;
}

uls_context_ptr_t
uls_push_context(uls_lex_ptr_t uls, uls_context_ptr_t ctx_new)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	uls_flags_t  mask_want_eof = ctx->flags & ULS_CTX_FL_WANT_EOFTOK;

	if (ctx_new == nilptr) {
		ctx_new = uls_alloc_object(uls_context_t);
		uls_init_context(ctx_new, uls_ref_callback(uls_gettok_raw), uls->xcontext.toknum_NONE);
		uls_input_reset(ctx->input, -1, 0);

		ctx_new->flags |= ULS_CTX_FL_GETTOK_RAW;
		if (mask_want_eof)
			ctx_new->flags |= ULS_CTX_FL_WANT_EOFTOK;
	}

	ctx_new->prev = ctx;
	uls->xcontext.context = ctx_new;

	return ctx_new;
}

const char*
uls_tok2keyw(uls_lex_ptr_t uls, int t)
{
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;
	const char *str;

	if ((e_vx = uls_find_tokdef_vx(uls, t)) == nilptr) {
		if (t >= 0 && t < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[t] & ULS_CH_1)) {
			str = "";
		} else {
			str = NULL;
		}
	} else {
		if ((e = e_vx->base) == nilptr) str = "";
		else str = e->keyword;
	}

	return str;
}

const char*
uls_tok2name(uls_lex_ptr_t uls, int t)
{
	uls_tokdef_vx_ptr_t e_vx;
	const char *str;

	if ((e_vx = uls_find_tokdef_vx(uls, t)) == nilptr) {
		if (t >= 0 && t < ULS_SYNTAX_TABLE_SIZE && (uls->ch_context[t] & ULS_CH_1)) {
			str = "";
		} else {
			str = NULL;
		}
	} else {
		str = e_vx->name;
	}

	return str;
}

const char*
uls_tok2keyw_2(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	const char *str;
	int len;

	if ((str = uls_tok2keyw(uls, t)) != NULL) {
		len = uls_strlen(str);
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
uls_tok2name_2(uls_lex_ptr_t uls, int t, uls_outparam_ptr_t parms)
{
	const char *str;
	int len;

	if ((str = uls_tok2name(uls, t)) != NULL) {
		len = uls_strlen(str);
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
_uls_get_tokid_list(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	char *ch_ctx = uls->ch_context;
	uls_tokdef_vx_ptr_t e_vx;
	int k, n1, n2, ch;
	int *outbuf;

	n1 = uls->tokdef_vx_array.n;
	for (n2=ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if (ch_ctx[ch] & ULS_CH_1) ++n2;
	}

	if ((k=n1+n2) == 0) {
		parms->native_data = NULL;
		return 0;
	}

	outbuf = (int *) uls_malloc(k * sizeof(int));
	for (k=0; k<n1; k++) {
		e_vx = slots_vx[k];
		outbuf[k] = e_vx->tok_id;
	}

	for (ch=0; ch < ULS_SYNTAX_TABLE_SIZE; ch++) {
		if ((ch_ctx[ch] & ULS_CH_1) && !__is_in_ilist(outbuf, n1, ch)) {
			outbuf[k++] = ch;
		}
	}

	parms->native_data = outbuf;
	return k;
}

void
_uls_put_tokid_list(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	int *outbuf = (int *) parms->native_data;
	uls_mfree(outbuf);
}

int
ulsjava_get_tokid_list(uls_lex_t* uls, int **ptr_outbuf)
{
	uls_outparam_t parms;
	int n;

	n = _uls_get_tokid_list(uls, uls_ptr(parms));
	*ptr_outbuf = parms.native_data;

	return n;
}

void
ulsjava_put_tokid_list(uls_lex_t* uls, int **ptr_outbuf)
{
	uls_outparam_t parms;

	parms.native_data = *ptr_outbuf;
	_uls_put_tokid_list(uls, uls_ptr(parms));
}

uls_tokid_simple_list_ptr_t
_uls_get_tokid_list_2(uls_lex_ptr_t uls)
{
	uls_tokid_simple_list_ptr_t lst;
	uls_outparam_t parms;

	lst = uls_alloc_object(uls_tokid_simple_list_t);

	lst->n_tokid_list = _uls_get_tokid_list(uls, uls_ptr(parms));
	lst->tokid_list = (int *) parms.native_data;
	lst->uls = uls;

	return lst;
}

void
_uls_put_tokid_list_2(uls_tokid_simple_list_ptr_t lst)
{
	uls_outparam_t parms;

	parms.native_data = lst->tokid_list;
	_uls_put_tokid_list(lst->uls, uls_ptr(parms));

	lst->tokid_list = nilptr;
	lst->n_tokid_list = 0;

	uls_dealloc_object(lst);
}

int
uls_init(uls_lex_ptr_t uls, const char* confname)
{
	char   specname[ULC_LONGNAME_MAXSIZ+1];
	char   linebuff[ULS_LINEBUFF_SIZ__ULD+1];
	const char* uld_fpath=NULL, *uld_tag;
	FILE   *fin_ulc, *fin_ulf, *fin_uld=NULL;
	int    rc, len_basedir, len_specname, typ_fpath;
	uls_outparam_t parms1;

	if (uls == nilptr || confname == NULL) {
		err_log("%s: invalid parameter!", __func__);
		return -1;
	}

	parms1.line = specname;
	typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
	len_basedir = parms1.n;
	len_specname = parms1.len;
	if (typ_fpath < 0) {
//		err_log("%s: invalid ulc name", confname);
		return -1;
	}

	if (typ_fpath == ULS_NAME_FILEPATH_ULD) {
		if ((fin_uld = ulc_search_for_fp(ULS_NAME_FILEPATH_ULD, confname, nilptr)) == NULL) {
			err_log("can't open file '%s'", confname);
			return -1;
		}
		uld_fpath = confname;

		rc =uls_fp_gets(fin_uld, linebuff, sizeof(linebuff), 0);

		if ( rc < 3 || !(linebuff[0] == '#' && linebuff[1] == '@')) {
			err_log("Invalid spec-name in %s.", confname);
			uls_fp_close(fin_uld);
			return -1;
		}

		// Get the base specname in the line.
		rc = str_trim_end(linebuff, rc);
		confname = linebuff + 2;

		parms1.line = specname;
		typ_fpath = uls_get_spectype(confname, uls_ptr(parms1));
		len_basedir = parms1.n;
		len_specname = parms1.len;

		if (typ_fpath != ULS_NAME_SPECNAME) {
			err_log("Invalid spec-name in %s.", confname);
			uls_fp_close(fin_uld);
			return -1;
		}
	}

	// confname : NOT-NULL
	if ((fin_ulc = uls_get_ulc_path(typ_fpath, confname, len_basedir, specname, len_specname, uls_ptr(parms1))) == NULL) {
		uls_fp_close(fin_uld);
		return -1;
	}

	fin_ulf = (FILE *) parms1.native_data;

	uls_initial_zerofy_object(uls);
	uls->flags = ULS_FL_STATIC;

	uls_init_namebuf(uls->ulc_name, ULC_LONGNAME_MAXSIZ);
	uls_init_bytespool(uls->ch_context, ULS_SYNTAX_TABLE_SIZE, 0);

	uls->numcnst_separator = ULS_DECIMAL_SEPARATOR_DFL;

	uls_init_array_type00(uls_ptr(uls->numcnst_prefixes), number_prefix, ULS_N_MAX_NUMBER_PREFIXES);
	uls->n_numcnst_prefixes = 0;

	uls_init_bytespool(uls->numcnst_suffixes, ULS_CNST_SUFFIXES_MAXSIZ + 1, 1);
	uls->numcnst_suffixes[0] = '\0';

	uls_init_array_type01(uls_ptr(uls->idfirst_charset), uch_range, 0);
	uls_init_array_type01(uls_ptr(uls->id_charset), uch_range, 0);

	uls_init_array_type01a(uls_ptr(uls->commtypes), commtype, ULS_N_MAX_COMMTYPES);
	uls->n1_commtypes = 0;

	uls_init_parray(uls_ptr(uls->quotetypes), quotetype, ULS_N_MAX_QUOTETYPES);

	if ((rc = uls_init_fp(uls, specname, fin_ulc, fin_ulf)) < 0) {
		uls_fp_close(fin_uld);
		return -1;
	} else {
		uls_grab(uls);
	}

	if (fin_uld != NULL) {
		if ((uld_tag = uld_fpath) == NULL) uld_tag = "";
		if (uld_load_fp(uls, fin_uld, uld_tag) < 0) {
			err_log("Fail to read uld-file %s!", uld_tag);
			return -1;
		}

		uls_fp_close(fin_uld);
	}

	return 0;
}

uls_lex_ptr_t
uls_create(const char* confname)
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
uls_reset(uls_lex_ptr_t uls)
{
	uls_pop_all(uls);
}

int
uls_destroy(uls_lex_ptr_t uls)
{
	if (uls == nilptr || uls->ref_cnt <= 0) {
		err_log("%s: called for invalid object!", __func__);
		return -1;
	}

	if (--uls->ref_cnt > 0) return uls->ref_cnt;

	uls_pop_all(uls);
	uls_xcontext_reset(uls_ptr(uls->xcontext));
	uls_dealloc_lex(uls);

	return 0;
}

int
uls_skip_blanks(uls_lex_ptr_t uls)
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
uls_is_quote_tok(uls_lex_ptr_t uls, int tok_id)
{
	uls_quotetype_ptr_t qmt;
	qmt = uls_find_quotetype_by_tokid(uls_ptr(uls->quotetypes), uls->quotetypes.n, tok_id);
	return qmt != nilptr ? 1 : 0;
}

const char*
uls_get_tag2(uls_lex_ptr_t uls, int* ptr_n_bytes)
{
	const char *tagstr;
	uls_outparam_t parms;

	tagstr = _uls_get_tag2(uls, uls_ptr(parms));
	if (ptr_n_bytes != NULL) {
		*ptr_n_bytes = parms.len;
	}

	return tagstr;
}

uls_tokdef_vx_ptr_t
set_extra_tokdef_vx(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
{
	uls_tokdef_vx_ptr_t e_vx;

	if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) == nilptr) {
		if (tok_id >= 0 && tok_id < ULS_SYNTAX_TABLE_SIZE &&
			(uls->ch_context[tok_id] & ULS_CH_1)) {
			e_vx = uls_insert_1char_tokdef_uch(uls_ptr(uls->onechar_table), tok_id);
		} else {
			return nilptr;
		}
	}

	e_vx->extra_tokdef = extra_tokdef;
	return e_vx;
}

uls_voidptr_t
uls_get_extra_tokdef(uls_lex_ptr_t uls, int tok_id)
{
	uls_tokdef_vx_ptr_t e_vx;

	if (__uls_tok(uls) == tok_id) {
		return uls_get_current_extra_tokdef(uls);
	}

	if ((e_vx = uls_find_tokdef_vx(uls, tok_id)) == nilptr)
		return nilptr;

	return e_vx->extra_tokdef;
}

int
uls_set_extra_tokdef(uls_lex_ptr_t uls, int tok_id, uls_voidptr_t extra_tokdef)
{
	int rval;

	if (set_extra_tokdef_vx(uls, tok_id, extra_tokdef) != nilptr) {
		rval = 0;
	} else {
		rval = -1;
	}

	return rval;
}

uls_voidptr_t
uls_get_current_extra_tokdef(uls_lex_ptr_t uls)
{
	return uls->tokdef_vx->extra_tokdef;
}

void
uls_set_current_extra_tokdef(uls_lex_ptr_t uls, uls_voidptr_t extra_tokdef)
{
	uls_tokdef_vx_ptr_t e_vx = uls->tokdef_vx;

	if (e_vx->flags & ULS_VX_ANONYMOUS) {
		e_vx = set_extra_tokdef_vx(uls, __uls_tok(uls), extra_tokdef);
	} else {
		e_vx->extra_tokdef = extra_tokdef;
	}
}

int
uls_get_tok(uls_lex_ptr_t uls)
{
	uls_context_ptr_t ctx = uls->xcontext.context;

	if (ctx->tok == uls->xcontext.toknum_EOI || ctx->tok == uls->xcontext.toknum_ERR) {
		return ctx->tok;
	}

	if (ctx->flags & ULS_CTX_FL_TOKEN_UNGOT) {
		ctx->flags &= ~ULS_CTX_FL_TOKEN_UNGOT;
		return ctx->tok;
	}

	if (ctx->tok == uls->xcontext.toknum_EOF) {
		if ((ctx = uls_pop(uls)) == nilptr) {
			ctx = make_eoi_lexeme(uls);
			return ctx->tok;
		}
	}

	ctx->flags &= ~(ULS_CTX_FL_QTOK | ULS_CTX_FL_EXTERN_TOKBUF);

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
uls_set_tok(uls_lex_ptr_t uls, int tokid, const char* lexeme, int l_lexeme)
{
	uls_context_ptr_t ctx = uls->xcontext.context;
	if (lexeme == NULL) return;

	if (l_lexeme < 0) {
		l_lexeme = uls_strlen(lexeme);
	}

	str_append(uls_ptr(ctx->tokbuf), 0, lexeme, l_lexeme);
	ctx->s_val = ctx->tokbuf.buf;

	ctx->s_val_len = l_lexeme;
	ctx->s_val_uchars = ustr_num_chars(ctx->s_val, l_lexeme, nilptr);
	ctx->tok = tokid;
}

void
uls_expect(uls_lex_ptr_t uls, int value)
{
	int val0 = __uls_tok(uls);

	if (val0 != value) {
		if (uls_isprint(val0))
			err_log("tok-'%c'", val0);
		else
			err_log("tok-%d", val0);

		if (uls_isprint(value))
			err_panic("tok-'%c' expected!!", value);
		else
			err_panic("tok-%d expected!!", value);
	}
}

void
uls_push(uls_lex_ptr_t uls, uls_context_ptr_t ctx)
{
	if (ctx == nilptr) return;
	uls_push_context(uls, ctx);
}

int
uls_push_isrc(uls_lex_ptr_t uls,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_push_context(uls, nilptr);

	if (__uls_change_isrc(uls, -1, isrc, fill_rawbuf, ungrab_proc) < 0) {
		uls_pop(uls);
		return -1;
	}

	return 0;
}

int
uls_set_isrc(uls_lex_ptr_t uls,
	uls_voidptr_t isrc, uls_fill_isource_t fill_rawbuf, uls_ungrab_isource_t ungrab_proc)
{
	uls_pop(uls);
	return uls_push_isrc(uls, isrc, fill_rawbuf, ungrab_proc);
}

uls_context_ptr_t
uls_pop(uls_lex_ptr_t uls)
{
	uls_source_ptr_t isrc;
	uls_context_ptr_t ctx = uls->xcontext.context, ctx_prev;
	uls_userdata_ptr_t ud, ud_inner;

	if ((ctx_prev=ctx->prev) == nilptr) {
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
uls_pop_all(uls_lex_ptr_t uls)
{
	while (!uls_is_context_initial(uls)) {
		uls_pop(uls);
	}
}

int
uls_push_line(uls_lex_ptr_t uls, const char* line, int len, int flags)
{
	if (line == NULL) {
		line = "";
		len = 0;
	} else if (len < 0) {
		len = uls_strlen(line);
	}

	uls_push_context(uls, nilptr);

	if (__uls_change_line(uls, line, len, flags) < 0) {
		uls_pop(uls);
		return -1;
	}

	if (uls_fillbuff_and_reset(uls) < 0) {
		err_log("%s: fail to fill the initial buff", __func__);
		return -1;
	}

	return 0;
}

int
uls_set_line(uls_lex_ptr_t uls, const char* line, int len, int flags)
{
	uls_pop(uls);
	return uls_push_line(uls, line, len, flags);
}

int
uls_cardinal_toknam(char* tok_nam, uls_lex_ptr_t uls, int tok_id)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	const char *ptr;
	uls_quotetype_ptr_t qmt;
	int has_lxm=1;

	if (tok_id == xctx->toknum_ID) {
		uls_strcpy(tok_nam, "ID");

	} else if (tok_id == xctx->toknum_NUMBER) {
		uls_strcpy(tok_nam, "NUMBER");

	} else if (tok_id == xctx->toknum_TMPL) {
		uls_strcpy(tok_nam, "TMPL");

	} else if (tok_id == xctx->toknum_EOI) {
		uls_strcpy(tok_nam, "EOI");
		has_lxm = 0;

	} else if (tok_id == xctx->toknum_EOF) {
		uls_strcpy(tok_nam, "EOF");

	} else if (tok_id == xctx->toknum_LINK) {
		uls_strcpy(tok_nam, "LINK");
		has_lxm = 0;

	} else if (tok_id == '\n') {
		uls_strcpy(tok_nam, "LF");
		has_lxm = 0;

	} else if (tok_id == '\t') {
		uls_strcpy(tok_nam, "TAB");
		has_lxm = 0;

	} else if ((qmt=uls_find_quotetype_by_tokid(uls_ptr(uls->quotetypes), uls->quotetypes.n, tok_id)) != nilptr) {
		if (qmt->tok_id == xctx->toknum_NONE) {
			uls_strcpy(tok_nam, "NONE");
			has_lxm = 0;
		} else {
			uls_strcpy(tok_nam, "LITSTR");
		}

	} else if ((ptr=uls_tok2name(uls, tok_id)) != NULL) {
		uls_strcpy(tok_nam, ptr);

	} else if (tok_id>=0 && tok_id<ULS_SYNTAX_TABLE_SIZE && uls_isprint(tok_id)) {
		uls_snprintf(tok_nam, ULS_CARDINAL_TOKNAM_SIZ, "%3d", tok_id);
		if (!(uls->ch_context[tok_id] & ULS_CH_1)) {
			has_lxm = 0;
		}

	} else {
		uls_snprintf(tok_nam, ULS_CARDINAL_TOKNAM_SIZ, "%5d", tok_id);
		has_lxm = 0;
	}

	return has_lxm;
}

int
uls_get_number_prefix(uls_outparam_ptr_t parms, char *prefix)
{
	const char *tok_str = parms->lptr;
	int i = 0;

	if (*tok_str == '-') {
		prefix[i++] = '-';
		++tok_str;
	}

	if (*tok_str == '.') {
		prefix[i++] = '0';
	} else {
		prefix[i++] = '0';
		prefix[i++] = 'x';
	}

	prefix[i] = '\0';
	parms->lptr = tok_str;

	return i;
}

int
uls_cardinal_toknam_deco(char *toknam_buff, const char *tok_nam)
{
	int len;
	len = uls_snprintf(toknam_buff, ULS_CARDINAL_TOKNAM_SIZ+1, "[%7s]", tok_nam);
	return len;
}

int
uls_cardinal_toknam_deco_lxmpfx(char *toknam_buff, char *lxmpfx, uls_lex_ptr_t uls,
	int tok_id, uls_outparam_ptr_t parms)
{
	uls_xcontext_ptr_t xctx = uls_ptr(uls->xcontext);
	char tok_nam[ULS_CARDINAL_TOKNAM_SIZ+1];
	int  has_lxm;

	has_lxm = uls_cardinal_toknam(tok_nam, uls, tok_id);
	uls_cardinal_toknam_deco(toknam_buff, tok_nam);

	if (tok_id == xctx->toknum_NUMBER) {
		uls_get_number_prefix(parms, lxmpfx);
	} else {
		lxmpfx[0] = '\0';
	}

	return has_lxm;
}

void
uls_dump_tok(uls_lex_ptr_t uls, const char *pfx, const char *suff)
{
	int tok_id = __uls_tok(uls), has_lxm;
	const char *tok_str = __uls_lexeme(uls);
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];
	char lxmpfx[ULS_CARDINAL_LXMPFX_MAXSIZ+1];
	uls_outparam_t parms;

	if (pfx == NULL) pfx = "";
	if (suff == NULL) suff = "";

	parms.lptr = tok_str;
	has_lxm = uls_cardinal_toknam_deco_lxmpfx(toknam_buff, lxmpfx, uls, tok_id, uls_ptr(parms));
	tok_str = parms.lptr;

	uls_printf("%s%s", pfx, toknam_buff);
	if (has_lxm) {
		uls_printf(" %s%s", lxmpfx, tok_str);
	}
	uls_printf("%s", suff);
}

void
_uls_dump_tok_2(uls_lex_ptr_t uls, const char* pfx, const char *id_str, const char *tok_str, const char *suff)
{
	char toknam_buff[ULS_CARDINAL_TOKNAM_SIZ+1];

	if (pfx == NULL) pfx = "";
	if (suff == NULL) suff = "";

	uls_cardinal_toknam_deco(toknam_buff, id_str);
	uls_printf("%s%s", pfx, toknam_buff);
	uls_printf(" %s%s", tok_str, suff);
}

void
uls_dumpln_tok(uls_lex_ptr_t uls)
{
	uls_dump_tok(uls, "\t", "\n");
}

int
uls_is_real(uls_lex_ptr_t uls)
{
	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		return __uls_is_real(__uls_lexeme(uls));
	}

	return 0;
}

int
uls_is_int(uls_lex_ptr_t uls)
{
	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		return !__uls_is_real(__uls_lexeme(uls));
	}

	return 0;
}

int
uls_is_zero(uls_lex_ptr_t uls)
{
	const char *ptr;

	if (__uls_tok(uls) == uls->xcontext.toknum_NUMBER) {
		ptr = __uls_lexeme(uls);
		if (ptr[0] == '0' || (ptr[0] == '.' && ptr[1] == '0')) {
			return 1;
		}
	}

	return 0;
}

const char*
uls_number_suffix(uls_lex_ptr_t uls)
{
	const char *ptr;
	char ch;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER)
		return "";

	for (ptr=__uls_lexeme(uls); (ch=*ptr)!='\0'; ptr++) {
		if (ch == ' ') {
			++ptr; break;
		}
	}

	return ptr;
}

const char*
uls_eof_tag(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_EOF)
		return NULL;

	ptr = __uls_eof_tag(__uls_lexeme(uls), parms);
	return ptr;
}

double
uls_lexeme_double(uls_lex_ptr_t uls)
{
	const char *ptr;
	double d_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		err_log("The current token is NOT number!");
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
uls_lexeme_uint32(uls_lex_ptr_t uls)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER || *(ptr = __uls_lexeme(uls)) == '-') {
		err_log("The current token is NOT unsigned-int-type!");
		return 0;
	}

	return __uls_lexeme_uint32(ptr);
}

uls_int32
uls_lexeme_int32(uls_lex_ptr_t uls)
{
	const char *ptr;
	uls_int32 u32_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		err_log("The current token is NOT signed-int-type!");
		return 0;
	}

	if (*(ptr = __uls_lexeme(uls)) == '-') {
		minus = 1;
		++ptr;
	}

	u32_val = __uls_lexeme_uint32(ptr);

	return minus ? - (uls_int32) u32_val : u32_val;
}

uls_uint64
uls_lexeme_uint64(uls_lex_ptr_t uls)
{
	const char *ptr;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER || *(ptr = __uls_lexeme(uls)) == '-') {
		err_log("The current token is NOT unsigned-int-type!");
		return 0;
	}

	return __uls_lexeme_uint64(ptr);
}

uls_int64
uls_lexeme_int64(uls_lex_ptr_t uls)
{
	const char *ptr;
	uls_int64 i64_val;
	int minus = 0;

	if (__uls_tok(uls) != uls->xcontext.toknum_NUMBER) {
		err_log("The current token is NOT signed-int-type!");
		return 0;
	}

	if (*(ptr = __uls_lexeme(uls)) == '-') {
		minus = 1;
		++ptr;
	}

	i64_val = __uls_lexeme_uint64(ptr);

	return minus ? - (uls_int64) i64_val : i64_val;
}

unsigned int
uls_lexeme_u(uls_lex_ptr_t uls)
{
	unsigned int u_val;

	if (sizeof(unsigned int) == sizeof(uls_uint32)) {
		u_val = (unsigned int) uls_lexeme_uint32(uls);

	} else if (sizeof(unsigned int) == sizeof(uls_uint64)) {
		u_val = (unsigned int) uls_lexeme_uint64(uls);

	} else {
		err_log("Unsupported integer(unsigned-int) size!");
		u_val = 0;
	}

	return u_val;
}

int
uls_lexeme_d(uls_lex_ptr_t uls)
{
	int i_val;

	if (sizeof(int) == sizeof(uls_int32)) {
		i_val = (int) uls_lexeme_int32(uls);

	} else if (sizeof(int) == sizeof(uls_int64)) {
		i_val = (int) uls_lexeme_int64(uls);

	} else {
		err_log("Unsupported integer(int) size!");
		i_val = 0;
	}

	return i_val;
}

unsigned long
uls_lexeme_lu(uls_lex_ptr_t uls)
{
	unsigned long lu_val;

	if (sizeof(unsigned long) == sizeof(uls_uint32)) {
		lu_val = (unsigned long) uls_lexeme_uint32(uls);

	} else if (sizeof(unsigned long) == sizeof(uls_uint64)) {
		lu_val = (unsigned long) uls_lexeme_uint64(uls);

	} else {
		err_log("Unsupported integer(unsigned-long) size!");
		lu_val = 0;
	}

	return lu_val;
}

long
uls_lexeme_ld(uls_lex_ptr_t uls)
{
	long ld_val;

	if (sizeof(long) == sizeof(uls_int32)) {
		ld_val = (long) uls_lexeme_int32(uls);

	} else if (sizeof(long) == sizeof(uls_int64)) {
		ld_val = (long) uls_lexeme_int64(uls);

	} else {
		err_log("Unsupported integer(long) size!");
		ld_val = 0;
	}

	return ld_val;
}

unsigned long long
uls_lexeme_llu(uls_lex_ptr_t uls)
{
	unsigned long long llu_val;

	if (sizeof(unsigned long long) == sizeof(uls_uint64)) {
		llu_val = (unsigned long long) uls_lexeme_uint64(uls);

	} else if (sizeof(unsigned long long) == sizeof(uls_uint32)) {
		llu_val = (unsigned long long) uls_lexeme_uint32(uls);

	} else {
		err_log("Unsupported integer(unsigned-long-long) size!");
		llu_val = 0;
	}

	return llu_val;
}

long long
uls_lexeme_lld(uls_lex_ptr_t uls)
{
	long long lld_val;

	if (sizeof(long long) == sizeof(uls_int64)) {
		lld_val = (long long) uls_lexeme_int64(uls);

	} else if (sizeof(long long) == sizeof(uls_int32)) {
		lld_val = (long long) uls_lexeme_int32(uls);

	} else {
		err_log("Unsupported integer(long long) size!");
		lld_val = 0;
	}

	return lld_val;
}

int
_uls_const_WANT_EOFTOK(void)
{
	return ULS_WANT_EOFTOK;
}

int
_uls_const_DO_DUP(void)
{
	return ULS_DO_DUP;
}

int
_uls_const_FILE_MS_MBCS(void)
{
	return ULS_FILE_MS_MBCS;
}

int
_uls_const_FILE_UTF8(void)
{
	return ULS_FILE_UTF8;
}

uls_uch_t
_uls_const_NEXTCH_NONE(void)
{
	return ULS_UCH_NONE;
}

int
_uls_toknum_EOI(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_EOI;
}

int
_uls_toknum_EOF(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_EOF;
}

int
_uls_toknum_ERR(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_ERR;
}

int
_uls_toknum_NONE(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_NONE;
}

int
_uls_toknum_ID(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_ID;
}

int
_uls_toknum_NUMBER(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_NUMBER;
}

int
_uls_toknum_TMPL(uls_lex_ptr_t uls)
{
	return uls->xcontext.toknum_TMPL;
}

int
_uls_is_ch_space(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_space(uls, uch);
}

int
_uls_is_ch_idfirst(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_idfirst(uls, uch);
}

int
_uls_is_ch_id(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_id(uls, uch);
}

int
_uls_is_ch_quote(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_quote(uls, uch);
}

int
_uls_is_ch_1ch_token(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_1ch(uls, uch);
}

int
_uls_is_ch_2ch_token(uls_lex_ptr_t uls, uls_uch_t uch)
{
	return uls_is_uch_2ch(uls, uch);
}

int
_uls_get_lineno(uls_lex_ptr_t uls)
{
	return __uls_get_lineno(uls);
}

void
_uls_set_lineno(uls_lex_ptr_t uls, int lineno)
{
	uls_context_set_tag(uls->xcontext.context, NULL, lineno);
}

void
_uls_inc_lineno(uls_lex_ptr_t uls, int amount)
{
	uls_context_inc_lineno(uls->xcontext.context, amount);
}

int
_uls_tok_id(uls_lex_ptr_t uls)
{
	return __uls_tok(uls);
}

const char*
_uls_lexeme(uls_lex_ptr_t uls)
{
	return __uls_lexeme(uls);
}

int
_uls_lexeme_len(uls_lex_ptr_t uls)
{
	return __uls_lexeme_len(uls);
}

int
_uls_lexeme_chars(uls_lex_ptr_t uls)
{
	return __uls_lexeme_chars(uls);
}

const char*
_uls_get_tag(uls_lex_ptr_t uls)
{
	return __uls_get_tag(uls);
}

int
_uls_get_taglen(uls_lex_ptr_t uls)
{
	return __uls_get_taglen(uls);
}

const char*
_uls_get_tag2(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	const char *tagstr = __uls_get_tag(uls);

	if (parms != nilptr) {
		parms->lptr = tagstr;
		parms->len = __uls_get_taglen(uls);
	}

	return tagstr;
}

void
uls_set_tag(uls_lex_ptr_t uls, const char* tag, int lno)
{
	uls_context_set_tag(uls->xcontext.context, tag, lno);
}

const char*
_uls_number_suffix(uls_lex_ptr_t uls, uls_outparam_ptr_t parms)
{
	const char *ptr;

	parms->lptr = ptr = uls_number_suffix(uls);
	parms->len = uls_strlen(ptr);

	return ptr;
}

uls_lex_ptr_t
ulsjava_create(uls_native_vptr_t confname, int len_confname)
{
	char *ustr = uls_strdup((const char *)confname, len_confname);
	uls_lex_ptr_t  uls;

	uls = uls_create(ustr);
	uls_mfree(ustr);

	return uls;
}

void
ulsjava_dump_tok(uls_lex_ptr_t uls, const void *pfx, int len_pfx, const void *suff, int len_suff)
{
	char *ustr1 = uls_strdup((const char *)pfx, len_pfx);
	char *ustr2 = uls_strdup((const char *)suff, len_suff);

	uls_dump_tok(uls, ustr1, ustr2);

	uls_mfree(ustr1);
	uls_mfree(ustr2);
}

void
ulsjava_set_tag(uls_lex_ptr_t uls, const void *tag, int len_tag, int lineno)
{
	char *ustr = uls_strdup((const char *)tag, len_tag);

	uls_set_tag(uls, ustr, lineno);
	uls_mfree(ustr);
}
