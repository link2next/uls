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
 * read_uls.c -- read from input file and dump tokens in binary/text stream --
 *     written by Stanley Hong <link2next@gmail.com>, August 2013.
 */
#include "read_uls.h"
#include "main.h"
#include <uls/uls_num.h>
#include <ctype.h>

static int
dump_stream_file(uls_lex_ptr_t uls, FILE* fp_out, int wcoord)
{
	csz_str_t tag_buf;
	int lno=-1, lineno, l_tagstr, wcoord_changed, stat=0;
	const char *lxm, *tagstr;

	char *buff2=NULL;
	int buff2_siz = 0;
	int i, j, m, is_all_printable;
	const char *lxm2;
	unsigned char ch2;

	csz_init(uls_ptr(tag_buf), 128);

	while (1) {
		uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1;
			break;
		}

		tagstr = uls_get_tag(uls);
		l_tagstr = uls_get_taglen(uls);
		lineno = uls_get_lineno(uls);
		lxm = uls_lexeme(uls);

		if (lno != lineno) {
			lno = lineno;
			wcoord_changed = 1;
		} else {
			wcoord_changed = 0;
		}

		if (l_tagstr != csz_length(uls_ptr(tag_buf)) || !ult_streql(csz_text(uls_ptr(tag_buf)), tagstr)) {
			csz_reset(uls_ptr(tag_buf));
			csz_append(uls_ptr(tag_buf), tagstr, l_tagstr);
			wcoord_changed = 1;
		}

		lxm2 = lxm;

		if (uls_is_quote(uls)) {
			is_all_printable = 1;

			for (i=0; lxm[i] != '\0'; i++) {
				ch2 = (unsigned char) lxm[i];
				if (!isprint(ch2)) {
					is_all_printable = 0;
				}
			}

			if (!is_all_printable) {
				m = 4 * i + 1;
				if (m > buff2_siz) {
					buff2 = (char *) uls_mrealloc(buff2, m);
					buff2_siz = m;
				}

				lxm2 = buff2;
				for (j=i=0; lxm[i] != '\0'; i++) {
					ch2 = (unsigned char) lxm[i];
					if (isprint(ch2)) {
						buff2[j++] = ch2;
					} else {
						buff2[j++] = '\\';
						buff2[j++] = 'x';

						m = ch2 / 16;
						buff2[j++] = (m >= 10) ? ('a' + (m-10)) : ('0'+ m);

						m = ch2 % 16;
						buff2[j++] = (m >= 10) ? ('a' + (m-10)) : ('0'+ m);
					}
				}
				buff2[j] = '\0';
			}
		}

		if (wcoord) {
			if (wcoord_changed && !uls_is_eoi(uls)) {
				uls_fprintf(fp_out, " %16s:%-4d [%7t] %s\n", tagstr, lineno, uls, lxm2);
			} else {
				uls_fprintf(fp_out, "                       [%7t] %s\n", uls, lxm2);
			}
		} else {
			uls_fprintf(fp_out, "\t[%7t] %s\n", uls, lxm2);
		}

		if (uls_is_eoi(uls)) break;
	}

	uls_mfree(buff2);
	csz_deinit(uls_ptr(tag_buf));

	return stat;
}

