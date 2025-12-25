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
 * write_uls.c -- write into uls file in binary stream form --
 *     written by Stanley Hong <link2next@gmail.com>, August 2013.
 */
#include "main.h"
#include "write_uls.h"
#include "lpz_lineproc.h"

int
print_tmpl_stream_file(uls_ostream_t *ostr, uls_lex_ptr_t uls)
{
	csz_str_t tag_buf;
	const char *lxm;
	int lno = -1, rc, tok_id, l_lxm, stat = 0;

	csz_init(uls_ptr(tag_buf), 128);

	lno = uls_get_lineno(uls);
	csz_append(uls_ptr(tag_buf), uls_get_tag(uls), uls_get_taglen(uls));
	__uls_print_tok_linenum(ostr, lno, csz_text(uls_ptr(tag_buf)), csz_length(uls_ptr(tag_buf)));

	uls_want_eof(uls);

	while (1) {
		tok_id = uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1;
			break;
		}

		if (uls_is_eof(uls) || uls_is_eoi(uls)) {
			break;
		}

		if (uls_get_taglen(uls) != csz_length(uls_ptr(tag_buf)) ||
			!ult_streql(csz_text(uls_ptr(tag_buf)), uls_get_tag(uls)) ) {
			lno = uls_get_lineno(uls);
			csz_reset(uls_ptr(tag_buf));
			csz_append(uls_ptr(tag_buf), uls_get_tag(uls), uls_get_taglen(uls));
			__uls_print_tok_linenum(ostr, lno, csz_text(uls_ptr(tag_buf)), csz_length(uls_ptr(tag_buf)));

		} else if (lno != uls_get_lineno(uls)) {
			lno = uls_get_lineno(uls);
			__uls_print_tok_linenum(ostr, lno, NULL, 0);
		}

		lxm = uls_lexeme(uls);
		l_lxm = uls_lexeme_len(uls);

		if (uls_is_id(uls) && exist_name_val_ent(lxm)) {
			tok_id = uls_toknum_tmpl(uls);
		}

		if ((rc = __uls_print_tok(ostr, tok_id, lxm, l_lxm)) < 0) {
			stat = -2; break;
		}
	}

	uls_pop(uls);
	csz_deinit(uls_ptr(tag_buf));

	return stat;
}

static int
uls_start_stream_filepath(uls_ostream_t *ostr, const char *filepath, const char *filter)
{
	uls_lex_ptr_t uls;
	uls_istream_ptr_t istr;
	int stat = 0;
#ifdef ULS_FDF_SUPPORT
	fdf_t    fdfilter;
#endif

#ifdef ULS_FDF_SUPPORT
	fdf_init(uls_ptr(fdfilter), NULL, filter);
#endif

	if (cmdline_filter != NULL) {
#ifdef ULS_FDF_SUPPORT
		if ((istr = uls_open_istream_filter_file(uls_ptr(fdfilter), filepath)) == uls_nil) {
			ult_log("%s: can't read %s", __func__, filepath);
			fdf_deinit(uls_ptr(fdfilter));
			return -1;
		}
#else
		ult_log("%s: fdf not supported!", __func__);
		return -1;
#endif
	} else {
		if ((istr = uls_open_istream_file(filepath)) == uls_nil) {
			ult_log("%s: can't read %s", __func__, filepath);
			return -1;
		}
	}

	uls = ostr->uls;
	if (uls_push_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		ult_log("%s: fail to prepare input-stream %s", ulc_config, filepath);
		stat = -4; goto end_1;
	}

	if (print_tmpl_stream_file(ostr, uls) < 0) {
		ult_log("%s: fail to uls-streaming to %d", filepath);
		stat = -5; goto end_1;
	}

 end_1:
	uls_destroy_istream(istr);
#ifdef ULS_FDF_SUPPORT
	fdf_deinit(uls_ptr(fdfilter));
#endif
	return stat;
}

int
write_uls_files(int n_fpaths, const char** fpaths)
{
	uls_lex_ptr_t uls = sam_lex;
	int  i, fd_out, stat=0;
	uls_ostream_ptr_t ostr;
	uls_tempfile_t tmpfile;

	uls_init_tempfile(uls_ptr(tmpfile));

	if ((fd_out = uls_open_tempfile(uls_ptr(tmpfile))) < 0) {
		ult_log("InternalError: can't create a temp-file.");
		return -1;
	}

	if ((ostr = __uls_create_ostream(fd_out, uls, out_ftype, tag_name)) == uls_nil) {
		ult_log("can't set uls-stream to %d", fd_out);
		stat = -2; goto end_2;
	}

	for (i=0; i<n_fpaths; i++) {
		if (uls_start_stream_filepath(ostr, fpaths[i], cmdline_filter) < 0) {
			ult_log("can't read %s", fpaths[i]);
			ostr->flags |= ULS_STREAM_ERR;
			stat = -5; break;
		}
	}

 end_2:
	if (ostr != uls_nil) {
		uls_destroy_ostream(ostr);
	}

	if (uls_close_tempfile(uls_ptr(tmpfile), output_file) < 0) {
		ult_log("InternalError: can't move the temp-file.");
		if (uls_close_tempfile(uls_ptr(tmpfile), NULL) < 0)
			ult_panic("InternalError: can't remove the temp-file.");
		stat = -4;
	}
	uls_destroy_tempfile(uls_ptr(tmpfile));

	return stat;
}

