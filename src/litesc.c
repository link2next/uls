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
ULS_QUALIFIED_METHOD(uls_escmap_canbe_escch)(uls_uch_t uch)
{
	int ind;

	if (uch >= ULS_ESCMAP_CANBE_ESCCH_START && uch <= ULS_ESCMAP_CANBE_ESCCH_END) {
		ind = uch - ULS_ESCMAP_CANBE_ESCCH_START;
	} else {
		ind = -1;
	}

	return ind;
}

void
ULS_QUALIFIED_METHOD(uls_init_escstr)(uls_escstr_ptr_t escstr, char ch, char *str, int len)
{
	escstr->esc_ch = ch;
	escstr->str = str;
	escstr->len = len;
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_alloc_escstr)(char ch, char *str, int len)
{
	uls_escstr_ptr_t escstr;

	escstr = (uls_escstr_ptr_t) uls_alloc_object(uls_escstr_t);
	uls_init_escstr(escstr, ch, str, len);

	return escstr;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escstr)(uls_escstr_ptr_t escstr)
{
}

void
ULS_QUALIFIED_METHOD(uls_dealloc_escstr)(uls_escstr_ptr_t escstr)
{
	uls_deinit_escstr(escstr);
	uls_dealloc_object(escstr);
}

void
ULS_QUALIFIED_METHOD(uls_init_escmap)(uls_escmap_ptr_t map)
{
	map->flags = 0;
	map->esc_sym = ULS_ESCMAP_DFL_ESCSYM;
	uls_init_parray(uls_ptr(map->escstr_list), escstr, ULS_ESCMAP_MAPSIZE);
	map->escstr_list.n = ULS_ESCMAP_MAPSIZE;
#ifndef ULS_DOTNET
	uls_bzero(uls_parray_slots(uls_ptr(map->escstr_list)), ULS_ESCMAP_MAPSIZE * sizeof(uls_escstr_ptr_t));
#endif
}

void
ULS_QUALIFIED_METHOD(__uls_deinit_escmap)(uls_escmap_ptr_t map)
{
	uls_deinit_parray(uls_ptr(map->escstr_list));
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escmap)(uls_escmap_ptr_t map)
{
	if ((map->flags & ULS_ESCMAP_SYSTEM) == 0) {
		__uls_deinit_escmap(map);
	}
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_alloc_escmap)()
{
	uls_escmap_ptr_t map;

	map = (uls_escmap_ptr_t) uls_alloc_object(uls_escmap_t);
	uls_init_escmap(map);

	return map;
}

void
ULS_QUALIFIED_METHOD(uls_dealloc_escmap)(uls_escmap_ptr_t map)
{
	int flags = map->flags;

	uls_deinit_escmap(map);

	if ((flags & (ULS_FL_STATIC | ULS_ESCMAP_SYSTEM)) == 0) {
		uls_dealloc_object(map);
	}
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_escstr)(uls_escmap_ptr_t map, char ch)
{
	uls_decl_parray_slots(slots_escstr, escstr);
	int ind;

	if ((ind=uls_escmap_canbe_escch(ch)) < 0) {
		return nilptr;
	}

	slots_escstr = uls_parray_slots(uls_ptr(map->escstr_list));
	return slots_escstr[ind];
}

ULS_QUALIFIED_RETTYP(uls_escmap_container_ptr_t)
ULS_QUALIFIED_METHOD(uls_alloc_escmap_container)(char esc_ch, char *str, int len)
{
	uls_escmap_container_ptr_t wrap;

	wrap = (uls_escmap_container_ptr_t) uls_alloc_object(uls_escmap_container_t);
	uls_init_escstr(uls_ptr(wrap->escstr), esc_ch, str, len);
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
	_uls_tool(isp_init)(uls_ptr(escmap_pool->strpool), 512);

	uls_init_parray(uls_ptr(escmap_pool->escstr_containers), escmap_container, ULS_ESCMAP_MAPSIZE);
	escmap_pool->escstr_containers.n = ULS_ESCMAP_MAPSIZE;
#ifndef ULS_DOTNET
	uls_bzero(uls_parray_slots(uls_ptr(escmap_pool->escstr_containers)), ULS_ESCMAP_MAPSIZE * sizeof(uls_escmap_container_ptr_t));
#endif
}

