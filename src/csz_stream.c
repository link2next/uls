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
  <file> csz_stream.c </file>
  <brief>
  	csz: C String terminated by Zero.
    Routines for dynamically manipulating infinite c-string.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#define __CSZ_STREAM__
#include "uls/csz_stream.h"
#include "uls/uls_log.h"
#include <string.h>

ULS_DECL_STATIC char*
__find_in_pool(uls_outbuf_ptr_t outbuf, int siz)
{
	csz_buf_line_ptr_t  e_prev, e, e_next;
	char *line0;

	if ((e=csz_global->active_list) == nilptr || siz > e->size) {
		outbuf->buf = NULL;
		outbuf->siz = 0;
		return NULL;
	}

	for (e_prev = nilptr; e != nilptr; e_prev = e, e = e_next) {
		e_next = e->next;

		if (e->size == siz || e_next == nilptr || e_next->size < siz) {
			// detach e from the list
			if (e_prev != nilptr) {
				e_prev->next = e_next;
			} else {
				csz_global->active_list = e->next;
			}
			break;
		}
	}

	outbuf->buf = line0 = e->line;
	outbuf->siz = e->size;

	e->line = NULL;
	e->size = 0;

	e->next = csz_global->inactive_list;
	csz_global->inactive_list = e;

	return line0;
}

ULS_DECL_STATIC int
__release_in_pool(char* ptr, int siz)
{
	csz_buf_line_ptr_t  e_prev, e, e0;

	if (siz <= 0 || csz_global->inactive_list == nilptr)
		return 0;

	e0 = csz_global->inactive_list;
	csz_global->inactive_list = e0->next;

	e0->line = ptr;
	e0->size = siz;

	for (e_prev = nilptr, e = csz_global->active_list; ; e_prev = e, e = e->next) {
		if (e == nilptr || siz >= e->size) {
			if (e_prev != nilptr) {
				e0->next = e;
				e_prev->next = e0;
			} else {
				e0->next = csz_global->active_list;
				csz_global->active_list = e0;
			}
			break;
		}
	}

	return 1;
}

ULS_DECL_STATIC void
__init_csz_pool(void)
{
	csz_buf_line_ptr_t  e;
	int i;

	csz_global->active_list = nilptr;
	csz_global->inactive_list = nilptr;

	for (i=0; i<N_LINES_IN_POOL; i++) {
		e =  uls_alloc_object(csz_buf_line_t);

		e->line = NULL; e->size = 0;
		e->next = csz_global->inactive_list;

		csz_global->inactive_list = e;
	}
}

ULS_DECL_STATIC void
__reset_csz_pool(void)
{
	csz_buf_line_ptr_t  e, e_next;

	for (e=csz_global->active_list; e!=nilptr; e=e_next) {
		e_next = e->next;

		e->line = uls_mfree(e->line);
		e->size = 0;

		e->next = csz_global->inactive_list;
		csz_global->inactive_list = e;
	}

	csz_global->active_list = nilptr;
}

ULS_DECL_STATIC void
__deinit_csz_pool(void)
{
	csz_buf_line_ptr_t  e, e_next;

	__reset_csz_pool();

	for (e = csz_global->inactive_list; e != nilptr; e = e_next) {
		e_next = e->next;
		uls_dealloc_object(e);
	}

	csz_global->inactive_list = nilptr;
}

ULS_DECL_STATIC _ULS_INLINE void
__str_modify(uls_outbuf_ptr_t outbuf, int n_delta, int k, const char* str, int len)
{
	char *bufptr = outbuf->buf;
	int  n;

	if ((n = k + len) > outbuf->siz) {
		if (outbuf->siz == 0) bufptr = NULL;
		n = uls_roundup(n, n_delta);
		outbuf->buf = bufptr = (char *) uls_mrealloc(bufptr, n);
		outbuf->siz = n;
	}

	if (str != NULL && len > 0) {
		uls_memcopy(bufptr+k, str, len);
	}
}

void
reset_csz(void)
{
	uls_lock_mutex(uls_ptr(csz_global->mtx));
	__reset_csz_pool();
	uls_unlock_mutex(uls_ptr(csz_global->mtx));
}

void
initialize_csz(void)
{
	if (CSZ_STREAM_DELTA_DFL % 2 != 0)
		err_panic_primitive("csz: internal error");

	csz_global = uls_alloc_object(csz_global_data_t);

	uls_init_mutex(uls_ptr(csz_global->mtx));
	csz_global->inactive_list = nilptr;
	csz_global->active_list = nilptr;

	__init_csz_pool();
}

void
finalize_csz(void)
{
	uls_lock_mutex(uls_ptr(csz_global->mtx));
	__deinit_csz_pool();
	uls_unlock_mutex(uls_ptr(csz_global->mtx));

	uls_deinit_mutex(uls_ptr(csz_global->mtx));
	uls_dealloc_object(csz_global);
}

void
str_init(uls_outbuf_ptr_t outbuf, int siz)
{
	if (siz == 0) {
		outbuf->buf = NULL;
		outbuf->siz = 0;
		outbuf->siz_delta = CSZ_STREAM_DELTA_DFL;
		return;
	}

	if (siz > 0) {
		siz = uls_ceil_log2(siz, 3);
	}

	uls_lock_mutex(uls_ptr(csz_global->mtx));
	__find_in_pool(outbuf, siz);
	uls_unlock_mutex(uls_ptr(csz_global->mtx));

	if (outbuf->buf == NULL) {
		if (siz < 0) siz = 128;
		outbuf->buf = uls_malloc_buffer(siz);
		outbuf->siz = siz;
	}

	outbuf->siz_delta = outbuf->siz;
}

