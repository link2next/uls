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
  <file> uls_util.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_MISC__
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"

#ifdef ULS_WINDOWS
#include "uls/uls_util_astr.h"
#include "uls/uls_util_wstr.h"
#else
#include <time.h>
#endif
#endif

ULS_DECL_STATIC unsigned int
ULS_QUALIFIED_METHOD(uls_gauss_log2)(unsigned int n, uls_outparam_ptr_t parms)
{
	unsigned int i, n_bits=sizeof(unsigned int)<<3;
	unsigned int m, m_prev;

	m_prev = 1;
	for (i=0; i<n_bits; i++) {
		if (!(m = 1 << i) || n<m) {
			break;
		}
		m_prev = m;
	}

	parms->x1 = m_prev;
	parms->x2 = i - 1;

	return parms->x2;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(downheap_vptr)(uls_heaparray_ptr_t hh, unsigned int i0)
{
	uls_sort_cmpfunc_t cmpfunc = hh->cmpfunc;
	unsigned int left, right, k;
	uls_voidptr_t m, m1, m2;

	m = hh->ary[i0];

	if ((left=uls_heaparray_lchild(i0)) >= hh->n_ary) {
		return;
	}

	m1 = hh->ary[left];

	if ((right=left+1) >= hh->n_ary) {
		if (cmpfunc(m, m1) < 0) {
			k = left;
		} else {
			return;
		}
	} else {
		m2 = hh->ary[right];
		if (cmpfunc(m, m1) < 0) {
			if (cmpfunc(m1, m2) < 0) {
				k = right;
			} else {
				k = left;
			}
		} else if (cmpfunc(m, m2) < 0) {
			k = right;
		} else {
			return;
		}
	}

	// swap ary[i0], ary[k]
	hh->ary[i0] = hh->ary[k];
	hh->ary[k] = m;

	downheap_vptr(hh, k);
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(extract_top_vptr)(uls_heaparray_ptr_t hh)
{
	unsigned int n2;
	_uls_decl_array(ary,uls_voidptr_t) = hh->ary;
	uls_voidptr_t m;

	m = ary[0]; // the first element

	n2 = hh->n_ary - 1; // the last one
	ary[0] = ary[n2];
	downheap_vptr(hh, 0);

	ary[n2] = m; // to arrange the sorted element.
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(get_ms_codepage)(uls_outparam_ptr_t parms)
{
	const char *name = parms->lptr;
	uls_outparam_t parms1;
	const char *cptr;
	int n, mbs;

	if (name[0] != 'c' || name[1] != 'p' || !uls_isdigit(name[2]))
		return -1;

	parms1.lptr = name + 2;
	n = (int) uls_skip_atou(uls_ptr(parms1));
	cptr = parms1.lptr;
	if (*cptr != '\0') return -1;

	if (n == 932 || n == 936 || n == 949 || n == 950 ||
		n == 20932 || n == 20936 || n == 20949 ||
		n == 51932 || n == 51936 || n == 51949 || n == 51950) {
		mbs = 2;
	} else {
		mbs = 1;
	}

	parms->n = mbs;
	return n;
}

#ifndef ULS_DOTNET
ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(sortcmp_obj4sort)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_obj4sort_ptr_t e1 = (const uls_obj4sort_ptr_t) a;
	const uls_obj4sort_ptr_t e2 = (const uls_obj4sort_ptr_t) b;

	return e1->cmpfunc(e1->vptr, e2->vptr);
}
#endif

void
ULS_QUALIFIED_METHOD(build_heaptree_vptr)(uls_heaparray_ptr_t hh,
	_uls_decl_array(ary,uls_voidptr_t), unsigned int n, uls_sort_cmpfunc_t cmpfunc)
{
	unsigned int two_exp, i2, j, lvl;
	uls_outparam_t parms1;

	hh->ary = ary;
	hh->n_ary = hh->ary_siz = n;
	hh->cmpfunc = cmpfunc;

	lvl = uls_gauss_log2(n, uls_ptr(parms1)) + 1;
	two_exp = parms1.x1;

	i2 = two_exp - 1;
	lvl = uls_gauss_log2(i2, uls_ptr(parms1)) + 1;
	two_exp = parms1.x1;

	while (i2 > 0) {
		for (j=two_exp-1; j<i2; j++) {
			downheap_vptr(hh, j);
		}

		i2 = two_exp - 1;
		two_exp >>= 1;
		lvl--;
	}
}

int
ULS_QUALIFIED_METHOD(__initialize_uls_misc)(void)
{
	char pathbuff[ULS_FILEPATH_MAX+1];
	const char *fpath, *cptr;

	int rc, mbs, len;
	uls_outparam_t parms;

	initialize_primitives();
	initialize_csz();
#ifndef ULS_DOTNET
	initialize_uls_lf();
	uls_add_default_convspecs(uls_lf_get_default());
	initialize_uls_sysprn();
	initialize_uls_syserr();
#endif

	if ((fpath = getenv("ULS_SYSPROPS")) == NULL || uls_dirent_exist(fpath) != ST_MODE_REG) {
		fpath = ULS_SYSPROPS_FPATH;

		if ((rc = uls_dirent_exist(fpath)) <= 0 || rc != ST_MODE_REG) {
			len = uls_strcpy(pathbuff, ULS_SHARE_DFLDIR);
			pathbuff[len++] = ULS_FILEPATH_DELIM;
			len += uls_strcpy(pathbuff + len, TMP_SYSPROPS_FNAME);

			fpath = pathbuff;
			if ((rc = uls_dirent_exist(fpath)) <= 0 || rc != ST_MODE_REG) {
				_uls_log(err_log)("ULS: can't find the system property file in %s.", ULS_SYSPROPS_FPATH);
				return -1;
			}
		}
	}

	if ((rc=initialize_sysprops(fpath)) < 0) {
		_uls_log(err_log)("ULS: can't load the system property file in %s(err=%d).", fpath, rc);
		return -1;
	}

	initialize_uls_fileio();

	if ((_uls_sysinfo_(home_dir) = uls_get_system_property("ULS_HOME")) == NULL) {
#if defined(ULS_WINDOWS) && !defined(ULS_DOTNET)
		char *homedir;

		parms.line = (char *) ULS_REG_INSTDIR_NAME;
		homedir = uls_win32_lookup_regval(ULS_REG_HOME, uls_ptr(parms));
		rc = parms.n;

		if (homedir == NULL) {
			_uls_log(err_log)("ULS: don't know about the installation information.");
			return -1;
		}

		_uls_sysinfo_(home_dir) = uls_add_system_property("ULS_HOME", homedir);
		uls_mfree(homedir);
#else
		_uls_log(err_log)("ULS: don't know the installed directory.");
		return -1;
#endif
	}

	if ((_uls_sysinfo_(etc_dir) = uls_get_system_property("ULS_ETC")) == NULL) {
		_uls_log(err_log)("ULS: can't find etc dir for uls!");
		return -1;
	}

	if ((_uls_sysinfo_(ulcs_dir) = uls_get_system_property("ULS_ULCS")) == NULL) {
		_uls_sysinfo_(ulcs_dir) = ULS_SHARE_DFLDIR;
	}

	if ((cptr = uls_get_system_property("ULS_MBCS")) == NULL) {
		_uls_log(err_log)("ULS: can't find the encoding of the system!");
		return -1;
	}

	if (uls_streql(cptr, "utf8")) {
		_uls_sysinfo_(encoding) = ULS_MBCS_UTF8;

	} else {
		parms.lptr = cptr;
		rc = get_ms_codepage(uls_ptr(parms));
		mbs = parms.n;

		if (rc >= 0) {
			_uls_sysinfo_(encoding) = ULS_MBCS_MS_MBCS;
			_uls_sysinfo_(codepage) = rc;
			_uls_sysinfo_(multibytes) = mbs;

		} else {
			_uls_log(err_log)("%s: unknown file-encoding %s", cptr);
			return -1;
		}
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(__finalize_uls_misc)(void)
{
	finalize_uls_fileio();
	finalize_sysprops();
#ifndef ULS_DOTNET
	finalize_uls_syserr();
	finalize_uls_sysprn();
	finalize_uls_lf();
#endif
	finalize_csz();
	finalize_primitives();
}

int
ULS_QUALIFIED_METHOD(splitint)(const char* line, _uls_tool_ptrtype_(outparam) parms)
{
	int   n, i=parms->n;
	int   minus=0, ch;

	for ( ; (ch=line[i])==' ' || ch=='\t'; i++)
		/* NOTHING */;

	if (line[i] == '-') {
		minus = 1;
		++i;
	}

	if (!uls_isdigit(ch=line[i])) {
		return 0;
	} else {
		n = ch - '0';
		++i;
	}

	for ( ; uls_isdigit(ch=line[i]); i++) {
		n = n*10 + (ch - '0');
	}

	if (minus) n = -n;
	parms->n = i;

	return n;
}

int
ULS_QUALIFIED_METHOD(canbe_tokname)(const char *str)
{
	int i, val;
	char ch;

	for (i=0; (ch=str[i])!='\0'; i++) {
		if (i > 0) {
			val = uls_isalnum(ch) || (ch == '_');
		} else {
			val = uls_isalpha(ch) || (ch == '_');
		}
		if (val == 0) return 0;
	}

	if (i > ULS_LEXSTR_MAXSIZ)
		return 0;

	return i;
}

const char*
ULS_QUALIFIED_METHOD(uls_skip_multiline_comment)(_uls_tool_ptrtype_(parm_line) parm_ln)
{
	const char* lptr = parm_ln->lptr, *lptr_end=parm_ln->lptr_end;
	int  ch, prev_ch, n=0;

	/*
	 * ret-val == NULL : NOT FOUND, the end of c-style comment
	 * ret-val == next-char behind the end of comment area
	 */
	for (prev_ch=-1; ; lptr++) {
		if (lptr == lptr_end) {
			lptr = NULL;
			break;
		}

		if ((ch=*lptr) == '\n') ++n;

		if (prev_ch == '*' && ch=='/') {
			++lptr;
			break;
		}

		prev_ch = ch;
	}

	parm_ln->len = n;
	return lptr;
}

const char*
ULS_QUALIFIED_METHOD(uls_skip_singleline_comment)(_uls_tool_ptrtype_(parm_line) parm_ln)
{
	const char* lptr = parm_ln->lptr, *lptr_end=parm_ln->lptr_end;
	int  ch;

	/*
	 * ret-val == NULL 0 : error
	 *ret-val == 0 : c++-style comment completed
	 */
	for ( ; lptr != lptr_end; lptr++) {
		ch = *lptr;
		if (ch =='\n') return lptr + 1;
	}

	return NULL;
}

#ifndef ULS_DOTNET

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_quick_sort)(uls_native_vptr_t ary, int n_ary, int elmt_size, uls_sort_cmpfunc_t cmpfunc)
{
	uls_decl_parray(obj4_sort_ary, obj4sort);
	uls_decl_parray_slots(slots_obj, obj4sort);
	uls_obj4sort_ptr_t obj4sort;
	uls_native_vptr_t ary_bak, ei, ej;
	int i, siz;

	if (n_ary <= 0) return;

	uls_init_parray(uls_ptr(obj4_sort_ary), obj4sort, n_ary);
	slots_obj = uls_parray_slots(uls_ptr(obj4_sort_ary));

	for (i=0; i<n_ary; i++) {
		obj4sort = slots_obj[i] = uls_alloc_object(uls_obj4sort_t);

		obj4sort->vptr = (char *) ary + i * elmt_size;
		obj4sort->idx = i;
		obj4sort->cmpfunc = cmpfunc;
	}

	_uls_quicksort_vptr(slots_obj, n_ary, sortcmp_obj4sort);

	siz = n_ary * elmt_size;
	ary_bak = uls_malloc(siz);
	uls_memcopy(ary_bak, ary, siz);

	for (i=0; i<n_ary; i++) {
		obj4sort = slots_obj[i];

		ei = (char *) ary + i * elmt_size;
		ej = (char *) ary_bak + obj4sort->idx * elmt_size;

		uls_memcopy(ei, ej, elmt_size);

		uls_dealloc_object(obj4sort);
		slots_obj[i] = nilptr;
	}

	uls_mfree(ary_bak);
	uls_deinit_parray(uls_ptr(obj4_sort_ary));
}

ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_bi_search)(const uls_voidptr_t keyw,
	uls_native_vptr_t ary, int n_ary, int elmt_size, uls_bi_comp_t cmpfunc)
{
	int   low, high, mid, cond;
	uls_native_vptr_t  e;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e = (char *) ary + mid * elmt_size;

		if ((cond=cmpfunc(e, keyw)) < 0) {
			low = mid + 1;
		} else if (cond > 0) {
			high = mid - 1;
		} else {
			return e;
		}
	}

	return nilptr;
}
#endif // ULS_DOTNET

ULS_DLL_EXTERN void
ULS_QUALIFIED_METHOD(uls_quick_sort_vptr)(_uls_decl_array(ary,uls_voidptr_t),
	int n_ary, uls_sort_cmpfunc_t cmpfunc)
{
	uls_heaparray_t heap_array;
	uls_voidptr_t m;

	if (n_ary <= 1) return;

	build_heaptree_vptr(uls_ptr(heap_array), ary, n_ary, cmpfunc);

	for ( ; heap_array.n_ary > 2; --heap_array.n_ary) {
		extract_top_vptr(uls_ptr(heap_array));
	}

	// swap(heap_array.ary[0], heap_array.ary[1])
	m = heap_array.ary[0];
	heap_array.ary[0] = heap_array.ary[1];
	heap_array.ary[1] = m;

	heap_array.n_ary = heap_array.ary_siz;
}

ULS_DLL_EXTERN uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_bi_search_vptr)(const uls_voidptr_t kwrd,
	_uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_bi_comp_t cmpfunc)
{
	int   low, high, mid, cond;
	uls_voidptr_t e;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e = ary[mid];

		if ((cond=cmpfunc(e, kwrd)) < 0) {
			low = mid + 1;
		} else if (cond > 0) {
			high = mid - 1;
		} else {
			return e;
		}
	}

	return nilptr;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_cmd_run)(uls_array_ref_slots_this_type01(cmdlst,cmd), int n_cmdlst, const char* kwrd,
	char *line, uls_voidptr_t data)
{
	int stat = -2;
	int   low, high, mid, cond;
	uls_cmd_ptr_t cmd;

	low = 0;
	high = n_cmdlst - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		cmd = uls_get_array_this_slot(cmdlst,mid);

		if ((cond = uls_strcmp(cmd->name, kwrd)) < 0) {
			low = mid + 1;
		} else if (cond > 0) {
			high = mid - 1;
		} else {
			cmd->user_data = data;
			if (cmd->proc(line, cmd) < 0) {
				stat = -1;
			} else {
				stat = 0;
			}
			break;
		}
	}

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(replace_cr_guard)(char *buff, int n, char crlf2chlf)
{
	char ch, ch_next;
	int i, i_cr, m;

	for (i=0; i < n - 1; i += m) {
		ch = buff[i];
		ch_next = buff[i + 1];

		i_cr = -1;
		if (ch == '\n') {
			if (ch_next == '\r') { // LF, CR
				buff[i] = '\r'; buff[i+1] = '\n';
				i_cr = i;
				m = 2;
			}
			else if (ch_next == '\n') { // LF, LF
				m = 1;
			}
			else { // LF x
				m = 2;
			}

		} else if (ch == '\r') {
			if (ch_next == '\r') { // CR, CR
				buff[i] = '\n';
				m = 1;
			}
			else if (ch_next == '\n') { // CR, LF
				i_cr = i;
				m = 2;
			}
			else { // CR x
				buff[i] = '\n';
				m = 2;
			}

		} else {
			if (ch_next == '\r') { // x, CR
				m = 1;
			}
			else if (ch_next == '\n') { // x, LF
				m = 1;
			}
			else { // x x
				m = 2;
			}
		}

		if (i_cr >= 0 && crlf2chlf != '\0') {
			buff[i_cr] = crlf2chlf;
		}
	}

	return i;
}

