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
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_LITESC__
#include "uls/litesc.h"
#include "uls/uls_misc.h"
#include "uls/uls_log.h"
#endif

int
ULS_QUALIFIED_METHOD(uls_escmap_canbe_escch)(uls_wch_t wch)
{
	int ind;

	if (wch == '\n') {
		ind = ULS_ESCCH_MAPSIZE;
	} else if (wch >= ULS_ESCCH_START && wch <= ULS_ESCCH_END) {
		ind = wch - ULS_ESCCH_START;
	} else {
		ind = -1;
	}

	return ind;
}

void
ULS_QUALIFIED_METHOD(uls_init_escstr)(uls_escstr_ptr_t escstr, char ch, int idx, int len)
{
	escstr->esc_ch = ch;
	escstr->idx = idx;
	escstr->len = len;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escstr)(uls_escstr_ptr_t escstr)
{
}

void
ULS_QUALIFIED_METHOD(uls_init_escmap)(uls_escmap_ptr_t map, uls_escmap_pool_ptr_t mempool)
{
	int n;

	map->flags = ULS_FL_STATIC;
	n = ULS_ESCCH_MAPSIZE + 1;
	uls_init_parray(uls_ptr(map->escstr_list), escstr, n);
	map->escstr_list.n = n;
#ifndef ULS_DOTNET
	uls_bzero(uls_parray_slots(uls_ptr(map->escstr_list)), n * sizeof(uls_escstr_ptr_t));
#endif
	map->mempool = mempool;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_deinit_escmap)(uls_escmap_ptr_t map)
{
	uls_deinit_parray(uls_ptr(map->escstr_list));
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escmap)(uls_escmap_ptr_t map)
{
	if ((map->flags & ULS_ESCMAP_FL_BUILTIN) == 0) {
		__uls_deinit_escmap(map);
	}
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_alloc_escmap)(uls_escmap_pool_ptr_t mempool)
{
	uls_escmap_ptr_t map;

	map = (uls_escmap_ptr_t) uls_alloc_object(uls_escmap_t);
	uls_init_escmap(map, mempool);
	map->flags &= ~ULS_FL_STATIC;

	return map;
}

void
ULS_QUALIFIED_METHOD(uls_dealloc_escmap)(uls_escmap_ptr_t map)
{
	int flags;

	if (map == nilptr || (map->flags & ULS_ESCMAP_FL_BUILTIN)) return;

	flags = map->flags;
	__uls_deinit_escmap(map);

	if ((flags & ULS_FL_STATIC) == 0) {
		uls_dealloc_object(map);
	}
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_escstr_nosafe)(uls_escmap_ptr_t map, int ind, char ch)
{
	uls_decl_parray_slots(slots_escstr, escstr);
	slots_escstr = uls_parray_slots(uls_ptr(map->escstr_list));
	return slots_escstr[ind];
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_escstr)(uls_escmap_ptr_t map, char ch)
{
	uls_escstr_ptr_t escstr;
	int ind;

	if ((ind = uls_escmap_canbe_escch(ch)) < 0) {
		escstr = nilptr;
	} else {
		escstr = uls_find_escstr_nosafe(map, ind, ch);
	}

	return escstr;
}

ULS_QUALIFIED_RETTYP(uls_escmap_container_ptr_t)
ULS_QUALIFIED_METHOD(uls_alloc_escmap_container)(char esc_ch, int idx, int len)
{
	uls_escmap_container_ptr_t wrap;

	wrap = (uls_escmap_container_ptr_t) uls_alloc_object(uls_escmap_container_t);
	uls_init_escstr(uls_ptr(wrap->escstr), esc_ch, idx, len);
	wrap->next = nilptr;

	return wrap;
}

void
ULS_QUALIFIED_METHOD(uls_dealloc_escmap_container)(uls_escmap_container_ptr_t wrap)
{
	uls_deinit_escstr(uls_ptr(wrap->escstr));
	uls_dealloc_object(wrap);
}

