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
 * uls_freq.c -- gathering the frequencies of keywords --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#define __ULS_FREQ__
#include "uls/uls_freq.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"

ULS_DECL_STATIC void
normalize_keyw_stat_list(uls_keyw_stat_list_ptr_t kwslst)
{
	uls_ref_parray_init(lst, keyw_stat, uls_ptr(kwslst->lst));
	uls_decl_parray_slots_init(slots_lst, keyw_stat, lst);
	uls_keyw_stat_ptr_t kwstat;
	double sum = 0.;
	int i, n=0, avg, n_lst = kwslst->lst.n;

	for (i=0; i<n_lst; i++) {
		kwstat = slots_lst[i];
		if (kwstat->freq > 0) {
			sum += kwstat->freq;
			++n;
		}
	}

	if (n==0) {
		avg = 1;
	} else {
		avg = (int) (sum / n);
	}

	for (i=0; i<n_lst; i++) {
		kwstat = slots_lst[i];
		if (kwstat->freq < 0) {
			kwstat->freq = avg;
		}
	}
}

ULS_DECL_STATIC int
ulf_read_config_var(int lno, char* lptr, ulf_header_ptr_t hdr)
{
	const char  *wrd;
	int len, stat = 0;
	uls_outparam_t parms;
	uls_version_t ver;
	uls_wrd_t wrdx;

	wrdx.lptr = lptr;
	wrd = _uls_splitstr(uls_ptr(wrdx));

	if (uls_streql(wrd, "INITIAL_HASHCODE:")) {
		wrd = _uls_splitstr(uls_ptr(wrdx));
		if (wrd[0] == '0' && wrd[1] == 'x') {
			parms.lptr = wrd + 2;
			hdr->init_hcode = uls_skip_atox(uls_ptr(parms));
			wrd = parms.lptr;
		} else {
			parms.lptr = wrd;
			hdr->init_hcode = uls_skip_atou(uls_ptr(parms));
			wrd = parms.lptr;
		}

	} else if (uls_streql(wrd, "HASH_ALGORITHM:")) {
		wrd = _uls_splitstr(uls_ptr(wrdx));

		len = uls_str_toupper(wrd, (char *) wrd, -1);
		if (len > ULS_LEXSTR_MAXSIZ || !uls_streql(wrd, ULS_HASH_ALGORITHM)) {
			stat = -1;
		} else {
			uls_set_namebuf_value(hdr->hash_algorithm, wrd);
		}

	} else if (uls_streql(wrd, "HASH_TABLE_SIZE:")) {
		if ((len=uls_atoi(_uls_splitstr(uls_ptr(wrdx)))) <= 0) {
			err_log("Improper hash_table_size", len);
			stat = -1;
		} else {
			hdr->hash_table_size = len;
		}

	} else if (uls_streql(wrd, "HASH_VERSION:")) {
		wrd = _uls_splitstr(uls_ptr(wrdx));

		if (uls_version_pars_str(wrd, uls_ptr(ver)) < 0 ||
			!uls_is_ulf_compatible(uls_ptr(ver), uls_ptr(hdr->hfunc_ver))) {
			err_log("unsupported version(hash-ulf)");
			stat = -1;
		}

		uls_version_copy(uls_ptr(hdr->hfunc_ver), uls_ptr(ver));

	} else {
		err_log("%s: unknown attribute in ULF", wrd);
		stat = -1;
	}

	return stat;
}