ULS_DECL_STATIC char
ULS_QUALIFIED_METHOD(check_eof_cr_lf)(char *ptr_ch)
{
	char ch_carry;

	ch_carry = *ptr_ch;
	if (ch_carry == '\r') {
		*ptr_ch = '\n';
	} else if (ch_carry != '\n') {
		ch_carry = '\0';
	}

	return ch_carry;
}

char
ULS_QUALIFIED_METHOD(uls_replace_cr_carry)(char ch_carry, char *buf, int n)
{
	int i;

	if (n <= 0) return '\0';

	if ((ch_carry == '\r' && buf[0] == '\n') ||
		(ch_carry == '\n' && buf[0] == '\r')) {
		buf[0] = ' ';
		i = 1;
	} else {
		i = 0;
	}

	if (i + 2 > n) {
		if (n - i == 1) {
			ch_carry = check_eof_cr_lf(buf + i);
		} else {
			ch_carry = '\0';
		}
		return ch_carry;
	}

	if ((i += replace_cr_guard(buf + i, n - i, ' ')) < n) {
		ch_carry = check_eof_cr_lf(buf + i);
	} else {
		ch_carry = '\0';
	}

	return ch_carry;
}

#if defined(ULS_WINDOWS) && !defined(ULS_DOTNET)

