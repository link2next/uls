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
 * lineproc.c -- reading lines in text file --
 *     written by Stanley Hong <link2next@gmail.com>, Dec. 2025.
 */
#define __LPZ_LINEPROC__
#include "lpz_lineproc.h"
#include "ult_log.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static lpz_buf_t* lpz_create(const char* filepath);
static void lpz_destroy(lpz_buf_t*);
static int lpz_getline(lpz_buf_t*, FILE* fin);

static char* extend_lb_buffer(lpz_buf_t* lb, int n);
static int lpz_append_str(lpz_buf_t* lb, const char *str);

int
lpz_lineproc(const char* filepath, lpz_proc_t lproc, void* data, unsigned flags)
{
	int      k, rc, stat = 0;
	lpz_buf_t *lb;
	char     *lptr;
	FILE     *fin;

	if (strcmp(filepath, "-") == 0) {
		fin = stdin;
	} else if ((fin = fopen(filepath, "r")) == NULL) {
		ult_log("can't open '%s'\n", filepath);
		return -1;
	}

	if ((lb = lpz_create(filepath)) == NULL) {
		stat = -2;
	} else {
		if (flags & LP_INITFUNC) {
			if (lproc(lb) < 0) stat = -3;
			lpz_setall_data(lb, data);
		} else {
			if (lpz_create_lb_ctx(lb, N_DFT_LNCONTEXTS) < 0) {
				stat = -4;
			} else {
				lpz_set_proc(lb, lproc);
				lpz_set_data(lb, data);
			}
		}
	}

	if (stat < 0) {
		lpz_destroy(lb);
		if (fin != stdin) fclose(fin);
		return stat;
	}

	for (lb->lineno = 1; (lb->flags & LPZ_FL_EOF) == 0; ) {
		if ((rc = lpz_getline(lb, fin)) < 0) {
			stat = -5;
			break;
		}
		/* rc = the length of added line including '\n' */

		/* lb->len_linebuff == total length of lb->linebuff - '\n' */
		lptr = lb->linebuff;
		++lb->n_lfs;

		if (rc == 0) { // EOF
			if (lb->len_linebuff == 0) break;
		} else {
			if ((k = lb->len_linebuff) > 0 && lptr[k-1] == '\\') {
				if (flags & LP_MULTILINE) {
					lptr[k-1] = '\n';
					continue;
				}
			}
		}

 again_1:
		if ( (rc = lb->context[lb->ctx_lvl].proc(lb)) < 0 ) {
			stat = -6;
			break;
		} else if (rc > 0) {
			if (rc == LPZ_AGAIN) {
				// ctx_lvl has been changed!
				goto again_1;
			} else if (rc == LPZ_DEFER) {
				lpz_append_str(lb, "\n");
				continue;  /* append another one-line */
			} else {
				stat = -7;
				break;
			}
		}

		lb->lineno += lb->n_lfs;
		lb->n_lfs = 0;
		lb->len_linebuff = 0;
	}

	lpz_destroy(lb);
	if (fin != stdin) fclose(fin);
	return stat;
}

static int lproc_default(lpz_buf_t* lb)
{
	ult_log("lpz_lineproc: context-level overflow!");
	return -1;
}

void* lpz_get_data(lpz_buf_t* lb)
{
	return lb->context[lb->ctx_lvl].data;
}

void lpz_set_data(lpz_buf_t* lb, void *p)
{
	lb->context[lb->ctx_lvl].data = p;
}

void lpz_setall_data(lpz_buf_t* lb, void *p)
{
	lpz_lb_ctx_t *ctx_list = lb->context;
	int  i;

	for (i = 0; i < lb->n_contexts; i++) {
		ctx_list[i].data = p;
	}
}

void lpz_set_proc(lpz_buf_t* lb, lpz_proc_t p) 
{
	lb->context[lb->ctx_lvl].proc = p;
}

static void
init_lpz_lb_ctx(lpz_lb_ctx_t* ctx, int n)
{
	int  i;

	for (i = 0; i < n; i++) {
		ctx[i].proc = lproc_default;
		ctx[i].data = NULL;
	}
}