void
ULS_QUALIFIED_METHOD(uls_deinit_escmap_pool)(uls_escmap_pool_ptr_t escmap_pool)
{
	uls_decl_parray_slots_init(slots_escstr_containers, escmap_container, uls_ptr(escmap_pool->escstr_containers));
	uls_escmap_container_ptr_t wrap, wrap_next, wrap_head;
	int i;

	for (i=0; i < ULS_ESCMAP_MAPSIZE; i++) {
		wrap_head = slots_escstr_containers[i];

		for (wrap = wrap_head; wrap != nilptr; wrap = wrap_next) {
			wrap_next = wrap->next;
			uls_dealloc_escmap_container(wrap);
		}

		slots_escstr_containers[i] = nilptr;
	}

	uls_deinit_parray(uls_ptr(escmap_pool->escstr_containers));
	_uls_tool(isp_deinit)(uls_ptr(escmap_pool->strpool));
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_find_escstr_in_escmap)(uls_escmap_pool_ptr_t escmap_pool, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots(slots_escstr_containers, escmap_container);
	uls_escmap_container_ptr_t wrap, wrap_head;
	uls_escstr_ptr_t e, e_ret = nilptr;
	int ind;

	if ((ind=uls_escmap_canbe_escch(esc_ch)) < 0) {
		return nilptr;
	}

	slots_escstr_containers = uls_parray_slots(uls_ptr(escmap_pool->escstr_containers));
	wrap_head = slots_escstr_containers[ind];

	if (str != NULL) {
		if (len < 0) len = _uls_tool_(strlen)(str);
		for (wrap = wrap_head; wrap != nilptr; wrap = wrap->next) {
			e = uls_ptr(wrap->escstr);
			if (e->str != NULL) {
				if (e->esc_ch == esc_ch && e->len == len && uls_streql(e->str, str)) {
					e_ret = e;
					break;
				}
			}
		}
	} else {
		for (wrap = wrap_head; wrap != nilptr; wrap = wrap->next) {
			e = uls_ptr(wrap->escstr);
			if (e->str == NULL) {
				if (e->esc_ch == esc_ch && e->len == len) {
					e_ret = e;
					break;
				}
			}
		}
	}

	return e_ret;
}

ULS_QUALIFIED_RETTYP(uls_escstr_ptr_t)
ULS_QUALIFIED_METHOD(uls_add_escstr_in_escmap)(uls_escmap_pool_ptr_t escmap_pool, char esc_ch, char *str, int len)
{
	uls_decl_parray_slots(slots_escstr_containers, escmap_container);
	uls_escmap_container_ptr_t wrap_head, wrap;
	int ind;

	if ((ind=uls_escmap_canbe_escch(esc_ch)) < 0) {
		return nilptr;
	}

	slots_escstr_containers = uls_parray_slots(uls_ptr(escmap_pool->escstr_containers));
	wrap_head = slots_escstr_containers[ind];

	wrap = uls_alloc_escmap_container(esc_ch, str, len);
	wrap->next = wrap_head;

	slots_escstr_containers[ind] = wrap;

	return uls_ptr(wrap->escstr);
}