ULS_DLL_EXTERN char*
ULS_QUALIFIED_METHOD(uls_win32_lookup_regval)(wchar_t* reg_dir, uls_outparam_ptr_t parms)
{
	wchar_t* reg_name = (wchar_t*) parms->line;
	int	n_wchars, stat = -1;
	HKEY   hKeyRoot, hRegKey;
	DWORD  value_type, bufsize;
	wchar_t  *lpKeyStr, keyRootBuff[8];
	LONG   rval;

	csz_str_t csz;
	char *ustr;

	if (reg_dir == NULL) return NULL;

	if ((lpKeyStr = wcschr(reg_dir, L':')) == NULL || reg_dir == lpKeyStr ||
		(rval=(LONG)(lpKeyStr-reg_dir)) >= sizeof(keyRootBuff)/sizeof(wchar_t)) {
		_uls_log(err_log)("incorrect format of reg-dir!");
		return NULL;
	}

	wcsncpy(keyRootBuff, reg_dir, rval);
	keyRootBuff[rval] = L'\0';
	reg_dir = ++lpKeyStr;

	if (!wcscmp(keyRootBuff, L"HKLM")) {
		hKeyRoot = HKEY_LOCAL_MACHINE;
	} else if (!wcscmp(keyRootBuff, L"HKCU")) {
		hKeyRoot = HKEY_CURRENT_USER;
	} else if (!wcscmp(keyRootBuff, L"HKCC")) {
		hKeyRoot = HKEY_CURRENT_CONFIG;
	} else if (!wcscmp(keyRootBuff, L"HKCR")) {
		hKeyRoot = HKEY_CLASSES_ROOT;
	} else if  (!wcscmp(keyRootBuff, L"HKU")) {
		hKeyRoot = HKEY_USERS;
	} else {
		_uls_log(err_log)("unknown reg key!");
		return NULL;
	}

	rval = RegOpenKeyExW(hKeyRoot, reg_dir, 0, KEY_READ, &hRegKey);
	if (rval != ERROR_SUCCESS) {
		_uls_log(err_log)("Can't find the home directory of ULS.");
		return NULL;
	}

	value_type = REG_SZ;
	rval = RegQueryValueExW(hRegKey, reg_name, 0, &value_type, NULL, &bufsize);
	if (rval != ERROR_SUCCESS) {
		if (rval == ERROR_FILE_NOT_FOUND)
			_uls_log(err_log)("Error: RegQueryValueEx: 'UlsHome' Not found");
		RegCloseKey(hRegKey);
		return NULL;
	}

	if ((lpKeyStr = (wchar_t *) uls_malloc(bufsize)) == NULL ||
		(rval = RegQueryValueExW(hRegKey, reg_name, 0,
			&value_type, (LPBYTE) lpKeyStr, &bufsize)) != ERROR_SUCCESS) {
		_uls_log(err_log)("RegQueryValueEx failed");
		ustr = NULL;

	} else {
		n_wchars = bufsize/sizeof(wchar_t) - 1;
		csz_init(uls_ptr(csz), (n_wchars+1) * 2);

		if ((ustr = uls_wstr2ustr(lpKeyStr, n_wchars, uls_ptr(csz))) == NULL) {
			parms->n = -1;
		} else {
			parms->n = csz_length(uls_ptr(csz));
			ustr = csz_export(uls_ptr(csz));
		}

		csz_deinit(uls_ptr(csz));
	}

	RegCloseKey(hRegKey);
	uls_mfree(lpKeyStr);

	return ustr;
}
#endif