void
lpz_level_up(lpz_buf_t* lb)
{
	if (++lb->ctx_lvl >= lb->n_contexts) {
		if ( (lb->context = (lpz_lb_ctx_t*) realloc(lb->context,
				(lb->n_contexts + N_DFT_LNCONTEXTS) * sizeof(lpz_lb_ctx_t)))==NULL) {
			ult_panic("realloc error of linecontext");
		}

		init_lpz_lb_ctx(lb->context + lb->n_contexts, N_DFT_LNCONTEXTS);
		lb->n_contexts += N_DFT_LNCONTEXTS;
	}
}

void
lpz_level_down(lpz_buf_t* lb)
{
	if (--lb->ctx_lvl < 0) 
		ult_panic("underflow of linecontext of %s", lb->tag);
}

int
lpz_create_lb_ctx(lpz_buf_t* lb, int n)
{
	if (n <= 0 || lb->context != NULL) {
		ult_log("lpz_create_lb_ctx: line-context is NOT free'd");
		return -1;
	}

	if ( (lb->context = (lpz_lb_ctx_t *) malloc(n * sizeof(lpz_lb_ctx_t))) == NULL) {
		ult_log("lpz_lineproc: lpz_lb_ctx malloc error!");
		return -1;
	}

	init_lpz_lb_ctx(lb->context, n);
	lb->n_contexts = n;
	lb->ctx_lvl = 0;

	return 0;
}

static char*
extend_lb_buffer(lpz_buf_t* lb, int n)
{
	int siz;

	if ((siz = lb->len_linebuff + n) >= lb->siz_linebuff) {
		siz += LINEBUFF_SZ;
		siz = ((siz + 7) >> 3) << 3;

		if (lb->linebuff == NULL) {
			lb->linebuff = (char *) malloc(siz);
		} else {
			lb->linebuff = (char *) realloc(lb->linebuff, siz);
		}

		if (lb->linebuff == NULL) {
			ult_panic("lpz_lineproc: linebuff realloc error!");
		}

		lb->siz_linebuff = siz;
	}

	return lb->linebuff;
}

static lpz_buf_t*
lpz_create(const char* filepath)
{
	lpz_buf_t  *lb;

	if ( (lb = (lpz_buf_t *) malloc(sizeof(lpz_buf_t)))==NULL) {
		ult_log("lpz_lineproc: linebuff calloc error!");
		return NULL;
	}

	lb->flags = 0;
	lb->len_linebuff = lb->siz_linebuff = 0;
	lb->linebuff = NULL;
	extend_lb_buffer(lb, 1);
	lb->idx_line = lb->len_line = 0;

	lb->tag = filepath;
	lb->lineno = lb->n_lfs = 0;

	lb->context = NULL;
	lb->ctx_lvl = lb->n_contexts = 0;

	return lb;
}

static void
lpz_destroy(lpz_buf_t* lb)
{
	if (lb != NULL) {
		if (lb->linebuff != NULL) free(lb->linebuff);
		if (lb->context != NULL) free(lb->context);
		free(lb);
	}
}

static int
lpz_append_str(lpz_buf_t* lb, const char *str)
{
	int len = strlen(str);
	char *line;

	line = extend_lb_buffer(lb, len);

	strcpy(line + lb->len_linebuff, str);
	lb->len_linebuff += len;

	return lb->len_linebuff;
}

static int
lpz_getline(lpz_buf_t* lb, FILE* fin)
{
	char *lptr = lb->linebuff;
	int  len = 0, k = lb->len_linebuff;
	int  ch;

	lb->idx_line = lb->len_linebuff;
	lb->len_line = 0;

	while (1) {
		if (ferror(fin)) return -1;
		if ((ch = getc(fin)) == EOF) {
			lb->flags |= LPZ_FL_EOF;
			break;
		}

		if (ch == '\n') {
			lb->len_line = len;
			++len;
			break;
		}

		++len;
		if (k >= lb->siz_linebuff) {
			lb->len_linebuff = k;
			lptr = extend_lb_buffer(lb, 1);
		}

		lptr[k++] = (char) ch;
	}

	lptr[k] = '\0';
	lb->len_linebuff = k;

	return len; // #-chars including '\n'
}
