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
  <file> two_lex.c </file>
  <brief>
    Read from 2 instances of uls simultaenously.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/


#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_init.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "sam1_lex.h"
#include "sam2_lex.h"

#define streql(a,b) (strcmp(a,b)==0)

const char *progname;
int  test_mode = -1;
int  opt_verbose;

uls_lex_t *sam1_lex, *sam2_lex;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  ULS can instantiate many uls lexical objects.");
	err_log("  '%s' demonstrates this feature of ULS", progname);
	err_log("");
	err_log(" Usage:");
	err_log("  %s <inputfile> ...", progname);
	err_log("");
	err_log("  For example,");
	err_log("      %s input1.txt", progname);
}

static int
options(int opt, char* optarg)
{
	int stat = 0;

	switch (opt) {
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

void
test_uls_2(char* fpath)
{
	int i;

	if (uls_push_file(sam2_lex, fpath, 0) < 0) {
		err_log(" file open error");
		return;
	}

	for (i=0; ; ) {
		if (uls_get_tok(sam2_lex) == T2_EOI) break;

		uls_printf("\t\t%3d", uls_get_lineno(sam2_lex));
		uls_dump_tok(sam2_lex, NULL, "\n");

		++i;
	}
}

int
test_uls(char* fpath)
{
	int fd;
	int i;

	if ((fd=open(fpath, O_RDONLY)) < 0) {
		err_log(" file open error");
		return -1;
	}

	if (uls_push_fd(sam1_lex, fd, 0) < 0) {
		err_log("can't set the istream!");
		return -1;
	}

	uls_set_tag(sam1_lex, fpath, 1);
	for (i=0; ; ) {
		if (uls_get_tok(sam1_lex) == T1_EOI) break;

		uls_printf("\t%3d ", uls_get_lineno(sam1_lex));
		uls_dumpln_tok(sam1_lex);

		if (uls_tok(sam1_lex) == T1_ID && streql(uls_lexeme(sam1_lex), "WWW")) {
			test_uls_2("input2.txt");
		}

		++i;
	}

	close(fd);
	return 0;
}

int
main(int argc, char* argv[])
{
	char *conf_name1 = "sam1.ulc";
	char *conf_name2 = "sam2.ulc";
	char *input_file;
	int  i, i0;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);

	if ((i0=uls_getopts(argc, argv, "m:vh", options)) <= 0) {
		return i0;
	}

	if ((sam1_lex=uls_create(conf_name1)) == NULL) {
		err_log("can't init uls-object");
		return 1;
	}

	if ((sam2_lex=uls_create(conf_name2)) == NULL) {
		err_log("can't init uls-object");
		uls_destroy(sam1_lex);
		return 1;
	}

	for (i=i0; i<argc; i++) {
		input_file = argv[i];
		test_uls(input_file);
	}

	uls_destroy(sam1_lex);
	uls_destroy(sam2_lex);

	return 0;
}