int
ULS_QUALIFIED_METHOD(uls_add_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots(slots_escstr, escstr);
	uls_escstr_ptr_t escstr;
	char *str2;
	int ind;

	if ((ind=uls_del_escstr(map, esc_ch)) < 0) {
		return -1;
	}

	if ((escstr = uls_find_escstr_in_escmap(escmap_pool, esc_ch, str, len)) == nilptr) {
		if (str != NULL) {
			if (len < 0) len = _uls_tool_(strlen)(str);

			if ((str2= _uls_tool(isp_find)(uls_ptr(escmap_pool->strpool), str, len)) == NULL) {
				if ((str2 = _uls_tool(isp_insert)(uls_ptr(escmap_pool->strpool), str, len)) == NULL) {
					return -1;
				}
			}
		} else {
			str2 = NULL;
		}

		escstr = uls_add_escstr_in_escmap(escmap_pool, esc_ch, str2, len);
		if (escstr == nilptr) return -1;
	}

	slots_escstr = uls_parray_slots(uls_ptr(map->escstr_list));
	slots_escstr[ind] = escstr;

	return 0;
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

int
ULS_QUALIFIED_METHOD(uls_register_escstr)(uls_escmap_pool_ptr_t escmap_pool,
	uls_escmap_ptr_t map, char esc_ch, const char *str, int len)
{
	uls_decl_parray_slots_init(slots_escstr, escstr, uls_ptr(map->escstr_list));
	uls_escstr_ptr_t escstr;
	int ind, len2, stat = 0;

	if (str != NULL) {
		if (len < 0) len = _uls_tool_(strlen)(str);
		len2 = len;
	} else {
		len2 = -len;
	}

	if ((escstr=uls_find_escstr_in_escmap(uls_ptr(uls_litesc->escmap_pool__global), esc_ch, str, len2)) != nilptr) {
		if ((ind=uls_del_escstr(map, esc_ch)) < 0) {
			stat = -1;
		} else {
			slots_escstr[ind] = escstr;
		}
	} else {
		stat = uls_add_escstr(escmap_pool, map, esc_ch, str, len);
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(uls_register_hex_escstr)(uls_escmap_ptr_t dst_map, uls_escmap_pool_ptr_t escmap_pool,
	char esc_ch, int n, int zero_pad, int do_unicode)
{
	int rval_flags = 0;

	if (zero_pad) {
		rval_flags |= ULS_FL_ESCSTR_ZEROPAD;
	}

	if (do_unicode) {
		rval_flags |= ULS_FL_ESCSTR_MAPUNICODE;
	} else {
		rval_flags |= ULS_FL_ESCSTR_MAPHEXA;
	}

	rval_flags |= n;

	if (uls_register_escstr(escmap_pool, dst_map, esc_ch, NULL, -rval_flags) < 0) {
		return -1;
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(__uls_dup_escmap)(uls_escmap_ptr_t src_map, uls_escmap_ptr_t dst_map, uls_escmap_pool_ptr_t escmap_pool, char esc_sym, int flags)
{
	uls_decl_parray_slots(slots_escstr_src, escstr);
	uls_decl_parray_slots(slots_escstr_dst, escstr);
	int i, rval, rval_flags;
	char esc_ch, buff[4];

	dst_map->esc_sym = esc_sym;

	slots_escstr_src = uls_parray_slots(uls_ptr(src_map->escstr_list));
	slots_escstr_dst = uls_parray_slots(uls_ptr(dst_map->escstr_list));

	for (i=0; i < ULS_ESCMAP_MAPSIZE; i++) {
		slots_escstr_dst[i] = slots_escstr_src[i];
	}

	if (flags & ULS_ESCMAP_LEGACY_OCT) {
		rval_flags = ULS_FL_ESCSTR_OCTAL;
		if (flags & ULS_ESCMAP_MODERN_U_ZEROPAD) {
			rval_flags |= ULS_FL_ESCSTR_ZEROPAD;
		}
		rval_flags |= 0x3;

		for (i=0; i<10; i++) {
			esc_ch = '0' + i;
			rval = uls_register_escstr(escmap_pool, dst_map, esc_ch, NULL, -rval_flags);
			if (rval < 0) {
				return -1;
			}
		}

	} else if (flags & ULS_ESCMAP_LEGACY_ZERO) {
		buff[0] = '\0'; buff[1] = '\0';
		rval = uls_register_escstr(escmap_pool, dst_map, '0', buff, 1);
		if (rval < 0) {
			return -1;
		}
	}

	if (flags & ULS_ESCMAP_LEGACY_HEX) {
		rval = uls_register_hex_escstr(dst_map, escmap_pool, 'x', 2, flags & ULS_ESCMAP_MODERN_U_ZEROPAD, 0);
		if (rval < 0) {
			return -1;
		}
	}

	if (flags & ULS_ESCMAP_MODERN_U4) {
		rval = uls_register_hex_escstr(dst_map, escmap_pool, 'u', 4, flags & ULS_ESCMAP_MODERN_U_ZEROPAD, 1);
		if (rval < 0) {
			return -1;
		}
	}

	if (flags & ULS_ESCMAP_MODERN_U8) {
		rval = uls_register_hex_escstr(dst_map, escmap_pool, 'U', 8, flags & ULS_ESCMAP_MODERN_U_ZEROPAD, 1);
		if (rval < 0) {
			return -1;
		}
	}

	if (flags & ULS_ESCMAP_LEGACY_QUES) uls_register_escstr(escmap_pool, dst_map, '?', "?", 1);

	if (flags & ULS_ESCMAP_MODERN_SQ) uls_register_escstr(escmap_pool, dst_map, '\'', "'", 1);
	if (flags & ULS_ESCMAP_MODERN_DQ) uls_register_escstr(escmap_pool, dst_map, '"', "\"", 1);
	if (flags & ULS_ESCMAP_MODERN_BS) uls_register_escstr(escmap_pool, dst_map, '\\', "\\", 1);
	if (flags & ULS_ESCMAP_MODERN_CR) uls_register_escstr(escmap_pool, dst_map, 'r', "\r", 1);

	buff[0] = dst_map->esc_sym;
	buff[1] = '\0';
	if (flags & ULS_ESCMAP_MODERN_ESCESC) uls_register_escstr(escmap_pool, dst_map, buff[0], buff, 1);

	return 0;
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_dup_escmap)(uls_escmap_ptr_t src_map, uls_escmap_pool_ptr_t escmap_pool, char esc_sym, int flags)
{
	uls_escmap_ptr_t dst_map;

	dst_map = uls_alloc_escmap();

	if (__uls_dup_escmap(src_map, dst_map, escmap_pool, esc_sym, flags) < 0) {
		uls_dealloc_object(dst_map);
		return nilptr;
	}

	return dst_map;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_escstr_unicode_opts)(const char *lptr)
{
	int n=0, stat = 0;
	char ch;

	if (_uls_tool_(strncmp)(lptr, "\\u", 2) == 0) {
		stat |= ULS_FL_ESCSTR_MAPUNICODE;
		lptr += 2;
	} else if (_uls_tool_(strncmp)(lptr, "\\x", 2) == 0) {
		stat |= ULS_FL_ESCSTR_MAPHEXA;
		lptr += 2;
	} else {
		return 0;
	}

	if (*lptr == '0') {
		stat |= ULS_FL_ESCSTR_ZEROPAD;
		++lptr;
	}

	ch = *lptr++;
	if ((n = ch - '0') <= 0) {
		stat = -1;
	} else {
		if (*lptr == 'x') {
			stat |= ULS_FL_ESCSTR_HEXA;
		} else {
			stat = -2;
		}
	}

	if (stat >= 0) {
		stat |= n;
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__parse_escmap_optgrp)(char *line, char esc_sym)
{
	char *lptr, *wrd, *lptr2, ch;
	int flags = 0;

	for (lptr = line; lptr != NULL; ) {
		wrd = lptr;
		if ((lptr = _uls_tool_(strchr)(lptr, ',')) != NULL) {
			*lptr++ = '\0';
		}

		if (wrd[0] == '\0') {
			continue;
		}

		if (wrd[1] == '\0') {
			ch = wrd[0];
			if (ch == '\'') flags |= ULS_ESCMAP_MODERN_SQ;
			else if (ch == '"') flags |= ULS_ESCMAP_MODERN_DQ;
			else if (ch == '\\') flags |= ULS_ESCMAP_MODERN_BS;
			else if (ch == 'r') flags |= ULS_ESCMAP_MODERN_CR;
			else if (ch == '?') flags |= ULS_ESCMAP_LEGACY_QUES;
			else if (ch == '0') flags |= ULS_ESCMAP_LEGACY_ZERO;
			else if (ch == esc_sym) flags |= ULS_ESCMAP_MODERN_ESCESC;
			else {
				flags = -1;
				break;
			}

		} else if (wrd[0] == 'u') {
			lptr2 = wrd + 1;
			if (*lptr2 == '0') {
				flags |= ULS_ESCMAP_MODERN_U_ZEROPAD;
				++lptr2;
			}

			ch = *lptr2;
			if (ch == '4') {
				flags |= ULS_ESCMAP_MODERN_U4;
			} else if (ch == '8') {
				flags |= ULS_ESCMAP_MODERN_U8;
			} else {
				flags = -1;
				break;
			}

		} else if (uls_streql(wrd, "oct")) {
			flags |= ULS_ESCMAP_LEGACY_OCT;

		} else if (uls_streql(wrd, "hex")) {
			flags |= ULS_ESCMAP_LEGACY_HEX;

		} else {
			flags = -1;
			break;
		}
	}

	return flags;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(parse_escmap_optgrp)(uls_escmap_ptr_t esc_map, uls_ptrtype_tool(outparam) parms)
{
	int flags = parms->flags;
	int rval_flags, flags_err, stat = 0;
	char *lptr, *lptr1, *lptr2, *lptr3, esc_sym;
	const char *feature_group;

	/*
	legacy(',",0,?,oct,hex)
	legacy(',",oct,hex;&)
	modern(u04,u08)
	modern(r,u8;\)
	modern(r,',",u4;&)
	verbatim(\;)
	verbatim(&;&)
	verbatim(u04;)
	verbatim(\,";)
	verbatim(\,')
	*/

	lptr1 = parms->line;
	if ((lptr= _uls_tool_(strchr)(lptr1, ')')) == NULL) {
		_uls_log(err_log)("misuse of options: no matching ')'");
		return -1;
	}

	lptr3 = lptr; // indicating ')'
	*lptr++ = '\0';

	if ((lptr2 = _uls_tool_(strchr)(lptr1, ';')) != NULL) {
		*lptr2++ = '\0';
		esc_sym = *lptr2;
	}
	else {
		lptr2 = lptr3;
		esc_sym = ULS_ESCMAP_DFL_ESCSYM;
	}

	rval_flags = __parse_escmap_optgrp(lptr1, esc_sym);
	if (rval_flags < 0) {
		_uls_log(err_log)("undefined flag specified on %s", lptr1);
		return -1;
	}
	flags |= rval_flags;

	if (flags & ULS_ESCMAP_MODE__LEGACY) {
		flags_err = flags & ~(ULS_ESCMAP_OPTGROUP__LEGACY | ULS_ESCMAP_MODE__LEGACY);
		feature_group = "legacy";
	} else if (flags & ULS_ESCMAP_MODE__MODERN) {
		flags_err = flags & ~(ULS_ESCMAP_OPTGROUP__MODERN | ULS_ESCMAP_MODE__MODERN);
		feature_group = "modern";
	} else if (flags & ULS_ESCMAP_MODE__VERBATIM) {
		flags_err = flags & ~(ULS_ESCMAP_OPTGROUP__VERBATIM | ULS_ESCMAP_MODE__VERBATIM);
		feature_group = "verbatim";
	} else if (flags & ULS_ESCMAP_MODE__LEGACY_FULL) {
		flags_err = flags & ~(ULS_ESCMAP_OPTGROUP__LEGACY | ULS_ESCMAP_MODE__LEGACY);
		feature_group = "legacy_full";
	} else if (flags & ULS_ESCMAP_MODE__VERBATIM_MODERATE) {
		flags_err = flags & ~(ULS_ESCMAP_OPTGROUP__VERBATIM | ULS_ESCMAP_MODE__VERBATIM);
		feature_group = "verbatim_moderate";
	} else { /* NEVER REACHED */
		flags_err = 0;
		feature_group = "undefined";
	}

	if (flags_err != 0) {
		_uls_log(err_log)("undefined features 0x%x specified on %s", flags_err, feature_group);
		return -1;
	}

	parms->uch = esc_sym;
	parms->line = lptr;
	parms->flags = flags;

	return stat;
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_parse_escmap_feature)(uls_escmap_pool_ptr_t escmap_pool, uls_ptrtype_tool(outparam) parms)
{
	char *line = parms->line;
	char  *wrd, *lptr, *mode_str, ch, ch_bak, esc_sym;
	uls_escmap_ptr_t esc_map;
	int len, flags=0;
	uls_type_tool(outparam) parms1;

	if (*(wrd = _uls_tool(skip_blanks)(line)) == '\0') {
		esc_map = uls_ptr(uls_litesc->uls_escstr__legacy_full);
		parms->line = NULL;
		return esc_map;
	}

	for (lptr = wrd + 1; (ch=*lptr) != '\0'; lptr++) {
		if (!_uls_tool_(isalnum)(ch)) break;
	}

	len = (int) (lptr - wrd);
	ch_bak = wrd[len];
	wrd[len] = '\0';
	mode_str = wrd;

	if (uls_streql(mode_str, "verbatim0")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__verbatim);
		parms->line = NULL;
		return esc_map;
	} else if (uls_streql(mode_str, "verbatim1")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__verbatim_moderate);
		parms->line = NULL;
		return esc_map;
	}

	if (uls_streql(mode_str, "legacy")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__legacy);
		flags |= ULS_ESCMAP_MODE__LEGACY;

	} else if (uls_streql(mode_str, "verbatim")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__verbatim);
		flags |= ULS_ESCMAP_MODE__VERBATIM;

	} else if (uls_streql(mode_str, "modern")) {
		esc_map = uls_ptr(uls_litesc->uls_escstr__modern);
		flags |= ULS_ESCMAP_MODE__MODERN;

	} else {
		esc_map = uls_ptr(uls_litesc->uls_escstr__legacy_full);
		esc_map = uls_dup_escmap(esc_map, escmap_pool, ULS_ESCMAP_DFL_ESCSYM, 0);
		if (ch_bak != '\0') wrd[len] = ch_bak;
		parms->line = lptr = wrd; // rollback
		return esc_map;
	}

	if (ch_bak != '\0') wrd[len] = ch_bak;

	if (*lptr == '(') {
		parms1.line = ++lptr;
		parms1.flags = flags;

		if (parse_escmap_optgrp(esc_map, uls_ptr(parms1)) < 0) {
			parms->line = NULL;
			return nilptr;
		}

		lptr = parms1.line;
		flags = parms1.flags;
		esc_sym = (char) parms1.uch;

		esc_map = uls_dup_escmap(esc_map, escmap_pool, esc_sym, flags);

	} else {
		if (*(lptr = _uls_tool(skip_blanks)(lptr)) != '\0') {
			esc_map = uls_dup_escmap(esc_map, escmap_pool, ULS_ESCMAP_DFL_ESCSYM, 0);
		} else {
			lptr = NULL;
		}
	}

	parms->line = lptr;
	return esc_map;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(extract_escstr_mapexpr)(uls_ptrtype_tool(outparam) parms)
{
	char  *lptr = (char *) parms->lptr, *lptr1;
	int len, ret_flag = 0;
	uls_type_tool(outparam) parms1;
	uls_type_tool(wrd) wrdx;

	lptr = _uls_tool(skip_blanks)(lptr);
	if (*lptr == '\0') return -1;
	parms->uch = *lptr;

	if (lptr[1] != ':') {
		_uls_log(err_log)("incorrect format!");
		return -2;
	}
	lptr += 2;

	if (*lptr == '\'') {
		++lptr;
		parms1.lptr = lptr;
		parms1.line = parms->line;
		len = uls_get_simple_escape_str('\'', uls_ptr(parms1));
		lptr = (char *) parms1.lptr;

	} else {
		wrdx.lptr = lptr1 = lptr;
		lptr1 = __uls_tool_(splitstr)(uls_ptr(wrdx));
		lptr = wrdx.lptr;

		if ((ret_flag = get_escstr_unicode_opts(lptr1)) < 0) {
			len = -1;
		} else if (ret_flag > 0) {
			len = 0; // NA
		} else { // ret_flag == 0
			parms1.lptr = lptr1;
			parms1.line = parms->line;
			len = uls_get_simple_escape_str('\0', uls_ptr(parms1));
		}
	}

	if (len >= 0) {
		parms->lptr = lptr;
		parms->len = len;
		parms->flags = ret_flag;
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(uls_parse_escmap_mapping)(uls_escmap_ptr_t esc_map, uls_escmap_pool_ptr_t escmap_pool, char *line)
{
	int len, rval, rval_flags, stat = 0;
	char  *lptr, *escstr_buf, esc_ch;
	uls_type_tool(outparam) parms1;

	len = _uls_tool_(strlen)(line);
	escstr_buf = (char *)_uls_tool_(malloc)(len + 1);

	for (lptr = line; ; ) {
		parms1.lptr = lptr;
		parms1.line = escstr_buf;

		if ((rval=extract_escstr_mapexpr(uls_ptr(parms1))) <= -1) {
			if (rval < -1) {
				_uls_log(err_log)("failed to extract esc-str!");
				stat = -1;
			}
			break;
		}

		lptr = (char *) parms1.lptr;
		esc_ch = (char) parms1.uch;
		rval_flags = parms1.flags;

		if (rval_flags & (ULS_FL_ESCSTR_MAPUNICODE|ULS_FL_ESCSTR_MAPHEXA)) {
			rval = uls_register_escstr(escmap_pool, esc_map, esc_ch, NULL, rval_flags);
		} else {
			len = parms1.len;
			rval = uls_register_escstr(escmap_pool, esc_map, esc_ch, escstr_buf, len);
		}

		if (rval < 0) {
			stat = -1;
			break;
		}
	}

	uls_mfree(escstr_buf);
	return stat;
}

ULS_QUALIFIED_RETTYP(uls_escmap_ptr_t)
ULS_QUALIFIED_METHOD(uls_parse_escmap)(char *line, uls_escmap_pool_ptr_t escmap_pool)
{
	uls_escmap_ptr_t esc_map;
	uls_type_tool(outparam) parms1;

	parms1.line = line;
	esc_map = uls_parse_escmap_feature(escmap_pool, uls_ptr(parms1));
	line = parms1.line;

	if (esc_map != nilptr && line != NULL) {
		if (uls_parse_escmap_mapping(esc_map, escmap_pool, line) < 0) {
			uls_dealloc_escmap(esc_map);
			esc_map = nilptr;
		}
	}

	return esc_map;
}

int
ULS_QUALIFIED_METHOD(uls_dec_escaped_char)(uls_escmap_ptr_t map, uls_ptrtype_tool(outparam) parms, _uls_ptrtype_tool(csz_str) cszbuf)
{
	char esc_ch = (char) parms->x1;
	const char *lptr1;
	uls_escstr_ptr_t escstr;
	char buff[8];
	uls_uch_t uch;
	int n, len, map_flags;

	if ((escstr = uls_find_escstr(map, esc_ch)) == nilptr) {
		// copy it verbatim
		buff[0] = map->esc_sym; buff[1] = esc_ch;
		_uls_tool(csz_append)(cszbuf, buff, 2);
		return 2;
	}

	if ((lptr1 = escstr->str) != NULL) {
		len = escstr->len;
		_uls_tool(csz_append)(cszbuf, lptr1, len);

	} else {
		map_flags = escstr->len;
		n = map_flags & ULS_FL_ESCSTR_MASK;

		if (_uls_tool_(isdigit)(esc_ch)) {
			uch = esc_ch - '0';
		} else {
			uch = 0;
		}

		len = -n;
		parms->uch = uch;
		parms->flags = map_flags;
	}

	return len;
}

int
ULS_QUALIFIED_METHOD(initialize_uls_litesc)()
{
	uls_escmap_pool_ptr_t escmap_pool;
	uls_escmap_ptr_t map;
	int i, rval_flags;
	char buff[4], esc_ch;

	uls_litesc = uls_alloc_object_clear(uls_litesc_sysinfo_t);

	escmap_pool = uls_ptr(uls_litesc->escmap_pool__global);
	uls_init_escmap_pool(escmap_pool);

	buff[0] = ULS_ESCMAP_DFL_ESCSYM;
	buff[1] = '\0';

	/* legacy */
	map = uls_ptr(uls_litesc->uls_escstr__legacy);
	uls_init_escmap(map);
	map->flags |= ULS_FL_STATIC | ULS_ESCMAP_SYSTEM;

	uls_add_escstr(escmap_pool, map, 'n', "\n", 1);
	uls_add_escstr(escmap_pool, map, 'r', "\r", 1);
	uls_add_escstr(escmap_pool, map, 't', "\t", 1);
	uls_add_escstr(escmap_pool, map, 'b', "\b", 1);
	uls_add_escstr(escmap_pool, map, 'a', "\a", 1);
	uls_add_escstr(escmap_pool, map, 'v', "\v", 1);
	uls_add_escstr(escmap_pool, map, 'f', "\f", 1);

	/* legacy-full */
	map = uls_ptr(uls_litesc->uls_escstr__legacy_full);
	uls_init_escmap(map);
	map->flags |= ULS_FL_STATIC | ULS_ESCMAP_SYSTEM;

	uls_add_escstr(escmap_pool, map, 'n', "\n", 1);
	uls_add_escstr(escmap_pool, map, 'r', "\r", 1);
	uls_add_escstr(escmap_pool, map, 't', "\t", 1);
	uls_add_escstr(escmap_pool, map, 'b', "\b", 1);
	uls_add_escstr(escmap_pool, map, 'a', "\a", 1);
	uls_add_escstr(escmap_pool, map, 'v', "\v", 1);
	uls_add_escstr(escmap_pool, map, 'f', "\f", 1);
	uls_add_escstr(escmap_pool, map, '"', "\"", 1);
	uls_add_escstr(escmap_pool, map, '\'', "'", 1);
	uls_add_escstr(escmap_pool, map, ULS_ESCMAP_DFL_ESCSYM, buff, 1);

	// octal
	rval_flags = ULS_FL_ESCSTR_MAPHEXA | ULS_FL_ESCSTR_OCTAL | 0x02;

	for (i=0; i<10; i++) {
		esc_ch = '0' + i;
		if (uls_add_escstr(escmap_pool, map, esc_ch, NULL, rval_flags) < 0) {
			_uls_log(err_log)("%s: failed at escape-octal", __FUNCTION__);
		}
	}

	// hexa-demcimal
	rval_flags = ULS_FL_ESCSTR_MAPHEXA | ULS_FL_ESCSTR_HEXA | 0x02;
	if (uls_add_escstr(escmap_pool, map, 'x', NULL, rval_flags) < 0) {
		_uls_log(err_log)("%s: failed at escape-hexa-decimal", __FUNCTION__);
	}

	/* modern */
	map = uls_ptr(uls_litesc->uls_escstr__modern);
	uls_init_escmap(map);
	map->flags |= ULS_FL_STATIC | ULS_ESCMAP_SYSTEM;

	uls_add_escstr(escmap_pool, map, 'n', "\n", 1);
	uls_add_escstr(escmap_pool, map, 't', "\t", 1);

	/* verbatim */
	map = uls_ptr(uls_litesc->uls_escstr__verbatim);
	uls_init_escmap(map);
	map->flags |= ULS_FL_STATIC | ULS_ESCMAP_SYSTEM;

	/* verbatim_moderate */
	map = uls_ptr(uls_litesc->uls_escstr__verbatim_moderate);
	uls_init_escmap(map);
	map->flags |= ULS_FL_STATIC | ULS_ESCMAP_SYSTEM;
	uls_add_escstr(escmap_pool, map, ULS_ESCMAP_DFL_ESCSYM, buff, 1);

	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_uls_litesc)()
{
	/* modern */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__modern));

	/* verbatim_moderate */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__verbatim_moderate));

	/* verbatim */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__verbatim));

	/* legacy-full */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__legacy_full));

	/* legacy */
	__uls_deinit_escmap(uls_ptr(uls_litesc->uls_escstr__legacy));

	/* dealloc string pool */
	uls_deinit_escmap_pool(uls_ptr(uls_litesc->escmap_pool__global));

	uls_dealloc_object(uls_litesc);
}