ULS_DECL_STATIC int
ulf_read_header(FILE* fin, ulf_header_ptr_t hdr)
{
	const char magic_code[9] = { (char) 0xEF, (char) 0xBB, (char) 0xBF, '#', '@', 'u', 'l', 'f', '-' };
	int  magic_code_len = 9;
	char  linebuff[ULS_LINEBUFF_SIZ__ULF+1], *lptr, *wrd;
	uls_version_t ver;
	int   linelen, lno;

	// Read Header upto '%%'
	lno = 0;

	if ((linelen = uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) < magic_code_len ||
		uls_memcmp(linebuff, magic_code, magic_code_len) != 0) {
		err_log("Improper ulf file format");
		return -1;
	}

	wrd = linebuff + magic_code_len;
	str_trim_end(wrd, linelen - magic_code_len);

	if (uls_version_pars_str(wrd, uls_ptr(ver)) < 0 ||
		!uls_is_ulf_compatible(uls_ptr(ver), uls_ptr(hdr->filever))) {
		err_log("unsupported version(ulf): %s", wrd);
		return -2;
	}

	uls_version_copy(uls_ptr(hdr->filever), uls_ptr(ver));

	while (1) {
		if ((linelen=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) lno = -1;
			break;
		}
		++lno;

		if (!uls_strncmp(linebuff, "%%%%", 2)) {
			break;
		}

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		if (ulf_read_config_var(lno, lptr, hdr) < 0) {
			lno = -1;
			break;
		}
	}

	return lno;
}

uls_keyw_stat_list_ptr_t
make_keyw_stat_for_load(uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst, ulf_header_ptr_t hdr)
{
	uls_keyw_stat_list_ptr_t kwslst;
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	uls_keyw_stat_ptr_t kwstat;
	uls_tokdef_ptr_t e, e_link;
	int   i, n_lst;

	kwslst = uls_alloc_object(uls_keyw_stat_list_t);

	// the length of tok_info_lst linked by e:link is same as 'n_tok_info_lst'
	n_lst = n_tok_info_lst;
	lst = uls_ptr(kwslst->lst);
	uls_init_parray(lst, keyw_stat, n_lst);
	slots_lst = uls_parray_slots(lst);

	// consume tok_info_lst upto n_lst
	for (i=0, e=tok_info_lst; e!=nilptr; e=e_link) {
		e_link = e->link;
		e->link = nilptr;

		kwstat = slots_lst[i] = uls_alloc_object(uls_keyw_stat_t);
		kwstat->keyw = uls_get_namebuf_value(e->keyword);
		kwstat->freq = -1;
		kwstat->keyw_info = e;

		++i;
	}

	_uls_quicksort_vptr(slots_lst, n_lst, keyw_stat_comp_by_keyw);
	kwslst->lst.n = n_lst;

	return kwslst;
}

void
ulf_init_header(ulf_header_ptr_t hdr)
{
	uls_version_make(uls_ptr(hdr->filever), 0, 0, 0);
	uls_version_make(uls_ptr(hdr->hfunc_ver), 0, 0, 0);

	hdr->init_hcode = 0;

	uls_init_namebuf(hdr->hash_algorithm, ULS_LEXSTR_MAXSIZ);
	uls_set_namebuf_value(hdr->hash_algorithm, ULS_HASH_ALGORITHM);

	hdr->hash_table_size = ULF_HASH_TABLE_SIZE;
}

void
ulf_deinit_header(ulf_header_ptr_t hdr)
{
	uls_deinit_namebuf(hdr->hash_algorithm);
}