// ================================================================

int
lex_gcc_line(uls_ostream_t *ostr, const char *line)
{
	uls_lex_ptr_t uls = ostr->uls;
	int lno = -1, rc, l_lxm, tok_id, stat = 0;
	csz_str_t tag_buf;
	const char *lxm;

	csz_init(uls_ptr(tag_buf), 128);

	lno = uls_get_lineno(uls);
	csz_append(uls_ptr(tag_buf), uls_get_tag(uls), uls_get_taglen(uls));

	if (uls_push_line(uls, line, -1, ULS_WANT_EOFTOK) < 0) {
		ult_log("can't set the istream!");
		return -1;
	}

	for ( ; ; ) {
		tok_id = uls_get_tok(uls);
		if (uls_is_err(uls)) {
			stat = -1;
			break;
		}

		if (uls_is_eof(uls)) {
			break;
		}

		if (uls_get_taglen(uls) != csz_length(uls_ptr(tag_buf)) ||
			!ult_streql(csz_text(uls_ptr(tag_buf)), uls_get_tag(uls)) ) {
			lno = uls_get_lineno(uls);
			csz_reset(uls_ptr(tag_buf));
			csz_append(uls_ptr(tag_buf), uls_get_tag(uls), uls_get_taglen(uls));
			__uls_print_tok_linenum(ostr, lno, csz_text(uls_ptr(tag_buf)), csz_length(uls_ptr(tag_buf)));

		} else if (lno != uls_get_lineno(uls)) {
			lno = uls_get_lineno(uls);
			__uls_print_tok_linenum(ostr, lno, NULL, 0);
		}

		lxm = uls_lexeme(uls);
		l_lxm = uls_lexeme_len(uls);

		if ((rc = __uls_print_tok(ostr, tok_id, lxm, l_lxm)) < 0) {
			stat = -2; break;
		}
	}

	uls_pop(uls);
	csz_deinit(uls_ptr(tag_buf));

	return stat;
}

static int
lproc_level_0(lpz_buf_t* lb)
{
	uls_ostream_ptr_t ostr = lpz_get_data(lb);
	char *line = lb->linebuff + lb->idx_line;
	int rc;

	if (*line == '#') {
		if (lb->idx_line > 0) {
			*line = '\0';
			// lb->len_linebuff = lb->idx_line
			rc = lex_gcc_line(ostr, lb->linebuff);
			*line = '#';
			if (rc < 0) {
				ult_log("can't proc the istream!");
				return LPZ_ERR;
			}
		}

		lpz_level_up(lb);
		return LPZ_AGAIN;
	}

	if (lb->len_linebuff < 32 && (lb->flags & LPZ_FL_EOF) == 0) {
		return LPZ_DEFER;
	}

	if (lex_gcc_line(ostr, lb->linebuff) < 0) {
		ult_log("can't proc the istream!");
		return LPZ_ERR;
	}

	return LPZ_OK;
}

static int
lproc_level_1(lpz_buf_t* lb)
{
//	uls_ostream_ptr_t ostr = lpz_get_data(lb);
	char *line = lb->linebuff + lb->idx_line;

	if (*line != '#') {
		lpz_level_down(lb);
		return LPZ_AGAIN;
	}

	return LPZ_OK;
}

static int
lproc_initfunc(lpz_buf_t* lb)
{
	if (lpz_create_lb_ctx(lb, 2) < 0) {
		return -1;
	}

	lb->context[0].proc = lproc_level_0;
	lb->context[0].data = NULL;

	lb->context[1].proc = lproc_level_1;
	lb->context[1].data = NULL;

	return LPZ_OK;
}

int
write_gcc_preprocd_file(uls_lex_ptr_t uls, const char *filepath)
{
	int  rc, fd_out, stat = 0;
	uls_ostream_ptr_t ostr;
	uls_tempfile_t tmpfile;

	uls_init_tempfile(uls_ptr(tmpfile));

	if ((fd_out = uls_open_tempfile(uls_ptr(tmpfile))) < 0) {
		ult_log("InternalError: can't create a temp-file.");
		return -1;
	}

	if ((ostr = __uls_create_ostream(fd_out, uls, out_ftype, tag_name)) == uls_nil) {
		ult_log("can't set uls-stream to %d", fd_out);
		stat = -2;
	} else {
		rc = lpz_lineproc(filepath, lproc_initfunc, ostr, LP_MULTILINE | LP_INITFUNC);
		if (rc < 0) {
			ult_log("can't read %s", filepath);
			ostr->flags |= ULS_STREAM_ERR;
			stat = -3;
		}

		uls_destroy_ostream(ostr);
	}

	if (uls_close_tempfile(uls_ptr(tmpfile), output_file) < 0) {
		ult_log("InternalError: can't move the temp-file.");
		if (uls_close_tempfile(uls_ptr(tmpfile), NULL) < 0)
			ult_panic("InternalError: can't remove the temp-file.");
		stat = -4;
	}
	uls_destroy_tempfile(uls_ptr(tmpfile));

	return stat;
}

