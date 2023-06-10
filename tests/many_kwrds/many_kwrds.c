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
  <file> many_keys.c </file>
  <brief>
    Test the case of many keywords in ULC-spec.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/


#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_fileio.h"
#include "uls/uls_init.h"

#include <stdlib.h>
#include <string.h>

#include "sample_lex.h"

#define FILEBUFF_SIZ  4095

const char *progname;
int  test_mode = -1;
int  opt_verbose;
const char *config_name;
char *input_file;

uls_lex_t *sample_lex;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  Demonstrates the lexical scanner having many keywords");
	err_log("");
	err_log(" Usage:");
	err_log("  %s [-c <config-file>] <inputfile> ...", progname);
	err_log("");
	err_log("  For example,");
	err_log("      %s input1.txt", progname);
	err_log("  A default config-file used, %s if you don't specifiy the config-file.", config_name);
}

static int
options(int opt, char* optarg)
{
	int stat = 0;

	switch (opt) {
	case 'c':
		config_name = optarg;
		break;

	case 'm':
		test_mode = atoi(optarg);
		break;

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

static FILE   *cur_fin;
static char   filebuff[FILEBUFF_SIZ+1];

int
proc_file(char* fpath)
{
	int len, stat = 0, lno = 0;

	if ((cur_fin=uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log("%s: fail to open '%s'", __FUNCTION__, fpath);
		return -1;
	}

	while (1) {
		if ((len=uls_fp_gets(cur_fin, filebuff, sizeof(filebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log("%s: error to read a line", __FUNCTION__);
				stat =-1;
			}
			break;
		}

		uls_set_line(sample_lex, filebuff, strlen(filebuff), 0);
		uls_inc_lineno(sample_lex, ++lno);

		if (uls_get_tok(sample_lex) != TOK_EOI) {
			uls_dumpln_tok(sample_lex);
		}
	}

	uls_fp_close(cur_fin);

	return stat;
}

int
main(int argc, char* argv[])
{
	int i, i0;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);
	config_name = "sample.ulc";
	input_file = "input1.txt";

	if ((i0=uls_getopts(argc, argv, "c:m:vh", options)) <= 0) {
		return i0;
	}

	if ((sample_lex = uls_create(config_name)) == NULL) {
		err_log("can't init uls-object");
		return -1;
	}

	for (i=i0; i<argc; i++) {
		input_file = argv[i];
		proc_file(input_file);
	}

	uls_destroy(sample_lex);

	return 0;
}
