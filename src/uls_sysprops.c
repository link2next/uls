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
  <file> uls_sysinfo.c </file>
  <brief>
    Managing system properities of ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2014.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_SYSPROPS__
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#endif

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__init_system_info)(uls_sysinfo_ptr_t sysinfo, int poolsiz)
{
	sysinfo->home_dir = sysinfo->etc_dir = NULL;
	sysinfo->ulcs_dir = NULL;
	sysinfo->ULS_BYTE_ORDER = uls_host_byteorder();

	sysinfo->LDBL_IEEE754_FMT = uls_check_longdouble_fmt(sysinfo->ULS_BYTE_ORDER);
	if (sysinfo->LDBL_IEEE754_FMT == ULS_IEEE754_BINARY64) {
		sysinfo->LDOUBLE_SIZE_BYTES = DOUBLE_SIZE_BYTES;
		sysinfo->LDOUBLE_EXPOSIZE_BITS = DOUBLE_EXPOSIZE_BITS;
		sysinfo->LDOUBLE_EXPO_BIAS = DOUBLE_EXPO_BIAS;
	} else if (sysinfo->LDBL_IEEE754_FMT == ULS_IEEE754_BINARY80) {
		sysinfo->LDOUBLE_SIZE_BYTES = 10;
		sysinfo->LDOUBLE_EXPOSIZE_BITS = 15;
		sysinfo->LDOUBLE_EXPO_BIAS = 16383;
	} else if (sysinfo->LDBL_IEEE754_FMT == ULS_IEEE754_BINARY128) {
		sysinfo->LDOUBLE_SIZE_BYTES = 16;
		sysinfo->LDOUBLE_EXPOSIZE_BITS = 15;
		sysinfo->LDOUBLE_EXPO_BIAS = 16383;
	} else {
		return -1;
	}

	sysinfo->LDOUBLE_SIZE_BITS = sysinfo->LDOUBLE_SIZE_BYTES*8;
	sysinfo->LDOUBLE_MENTISA_STARTBIT = 1 + sysinfo->LDOUBLE_EXPOSIZE_BITS;

	uls_mfree(sysinfo->ULC_SEARCH_PATH);
	sysinfo->ULC_SEARCH_PATH = NULL;

	if (sysinfo->n_alloc_strpool > 0) {
		uls_mfree(sysinfo->strpool);
		sysinfo->strpool = NULL;
	}

	if (poolsiz > 0) {
		sysinfo->n_alloc_strpool = poolsiz;
		sysinfo->strpool = (char *) _uls_tool_(malloc)(poolsiz);
	}

	sysinfo->n_strpool = 0;
	return 0;
}

