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
  <file> test_1st.c </file>
  <brief>
    After installing ULS libraries, build this program to check that ULS correctly works.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <uls.h>
#include <uls/simple_lex.h>

const char *progname;
int  opt_verbose;

char *config_name;
char *input_file;

uls_lex_t *simple_lex;

static void usage(void)
{
	err_log("usage(%s): dumping the tokens defined as in 'simple.ulc'", progname);
	err_log("\t%s -c <config-file> <file1> ...", progname);
}

static int
options(int opt, char* optarg)
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

void
test_uls(char* fpath)
{
	int fd;

	if ((fd=open(fpath, O_RDONLY)) < 0) {
		err_log(" file open error");
		return;
	}

	if (uls_set_fd(simple_lex, fd, 0) < 0) {
		err_log("can't set the istream!");
		return;
	}

	uls_set_tag(simple_lex, fpath, 1);

	for ( ; ; ) {
		if (uls_get_tok(simple_lex) == TOK_EOI) break;
		uls_dump_tok(simple_lex, "\t", "\n");
	}

	close(fd);
}

int
test_uls_creating(int argc, char* argv[], int i0)
{
	int i;

	if ((simple_lex = uls_create(config_name)) == NULL) {
		err_log("%s: can't init uls-object of '%s'", __FUNCTION__, config_name);
		ulc_list_searchpath("simple");
		return -1;
	}

	for (i=i0; i<argc; i++) {
		input_file = argv[i];
		test_uls(input_file);
	}

	uls_destroy(simple_lex);

	return 0;
}

int
main(int argc, char* argv[])
{
	int i0;
#ifdef _ULS_WANT_STATIC_LIBS
	initialize_uls();
#endif
	progname = uls_filename(argv[0], NULL);
	// opt_verbose = 1;
	config_name = "simple";
	input_file = "input1.txt";

	if ((i0=uls_getopts(argc, argv, "vhV", options)) <= 0) {
		return i0;
	}

	test_uls_creating(argc, argv, i0);

	return 0;
}