int
ULS_QUALIFIED_METHOD(uls_get_simple_escape_char)(uls_outparam_ptr_t parms)
{
	int processed = 1;
	char ch2;

	//
	// the legacy pattern of escaped characters in literal string
	//
	switch (parms->x1) {
	case 'n': ch2 = '\n'; break;
	case 't': ch2 = '\t'; break;
	case 'r': ch2 = '\r'; break;
	// BUGFIX-205: '\\', '\'', '"', '?' added
	case '\\': ch2 = '\\'; break;
	case '\'': ch2 = '\''; break;
	case '"': ch2 = '"'; break;
	case '?': ch2 = '?'; break;
	case 'b': ch2 = '\b'; break;
	case 'a': ch2 = '\a'; break;
	case 'f': ch2 = '\f'; break;
	case 'v': ch2 = '\v'; break;
	default :
		processed = 0;
		ch2 = (char) parms->x1;
		break;
	}

	parms->x2 = ch2;
	return processed;
}

int
ULS_QUALIFIED_METHOD(uls_get_simple_escape_str)(char quote_ch, uls_outparam_ptr_t parms)
{
	const char *lptr = parms->lptr;
	char* outbuf = parms->line;
	int rval, escape = 0, j, k=0;
	uls_outparam_t parms1;
	char ch, ch2;

	for ( ; ; lptr++) {
		if ((ch = *lptr) == '\0') {
			if (escape) {
//				err_log("the escape char('\\') at the end of string!");
				outbuf[k++] = '\\';
			} else if (quote_ch != '\0') {
				_uls_log(err_log)("unterminated literal string!");
				parms->lptr = lptr;
				return -1;
			}
			break;
		}

		if (escape) {
			if (ch == 'x') {
				for (ch2=0,j=0; j<2; j++) {
					if (!uls_isxdigit(ch=lptr[j+1])) {
						if (j == 0) {
							_uls_log(err_log)("%s: No hexa-string format!", __FUNCTION__);
							parms->lptr = lptr;
							return -1;
						}
						break;
					}
					ch2 |= uls_isdigit(ch) ? ch - '0' : 10 + (uls_toupper(ch) - 'A');
				}
				outbuf[k++] = ch2;
				lptr += j;
			} else {
				parms1.x1 = ch;
				if ((rval = uls_get_simple_escape_char(uls_ptr(parms1))) > 0) {
					outbuf[k++] = (char) parms1.x2;
				} else {
					outbuf[k++] = '\\'; outbuf[k++] = ch; // copy it verbatim
				}
			}
	
			escape = 0;

		} else {
			if (ch == quote_ch) {
				++lptr;
				break;
			}

			if (ch == '\\') {
				escape = 1;
			} else {
				outbuf[k++] = ch;
			}
		}
	}

	outbuf[k] = '\0';
	parms->lptr = lptr;

	return k;
}