void
str_free(uls_outbuf_ptr_t outbuf)
{
	char *line = outbuf->buf;

	if (line != NULL) {
		uls_lock_mutex(uls_ptr(csz_global->mtx));
		if (__release_in_pool(line, outbuf->siz)) {
			line = NULL;
		}
		uls_unlock_mutex(uls_ptr(csz_global->mtx));
		uls_mfree(line);
	}

	outbuf->buf = NULL;
	outbuf->siz = 0;
}

void
__str_putc(uls_outbuf_ptr_t outbuf, int n_delta, int k, char ch)
{
	char *bufptr = outbuf->buf;

	if (outbuf->siz <= 0) {
		bufptr = outbuf->buf = NULL;
		outbuf->siz = 0;
	} else if (bufptr == NULL) {
		outbuf->siz = 0;
	}

	if (k < 0)
		err_panic_primitive("csz: misuse of index!");

	if (k >= outbuf->siz) {
		outbuf->siz = uls_roundup(k+1, n_delta);
		outbuf->buf = bufptr = (char *) uls_mrealloc(bufptr, outbuf->siz);
	}

	bufptr[k] = ch;
}

void
str_modify(uls_outbuf_ptr_t outbuf, int k, const char* str, int len)
{
	if (str != NULL) {
		if (len < 0) len = uls_strlen(str);
	} else {
		if (len < 0) return;
	}

	__str_modify(outbuf, outbuf->siz_delta, k, str, len);
}

int
str_append(uls_outbuf_ptr_t outbuf, int k, const char* str, int len)
{
	int k2;

	if (str != NULL) {
		if (len < 0) len = uls_strlen(str);
		k2 = k + len;
	} else {
		if (len < 0) return k;
		k2 = k;
	}

	__str_modify(outbuf, outbuf->siz_delta, k, str, len+1);
	k += len;
	outbuf->buf[k] = '\0';

	return k2;
}

int
str_puts(uls_outbuf_ptr_t outbuf, int k, const char* str)
{
	int l_str;

	l_str = uls_strlen(str);
	__str_modify(outbuf, outbuf->siz_delta, k, str, l_str+1);

	return k + l_str;
}

void
csz_init(csz_str_ptr_t csz, int n_delta)
{
	str_init(uls_ptr(csz->pool), 0);

	if (n_delta <= 0) {
		n_delta = CSZ_STREAM_DELTA_DFL;
	}

	csz->alloc_delta = uls_roundup(n_delta,  sizeof(uls_uch_t));
	csz->len = 0;
}

void
csz_deinit(csz_str_ptr_t csz)
{
	str_free(uls_ptr(csz->pool));
	csz->len = 0;
}

csz_str_ptr_t
csz_create(int n_delta)
{
	csz_str_ptr_t csz;

	csz = uls_alloc_object(csz_str_t);
	csz_init(csz, n_delta);

	return csz;
}

void
csz_destroy(csz_str_ptr_t csz)
{
	csz_deinit(csz);
	uls_dealloc_object(csz);
}

void
csz_reset(csz_str_ptr_t csz)
{
	int siz0 = csz->alloc_delta << 8;

	if (csz->pool.siz > siz0) {
		siz0 >>= 1;
		csz->pool.buf = (char *) uls_mrealloc(csz->pool.buf, siz0);
		csz->pool.siz = siz0;
	}

	csz->len = 0;
}

char*
csz_modify(csz_str_ptr_t csz, int k, const char* str, int len)
{
	int k2;

	if (str != NULL) {
		if (len < 0) len = uls_strlen(str);
	} else {
		if (len < 0) return NULL;
	}

	__str_modify(uls_ptr(csz->pool), csz->alloc_delta, k, str, len);
	if ((k2 = k + len) > csz->len) csz->len = k2;

	return csz_data_ptr(csz) + k;
}

char*
csz_append(csz_str_ptr_t csz, const char* str, int len)
{
	int k;

	if (str != NULL) {
		if (len < 0) len = uls_strlen(str);
	} else {
		if (len < 0) return NULL;
	}

	k = csz->len;
	__str_modify(uls_ptr(csz->pool), csz->alloc_delta, k, NULL, len+1);

	if (str != NULL) {
		uls_memcopy(csz->pool.buf + k, str, len);
	}

	csz->len = k + len;
	csz->pool.buf[csz->len] = '\0';

	return csz->pool.buf + k;
}

void
csz_puts(csz_str_ptr_t csz, const char* str)
{
	int l_str = uls_strlen(str);
	csz->len = str_append(uls_ptr(csz->pool), csz->len, str, l_str);
}

void
csz_putc(csz_str_ptr_t csz, char ch)
{
	__str_putc(uls_ptr(csz->pool), csz->alloc_delta, csz->len, ch);
	if (ch != '\0') ++csz->len;
}

void
csz_add_eos(csz_str_ptr_t csz)
{
	__str_putc(uls_ptr(csz->pool), csz->alloc_delta, csz->len, '\0');
	++csz->len;
}

char*
csz_text(csz_str_ptr_t csz)
{
	__str_putc(uls_ptr(csz->pool), csz->alloc_delta, csz->len, '\0');
	return csz->pool.buf;
}

char*
csz_export(csz_str_ptr_t csz)
{
	char *ptr;

	ptr = csz_text(csz);

	csz->pool.buf = NULL;
	csz->pool.siz = 0;
	csz->len = 0;

	return ptr;
}
