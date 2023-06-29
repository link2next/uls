
#include "filelist.h"
#include "main.h"

static int
start_stream_filepath_bin(ostream_t *ostr, lex_t* uls, const char* fpath)
{
	istream_t *istr;
	int stat = 0;

	if ((istr = open_istream_file(fpath)) == NULL) {
		err_log("%s: can't read %s", __func__, fpath);
		return -1;
	}

	if (bind_tmpl(istr, name_val, n_name_val, uls) < 0) {
		err_log("%s: fail to uls-streaming to %d", fpath);
		stat = -2; goto end_1;
	}

	if (push_istream(uls, istr, 0) < 0) {
		err_log("%s: fail to prepare input-stream", __func__);
		stat = -3; goto end_1;
	}

	if (start_stream(ostr, uls, ULS_LINE_NUMBERING) < 0) {
		err_log("%s: fail to uls-streaming to %d", fpath);
		stat = -4; goto end_1;
	}

 end_1:
	close_istream(istr);

	return stat;
}

int
write_stream_body_from_bin(FILE* fp_list, lex_t* uls, ostream_t* ostr)
{
	char  linebuff[1024], *lptr, *fpath;
	int   len, stat = 0;

	while ((len=fgets(fp_list, linebuff, sizeof(linebuff))) >= 0) {
		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		len -= (int) (lptr - linebuff);
		len = str_trim_end(lptr, len);
		fpath = lptr;

		if (!opt_silent)
			err_log("processing %s, ...", fpath);

		if (start_stream_filepath_bin(ostr, uls, fpath) < 0) {
			err_log("fail to uls-streaming %s, ...", fpath);
			--stat;
			break;
		}
	}

	return stat;
}

int
write_stream_body_from_raw(int fd_list, lex_t* uls, ostream_t* ostr)
{
#ifdef ULS_FDF_SUPPORT
	fdf_t  fdfilter;
#endif
	istream_t *istr;
	int  stat = 0;

	if (bind_ostream(ostr, uls) < 0) {
		err_log("%s: can't bind!", __func__);
		return -1;
	}
#ifdef ULS_FDF_SUPPORT
	fdf_init(&fdfilter, fdf_iprovider_filelist, cmdline_filter);
	if ((istr = open_istream_filter(&fdfilter, fd_list)) == NULL) {
		err_log("%s: can't conjecture file type!", __func__);
		fdf_deinit(&fdfilter);
		return -2;
	}
#else
	if ((istr = open_istream(fd_list)) == NULL) {
		err_log("%s: can't conjecture file type!", __func__);
		return -2;
	}
#endif
	if (istr->header.filetype != ULS_STREAM_RAW) {
		if (!streql(uls->ulc_name, istr->header.specname) ||
			!vers_compatible(&uls->stream_filever, &istr->header.filever)) {
			err_log("%s: Unsupported version: ", istr->header.subname);
			stat = -3; goto end_1;
		}
	}

	if (bind_tmpl(istr, name_val, n_name_val, uls) < 0) {
		err_log("%s: fail to prepare input-stream", __func__);
		stat = -4; goto end_1;
	}

	if (push_istream(uls, istr, 0) < 0) {
		err_log("%s: fail to prepare input-stream", __func__);
		stat = -5; goto end_1;
	}

	if (start_stream(ostr, uls, ULS_LINE_NUMBERING) < 0) {
		err_log("%s: fail to uls-streaming to %d", __func__);
		stat = -6; goto end_1;
	}

 end_1:
	close_istream(istr);
#ifdef ULS_FDF_SUPPORT
	fdf_deinit(&fdfilter);
#endif
	return stat;
}

int
proc_filelist(int ftype, lex_t *uls)
{
	int   fd_list, rc, stat=0;
	ostream_t *ostr=NULL;
	FILE  *fp_list;
	tempfile_t tmpfile;
	int fd_out;

	if (opt_verbose)
		err_log("Loading uls config file %s, ...", config_path);

	if ((fd_out = open_tempfile(&tmpfile)) < 0) {
		err_log("file(for writing) open error");
		return -1;
	}

	if ((ostr = create_ostream(fd_out, out_ftype, tag_name, !opt_no_numbering)) == NULL) {
		err_log("can't set uls-stream to %d", fd_out);
		stat = -1; goto err_1;
	}

	if (bind_ostream(ostr, uls) < 0) {
		err_log("%s: can't bind!", __func__);
		ostr->flags |= ULS_STREAM_ERR;
		stat = -2; goto err_1;
	}

	if ((fd_list = fileopen(filelist, O_RDONLY)) < 0 || chdir(target_dir) < 0) {
		err_log("can't change to %s", target_dir);
		stat = -3;

	} else {
		if (ftype == ULS_STREAM_RAW) {
			rc = write_stream_body_from_raw(fd_list, uls, ostr);
		} else {
			if ((fp_list=fdopen(fd_list, "r")) == NULL) {
				err_log("%s: improper fd of list", __func__);
				stat = -4;
			} else {
				rc = write_stream_body_from_bin(fp_list, uls, ostr);
				fileclose(fp_list);
				fd_list = -5;
			}
		}

		if (rc < 0) {
			stat = -6;
		}
	}

	chdir(home_dir);

	fileclose(fd_list);
	close_ostream(ostr);
	ostr = NULL;

	if (stat == 0) {
		if (!opt_silent)
			err_log("Writing to '%s', ...", output_file);

		if (close_tempfile(&tmpfile, output_file) < 0) {
			err_log("%s: can't move file %s into %s",
				__func__, tmpfile.filepath, output_file);
			stat = -7; goto err_1;
		}
	}

	return stat;

err_1:
	if (ostr != NULL) close_ostream(ostr);
	if (close_tempfile(&tmpfile, NULL) < 0)
		err_panic("InternalError: can't remove the temp-file.");
	return stat;
}