void
ULS_QUALIFIED_METHOD(uls_init_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	_uls_ptrtype_tool(csz_str) csz;
	int n;

	csz = uls_ptr(escmap_pool->strpool);
	_uls_tool(csz_init)(csz, 32);
	_uls_tool(csz_add_eos)(csz);

	n = ULS_ESCCH_MAPSIZE + 1;
	uls_init_parray(uls_ptr(escmap_pool->escstr_containers), escmap_container, n);
	escmap_pool->escstr_containers.n = n;
#ifndef ULS_DOTNET
	uls_bzero(uls_parray_slots(uls_ptr(escmap_pool->escstr_containers)), n * sizeof(uls_escmap_container_ptr_t));
#endif
	escmap_pool->ref_cnt = 1;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__uls_deinit_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	uls_decl_parray_slots_init(slots_escstr_containers, escmap_container, uls_ptr(escmap_pool->escstr_containers));
	uls_escmap_container_ptr_t wrap, wrap_next, wrap_head;
	int i;

	for (i=0; i <= ULS_ESCCH_MAPSIZE; i++) {
		wrap_head = slots_escstr_containers[i];

		for (wrap = wrap_head; wrap != nilptr; wrap = wrap_next) {
			wrap_next = wrap->next;
			uls_dealloc_escmap_container(wrap);
		}

		slots_escstr_containers[i] = nilptr;
	}

	uls_deinit_parray(uls_ptr(escmap_pool->escstr_containers));
	_uls_tool(csz_deinit)(uls_ptr(escmap_pool->strpool));
}

void
ULS_QUALIFIED_METHOD(uls_grab_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	++escmap_pool->ref_cnt;
}

