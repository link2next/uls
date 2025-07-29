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
 * filelist.c -- To process multiple uls-file or raw file from ...
 *     written by Stanley Hong <link2next@gmail.com>, August 2013.
 */
#include "main.h"
#include "filelist.h"
#include "write_uls.h"

#ifdef ULS_FDF_SUPPORT
static int
conglomerate_uls_files_via_filter(int fd_list, const char *cmd_flt, uls_ostream_ptr_t ostr)
{
	uls_lex_ptr_t uls = ostr->uls;
	uls_istream_ptr_t istr;
	fdf_t  fdfilter;
	int  stat = 0;

	fdf_init(uls_ptr(fdfilter), fdf_iprovider_filelist, cmd_flt);

	if ((istr = uls_open_istream_filter(uls_ptr(fdfilter), fd_list)) == uls_nil) {
		err_log("%s: can't conjecture file type!", __func__);
		fdf_deinit(uls_ptr(fdfilter));
		return -2;
	}

	if (uls_push_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		err_log("%s: fail to prepare input-stream", __func__);
		stat = -3; goto end_1;
	}

	uls_set_tag(uls, tag_name, -1);

	if (print_tmpl_stream_file(ostr, uls) < 0) {
		err_log("%s: fail to uls-streaming.", __func__);
		stat = -4; goto end_1;
	}

 end_1:
	uls_destroy_istream(istr);
	fdf_deinit(uls_ptr(fdfilter));

	return stat;
}
#endif

static int
conglomerate_uls_file(int fd, const char *tagstr, uls_ostream_ptr_t ostr)
{
	uls_lex_ptr_t uls = ostr->uls;
	uls_istream_ptr_t istr;
	int  stat = 0;

	if ((istr = uls_open_istream(fd)) == uls_nil) {
		err_log("%s: can't conjecture file type!", __func__);
		return -2;
	}

	uls_set_istream_tag(istr, tagstr);

	if (uls_push_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		err_log("%s: fail to prepare input-stream", __func__);
		stat = -6; goto end_1;
	}

	if (print_tmpl_stream_file(ostr, uls) < 0) {
		err_log("%s: fail to uls-streaming.", __func__);
		stat = -7; goto end_1;
	}

 end_1:
	uls_destroy_istream(istr);

	return stat;
}

static int
conglomerate_files(FILE *fp_list, uls_ostream_t* ostr)
{
	char linebuff[ULS_LINEBUFF_SIZ+1], *lptr;
	int len, fd, rc, stat = 0;

	while (1) {
		if ((len=uls_fp_gets(fp_list, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				stat = -1;
			}
			break;
		}

		if (*(lptr = skip_blanks(linebuff)) == '\0' || *lptr == '#')
			continue;

		len = str_trim_end(lptr, -1);

		err_log("%s:", lptr);

		if ((fd = ult_fd_open_rdonly(lptr)) < 0) {
			err_log("Can't open '%s'! continuing...", lptr);
			continue;
		}

		rc = conglomerate_uls_file(fd, lptr, ostr);
		ult_fd_close(fd);

		if (rc < 0) {
			err_log("Fail to process '%s'! breaking...", lptr);
			stat = -1;
			break;
		}
	}

	return stat;
}

int
proc_filelist(const char *tgt_dir)
{
	int   fd_out, rc, stat=0;
	uls_ostream_ptr_t ostr=uls_nil;
	uls_tempfile_t tmpfile;
#ifdef ULS_FDF_SUPPORT
	int fd_list;
#endif
	FILE *fp_list;
	const char *cmdline;

	if (opt_verbose)
		err_log("Loading uls config file %s, ...", ulc_config);

	uls_init_tempfile(uls_ptr(tmpfile));

	if ((fd_out = uls_open_tempfile(uls_ptr(tmpfile))) < 0) {
		err_log("file(for writing) open error");
		return -1;
	}

	if ((ostr = __uls_create_ostream(fd_out, sam_lex, out_ftype, tag_name)) == NULL) {
		err_log("can't set uls-stream to %d", fd_out);
		stat = -1; goto end_1;
	}

	if (cmdline_filter != NULL) {
		if (ult_streql(cmdline_filter, "pass")) {
			cmdline = NULL;
		} else {
			cmdline = cmdline_filter;
		}

		if (ult_is_inputfiles_raw(filelist) <= 0) {
			err_log("All the files in '%s' must be text to use fd-filter!", filelist);
			stat = -1; goto end_1;
		}

#ifdef ULS_FDF_SUPPORT
		if ((fd_list = ult_fd_open_rdonly(filelist)) < 0) {
			err_log("%s: not found!", filelist);
			stat = -3;
		} else {
			if (uls_chdir(tgt_dir) < 0) {
				err_log("can't change to %s", tgt_dir);
				stat = -4;
			} else if ((rc = conglomerate_uls_files_via_filter(fd_list, cmdline, ostr)) < 0) {
				stat = -5;
			}
		}
		ult_fd_close(fd_list);
#else
		err_log("%s: fdf not supported!", __func__);
		stat = -1;
#endif
	} else {
		if ((fp_list = uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
			err_log("%s: not found!", filelist);
			stat = -3;
		} else {
			if (uls_chdir(tgt_dir) < 0) {
				err_log("can't change to %s", tgt_dir);
				stat = -4;
			} else if ((rc = conglomerate_files(fp_list, ostr)) < 0) {
				err_log("fail to process %s.", filelist);
				stat = -5;
			}
		}
		uls_fp_close(fp_list);
	}

	uls_destroy_ostream(ostr);
	ostr = NULL;

	if (uls_chdir(home_dir) < 0) {
		err_log("can't return to home-dir");
		stat = -6;
	}

	if (stat == 0) {
		if (opt_verbose)
			err_log("Writing to '%s', ...", output_file);

		if (uls_close_tempfile(uls_ptr(tmpfile), output_file) < 0) {
			err_log("%s: can't move file %s into %s",
				__func__, tmpfile.filepath, output_file);
			stat = -7;
		}
	}

end_1:
	if (ostr != NULL) uls_destroy_ostream(ostr);
	uls_destroy_tempfile(uls_ptr(tmpfile));

	return stat;
}
