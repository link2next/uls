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
 * lpz_lineproc.h -- reading lines in text file --
 *     written by Stanley Hong <link2next@gmail.com>, Dec. 2025.
 */
#ifndef __LPZ_LINEPROC_H__
#define __LPZ_LINEPROC_H__

#define LINEBUFF_SZ       128
#define N_DFT_LNCONTEXTS  2

#define LP_INITFUNC     0x01
#define LP_MULTILINE    0x02

typedef struct _lpz_lb_ctx lpz_lb_ctx_t;
typedef struct _lpz_buf lpz_buf_t;

#define LPZ_ERR   -1
#define LPZ_OK     0
#define LPZ_AGAIN  1
#define LPZ_DEFER  2
typedef int (*lpz_proc_t)(lpz_buf_t* lb);

struct _lpz_lb_ctx {
	lpz_proc_t  proc;
	void       *data;
};

#define LPZ_FL_EOF 0x01
struct _lpz_buf {
	int flags;

	char  *linebuff;
	int   len_linebuff, siz_linebuff;
	int   idx_line, len_line;

	lpz_lb_ctx_t    *context;
	int ctx_lvl, n_contexts;

	const char   *tag;
	int    lineno, n_lfs;
};

#ifdef __LPZ_LINEPROC__
#define EXTERNAL
#else
#define EXTERNAL  extern
#endif

EXTERNAL int  lpz_lineproc(const char* filepath, lpz_proc_t lproc, void* data, unsigned flags);
EXTERNAL int  lpz_create_lb_ctx(lpz_buf_t* lb, int n);

EXTERNAL void* lpz_get_data(lpz_buf_t* lb);
EXTERNAL void  lpz_set_data(lpz_buf_t* lb, void* p);
EXTERNAL void  lpz_setall_data(lpz_buf_t* lb, void* p);
EXTERNAL void  lpz_set_proc(lpz_buf_t* lb, lpz_proc_t p);

EXTERNAL void  lpz_level_up(lpz_buf_t* lb);
EXTERNAL void  lpz_level_down(lpz_buf_t* lb);

#undef EXTERNAL
#endif /* __LPZ_LINEPROC_H__ */