void
ULS_QUALIFIED_METHOD(uls_ungrab_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	if (escmap_pool->ref_cnt <= 0) {
		_uls_log(err_panic)("%s: InternalError!", __func__);
	}

	if (--escmap_pool->ref_cnt <= 0) {
		__uls_deinit_escmap_pool(escmap_pool);
	}
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	uls_ungrab_escmap_pool(escmap_pool);
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_search_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots(slots_escstr_containers, escmap_container);
	const char *wrd, *strpool = _uls_tool(csz_text)(uls_ptr(escmap_pool->strpool));
	uls_escmap_container_ptr_t wrap, wrap_head;
	uls_escstr_ptr_t e, e_ret = nilptr;
	int ind;

	if ((ind = uls_escmap_canbe_escch(esc_ch)) < 0) {
		return nilptr;
	}

	slots_escstr_containers = uls_parray_slots(uls_ptr(escmap_pool->escstr_containers));
	wrap_head = slots_escstr_containers[ind];

	if (str != NULL) {
		for (wrap = wrap_head; wrap != nilptr; wrap = wrap->next) {
			e = uls_ptr(wrap->escstr);
			if (e->idx >= 0) {
				wrd = strpool + e->idx;
				if (e->len == len && uls_streql(wrd, str)) {
					e_ret = e;
					break;
				}
			}
		}
	} else {
		for (wrap = wrap_head; wrap != nilptr; wrap = wrap->next) {
			e = uls_ptr(wrap->escstr);
			if (e->idx < 0) {
				if (e->len == len) {
					e_ret = e;
					break;
				}
			}
		}
	}

	return e_ret;
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(__uls_add_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool, int ind, char esc_ch, int idx, int len)
{
	uls_decl_parray_slots(slots_escstr_containers, escmap_container);
	uls_escmap_container_ptr_t wrap_head, wrap;

	slots_escstr_containers = uls_parray_slots(uls_ptr(escmap_pool->escstr_containers));
	wrap_head = slots_escstr_containers[ind];

	wrap = uls_alloc_escmap_container(esc_ch, idx, len);
	wrap->next = wrap_head;

	slots_escstr_containers[ind] = wrap;
	return uls_ptr(wrap->escstr);
}

void
ULS_QUALIFIED_METHOD(__uls_add_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, int ind, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots(slots_escstr, escstr);
	_uls_ptrtype_tool(csz_str) csz = uls_ptr(escmap_pool->strpool);
	uls_escstr_ptr_t escstr;
	int strpos, len1, len_strpool;
	const char *str2, *strpool;

	if (str != NULL) {
		strpool = csz_data_ptr(csz);
		len_strpool = csz_length(csz);

		for (strpos = 0; ; strpos += len1 + 1) {
			if (strpos >= len_strpool) { // not-found
				strpos = len_strpool;
				_uls_tool(csz_append)(csz, str, len);
				_uls_tool(csz_add_eos)(csz);
				break;
			}

			str2 = strpool + strpos;
			len1 = _uls_tool_(strlen)(str2);
			if (len == len1 && uls_streql(str2, str)) { // found
				break;
			}
		}
	} else {
		strpos = -1;
	}

	escstr = __uls_add_escmap_pool(escmap_pool, ind, esc_ch, strpos, len);
	slots_escstr = uls_parray_slots(uls_ptr(map->escstr_list));
	slots_escstr[ind] = escstr;
}

int
ULS_QUALIFIED_METHOD(uls_add_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len)
{
	int ind, stat = 0;

	if ((ind = uls_del_escstr(map, esc_ch)) < 0) {
		stat = -1;
	} else {
		__uls_add_escstr(escmap_pool, map, ind, esc_ch, str, len);
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_del_escstr)(uls_escmap_ptr_t map, char esc_ch)
{
	uls_decl_parray_slots(slots_escstr, escstr);
	uls_escstr_ptr_t escstr;
	int ind;

	if ((ind=uls_escmap_canbe_escch(esc_ch)) < 0) {
//		_uls_log(err_log)("Unregistered ch(0x%02x)...return", esc_ch);
		return -1;
	}

	slots_escstr = uls_parray_slots(uls_ptr(map->escstr_list));
	if ( (escstr = slots_escstr[ind]) != nilptr) {
		slots_escstr[ind] = nilptr;
	}

	return ind;
}

void
ULS_QUALIFIED_METHOD(__uls_register_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, int ind, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots_init(slots_escstr, escstr, uls_ptr(map->escstr_list));
	uls_escstr_ptr_t escstr;

	if ((escstr = uls_search_escmap_pool(escmap_pool, esc_ch, str, len)) != nilptr) {
		slots_escstr[ind] = escstr;
	} else {
		if (str != NULL) {
			__uls_add_escstr(escmap_pool, map, ind, esc_ch, str, len);
		} else {
			escstr = __uls_add_escmap_pool(escmap_pool, ind, esc_ch, -1, len);
			slots_escstr[ind] = escstr;
		}
	}
}

void
ULS_QUALIFIED_METHOD(uls_register_escstr_nosafe)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len)
{
	int ind = uls_escmap_canbe_escch(esc_ch);
	__uls_register_escstr(escmap_pool, map, ind, esc_ch, str, len);
}

int
ULS_QUALIFIED_METHOD(uls_register_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len)
{
	int ind, stat = 0;

	if (str != NULL) {
		if (len < 0) len = _uls_tool_(strlen)(str);
	}

	if ((ind = uls_del_escstr(map, esc_ch)) < 0) {
		stat = -1;
	} else {
		__uls_register_escstr(escmap_pool, map, ind, esc_ch, str, len);
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(__uls_clone_escmap)(uls_escmap_ptr_t src_map, uls_escmap_ptr_t dst_map, uls_escmap_pool_ptr_t dst_escmap_pool)
{
	uls_decl_parray_slots(slots_escstr_src, escstr);
	uls_decl_parray_slots(slots_escstr_dst, escstr);
	uls_escstr_ptr_t escstr;

	uls_escmap_pool_ptr_t src_escmap_pool = src_map->mempool;
	const char *wrd, *strpool = _uls_tool(csz_text)(uls_ptr(src_escmap_pool->strpool));
	int i, len;

	slots_escstr_src = uls_parray_slots(uls_ptr(src_map->escstr_list));

	if (dst_escmap_pool != nilptr) {
		slots_escstr_dst = uls_parray_slots(uls_ptr(dst_map->escstr_list));

		for (i = 0; i <= ULS_ESCCH_MAPSIZE; i++) {
			escstr = slots_escstr_src[i];
			if (escstr == nilptr) {
				slots_escstr_dst[i] = nilptr;
				continue;
			}

			if (escstr->idx >= 0) {
				if ((len = escstr->len) > 0) {
					wrd = strpool + escstr->idx;
				} else {
					wrd = "";
				}
			} else {
				wrd = NULL;
				len = escstr->len;
			}
			uls_register_escstr_nosafe(dst_escmap_pool, dst_map, escstr->esc_ch, wrd, len);
		}

	} else {
		dst_escmap_pool = dst_map->mempool;
		slots_escstr_dst = uls_parray_slots(uls_ptr(dst_map->escstr_list));
		for (i = 0; i <= ULS_ESCCH_MAPSIZE; i++) {
			slots_escstr_dst[i] = slots_escstr_src[i];
		}
	}
}

void
ULS_QUALIFIED_METHOD(__uls_set_escmap)(uls_escmap_ptr_t dst_map, int flags)
{
	uls_escmap_pool_ptr_t escmap_pool = dst_map->mempool;
	int i, esc_flags;

	if (flags & ULS_ESCMAP_MODERN_EOL) uls_register_escstr_nosafe(escmap_pool, dst_map, '\n', "", 0);
	if (flags & ULS_ESCMAP_MODERN_LF) uls_register_escstr_nosafe(escmap_pool, dst_map, 'n', "\n", 1);
	if (flags & ULS_ESCMAP_MODERN_TAB) uls_register_escstr_nosafe(escmap_pool, dst_map, 't', "\t", 1);

	if (flags & ULS_ESCMAP_LEGACY_SQ) uls_register_escstr_nosafe(escmap_pool, dst_map, '\'', "'", 1);
	if (flags & ULS_ESCMAP_LEGACY_DQ) uls_register_escstr_nosafe(escmap_pool, dst_map, '"', "\"", 1);
	if (flags & ULS_ESCMAP_LEGACY_CR) uls_register_escstr_nosafe(escmap_pool, dst_map, 'r', "\r", 1);
	if (flags & ULS_ESCMAP_LEGACY_BS) uls_register_escstr_nosafe(escmap_pool, dst_map, 'b', "\b", 1);
	if (flags & ULS_ESCMAP_LEGACY_BELL) uls_register_escstr_nosafe(escmap_pool, dst_map, 'a', "\a", 1);
	if (flags & ULS_ESCMAP_LEGACY_FF) uls_register_escstr_nosafe(escmap_pool, dst_map, 'f', "\f", 1);
	if (flags & ULS_ESCMAP_LEGACY_VF) uls_register_escstr_nosafe(escmap_pool, dst_map, 'v', "\v", 1);
	if (flags & ULS_ESCMAP_LEGACY_QUES) uls_register_escstr_nosafe(escmap_pool, dst_map, '?', "?", 1);

	if (flags & ULS_ESCMAP_LEGACY_OCT) {
		esc_flags = ULS_FL_ESCSTR_OCTAL | 0x02; // at most 2 more octal-digits
		for (i = 0; i < 8; i++) {
			uls_register_escstr_nosafe(escmap_pool, dst_map,  '0' + i, NULL, esc_flags);
		}
	}

	if (flags & ULS_ESCMAP_LEGACY_HEX) {
		esc_flags = ULS_FL_ESCSTR_HEXA | 0x02; // #-of-hexa-digits
		uls_register_escstr_nosafe(escmap_pool, dst_map, 'x', NULL, esc_flags);
	}

	if (flags & ULS_ESCMAP_MODERN_U4) {
		esc_flags = ULS_FL_ESCSTR_UNICODE | 0x04; // #-of-hexa-digits
		uls_register_escstr_nosafe(escmap_pool, dst_map, 'u', NULL, esc_flags);
	}

	if (flags & ULS_ESCMAP_MODERN_U8) {
		esc_flags = ULS_FL_ESCSTR_UNICODE | 0x08; // #-of-hexa-digits
		uls_register_escstr_nosafe(escmap_pool, dst_map, 'U', NULL, esc_flags);
	}
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_dup_escmap)(uls_escmap_ptr_t src_map, uls_escmap_pool_ptr_t escmap_pool, int flags)
{
	uls_escmap_ptr_t dst_map;

	dst_map = uls_alloc_escmap(escmap_pool);
	__uls_clone_escmap(src_map, dst_map, escmap_pool);
	__uls_set_escmap(dst_map, flags);

	return dst_map;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_escstr_bin_opts)(uls_ptrtype_tool(outparam) parms)
{
	const char *lptr = parms->lptr;
	int n, stat = 0;
	int ch, rc = 0;

	if (_uls_tool_(strncmp)(lptr, "\\x", 2) == 0) {
		stat |= ULS_FL_ESCSTR_HEXA;
		rc = 2;
	} else if (_uls_tool_(strncmp)(lptr, "\\o", 2) == 0) {
		stat |= ULS_FL_ESCSTR_OCTAL;
		rc = 2;
	} else if (_uls_tool_(strncmp)(lptr, "\\u", 2) == 0) {
		stat |= ULS_FL_ESCSTR_UNICODE;
		rc = 2;
	} else {
		parms->x1 = 0;
		return 0;
	}

	lptr += rc;

	if (*lptr == '0') {
		stat |= ULS_FL_ESCSTR_FIXED_NDIGITS;
		++lptr;
	}

	if ((ch = *lptr) < '2' || ch > '8') {
		return -1;
	}
	++lptr;
	n = ch - '0';

	if ((stat & ULS_FL_ESCSTR_HEXA) && n != 2) {
		stat = -1;
	} else if ((stat & ULS_FL_ESCSTR_OCTAL) && !(n == 2 || n == 3)) {
		stat = -2;
	} else if ((stat & ULS_FL_ESCSTR_UNICODE) && !(n == 4 || n == 8)) {
		stat = -3;
	} else {
		stat |= n;
	}

	if (stat < 0) return -2;

	if (*lptr == 'A') {
		++lptr;
		stat |= ULS_FL_ESCSTR_HEXA_AF;
	}

	if (*lptr == 'a') {
		++lptr;
		stat |= ULS_FL_ESCSTR_HEXA_af;
	}

	parms->x1 = (unsigned int) (lptr - parms->lptr);
	parms->lptr = lptr;
	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__parse_escmap_optgrp)(char *line)
{
	char *lptr, *wrd, ch;
	int i, bFirst = 1, flags = 0;

	for (lptr = line; lptr != NULL; ) {
		wrd = lptr;

		if ((lptr = _uls_tool_(strchr)(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}

		if (uls_streql(wrd, "eos")) {
			flags |= ULS_ESCMAP_MODERN_EOS;
		} else if (uls_streql(wrd, "esc")) {
			flags |= ULS_ESCMAP_MODERN_ESC;
		} else if (uls_streql(wrd, "etc")) {
			flags |= ULS_ESCMAP_MODERN_ETC;
		} else if (uls_streql(wrd, "eol")) {
			flags |= ULS_ESCMAP_MODERN_EOL;
		} else if (uls_streql(wrd, "u4")) {
			flags |= ULS_ESCMAP_MODERN_U4;
		} else if (uls_streql(wrd, "U8")) {
			flags |= ULS_ESCMAP_MODERN_U8;
		} else if (uls_streql(wrd, "hex")) {
			flags |= ULS_ESCMAP_LEGACY_HEX;
		} else if (uls_streql(wrd, "oct")) {
			flags |= ULS_ESCMAP_LEGACY_OCT;
		} else {
			if (!bFirst) {
				_uls_log(err_log)("Unknown keyword(%s) for esc-chars map", wrd);
				flags = -1;
				break;
			}

			for (i = 0; (ch=wrd[i]) != '\0'; i++) {
				if (ch == 'r') flags |= ULS_ESCMAP_LEGACY_CR;
				else if (ch == '\'') flags |= ULS_ESCMAP_LEGACY_SQ;
				else if (ch == '"') flags |= ULS_ESCMAP_LEGACY_DQ;
				else if (ch == '?') flags |= ULS_ESCMAP_LEGACY_QUES;
				else {
					_uls_log(err_log)("Unknown keyword(%s) exists for esc-chars map", wrd);
					flags = -1;
					break;
				}
			}
		}

		bFirst = 0;
	}

	return flags;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(parse_escmap_optgrp)(uls_escmap_ptr_t esc_map, uls_ptrtype_tool(outparam) parms)
{
	int rval_flags, stat = 0;
	char *lptr, *lptr1;

	lptr1 = parms->line;
	if ((lptr = _uls_tool_(strchr)(lptr1, ')')) == NULL) {
		_uls_log(err_log)("misuse of options: no matching ')'");
		return -1;
	}

	*lptr++ = '\0'; // overwriting ')'

	rval_flags = __parse_escmap_optgrp(lptr1);
	if (rval_flags < 0) {
		_uls_log(err_log)("An undefined flag specified");
		return -1;
	}

	parms->line = lptr;
	parms->flags |= rval_flags;

	return stat;
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_parse_escmap_feature)(uls_ptrtype_tool(outparam) parms)
{
	char *line = parms->line;
	char  *wrd, *lptr, *mode_str, ch, ch_bak;
	uls_escmap_ptr_t esc_map;
	uls_type_tool(outparam) parms1;
	int len, do_dup, flags = 0, stat = 0;

	parms->x1 = do_dup = 0;

	if (*(wrd = _uls_tool(skip_blanks)(line)) == '\0') {
		esc_map = uls_ptr(uls_litesc->uls_escstr__legacy);
		parms->line = NULL;
		parms->flags = flags;
		return esc_map;
	}

	for (lptr = wrd + 1; (ch=*lptr) != '\0'; lptr++) {
		if (!_uls_tool_(isalnum)(ch)) break;
	}

	len = (int) (lptr - wrd);
	ch_bak = wrd[len];
	wrd[len] = '\0';
	mode_str = wrd;

	if (uls_streql(mode_str, "modern")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__modern);
		flags |= ULS_ESCMAP_MODERN_EOS | ULS_ESCMAP_MODERN_ESC;
	} else if (uls_streql(mode_str, "verbatim")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__verbatim);
	} else if (uls_streql(mode_str, "legacy")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__legacy);
		flags |= ULS_ESCMAP_MODERN_EOS | ULS_ESCMAP_MODERN_ESC;
	} else {
		if (uls_streql(mode_str, "verbatim1")) {
			esc_map = uls_ptr(uls_litesc->uls_escstr__verbatim1);
			flags |= ULS_ESCMAP_MODERN_EOS | ULS_ESCMAP_MODERN_ESC;
			parms->line = wrd + len;
		} else {
			esc_map = uls_ptr(uls_litesc->uls_escstr__legacy); /* default */
			flags |= ULS_ESCMAP_MODERN_EOS | ULS_ESCMAP_MODERN_ESC;
			parms->line = wrd;
		}
		stat = 1;
	}

	if (ch_bak != '\0') wrd[len] = ch_bak;
	if (stat > 0) {
		parms->flags = flags;
		return esc_map;
	}

	if (*lptr == '(') {
		parms1.line = ++lptr;
		parms1.flags = flags;

		if (parse_escmap_optgrp(esc_map, uls_ptr(parms1)) < 0) {
			parms->line = NULL;
			return nilptr;
		}

		lptr = parms1.line;
		flags = parms1.flags;

		do_dup = 1;
	}

	parms->line = lptr;
	parms->flags = flags;
	parms->x1 = do_dup;

	return esc_map;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(extract_escstr_mapexpr)(char *line, uls_ptrtype_tool(outparam) parms)
{
	char  *lptr = line, *lptr1;
	int i, len, ret_flag = 0;
	uls_type_tool(outparam) parms1;
	uls_type_tool(wrd) wrdx;
	char end_qch;

	lptr = _uls_tool(skip_blanks)(lptr);
	if (*lptr == '\0') return -1;

	if (*lptr == '~' && lptr[1] != ':') {
		ret_flag |= ULS_FL_ESCSTR_DEL;

		wrdx.lptr = ++lptr;
		lptr1 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		lptr = wrdx.lptr;

		len = 0;
		if (uls_streql(lptr1, "oct")) {
			ret_flag |= ULS_FL_ESCSTR_OCTAL;
		} else if (uls_streql(lptr1, "hex")) {
			ret_flag |= ULS_FL_ESCSTR_HEXA;
		} else {
			len = -2;
		}

		parms->wch = '\0'; // NA
		parms->n = (int) (lptr - line);
		parms->len = len;
		parms->flags = ret_flag;
		return len;
	}

	parms->wch = *lptr;

	if (lptr[1] != ':') {
		_uls_log(err_log)("Incorrect format: ':' is needed");
		return -3;
	}
	lptr += 2;

	end_qch = 'A';
	if (*lptr == '\0') {
		ret_flag |= ULS_FL_ESCSTR_DEL;
		lptr1 = lptr;
	} else if (*lptr == '"') {
		lptr1 = ++lptr;
		end_qch = '"';
	} else {
		lptr1 = lptr = _uls_tool(skip_blanks)(lptr);
		parms1.lptr = lptr;

		if ((ret_flag = get_escstr_bin_opts(uls_ptr(parms1))) <= 0) {
			if (ret_flag < 0) {
				_uls_log(err_log)("incorrect format!");
				return -4;
			}
			end_qch = '\0';
		} else {
			lptr += parms1.x1;
		}
	}

	len = 0;
	parms->line[0] = '\0';

	if (end_qch != 'A') {
		parms1.lptr = lptr1;
		parms1.line = parms->line;

		if ((len = uls_get_simple_escape_str(end_qch, uls_ptr(parms1))) < 0) {
			len = -4;
		} else {
			lptr = (char *) parms1.lptr;
			for (i = 0; i < len; i++) {
				// return an error if it contains '\0'
				if (parms1.line[i] == '\0') {
					_uls_log(err_log)("escstr cannot contain null-char!");
					len = -5;
					break;
				}
			}
		}
	}

	parms->n = (int) (lptr - line);
	parms->len = len;
	parms->flags = ret_flag;
	return len;
}

int
ULS_QUALIFIED_METHOD(uls_parse_escmap_mapping)(uls_escmap_ptr_t esc_map, char *line)
{
	uls_escmap_pool_ptr_t escmap_pool = esc_map->mempool;
	int len, rval, rval_flags, stat = 0;
	char  esc_ch, *lptr, *escstr_buf, buff[128];
	uls_type_tool(outparam) parms1;

	if ((len = _uls_tool_(strlen)(line)) + 1 > sizeof(buff)) {
		escstr_buf = (char *) _uls_tool_(malloc)(len + 1);
	} else {
		escstr_buf = buff;
	}

	for (lptr = line; ; ) {
		parms1.line = escstr_buf;

		if ((len = extract_escstr_mapexpr(lptr, uls_ptr(parms1))) < 0) {
			if (len < -1) {
				_uls_log(err_log)("Failed to extract esc-str!");
				stat = -1;
			}
			break;
		}

		lptr += parms1.n;
		esc_ch = (char) parms1.wch;
		rval_flags = parms1.flags;

		if (rval_flags & (ULS_FL_ESCSTR_HEXA | ULS_FL_ESCSTR_OCTAL | ULS_FL_ESCSTR_UNICODE)) {
			if (rval_flags & ULS_FL_ESCSTR_DEL) {
				if (rval_flags & ULS_FL_ESCSTR_HEXA) {
					uls_del_escstr(esc_map, 'x');
				}
				if (rval_flags & ULS_FL_ESCSTR_OCTAL) {
					for (rval = 0; rval < 8; rval++) {
						uls_del_escstr(esc_map, '0' + rval);
					}
				}
				rval = 0;
			} else {
				rval = uls_register_escstr(escmap_pool, esc_map, esc_ch, NULL, rval_flags);
			}
		} else {
			if (rval_flags & ULS_FL_ESCSTR_DEL) {
				uls_del_escstr(esc_map, esc_ch);
				rval = 0;
			} else {
				rval = uls_register_escstr(escmap_pool, esc_map, esc_ch, escstr_buf, len);
			}
		}

		if (rval < 0) {
			stat = -1;
			break;
		}
	}

	if (escstr_buf != buff) {
		uls_mfree(escstr_buf);
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_dec_escaped_char)(uls_escstr_ptr_t escstr, uls_escmap_ptr_t map,
	uls_ptrtype_tool(outparam) parms, _uls_ptrtype_tool(csz_str) cszbuf)
{
	char esc_ch = (char) parms->x1;
	const char *wrd, *strpool = _uls_tool(csz_text)(uls_ptr(map->mempool->strpool));
	int n_xdigits, len, flags;

	if (escstr->idx >= 0) {
		if ((len = escstr->len) > 0) {
			wrd = strpool + escstr->idx;
			_uls_tool(csz_append)(cszbuf, wrd, len);
		}
	} else {
		flags = escstr->len;
		n_xdigits = flags & ULS_FL_ESCSTR_MASK;

		if (_uls_tool_(isdigit)(esc_ch)) {
			parms->wch = esc_ch - '0';
		} else {
			parms->wch = 0;
		}

		len = -n_xdigits;
		parms->flags = flags;
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(initialize_uls_litesc)()
{
	uls_escmap_pool_ptr_t escmap_pool;
	uls_escmap_ptr_t map;
	int map_flags;

	uls_litesc = uls_alloc_object_clear(uls_litesc_sysinfo_t);

	escmap_pool = uls_ptr(uls_litesc->escmap_pool__global);
	uls_init_escmap_pool(escmap_pool);

	/* legacy */
	map = uls_ptr(uls_litesc->uls_escstr__legacy);
	uls_init_escmap(map, escmap_pool);
	map->flags |= ULS_ESCMAP_FL_BUILTIN;

	map_flags = ULS_ESCMAP_MODERN_LF | ULS_ESCMAP_MODERN_TAB | ULS_ESCMAP_LEGACY_CR;
	map_flags |= ULS_ESCMAP_LEGACY_BS | ULS_ESCMAP_LEGACY_BELL;
	map_flags |= ULS_ESCMAP_LEGACY_FF | ULS_ESCMAP_LEGACY_VF;
	map_flags |= ULS_ESCMAP_LEGACY_HEX | ULS_ESCMAP_LEGACY_OCT;
//	map_flags |= ULS_ESCMAP_LEGACY_SQ | ULS_ESCMAP_LEGACY_DQ;
//	map_flags |= ULS_ESCMAP_LEGACY_QUES;
	__uls_set_escmap(map, map_flags);

	/* modern */
	map = uls_ptr(uls_litesc->uls_escstr__modern);
	uls_init_escmap(map, escmap_pool);
	map->flags |= ULS_ESCMAP_FL_BUILTIN;
	map_flags = ULS_ESCMAP_MODERN_LF | ULS_ESCMAP_MODERN_TAB;
	__uls_set_escmap(map, map_flags);

	/* verbatim1 */
	map = uls_ptr(uls_litesc->uls_escstr__verbatim1);
	uls_init_escmap(map, escmap_pool);
	map->flags |= ULS_ESCMAP_FL_BUILTIN;

	/* verbatim */
	map = uls_ptr(uls_litesc->uls_escstr__verbatim);
	uls_init_escmap(map, escmap_pool);
	map->flags |= ULS_ESCMAP_FL_BUILTIN;

	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_uls_litesc)()
{
	/* legacy */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__legacy));

	/* modern */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__modern));

	/* verbatim1 */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__verbatim1));

	/* verbatim */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__verbatim));

	/* dealloc string pool */
	uls_deinit_escmap_pool(uls_ptr(uls_litesc->escmap_pool__global));

	uls_dealloc_object(uls_litesc);
}
