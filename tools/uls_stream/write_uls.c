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
#include "write_uls.h"
#include "main.h"

int
print_tmpl_stream_file(uls_ostream_t *ostr, uls_lex_t* uls)
{
	csz_str_t tag_buf;
	const char *lxm;
	int lno=-1, rc, tok_id, l_lxm, stat=0;

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

		if (uls_get_taglen(uls) != csz_length(uls_ptr(tag_buf)) ||
			!ult_streql(csz_text(uls_ptr(tag_buf)), uls_get_tag(uls)) ) {
			lno = uls_get_lineno(uls);
			csz_reset(uls_ptr(tag_buf));
			csz_append(uls_ptr(tag_buf), uls_get_tag(uls), uls_get_taglen(uls));
			__uls_print_tok_linenum(ostr, lno, csz_text(uls_ptr(tag_buf)), csz_length(uls_ptr(tag_buf)));

		} else if (lno != uls_get_lineno(uls)) {
			lno = uls_get_lineno(uls);
			uls_print_tok_linenum(ostr, lno, NULL);
		}

		if (uls_is_eof(uls) || uls_is_eoi(uls)) {
			break;
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
uls_start_stream_filepath(uls_ostream_t *ostr, const char* fpath, const char* filter)
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
		if ((istr = uls_open_istream_filter_file(uls_ptr(fdfilter), fpath)) == uls_nil) {
			err_log("%s: can't read %s", __func__, fpath);
			fdf_deinit(uls_ptr(fdfilter));
			return -1;
		}
#else
		err_log("%s: fdf not supported!", __func__);
		return -1;
#endif
	} else {
		if ((istr = uls_open_istream_file(fpath)) == uls_nil) {
			err_log("%s: can't read %s", __func__, fpath);
			return -1;
		}
	}

	uls = ostr->uls;
	if (uls_push_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		err_log("%s: fail to prepare input-stream %s", ulc_config, fpath);
		stat = -4; goto end_1;
	}

	if (print_tmpl_stream_file(ostr, uls) < 0) {
		err_log("%s: fail to uls-streaming to %d", fpath);
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
		err_log("InternalError: can't create a temp-file.");
		return -1;
	}

	if ((ostr = __uls_create_ostream(fd_out, uls, out_ftype, tag_name)) == uls_nil) {
		err_log("can't set uls-stream to %d", fd_out);
		stat = -2; goto end_2;
	}

	for (i=0; i<n_fpaths; i++) {
		if (uls_start_stream_filepath(ostr, fpaths[i], cmdline_filter) < 0) {
			err_log("can't read %s", fpaths[i]);
			ostr->flags |= ULS_STREAM_ERR;
			stat = -5; break;
		}
	}

 end_2:
	if (ostr != uls_nil) {
		uls_destroy_ostream(ostr);
	}

	if (uls_close_tempfile(uls_ptr(tmpfile), output_file) < 0) {
		err_log("InternalError: can't move the temp-file.");
		if (uls_close_tempfile(uls_ptr(tmpfile), NULL) < 0)
			err_panic("InternalError: can't remove the temp-file.");
		stat = -4;
	}
	uls_destroy_tempfile(uls_ptr(tmpfile));

	return stat;
}