void
ULS_QUALIFIED_METHOD(isp_init)(uls_isp_ptr_t isp, int init_size)
{
	if (init_size < 0)
		init_size = 512;

	isp->buff = (char *) uls_malloc(init_size);
	isp->siz_strpool = init_size;
	isp->len_strpool = 0;
}

void
ULS_QUALIFIED_METHOD(isp_reset)(uls_isp_ptr_t isp)
{
	isp->len_strpool = 1;
}

void
ULS_QUALIFIED_METHOD(isp_deinit)(uls_isp_ptr_t isp)
{
	isp->len_strpool = 0;

	if (isp->siz_strpool > 0) {
		uls_mfree(isp->buff);
		isp->buff = NULL;
	}
}

char*
ULS_QUALIFIED_METHOD(isp_find)(uls_isp_ptr_t isp, const char* str, int len)
{
	char *ptr;
	int l, ind;

	if (len < 0) {
		len = uls_strlen(str);
	}

	for (ind=0; ind < isp->len_strpool; ind += l+1) {
		ptr = isp->buff + ind;

		l = uls_strlen(ptr);
		if (l == len && uls_streql(str, ptr)) {
			return ptr;
		}
	}

	return NULL;
}

char*
ULS_QUALIFIED_METHOD(isp_insert)(uls_isp_ptr_t isp, const char* str, int len)
{
	// assert: str != NULL AND len > 0
	char *ptr;
	int i, l;

	if (len < 0) len = uls_strlen(str);

	l = isp->siz_strpool - isp->len_strpool;
	if (len + 1 > l) {
		_uls_log(err_log)("%s: isp full!", __FUNCTION__);
		return NULL;
	}

	ptr = isp->buff + isp->len_strpool;
	for (i=0; i<len; i++) *ptr++ = str[i];
	*ptr = '\0';

	l = isp->len_strpool;
	isp->len_strpool += len + 1;

	return isp->buff + l;
}