int
dump_uls_file(const char *filepath, FILE *fp_out)
{
	int stat=0;
	uls_lex_ptr_t uls;
	uls_istream_ptr_t istr;

#ifdef ULS_FDF_SUPPORT
	fdf_t    fdfilter;
#endif

#ifdef ULS_FDF_SUPPORT
	fdf_init(uls_ptr(fdfilter), NULL, cmdline_filter);
#endif

	if (cmdline_filter != NULL) {
#ifdef ULS_FDF_SUPPORT
		if ((istr = uls_open_istream_filter_file(uls_ptr(fdfilter), filepath)) == uls_nil) {
			ult_log("%s: can't read %s", __func__, filepath);
			return -1;
		}
#else
		ult_log("%s: fdf not supported!", __func__);
		return -1;
#endif
	} else {
		if ((istr = uls_open_istream_file(filepath)) == uls_nil) {
			ult_log("can't read %s", filepath);
			return -1;
		}
	}

	if ((uls=istr->uls) == uls_nil) {
		uls = sam_lex;
	}

	if (uls_set_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		ult_log("%s: fail to prepare input-stream %s", ulc_config, filepath);
		stat = -4; goto end_1;
	}

	if ((stat=dump_stream_file(uls, fp_out, !opt_no_numbering)) < 0) {
		ult_log("fail to read uls-file %s", filepath);
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
dump_uls_files(int n_fpaths, const char** fpaths, FILE *fp_out)
{
	int i, rc, stat=0;
	char *input_file;

	for (i=0; i<n_fpaths; i++) {
		input_file = ult_strdup(fpaths[i]);
		uls_path_normalize(input_file, input_file);

		rc = dump_uls_file(input_file, fp_out);
		uls_mfree(input_file);
		if (rc < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
}

static void
__flushline_lxmbuff(csz_str_ptr_t lxm_buf, FILE *fp)
{
	const char *txtstr;

	if (csz_length(lxm_buf) > 0) {
		txtstr = csz_text(lxm_buf);
		uls_fprintf(fp, "%s\n", txtstr);
		csz_reset(lxm_buf);
	}
}

static int
rawdump_stream_file(uls_lex_ptr_t uls, FILE* fp_out)
{
#define NCHARS_PER_LINE 128
	csz_str_t tag_buf;
	csz_str_t lxm_buf;
	int lno = -1, lineno, l_tagstr, wcoord_changed, stat=0;
	const char *lxm, *lxm2, *tagstr;
	static const char *escchars = "ntrabfv";

	char ch, *buff2 = NULL;
	int buff2_siz = 0;
	int tok, i, k, m, is_octal, bLineFeed;
	int len1, len2, l_lxm, l_lxm2;
	unsigned char ch2;
	uls_quotetype_ptr_t qmt;
	const char *smark, *emark, *cptr;

	csz_init(uls_ptr(tag_buf), 128);
	csz_init(uls_ptr(lxm_buf), 1024);

	while (1) {
		tok = uls_get_tok(uls);
		if (uls_is_err(uls)) {
			stat = -1;
			break;
		}

		lxm = uls_tokstr(uls);
		l_lxm = uls_tokstr_len(uls);

		tagstr = uls_get_tag(uls);
		l_tagstr = uls_get_taglen(uls);
		lineno = uls_get_lineno(uls);

		if (lno != lineno) {
			lno = lineno;
			wcoord_changed = 1;
		} else {
			wcoord_changed = 0;
		}

		if (l_tagstr != csz_length(uls_ptr(tag_buf)) ||
			!ult_streql(csz_text(uls_ptr(tag_buf)), tagstr)) {
			csz_reset(uls_ptr(tag_buf));
			csz_append(uls_ptr(tag_buf), tagstr, l_tagstr);
			wcoord_changed = 1;
		}

		lxm2 = lxm;
		l_lxm2 = l_lxm;

		if ((qmt = uls_get_quote(uls, tok)) != NULL) {
			if (uls_is_quote_userdef(qmt) ||
				!uls_is_quote_symmetric(qmt)) {
				return -1;
			}

			smark = uls_get_namebuf_value(qmt->start_mark);
			emark = uls_get_namebuf_value(qmt->end_mark);

			len1 = uls_strlen(smark);
			len2 = uls_strlen(emark);

			if ((m = len1 + 4 * l_lxm + len2 + 1) > buff2_siz) {
				buff2 = (char *) uls_mrealloc(buff2, m);
				buff2_siz = m;
			}

			if (len1 > 0) {
				uls_strcpy(buff2, smark);
			}
			k = len1;

			for (i = 0; lxm[i] != '\0'; i++) {
				ch = lxm[i];
				if (isprint(ch)) {
					buff2[k++] = ch;
				} else if (ch == '\n') {
					buff2[k++] = '\\';
					buff2[k++] = 'n';
				} else if (ch == '\t') {
					buff2[k++] = '\\';
					buff2[k++] = 't';
				} else if (ch == '\r') {
					buff2[k++] = '\\';
					buff2[k++] = 'r';
				} else if ((cptr = strchr(escchars, ch)) != NULL) {
					buff2[k++] = '\\';
					buff2[k++] = *cptr;
				} else {
					ch2 = (unsigned char) ch;
					buff2[k++] = '\\';
					buff2[k++] = 'x';

					m = ch2 / 16;
					buff2[k++] = (m >= 10) ? ('a' + (m-10)) : ('0'+ m);

					m = ch2 % 16;
					buff2[k++] = (m >= 10) ? ('a' + (m-10)) : ('0'+ m);
				}
			}

			if (len2 > 0) {
				uls_strcpy(buff2 + k, emark);
			}
			k += len2;

			buff2[k] = '\0';
			lxm2 = buff2;
			l_lxm2 = k;

		} else if (uls_is_number(uls)) {
			is_octal = 0;
			if (l_lxm >= 2 && lxm[0] == '0' && lxm[1] == 'o') {
				is_octal = 1;
			}

			if (is_octal) {
				if (l_lxm < 2) {
					ult_log("%s: incorrect octal format!", lxm);
					return -1;
				}
				if ((m = l_lxm + 1) > buff2_siz) {
					buff2 = (char *) uls_mrealloc(buff2, m);
					buff2_siz = m;
				}
				buff2[0] = '0';
				buff2[1] = 'x';

				len2 = uls_oct2hex_str(lxm + 2, l_lxm - 2, buff2 + 2);

				lxm2 = buff2;
				l_lxm2 = 2 + len2;
			}
		}

		bLineFeed = 0;
		if (csz_length(uls_ptr(lxm_buf)) + 1 + l_lxm2 > NCHARS_PER_LINE) { // +1 for ' '
			__flushline_lxmbuff(uls_ptr(lxm_buf), fp_out);
			bLineFeed = 1;
		}

		if (wcoord_changed && !uls_is_eoi(uls)) {
			if (!bLineFeed) {
				__flushline_lxmbuff(uls_ptr(lxm_buf), fp_out);
			}
//			uls_fprintf(fp_out, "# %d \"%s\"\n", lineno, tagstr);
//			bLineFeed = 1;
		}

		if (l_lxm2 > NCHARS_PER_LINE) {
			if (!bLineFeed) {
				__flushline_lxmbuff(uls_ptr(lxm_buf), fp_out);
			}
			uls_fprintf(fp_out, "%s\n", lxm2);
			bLineFeed = 1;
		} else {
			csz_putc(uls_ptr(lxm_buf), ' ');
			csz_append(uls_ptr(lxm_buf), lxm2, l_lxm2);
		}

		if (uls_is_eoi(uls)) break;
	}

	__flushline_lxmbuff(uls_ptr(lxm_buf), fp_out);

	csz_deinit(uls_ptr(tag_buf));
	csz_deinit(uls_ptr(lxm_buf));
	uls_mfree(buff2);

	return stat;
}


int
rawdump_uls_file(const char *filepath, FILE *fp_out)
{
	int stat = 0;
	uls_lex_ptr_t uls;
	uls_istream_ptr_t istr;

	if ((istr = uls_open_istream_file(filepath)) == uls_nil) {
		ult_log("can't read %s", filepath);
		return -1;
	}

	if ((uls=istr->uls) == uls_nil) {
		uls = sam_lex;
	}

	if (uls_set_istream(uls, istr, uls_ptr(tmpl_list), 0) < 0) {
		ult_log("%s: fail to prepare input-stream %s", ulc_config, filepath);
		stat = -2;
	} else if ((stat = rawdump_stream_file(uls, fp_out)) < 0) {
		ult_log("fail to read uls-file %s", filepath);
		stat = -3;
	}

	uls_destroy_istream(istr);
	return stat;
}

int
rawdump_uls_files(int n_fpaths, const char** fpaths, FILE *fp_out)
{
	int i, rc, stat=0;
	char *input_file;

	for (i=0; i<n_fpaths; i++) {
		input_file = ult_strdup(fpaths[i]);
		uls_path_normalize(input_file, input_file);

		rc = rawdump_uls_file(input_file, fp_out);
		uls_mfree(input_file);
		if (rc < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
}
