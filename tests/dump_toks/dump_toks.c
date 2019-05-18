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
  <file> dump_toks.c </file>
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
#include "uls/uls_fileio.h"
#include "uls/uls_init.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "sample_lex.h"

const char *progname;
int  opt_verbose;

const char *config_name;
char *input_file;
int   test_mode = -1;

uls_lex_t *sample_lex;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  Dumps tokens in the inputfiles");
	err_log("  according to the token defintions in %s", config_name);
	err_log("");
	err_log(" Usage:");
	err_log("  %s [-c <config-file>] <inputfile>", progname);
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
test_initial_uls(char* fpath)
{
	uls_lex_t *uls = sample_lex;
	double f_val;
	uls_int64 i_val;
	const char *tagstr;
	int t, taglen;
	uls_outparam_t parms;

	if (uls_push_file(uls, fpath, ULS_WANT_EOFTOK) < 0) {
		return -1;
	}

	uls_printf(" %3d : %3d  '%s'\n",
		uls_get_lineno(uls), uls_tok(uls), uls_lexeme(uls));

	for ( ; ; ) {
		if ((t=uls_get_tok(uls)) == TOK_EOI) break;

		uls_printf(" %3d : %3d  '%s'\n",
			uls_get_lineno(uls), uls_tok(uls), uls_lexeme(uls));

		if (t == TOK_NUMBER) {
			if (uls_is_real(uls)) {
				f_val = uls_lexeme_double(uls);
				uls_printf("\tf_val = %f\n", f_val);
			} else {
				i_val = uls_lexeme_lld(uls);
				uls_printf("\ti_val = %lld\n", i_val);
			}
		} else if (t == TOK_EOF) {
			tagstr = uls_eof_tag(uls, &parms);
			taglen =  parms.len;
			uls_printf("\tEOF : '%s'%d\n", tagstr, taglen);
		}
	}

	return 0;
}

void
test_uls(char* fpath)
{
	uls_lex_t *uls = sample_lex;
	int fd, t;

	if ((fd=open(fpath, O_RDONLY)) < 0) {
		err_log(" file open error");
		return;
	}

	if (uls_set_fd(uls, fd, ULS_DO_DUP) < 0) {
		err_log("can't set the istream!");
		return;
	}

	uls_set_tag(uls, fpath, 1);

	for ( ; ; ) {
		if ((t=uls_get_tok(uls)) == TOK_ERR) {
			err_log("TOK_ERR!");
			uls_pop(uls);
			break;
		} else if (t == TOK_EOI) {
			break;
		}

		uls_printf("%3d", uls_get_lineno(uls));
		uls_dumpln_tok(uls);
	}

	close(fd);
}

typedef struct {
	int tok_id;
	int prec;
	int node_id;
	void (*node_op)(void* a, void* b);
} sample_xdef_t;

void mul_nodetree(void* a, void* b) {}
void add_nodetree(void* a, void* b) {}
void div_nodetree(void* a, void* b) {}
void and_nodetree(void* a, void* b) {}
void xor_nodetree(void* a, void* b) {}

sample_xdef_t xdefs[5] = {
	{ '*', 27, 1, mul_nodetree },
	{ '+', 26, 2, add_nodetree },
	{ '/', 25, 3, div_nodetree },
	{ '&', 24, 4, and_nodetree },
	{ '^', 23, 5, xor_nodetree }
};

void
test_uls_xdef(char* fpath)
{
	uls_lex_t *uls = sample_lex;
	sample_xdef_t *xdef;
	const char *ptr;
	char buff[64];
	int i;

	for (i=0; i<5; i++) {
		if (uls_set_extra_tokdef(uls, xdefs[i].tok_id, xdefs + i) < 0) {
			ptr = uls_tok2keyw(uls, xdefs[i].tok_id);
			if (ptr == NULL) ptr = "???";
			err_log("fail to set x-tokdef for tok %s.", ptr);
		}
	}

	if (uls_push_file(uls, fpath, 0) < 0) {
		err_log(" file open error");
		return;
	}

	for ( ; ; ) {
		if (uls_get_tok(uls) == TOK_EOI) break;

		xdef = (sample_xdef_t *) uls_get_current_extra_tokdef(uls);

		uls_printf("%3d", uls_get_lineno(uls));

		if (xdef != NULL) {
			uls_snprintf(buff, sizeof(buff), " prec=%d node_id=%d\n", xdef->prec, xdef->node_id);
		} else {
			buff[0] = '\n'; buff[1] = '\0';
		}

		uls_dump_tok(uls, "\t", buff);
	}
}

int
uls_fill_FILE_source(uls_source_t* isrc, char* buf, int buflen, int bufsiz)
{
	FILE  *fp = (FILE *) isrc->usrc;
	return fread(buf + buflen, sizeof(char), bufsiz - buflen, fp);
}

static void
uls_ungrab_FILE_source(uls_source_t* isrc)
{
	FILE  *fp = (FILE *) isrc->usrc;
	uls_fp_close(fp);
}

void
test_uls_isrc(char* fpath)
{
	uls_lex_t *uls = sample_lex;
	FILE   *fp;

	if ((fp=uls_fp_open(fpath, ULS_FIO_READ)) == NULL) {
		err_log(" file open error");
		return;
	}

	uls_push_isrc(uls, (void *) fp,
		uls_fill_FILE_source, uls_ungrab_FILE_source);

	uls_set_tag(uls, fpath, 1);

	for ( ; ; ) {
		if (uls_get_tok(uls) == TOK_EOI) break;

		uls_printf("%3d", uls_get_lineno(uls));
		uls_dumpln_tok(uls);
	}
}

int
main(int argc, char* argv[])
{
	int i0;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);
	config_name = "sample.ulc";

	if ((i0=uls_getopts(argc, argv, "c:vm:hz", options)) <= 0) {
		return i0;
	}

	if (i0 < argc) {
		input_file = argv[i0];
	}

	if ((sample_lex=uls_create(config_name)) == NULL) {
		err_log("can't init uls-object");
		return -1;
	}

	switch (test_mode) {
	case 0:
		test_uls(input_file);
		break;
	case 1:
		test_initial_uls(input_file);
		break;
	case 2:
		test_uls_xdef(input_file);
		break;
	case 3:
		test_uls_isrc(input_file);
		break;
	default:
		break;
	}

	uls_destroy(sample_lex);

	return 0;
}
