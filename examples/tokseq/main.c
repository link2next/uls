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
  <file> token_stream.c </file>
  <brief>
    After installing ULS libraries, build this program to check that ULS correctly works.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Jul 2015.
  </author>
*/

#include <uls.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "tokseq_lex.h"

#define N_CMD_ARGS  16
#define N_TMPLVAL_ARRAY 8

const char * progname;

const char * config_name;
const char * input_file;

int  opt_verbose;

uls_lex_t *sample_lex;
uls_tmpl_list_t *tmpl_list;

int
print_uls_file(uls_lex_t *uls, const char * fpath, uls_ostream_t *ostr)
{
	int t, stat=0;
	const char * lxm;

	if (uls_push_file(uls, fpath, 0) < 0) {
		err_log("%s: fail to prepare input-stream %s.", __func__, fpath);
		return -1;
	}

	for ( ; ; ) {
		t = uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1; break;
		}

		if (uls_is_eof(uls) || uls_is_eoi(uls)) break;

		lxm = uls_lexeme(uls);

		if (uls_is_id(uls) && uls_get_tmpl_value(tmpl_list, lxm) != NULL) {
			t = uls_toknum_tmpl(uls);
		}

		if (uls_print_tok(ostr, t, lxm) < 0) {
			stat = -1; break;
		}
	}

	return stat;
}

int
read_uls_file(uls_lex_t *uls, const char * fpath)
{
	int stat=0;
	uls_istream_t *istr;
	const char * lxm;

	if ((istr = uls_open_istream_file(fpath)) == NULL) {
		err_log("can't read %s", fpath);
		return -1;
	}

	if (uls_push_istream(uls, istr, tmpl_list, 0) < 0) {
		err_log("%s: fail to prepare input-stream %s.", __func__, fpath);
		uls_destroy_istream(istr);
		return -1;
	}

	for ( ; ; ) {
		uls_get_tok(uls);

		if (uls_is_err(uls)) {
			stat = -1; break;
		}

		if (uls_is_eoi(uls)) break;

		lxm = uls_lexeme(uls);

		uls_printf("\t[%7t] %s\n", uls, lxm);
	}

	uls_destroy_istream(istr);
	return stat;
}

int
run_test_scenario(const char * out_dir)
{
	char uls_fname[1024];
	const char  *tptr, *tname;
	uls_ostream_t *ostr;
	int stat=0;

	uls_snprintf(uls_fname, 1024, "%s%c%s", out_dir, ULS_FILEPATH_DELIM, "a1.uls");

	// Step-1: Instantiate template vars <TVAR1,TVAR2>
	tname = "AVAR";
	tptr = "unsigned long long";
	uls_add_tmpl(tmpl_list, tname, tptr);

	tname = "BVAR";
	tptr = "long double";
	uls_add_tmpl(tmpl_list, tname, tptr);

	// Step-2: writing stream to output file 'a1.uls'
	if ((ostr = uls_create_ostream_file(uls_fname, sample_lex, "<tag_name>")) == NULL) {
		err_log("can't set uls-stream to %s", uls_fname);
		return -1;
	}

	print_uls_file(sample_lex, input_file, ostr);
	uls_destroy_ostream(ostr);

	// Step-3: re-read the uls file "a1.uls" with the template vars.
	read_uls_file(sample_lex, uls_fname);

	return stat;
}

static void usage(void)
{
	err_log("usage(%s): dumping the tokens defined as in 'simple.ulc'", progname);
	err_log("\t%s -c <config-file> <file1> ...", progname);
}

static int
options(int opt, char * opttarg)
{
	int   stat = 0;

	switch (opt) {
	case 'v':
		opt_verbose = 1;
		break;
	case 'h':
		usage();
		stat = 1;
		break;
	default:
		err_log("undefined option -%c", opt);
		usage();
		stat = -1;
		break;
	}

	return stat;
}

int
test_main(int argc, char **targv)
{
	char * out_dir;
	int i0;

	// opt_verbose = 1;
	config_name = "tokseq.uld";
	out_dir = ".";

	if ((i0=uls_getopts(argc, targv, "vhV", options)) <= 0) {
		err_log("Fail to parse command options.");
		return i0;
	}

	if (i0 < argc) {
		input_file = targv[i0];
		if (uls_dirent_exist(input_file) != ST_MODE_FILE) {
			err_panic("%s: not a file'", input_file);
		}
	} else {
		input_file = "tokseq_sam.txt";
	}

	if (i0 + 1 < argc) {
		out_dir = targv[i0 + 1];
		if (uls_dirent_exist(out_dir) != ST_MODE_DIR) {
			err_panic("%s: not a directory'", out_dir);
		}
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log("%s: can't init uls-object of '%s'", __func__, config_name);
		ulc_list_searchpath("simple");
		return -1;
	}

	run_test_scenario(out_dir);
	uls_destroy(sample_lex);

	return 0;
}

int
main(int argc, char **argv)
{
	int stat;

	progname = argv[0];

	tmpl_list = uls_create_tmpls(N_TMPLVAL_ARRAY, 0);
	stat = test_main(argc, argv);
	uls_destroy_tmpls(tmpl_list);

	return stat;
}
