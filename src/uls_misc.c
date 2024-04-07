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
  <file> uls_misc.c </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_MISC__
#include "uls/uls_misc.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#endif

int
ULS_QUALIFIED_METHOD(splitint)(const char* line, uls_ptrtype_tool(outparam) parms)
{
	int  i = parms->n1;
	char ch;
	int rc;

	for ( ; (ch=line[i])==' ' || ch=='\t'; i++)
		/* NOTHING */;

	rc = _uls_tool(is_pure_integer)(line + i, parms);
	if (rc < 0) rc = -rc;
	parms->n1 = i += rc;

	return parms->n;
}

const char*
ULS_QUALIFIED_METHOD(uls_skip_multiline_comment)(uls_ptrtype_tool(parm_line) parm_ln)
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
ULS_QUALIFIED_METHOD(uls_skip_singleline_comment)(uls_ptrtype_tool(parm_line) parm_ln)
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

ULS_DECL_STATIC unsigned int
ULS_QUALIFIED_METHOD(uls_gauss_log2)(unsigned int n, uls_ptrtype_tool(outparam) parms)
{
	unsigned int i, n_bits = sizeof(unsigned int)<<3;
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

#ifndef ULS_DOTNET
ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(sortcmp_obj4sort)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_obj4sort_ptr_t e1 = (const uls_obj4sort_ptr_t) a;
	const uls_obj4sort_ptr_t e2 = (const uls_obj4sort_ptr_t) b;

	return e1->cmpfunc(e1->vptr, e2->vptr);
}
#endif

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

void
ULS_QUALIFIED_METHOD(build_heaptree_vptr)(uls_heaparray_ptr_t hh,
	_uls_decl_array(ary,uls_voidptr_t), unsigned int n, uls_sort_cmpfunc_t cmpfunc)
{
	unsigned int two_exp, i2, j, lvl;
	uls_type_tool(outparam) parms1;

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

#ifndef ULS_DOTNET
void
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
	obj4_sort_ary.n = n_ary;

	_uls_quicksort_vptr(slots_obj, n_ary, sortcmp_obj4sort);

	siz = n_ary * elmt_size;
	ary_bak = _uls_tool_(malloc)(siz);
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

uls_voidptr_t
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

void
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

uls_voidptr_t
ULS_QUALIFIED_METHOD(uls_bi_search_vptr)(const uls_voidptr_t keyw,
	_uls_decl_array(ary,uls_voidptr_t), int n_ary, uls_bi_comp_t cmpfunc)
{
	int   low, high, mid, cond;
	uls_voidptr_t e;

	low = 0;
	high = n_ary - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		e = ary[mid];

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

int
ULS_QUALIFIED_METHOD(uls_get_simple_escape_char)(uls_ptrtype_tool(outparam) parms)
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
ULS_QUALIFIED_METHOD(uls_get_simple_escape_str)(char quote_ch, uls_ptrtype_tool(outparam) parms)
{
	const char *lptr = parms->lptr;
	char* outbuf = parms->line;
	uls_type_tool(outparam) parms1;
	int escape = 0, j, k=0;
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
					if (!_uls_tool_(isxdigit)(ch=lptr[j+1])) {
						if (j == 0) {
							_uls_log(err_log)("%s: No hexa-string format!", __func__);
							parms->lptr = lptr;
							return -1;
						}
						break;
					}
					ch2 |= _uls_tool_(isdigit)(ch) ? ch - '0' : 10 + (_uls_tool_(toupper)(ch) - 'A');
				}
				outbuf[k++] = ch2;
				lptr += j;
			} else {
				parms1.x1 = ch;
				if (uls_get_simple_escape_char(uls_ptr(parms1))) {
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

int
ULS_QUALIFIED_METHOD(uls_get_simple_unescape_char)(int ch)
{
	int ch2;

	switch (ch) {
	case '\n': ch2 = 'n'; break;
	case '\t': ch2 = 't'; break;
	case '\r': ch2 = 'r'; break;
	case '\b': ch2 = 'b'; break;
	case '\a': ch2 = 'a'; break;
	case '\f': ch2 = 'f'; break;
	case '\v': ch2 = 'v'; break;
	case '\\': ch2 = '\\'; break;
	case '\'': ch2 = '\''; break;
	case '"': ch2 = '"'; break;
	case '\0': ch2 = '0'; break;
	default :
		ch2 = -1;
		break;
	}

	return ch2;
}

int
ULS_QUALIFIED_METHOD(uls_get_simple_unescape_str)(uls_ptrtype_tool(outparam) parms)
{
	char* outbuf = parms->line;
	const char *lptr;
	int ch, ch2;
	int k = 0;

	for (lptr = parms->lptr; (ch=*lptr) != '\0'; lptr++) {
		if ((ch2 = uls_get_simple_unescape_char(ch)) < 0) {
			outbuf[k++] = ch;
		} else {
			outbuf[k++] = '\\';
			outbuf[k++] = ch2;
		}
	}

	outbuf[k] = '\0';
	return k;
}

FILE*
ULS_QUALIFIED_METHOD(uls_get_spec_fp)(const char* dirpath_list, const char* fpath, uls_ptrtype_tool(outparam) parms)
{
	char filepath_buff[ULS_FILEPATH_MAX+1];
	const char *fptr, *lptr0, *lptr;
	int len, len_fptr;
	FILE *fp_in;
	uls_type_tool(outparam) parms1;

	if (fpath == NULL) return NULL;

	if (dirpath_list == NULL || _uls_tool(is_absolute_path)(fpath) > 0) {
		fp_in = _uls_tool_(fp_open)(fpath, ULS_FIO_READ);

		parms1.lptr = fpath;
		lptr = __uls_tool_(filename)(uls_ptr(parms1));

		if (parms != nilptr) {
			parms->lptr = fpath;
			parms->len = (int) (lptr - fpath);
		}

		return fp_in;
	}

	fp_in = NULL;
	for (lptr0 = dirpath_list; lptr0 != NULL; ) {
		if ((lptr = _uls_tool_(strchr)(lptr0, ULS_DIRLIST_DELIM)) != NULL) {
			len_fptr = (int) (lptr - lptr0);
			fptr = lptr0;
			lptr0 = ++lptr;
		} else {
			len_fptr = _uls_tool_(strlen)(lptr0);
			fptr = lptr0;
			lptr0 = NULL;
		}

		if (len_fptr > 0) {
			len = _uls_tool_(strncpy)(filepath_buff, fptr, len_fptr);
			filepath_buff[len++] = ULS_FILEPATH_DELIM;
			_uls_tool_(strcpy)(filepath_buff+len, fpath);
		} else {
			_uls_tool_(strcpy)(filepath_buff, fpath);
		}

		if ((fp_in=_uls_tool_(fp_open)(filepath_buff, ULS_FIO_READ)) != NULL) {
			if (parms != nilptr) {
				parms->lptr = fptr;
				parms->len = len_fptr;
			}
			break;
		}
	}

	return fp_in;
}
