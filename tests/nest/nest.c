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
  <file> nested.c </file>
  <brief>
    Read from input file and just dump tokens.
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

#include "sample_lex.h"

#define streql(a,b) (strcmp(a,b)==0)

const char *progname;
int  test_mode = -1;
int  opt_verbose;
const char *config_name;
char *input_file;

uls_lex_t *sample_lex;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  Tokenizes the input that includes another file.");
	err_log("  Tests if uls_unget_tok() is properly operating.");
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

void
test_uls(char* fpath)
{
	int fd;
	int tok;
	int i, n_pushes, tabs=0;
	char buff[16];

	if ((fd=open(fpath, 0)) < 0) {
		err_log(" file open error");
		return;
	}

	if (uls_set_fd(sample_lex, fd, 0) < 0) {
		err_log("can't set the istream!");
		return;
	}

	uls_set_tag(sample_lex, fpath, 1);
	uls_want_eof(sample_lex);

	for (n_pushes=0; ; ) {
		if ((tok=uls_get_tok(sample_lex)) == TOK_EOI) break;

		if (n_pushes > 0) {
			uls_unget_tok(sample_lex);
			--n_pushes;
			continue;
		}

		for (i=0; i<tabs; i++) uls_printf("\t");

		if (tok == TOK_EOF) {
			uls_printf(" EOF: '%s'\n", uls_lexeme(sample_lex));
			--tabs;
			continue;
		}

		uls_printf("%3d", uls_get_lineno(sample_lex));
		uls_dump_tok(sample_lex, "\t", "\n");

		if (tok == TOK_ID) {
			if (streql(uls_lexeme(sample_lex), "WWW")) {
				uls_push_file(sample_lex, "./input1_0.txt", ULS_WANT_EOFTOK);
				uls_push_file(sample_lex, "./input1_1.txt", ULS_WANT_EOFTOK);
				tabs += 2;

			} else if (streql(uls_lexeme(sample_lex), "VVV")) {
				for (i=2; i>=0; i--) {
					uls_snprintf(buff, sizeof(buff), "VVV%d", i);

					uls_unget_ch(sample_lex, ' ');
					uls_unget_str(sample_lex, buff);
				}

			} else if (streql(uls_lexeme(sample_lex), "UUU")) {
				n_pushes = 3;
			}
		}
	}

	close(fd);
}

int
main(int argc, char* argv[])
{
	int i, i0;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);
	config_name = "sample.ulc";

	if ((i0=uls_getopts(argc, argv, "c:m:vh", options)) <= 0) {
		return i0;
	}

	for (i=i0; i<argc; i++) {
		input_file = argv[i];

		if ((sample_lex = uls_create(config_name)) == NULL) {
			err_log("can't init uls-object");
			return -1;
		}

		test_uls(input_file);

		uls_destroy(sample_lex);
	}

	return 0;
}