uls_keyw_stat_list_ptr_t
ulf_load(uls_tokdef_ptr_t tok_info_lst, int n_tok_info_lst,
	FILE *fin, ulf_header_ptr_t hdr, uls_kwtable_ptr_t kw_tbl)
{
	uls_keyw_stat_list_ptr_t kwslst;
	uls_keyw_stat_ptr_t keyw_stat;

	char  linebuff[ULS_LINEBUFF_SIZ__ULF+1], *lptr, *wrd;
	int   linelen, lno;

	uls_hashfunc_t hashfunc;
	uls_dflhash_state_ptr_t hash_stat;

	uls_wrd_t wrdx;

	ulf_init_header(hdr);

	uls_version_make(uls_ptr(hdr->filever),
		ULF_VERSION_MAJOR, ULF_VERSION_MINOR, ULF_VERSION_DEBUG);

	uls_version_make(uls_ptr(hdr->hfunc_ver),
		ULF_VERSION_HASHFUNC_MAJOR, ULF_VERSION_HASHFUNC_MINOR,
		ULF_VERSION_HASHFUNC_DEBUG);

	if ((kwslst = make_keyw_stat_for_load(tok_info_lst, n_tok_info_lst, hdr)) == nilptr) {
		if (n_tok_info_lst > 0) {
			err_log("%s: fail to make keyw_stats", __func__);
			return nilptr;
		}
	}

	if ((lno=ulf_read_header(fin, hdr)) < 0) {
		err_log("%s: fail to read ulf-header", __func__);
		ulc_free_kwstat_list(kwslst);
		return nilptr;
	}

	if (uls_streql(uls_get_namebuf_value(hdr->hash_algorithm), ULS_HASH_ALGORITHM)) {
		kw_tbl->dflhash_stat.init_hcode = hdr->init_hcode;
		hashfunc = nilptr; // use the default hash-func as it is.
		hash_stat = uls_ptr(kw_tbl->dflhash_stat); // the params for the default hahs-func.
	} else {
		err_log("%s: unknown hash-algorithm '%s'", __func__, uls_get_namebuf_value(hdr->hash_algorithm));
		ulc_free_kwstat_list(kwslst);
		return nilptr;
	}

	uls_reset_kwtable(kw_tbl, hdr->hash_table_size, hashfunc, hash_stat);

	while (1) {
		if ((linelen=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				err_log("%s: fail to read ulf-header(io-error)", __func__);
				ulc_free_kwstat_list(kwslst);
				return nilptr;
			}
			break;
		}
		++lno;

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		wrdx.lptr = lptr;
		wrd = _uls_splitstr(uls_ptr(wrdx));

		if ((keyw_stat=ulc_search_kwstat_list(kwslst, wrd)) != nilptr) {
			wrd = _uls_splitstr(uls_ptr(wrdx));
			keyw_stat->freq = uls_atoi(wrd);
		}

		lptr = wrdx.lptr;
	}

	normalize_keyw_stat_list(kwslst);

	return kwslst;
}

int
keyw_stat_comp_by_freq(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_keyw_stat_ptr_t a1 = (const uls_keyw_stat_ptr_t ) a;
	const uls_keyw_stat_ptr_t b1 = (const uls_keyw_stat_ptr_t ) b;

	return b1->freq - a1->freq;
}

int
ulf_create_file(int n_hcodes, uls_uint32 *hcodes,
	int htab_siz, uls_keyw_stat_list_ptr_t kwslst, FILE* fout)
{
	uls_ref_parray_init(lst, keyw_stat, uls_ptr(kwslst->lst));
	uls_decl_parray_slots(slots_lst, keyw_stat);
	uls_keyw_stat_ptr_t kwstat;
	int i;


	if (htab_siz <= 0) return -1;

	if (uls_sysprn_open(fout, nilptr) < 0) {
		err_log("%s: create an output file", __func__);
		return -1;
	}

	uls_fp_putc(fout, (char) 0xEF);
	uls_fp_putc(fout, (char) 0xBB);
	uls_fp_putc(fout, (char) 0xBF);
	uls_sysprn("#@ulf-%d.%d", ULF_VERSION_MAJOR, ULF_VERSION_MINOR);
	uls_sysprn(".%d\n\n", ULF_VERSION_DEBUG);

	uls_sysprn("HASH_ALGORITHM: %s\n", ULS_HASH_ALGORITHM);
	uls_sysprn("HASH_VERSION: %d.%d", ULF_VERSION_HASHFUNC_MAJOR, ULF_VERSION_HASHFUNC_MINOR);
	uls_sysprn(".%d\n", ULF_VERSION_HASHFUNC_DEBUG);

	uls_sysprn("HASH_TABLE_SIZE: %d\n", htab_siz);

	uls_sysprn("INITIAL_HASHCODE: ");
	for (i=0; i<n_hcodes; i++) {
		uls_sysprn(" 0x%08X", hcodes[i]);
	}
	uls_sysprn("\n");

	uls_sysprn("\n%%%%\n\n");

	slots_lst = uls_parray_slots(lst);
	for (i=0; i<kwslst->lst.n; i++) {
		kwstat = slots_lst[i];
		if (kwstat->freq >= 0)
			uls_sysprn("%-24s %d\n", kwstat->keyw, kwstat->freq);
	}

	uls_sysprn_close();
	return 0;
}
