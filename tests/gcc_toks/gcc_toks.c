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
  <file> gcc_toks.c </file>
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
#include <string.h>

#include "gcc_lex.h"

const char *progname;
int  opt_verbose;

int   test_mode = -1;
char *config_name;

const char *input_file;

uls_lex_t *gcc_lex;

static char *lbuff;
static int lbuff_siz;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  Dumps tokens in the inputfiles");
	err_log("  according to the token defintions of %s", config_name);
	err_log("");
	err_log(" Usage:");
	err_log("  %s <target-dir> <inputfile ...>", progname);
	err_log("");
	err_log("  For example,");
	err_log("      %s kernel input1.txt", progname);
	err_log("  A default config-file used, %s if you don't specifiy the config-file.", config_name);
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
		++opt_verbose;
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
proc_file_coord(const char *buf)
{
	int lno, len, siz;
	char *lptr, *wrd;

	if ( (len = strlen(buf))>= lbuff_siz) {
		siz = len + 128;
		lbuff = (char *) uls_mrealloc(lbuff, siz);
		lbuff_siz = siz;
	}
	// len  < buff_siz

	uls_memcopy(lbuff, buf, len);
	lbuff[len] = '\0';
	lptr = lbuff;

	// line
	wrd = uls_splitstr(&lptr);
	lno = atoi(wrd);

	// filename
	lptr = wrd = skip_blanks(lptr);
	if (*wrd == '\0') {
		wrd = NULL;
	} else {
		++wrd;
		lptr = split_litstr(wrd, *lptr);
	}

	uls_set_tag(gcc_lex, wrd, lno - 1);

	return 1;
}

int
lex_input_file(const char *fpath)
{
	const char *tokstr;
	int t;

	if (uls_push_file(gcc_lex, fpath, 0) < 0) {
		err_log("can't set the istream!");
		return -1;
	}

	uls_set_tag(gcc_lex, fpath, -1);

	for ( ; ; ) {
		if ((t=uls_get_tok(gcc_lex)) == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log("program abnormally terminated!");
			}
			break;
		}

		if (t == TOK_EOL) {
			continue;
		}

		tokstr = uls_lexeme(gcc_lex);
		if (t == TOK_WCOORD) {
			proc_file_coord(tokstr);
			continue;
		}

		uls_printf("%s:%3d", uls_get_tag(gcc_lex), uls_get_lineno(gcc_lex));
		uls_dump_tok(gcc_lex, " ", "\n");
	}

	return 0;
}

int
test_gnu_c(const char **args, int n_args)
{
	const char *fpath;
	int i, stat=0;

	if (n_args < 1) {
		err_log("%s: invalid parameter!", __FUNCTION__);
		return -1;
	}

	for (i=0; i<n_args; i++) {
		fpath = args[i];

		if (lex_input_file(fpath) < 0) {
			stat = -1;
			break;
		}
	}

	return stat;
}

int
lex_input_line()
{
	const char *tokstr;
	const char *tagstr;
	int t;

	if (uls_push_line(gcc_lex, "// comment here!\nTo parse\ngcc pre-processed files", -1, 0) < 0) {
		err_log("can't set the istream!");
		return -1;
	}

	for ( ; ; ) {
		if ((t=uls_get_tok(gcc_lex)) == TOK_EOI || t == TOK_ERR) {
			if (t == TOK_ERR) {
				err_log("program abnormally terminated!");
			}
			break;
		}

		tokstr = uls_lexeme(gcc_lex);
		if (t == TOK_WCOORD) {
			proc_file_coord(tokstr);
			continue;
		}

		tagstr = uls_get_tag(gcc_lex);
		if (*tagstr != '\0') uls_printf("%s:", tagstr);
		uls_printf("%3d", uls_get_lineno(gcc_lex));

		if (t == TOK_EOL) {
			uls_printf(" [    EOL]\n");
		} else {
			uls_dump_tok(gcc_lex, " ", "\n");
		}
	}

	return 0;
}

int
main(int argc, char* argv[])
{
	int i0;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);
	config_name = "gcc.ulc";

	if ((i0=uls_getopts(argc, argv, "m:vhz", options)) <= 0) {
		return i0;
	}

	if (i0 < argc) {
		input_file = argv[i0];
	}

	if ((gcc_lex=uls_create(config_name)) == NULL) {
		err_log("can't init uls-object");
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_gnu_c((const char **) argv+i0, argc-i0);
		break;
	case 1:
		lex_input_line();
		break;

	default:
		break;
	}

	uls_destroy(gcc_lex);

	uls_mfree(lbuff);

	return 0;
}