ULS_DECL_STATIC ULS_QUALIFIED_RETTYP(uls_sysprop_ptr_t)
ULS_QUALIFIED_METHOD(__get_system_property)(uls_sysinfo_ptr_t sysinfo, const char *name)
{
	uls_sysprop_ptr_t sys_prop;
	int i;

	for (i=0; i<sysinfo->n_properties; i++) {
		sys_prop = uls_get_array_slot_type00(uls_ptr(sysinfo->properties), i);
		if (uls_streql(name, uls_get_namebuf_value_this(sys_prop->name))) {
			return sys_prop;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC char*
ULS_QUALIFIED_METHOD(get_nameval_pair)(uls_parm_line_ptr_t parm_ln)
{
	char *line0, *line, *lptr, ch;
	int  i;

	line0 = lptr = parm_ln->line;

	if ((lptr = (char *) uls_strchr(lptr, '=')) == NULL) {
		return NULL;
	}

	*lptr++ = '\0';
	line = lptr;

	for (i=(int)uls_strlen(line)-1; i>=0; i--) {
		ch = line[i];
		if (!(ch==' ' || ch=='\t' || ch=='\n' || ch=='\r')) {
			break;
		}
	}

	if (line[0] == '\'') {
		if (line[i] != '\'') {
			return NULL;
		}
		line[i--] = '\0';
		++line; --i;
	} else {
		line[++i] = '\0';
	}

	parm_ln->line = line;
	return line0;
}

void
ULS_QUALIFIED_METHOD(uls_init_sysprop)(uls_sysprop_ptr_t sys_prop)
{
	uls_init_namebuf_this(sys_prop->name, ULS_LEXSTR_MAXSIZ);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_sysprop)(uls_sysprop_ptr_t sys_prop)
{
	uls_deinit_namebuf_this(sys_prop->name);
}

void
ULS_QUALIFIED_METHOD(uls_init_sysinfo)(uls_sysinfo_ptr_t sysinfo)
{
	uls_initial_zerofy_object(sysinfo);
	__init_system_info(sysinfo, 0);
	uls_init_array_type00(uls_ptr(sysinfo->properties), sysprop, ULS_N_SYSPROPS);
}

void
ULS_QUALIFIED_METHOD(uls_deinit_sysinfo)(uls_sysinfo_ptr_t sysinfo)
{
	uls_sysprop_ptr_t sys_prop;
	int i;

	__init_system_info(sysinfo, 0);

	for (i=0; i<sysinfo->n_properties; i++) {
		sys_prop = uls_get_array_slot_type00(uls_ptr(sysinfo->properties), i);
		uls_deinit_sysprop(sys_prop);
	}
	sysinfo->n_properties = 0;

	uls_deinit_array_type00(uls_ptr(sysinfo->properties), sysprop);
}

ULS_QUALIFIED_RETTYP(uls_sysinfo_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_sysinfo)(void)
{
	uls_sysinfo_ptr_t sysinfo;

	sysinfo = uls_alloc_object(uls_sysinfo_t);
	uls_init_sysinfo(sysinfo);

	return sysinfo;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_sysinfo)(uls_sysinfo_ptr_t sysinfo)
{
	uls_deinit_sysinfo(sysinfo);
	uls_dealloc_object(sysinfo);
}

int
ULS_QUALIFIED_METHOD(uls_load_system_properties)(const char *fpath, uls_sysinfo_ptr_t sysinfo)
{
	FILE *fp;
	char linebuff[ULS_LINEBUFF_SIZ+1];
	char *line, *name;
	int len, stat=0;
	uls_parm_line_t parm_ln;

	if (__init_system_info(sysinfo, ULS_PROPPOOL_DFLSIZ) < 0) {
		return -1;
	}

	if ((fp = uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		return -1;
	}

	while (1) {
		if ((len = uls_fp_getline(fp, linebuff, sizeof(linebuff))) < 0) {
			if (len < -1) {
				stat = -2;
			}
			break;
		}

		if (*(line = skip_blanks(linebuff)) == '\0') {
			continue;
		}

		parm_ln.line = line;
		name = get_nameval_pair(uls_ptr(parm_ln));
		line = parm_ln.line;

		if (uls_add_system_property(name, line) == NULL) {
			stat = -1;
			break;
		}
	}

	uls_fp_close(fp);
	return stat;
}

void
ULS_QUALIFIED_METHOD(uls_arch2be_array)(char *ary, int n)
{
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
		uls_reverse_bytes(ary, n);
	}
}

void
ULS_QUALIFIED_METHOD(uls_be2arch_array)(char *ary, int n)
{
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_LITTLE_ENDIAN) {
		uls_reverse_bytes(ary, n);
	}
}

void
ULS_QUALIFIED_METHOD(uls_arch2le_array)(char *ary, int n)
{
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_BIG_ENDIAN) {
		uls_reverse_bytes(ary, n);
	}
}

void
ULS_QUALIFIED_METHOD(uls_le2arch_array)(char *ary, int n)
{
	if (_uls_sysinfo_(ULS_BYTE_ORDER) == ULS_BIG_ENDIAN) {
		uls_reverse_bytes(ary, n);
	}
}

const char*
ULS_QUALIFIED_METHOD(uls_add_system_property)(const char *name, const char *val)
{
	uls_sysinfo_ptr_t sysinfo = uls_sysinfo;
	uls_sysprop_ptr_t sys_prop;
	char *cptr;
	int len;

	if ((sys_prop = __get_system_property(sysinfo, name)) == nilptr) {
		if (sysinfo->n_properties >= ULS_N_SYSPROPS) {
			return NULL;
		}

		sys_prop = uls_get_array_slot_type00(uls_ptr(sysinfo->properties), sysinfo->n_properties);
		++sysinfo->n_properties;

		uls_init_sysprop(sys_prop);
		uls_set_namebuf_value_this(sys_prop->name, name);
	}

	len = uls_strlen(val) + 1;
	if (sysinfo->n_strpool + len > sysinfo->n_alloc_strpool) {
		sysinfo->n_alloc_strpool = sysinfo->n_strpool + len + ULS_PROPPOOL_DFLSIZ;
		sysinfo->strpool = (char *) uls_mrealloc(sysinfo->strpool, sysinfo->n_alloc_strpool);
	}

	cptr = sysinfo->strpool + (sys_prop->stridx = sysinfo->n_strpool);
	uls_strcpy(cptr, val);
	sysinfo->n_strpool += len;

	return cptr;
}

const char*
ULS_QUALIFIED_METHOD(uls_get_system_property)(const char *name)
{
	uls_sysinfo_ptr_t sysinfo = uls_sysinfo;
	uls_sysprop_ptr_t sys_prop;
	const char *val;

	if ((sys_prop = __get_system_property(sysinfo, name)) != nilptr) {
		val = sysinfo->strpool + sys_prop->stridx;
	} else {
		val = NULL;
	}

	return val;
}

int
ULS_QUALIFIED_METHOD(initialize_sysprops)(const char *fpath)
{
	int rc;

	uls_sysinfo = uls_create_sysinfo();

	if ((rc = uls_load_system_properties(fpath, uls_sysinfo)) < 0) {
		return -1;
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(finalize_sysprops)(void)
{
	uls_destroy_sysinfo(uls_sysinfo);
	uls_sysinfo = nilptr;
}
