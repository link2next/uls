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
  <file> id_stat.c </file>
  <brief>
    Read from input file and gather statistics of identifiers.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/


#include "uls/uls_lex.h"
#include "uls/uls_log.h"
#include "uls/uls_util.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_init.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "sample_lex.h"

#define streql(a,b) (strcmp(a,b)==0)

const char *progname;
int  test_mode = -1;
int  opt_verbose;
const char *config_name;
char *input_file;

char *target_dir;
char home_dir[1024];
FILE *fp_list;
const char *filelist;

uls_lex_t sample_lex;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  Dumps the identifiers in inputfiles");
	err_log("  according to the token defintions in %s", config_name);
	err_log("");
	err_log(" Usage:");
	err_log("\t%s [-c <config-file>]  <inputfile>", progname);
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

	case 'l':
		filelist = optarg;
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

typedef struct _id_stat id_stat_t;
struct _id_stat {
	char name[64];
	int freq;
};

id_stat_t *id_stat_array;
int n_id_stat_array;
int n_alloc_id_stat_array;

id_stat_t*
find_id_stat(const char *name)
{
	id_stat_t *e;
	int i;

	for (i=0; i<n_id_stat_array; i++) {
		e = id_stat_array + i;
		if (streql(e->name, name))
			return e;
	}

	return NULL;
}

id_stat_t*
append_id_stat(const char *name)
{
	id_stat_t *e;

	if (n_id_stat_array >= n_alloc_id_stat_array) {
		n_alloc_id_stat_array += 128;
		id_stat_array = uls_mrealloc(id_stat_array, n_alloc_id_stat_array * sizeof(id_stat_t));
	}

	e = id_stat_array + n_id_stat_array++;
	strcpy(e->name, name);
	e->freq = 0;

	return e;
}

int
proc_file(char* fpath)
{
	id_stat_t *e;
	int tok;

	if (fpath == NULL) return -1;

	uls_set_file(&sample_lex, fpath, 0);

	for ( ; ; ) {
		tok = uls_get_tok(&sample_lex);

		if (tok == TOK_EOI) break;

		if (tok == TOK_ID) {
			if ((e=find_id_stat(uls_lexeme(&sample_lex))) == NULL) {
				e = append_id_stat(uls_lexeme(&sample_lex));
			}

			++e->freq;
		}
	}

	return 0;
}

int
test_id_stat(char* fpath)
{
	int tok;

	if (fpath == NULL) return -1;

	uls_push_file(&sample_lex, fpath, 0);

	for ( ; ; ) {
		tok = uls_get_tok(&sample_lex);
		if (tok == TOK_EOI) break;

		uls_printf("%3d: <%3d> ", uls_get_lineno(&sample_lex), tok);
		uls_dump_tok(&sample_lex, NULL, "\n");
	}

	return 0;
}

int
proc_filelist(FILE *fin)
{
	char  linebuff[1024], *lptr, *fpath;
	int   i, len, lno = 0, stat=0;

	while (1) {
		if ((len=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log("%s: error to read a line", __FUNCTION__);
				stat =-1;
			}
			break;
		}

		++lno;

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		fpath = lptr;
		for (i=strlen(fpath)-1; i>=0; i--) {
			if (!isspace(fpath[i])) break;
		}
		fpath[++i] = '\0';

		if (proc_file(fpath) < 0) {
			err_log("fail to process '%s'", fpath);
			stat = -1;
			break;
		}
	}

	return stat;
}

static int
comp_by_idname(const uls_voidptr_t a, const uls_voidptr_t b)
{
	id_stat_t *a1 = (id_stat_t *) a;
	id_stat_t *b1 = (id_stat_t *) b;

	return strcmp(a1->name, b1->name);
}

int
get_id_stats(int argc, char* argv[], int i0)
{
	id_stat_t *e;
	int i;

	if (filelist != NULL ) {
		if (i0 < argc) {
			target_dir = argv[i0++];
		}

		if ((fp_list=uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
			err_log("%s: fail to read '%s'", __FUNCTION__, filelist);
			return -1;
		}

		if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
			err_panic("can't get the pwd!");
		}

		if (target_dir != NULL) {
			if (uls_chdir(target_dir) < 0) {
				err_log("can't change to %s", target_dir);
				uls_fp_close(fp_list);
				return -1;
			}
		}

		proc_filelist(fp_list);
		uls_fp_close(fp_list);
		uls_chdir(home_dir);

	} else {
		for (i=i0; i<argc; i++) {
			proc_file(argv[i]);
		}
	}

	uls_quick_sort(id_stat_array, n_id_stat_array, sizeof(id_stat_t), comp_by_idname);

	for (i=0; i<n_id_stat_array; i++) {
		e = id_stat_array + i;
		uls_printf("%24s %8d\n", e->name, e->freq);
	}

	return 0;
}

int
test_initial_tok(uls_lex_ptr_t uls)
{
	int i, tok;

	for (i=0; i<3; i++) {
		tok = uls_get_tok(uls);
		printf("[%d] tok_id=%d, lexeme='%s'\n", i, tok, uls_lexeme(uls));
	}

	return 0;
}

int
main(int argc, char* argv[])
{
	int i0, i, rc;

	initialize_uls();
	progname = uls_filename(argv[0], NULL);
	target_dir = NULL;
	config_name = "sample.ulc";
	input_file = "input1.txt";

	if ((i0=uls_getopts(argc, argv, "c:l:m:vh", options)) <= 0) {
		return i0;
	}

	if (uls_init(&sample_lex, config_name) < 0) {
		err_log("can't init uls-object");
		return -1;
	}

	switch (test_mode) {
	case 0:
		rc = get_id_stats(argc, argv, i0);
		break;
	case 1:
		for (i=i0; i<argc; i++) {
			rc = test_id_stat(argv[i]);
			if (rc < 0) break;
		}
		break;
	case 2:
		rc = test_initial_tok(&sample_lex);
		break;
	default:
		rc = 0;
		break;
	}

	uls_destroy(&sample_lex);
	uls_mfree(id_stat_array);

	return 0;
}
