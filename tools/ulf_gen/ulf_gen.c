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
  <file> ulf_gen.c </file>
  <brief>
    Generating ULF(Unified Lexical Frequencies) file gathering statistics of keywords usage.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, August 2011.
  </author>
*/

#define ULS_DECL_PROTECTED_PROC
#include "uls.h"
#include "uls/uls_misc.h"
#include "uls/uls_freq.h"

#include "ult_utils.h"
#include <ctype.h>
#include <time.h>

#define THIS_PROGNAME "ulf_gen"
#define DFL_N_SAMPLES 1000

_ULS_DEFINE_STRUCT(round_stat)
{
	int n_buckets;
	double avg, sigma2;
	double gamma;
};

_ULS_DEFINE_STRUCT(stat_of_round)
{
	uls_hash_stat_t hcodes;
	round_stat_t state;
};

char *progname;
char home_dir[ULS_FILEPATH_MAX+1];
const char *config_file;
char *filelist;

char *out_file;
char out_file_buff[ULS_FILEPATH_MAX+1];
int  opt_verbose;
int  opt_optimize_level;

uls_lex_ptr_t sam_lex;
uls_hashfunc_t ulf_hashfunc;
int n_samples;
int *g_hash_buckets;

static int i_weight_list, j_weight_list, k_weight_list;

#define NUM_WPRIMES  25
static int weight_plist[NUM_WPRIMES] = {
	 2,  3,  5,  7,
	11, 13, 17, 19,
	23, 29,
	31, 37,
	41, 43, 47,
	53, 59,
	61, 67,
	71, 73, 79,
	83, 89,
	97
};

void init_weights_indices();
int next_weights_indices(int space_size);

#define ULFGEN_OPTSTR "L:l:o:O:n:s:vVHh"

#ifdef HAVE_GETOPT
#include <getopt.h>

static const struct option longopts[] = {
	{ "lang",  required_argument,      NULL, 'L' },
	{ "list",  required_argument,      NULL, 'l' },
	{ "output",  required_argument,    NULL, 'o' },
	{ "num-iter",  required_argument,  NULL, 'n' },
	{ "size-hash",  required_argument, NULL, 's' },
	{ "optimize",  required_argument,  NULL, 'O' },
	{ "verbose",  no_argument,         NULL, 'v' },
	{ "version", no_argument,          NULL, 'V' },
	{ "Help",    no_argument,          NULL, 'H' },
	{ "help",    no_argument,          NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
#endif

static void usage_synopsis(void)
{
	err_log("Usage: %s -L <ulc-file> [options] <file|dir>", progname);
	err_log("  %s generates the frequencies of the keywords of a language.", progname);
	err_log("       %s -L <ulc-file> <file1> <file2> ...", progname);
	err_log("       %s -L <ulc-file> -o /usr/local/share/a.ulf <file1> <file2> ...", progname);
	err_log("       %s -L <ulc-file> -l <list-file> <target-dir>", progname);
}

static void usage_desc(void)
{
#ifdef ULS_WINDOWS
	err_log("  -L <ulc-spec>      Specify the lexical-spec(*.ulc) of the language");
	err_log("  -l <list-file>     Specify the list of data source files");
	err_log("  -o <a-file>        Specify the the output filepath(*.ulf)");
	err_log("  -O <level=1,2,3>   Specify the optimizing level for hashcode");
	err_log("  -n <num>           Specify the number of random samples");
	err_log("  -v, --verbose      verbose mode");
	err_log("  -V, --version      Print the version information");
	err_log("  -h, --help         Display the short help");
#else
	err_log("  -L, --lang=<ulc-spec>   Specify the lexical-spec(*.ulc) of the language");
	err_log("  -l, --list=<list-file>  Specify the list of data source files");
	err_log("  -o, --output=<a-file>   Specify the the output filepath(*.ulf)");
	err_log("  -O, --optimize <1,2,3>  Specify the optimizing level for hashcode");
	err_log("  -n <num>                Specify the number of random samples");
	err_log("  -v, --verbose           verbose mode");
	err_log("  -V, --version           Print the version information");
	err_log("  -h, --help              Display the short help");
#endif
}

static void usage_brief(void)
{
	usage_synopsis();
	err_log("");

	usage_desc();
	err_log("");
}

static void usage(void)
{
	usage_brief();
}

static void usage_long(void)
{
	usage_brief();

	err_log("%s collects the usage statistics of given language", progname);
	err_log(" by sampling from source code files.");
	err_log("It dumps the frequencies of keywords.");
	err_log("After saving them in the file suffixed by '*.ulf', you can use it together with its ulc-file.");
	err_log("For example, Let the output file of 'sample.ulc' be 'sample.ulf'.");
	err_log("The file 'sample.ulf' is together read if only it exists in the same directory of sample.ulc.");
	err_log("The ulf-file is optional but can accelerates the generated lexical tokenizer.");
	err_log("");

	err_log("To get the ulf-file of 'sample.ulc' from listed source code files,");
	err_log("  %s -L sample.ulc a.c b.c src/d.c", progname);
	err_log("");

	err_log("To save the output-file to other file path than default, use o-option.");
	err_log("  %s -o /opt/share/b.ulf -L sample src/main.c src/main.h", progname);
	err_log("");

	err_log("To process all the file paths in the list file 'a.list'");
	err_log("  %s -L sample -l a.list /package/home", progname);
	err_log("This processes all the file paths joined by '/package/home'.");
	err_log("");
}

static int ulfgen_options(int opt, char* optarg)
{
	int   stat = 0;

	switch (opt) {
	case 'L':
		config_file = optarg;
		break;

	case 'l':
		filelist = ult_strdup(optarg);
		uls_path_normalize(filelist, filelist);
		break;

	case 'n':
		n_samples = atoi(optarg);
		break;

	case 'o':
		out_file = ult_strdup(optarg);
		uls_path_normalize(out_file, out_file);
		break;

	case 's':
		break;

	case 'O':
		opt_optimize_level = atoi(optarg);
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'V':
		uls_printf("%s %s, written by %s,\n\tis provided under %s.\n",
			progname, ULF_GEN_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		stat = 1;
		break;

	case 'H':
		usage_long();
		stat = 3;
		break;

	case 'h':
		usage();
		stat = 2;
		break;

	default:
		err_log("undefined option -%c", opt);
		stat = -1;
		break;
	}

	return stat;
}

static int
parse_options(int argc, char* argv[])
{
#ifdef HAVE_GETOPT
	int   rc, opt, longindex;
#endif
	int   i0;

	if (ult_getcwd(home_dir, sizeof(home_dir)) < 0)
		err_panic("%s: fail to getcwd()", __func__);

	filelist = NULL;
	out_file = NULL;
	n_samples = DFL_N_SAMPLES;

#ifdef HAVE_GETOPT
	while ((opt=getopt_long(argc, argv, ULFGEN_OPTSTR, longopts, &longindex)) != -1) {
		if ((rc=ulfgen_options(opt, optarg)) != 0) {
			if (rc > 0) rc = 0;
			return rc;
		}
	}
	i0 = optind;
#else
	if ((i0=uls_getopts(argc, argv, ULFGEN_OPTSTR, ulfgen_options)) <= 0) {
		return i0;
	}
#endif

	if (config_file == NULL) {
		err_log("specify the path of config-file(*.ulc)");
		return -1;
	}

	return i0;
}

static void
init_stat_round(stat_of_round_ptr_t p_round)
{
	memset(p_round, 0x00, sizeof(stat_of_round_t));
	uls_init_hash_stat(uls_ptr(p_round->hcodes));
}

static void
deinit_stat_round(stat_of_round_ptr_t p_round)
{
	uls_deinit_hash_stat(uls_ptr(p_round->hcodes));
}

static void
copy_stat_round(stat_of_round_ptr_t p_stat_src, stat_of_round_ptr_t p_stat_dst)
{
	uls_copy_hash_stat(uls_ptr(p_stat_src->hcodes),
		uls_ptr(p_stat_dst->hcodes));
	p_stat_dst->state = p_stat_src->state;
}

void
reset_hash_buckets()
{
	memset(g_hash_buckets, 0x00, ULF_HASH_TABLE_SIZE * sizeof(int));
}

int
incl_if_keyw(uls_keyw_stat_list_t *ks_lst, const char* keyw)
{
	uls_keyw_stat_ptr_t kwstat;
	int stat = 0;

	kwstat = ulf_search_kwstat_list(ks_lst, keyw);

	if (kwstat != uls_nil) {
		if (kwstat->freq < INT_MAX) {
			++kwstat->freq;
			stat = 1;
		} else {
			stat = -1;
		}
	}

	return stat;
}

void
dump_hash_freq(stat_of_round_ptr_t p_round)
{
	uls_hash_stat_t *hs = &p_round->hcodes;
	int i;

	uls_printf("weight = { %d, %d, %d }\n",
		hs->weight[0],
		hs->weight[1],
		hs->weight[2]
	);

	uls_printf("avg(%.2f), sigma2(%.2f), gamma(%.2f)\n",
		p_round->state.avg, p_round->state.sigma2, p_round->state.gamma);

	if (opt_verbose >= 2) {
		uls_printf("hash-table distribution:\n");
		for (i=0; i < ULF_HASH_TABLE_SIZE; i++) {
			uls_printf("\t%2d] %d\n", i, g_hash_buckets[i]);
		}
		uls_printf("\n");
	}
}

int
proc_file(uls_keyw_stat_list_t *ks_lst, const char *fpath)
{
	if (fpath == NULL) return 0;

	if (uls_push_file(sam_lex, fpath, 0) < 0) {
		return -1;
	}

	err_log("processing %s, ...", fpath);
	for ( ; ; ) {
		uls_get_tok(sam_lex);
		if (uls_is_eoi(sam_lex)) break;

		incl_if_keyw(ks_lst, uls_lexeme(sam_lex));
	}

	return 1;
}

int
proc_filelist(uls_keyw_stat_list_t *ks_lst, FILE *fp_list)
{
	char  linebuff[ULS_FILEPATH_MAX+1], *lptr, *fpath;
	int   i, len, lno = 0, stat=0;

	while (1) {
		if ((len=uls_fp_gets(fp_list, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) {
				err_log("%s: io-error", __func__);
				stat = -1;
			}
			break;
		}

		++lno;

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		fpath = lptr;
		for (i=(int)strlen(fpath)-1; i>=0; i--) {
			if (!isspace(fpath[i])) break;
		}
		fpath[++i] = '\0';

		if (proc_file(ks_lst, fpath) < 0) {
			err_log("fail to process '%s'", fpath);
			stat = -1;
			break;
		}
	}

	return stat;
}

void
uls_hashfunc_set_params(stat_of_round_ptr_t p_round, int w0, int w1, int w2)
{
	uls_hash_stat_t *hs = &p_round->hcodes;

	hs->weight[0] = w0;
	hs->weight[1] = w1;
	hs->weight[2] = w2;
}

void
hashfunc_set_params_random(stat_of_round_ptr_t p_round)
{
	uls_hash_stat_t *hs = &p_round->hcodes;
	int w0, w1, w2;

	w0 = rand() % 100;
	if (rand() % 2) w0 = -w0;

	w1 = rand() % 100;
	if (rand() % 2) w1 = -w1;

	w2 = rand() % 100;
	if (rand() % 2) w1 = -w1;

	hs->weight[0] = w0;
	hs->weight[1] = w1;
	hs->weight[2] = w2;
}

void
go_round(uls_keyw_stat_list_t *ks_lst, stat_of_round_ptr_t p_round)
{
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	uls_keyw_stat_ptr_t e;
	double sum1, sum2, avg;
	int i, hash, n;

	lst = uls_ptr(ks_lst->lst);
	slots_lst = uls_parray_slots(lst);

	reset_hash_buckets();
	for (i=0; i < ks_lst->lst.n; i++) {
		e = slots_lst[i];
		hash = ulf_hashfunc(&p_round->hcodes, e->keyw);
		if (g_hash_buckets[hash] < INT_MAX)
			++g_hash_buckets[hash];
	}

	n = 0;
	sum1 = sum2 = 0.;
	for (hash=0; hash < ULF_HASH_TABLE_SIZE; hash++) {
		if ((i=g_hash_buckets[hash]) > 0) {
			sum1 += i;
			sum2 += i*i;
			++n;
		}
	}

	p_round->state.n_buckets = n;
	p_round->state.avg = avg = sum1 / n;
	p_round->state.sigma2 = sum2 / n - avg * avg;
	p_round->state.gamma =  ( 1 / (1 + p_round->state.sigma2) + 1) * n;

	if (opt_verbose >= 3) {
		uls_hash_stat_t *hs = &p_round->hcodes;
		int w0, w1, w2;

		w0 = hs->weight[0];
		w1 = hs->weight[1];
		w2 = hs->weight[2];
		uls_printf("w0=%d, w1=%d, w2=%d:\n", w0, w1, w2);

		uls_printf("\t#buckets=%d, sum1=%f, sum2=%f\n", n, sum1, sum2);
		uls_printf("\tavg=%f, sigma2=%f --> gamma=%f\n\n",
			avg, p_round->state.sigma2, p_round->state.gamma);
	}
}

void
calc_good_hcode_O0(uls_keyw_stat_list_t *ks_lst, stat_of_round_ptr_t p_round)
{
	stat_of_round_t  round;
	int     i;

	init_stat_round(uls_ptr(round));

	for (i = 0; i<n_samples; i++) {
		hashfunc_set_params_random(uls_ptr(round));
		go_round(ks_lst, uls_ptr(round));
		if (round.state.gamma > p_round->state.gamma) {
			copy_stat_round(uls_ptr(round), p_round);
		}
	}

	deinit_stat_round(uls_ptr(round));
}

void
calc_good_hcode_O1(uls_keyw_stat_list_t *ks_lst, stat_of_round_ptr_t p_round)
{
	stat_of_round_t  round;
	int w0, w1, w2;

	init_weights_indices();
	init_stat_round(uls_ptr(round));

	do {
		w0 = i_weight_list - 9;
		w1 = j_weight_list - 9;
		w2 = k_weight_list - 9;

		uls_hashfunc_set_params(uls_ptr(round), w0,  w1, w2);
		go_round(ks_lst, uls_ptr(round));
		if (round.state.gamma > p_round->state.gamma) {
			copy_stat_round(uls_ptr(round), p_round);
		}
	} while (next_weights_indices(19));

	deinit_stat_round(uls_ptr(round));
}

void
calc_good_hcode_O2(uls_keyw_stat_list_t *ks_lst, stat_of_round_ptr_t p_round)
{
	stat_of_round_t  round;
	int w0, w1, w2;

	init_weights_indices();
	init_stat_round(uls_ptr(round));

	while (next_weights_indices(NUM_WPRIMES)) {
		w0 = weight_plist[i_weight_list];
		w1 = weight_plist[j_weight_list];
		w2 = weight_plist[k_weight_list];

		uls_hashfunc_set_params(uls_ptr(round), w0,  w1, w2);
		go_round(ks_lst, uls_ptr(round));
		if (round.state.gamma > p_round->state.gamma) {
			copy_stat_round(uls_ptr(round), p_round);
		}
	}

	deinit_stat_round(uls_ptr(round));
}

static int
__create_file_internal(uls_keyw_stat_list_t *ks_lst, const char *tgt_dir,
	FILE *fp_list, FILE *fp_out, int n_args, char *args[])
{
	stat_of_round_t best_round_stat;
	uls_hash_stat_t *hs;
	int i, rval;

	if (ks_lst->lst.n <= 0) {
		err_log("%s: No keywords!", __func__);
		init_stat_round(uls_ptr(best_round_stat));
		hs = &best_round_stat.hcodes;
		uls_hashfunc_set_params(uls_ptr(best_round_stat), 1, 1, 1);
		rval = ulf_create_file(hs, ks_lst, fp_out);
		deinit_stat_round(uls_ptr(best_round_stat));
		return rval;
	}

	// 1. stastics of keyword frequencies
	if (fp_list != NULL) {
		if (ult_chdir(tgt_dir) < 0) {
			err_log("can't change to %s", tgt_dir);
			return -1;
		}

		if (proc_filelist(ks_lst, fp_list) < 0) {
			err_log("Failed to process files in %s", tgt_dir);
			return -1;
		}

		if (ult_chdir(home_dir) < 0) {
			err_log("fail to chdir(%s)", home_dir);
			return -1;
		}

	} else {
		for (i = 0; i < n_args; i++) {
			proc_file(ks_lst, args[i]);
		}
	}

	init_stat_round(uls_ptr(best_round_stat));
	hs = &best_round_stat.hcodes;

	// 2. distribution of keywords in hash-table
	uls_hashfunc_set_params(uls_ptr(best_round_stat), 1, 1, 1);
	go_round(ks_lst, uls_ptr(best_round_stat));

	calc_good_hcode_O0(ks_lst, uls_ptr(best_round_stat));

	if (opt_optimize_level >= 1) {
		calc_good_hcode_O1(ks_lst, uls_ptr(best_round_stat));
		uls_printf("O1: weight = { %d, %d, %d }\n",
			hs->weight[0],
			hs->weight[1],
			hs->weight[2]
		);
	}

	if (opt_optimize_level >= 2) {
		calc_good_hcode_O2(ks_lst, uls_ptr(best_round_stat));
		uls_printf("O2: weight = { %d, %d, %d }\n",
			hs->weight[0],
			hs->weight[1],
			hs->weight[2]
		);
	}

	dump_hash_freq(uls_ptr(best_round_stat));
	rval = ulf_create_file(hs, ks_lst, fp_out);
	deinit_stat_round(uls_ptr(best_round_stat));
	return rval;
}

int
main_proc(const char *tgt_dir, FILE *fp_list,
	const char *out_filepath, int n_args, char *args[])
{
	int stat = 0;
	uls_keyw_stat_list_t *ks_lst;
	FILE *fp_out;

	g_hash_buckets = (int *) uls_malloc(ULF_HASH_TABLE_SIZE * sizeof(int));

	if ((fp_out=uls_fp_open(out_filepath, ULS_FIO_CREAT)) == NULL) {
		err_log("%s: fail to create '%s'", __func__, out_filepath);
		return -1;
	}

	err_log("Gathering the statistics of keywords usage, ...");

	ks_lst = ulc_export_kwtable(uls_ptr(sam_lex->idkeyw_table));
	if (ks_lst == uls_nil) {
		err_log("No keyword information!");
		stat = -1;
	} else {
		// ks_lst is sorted by keyw, alphabetic order
		err_log("Writing the frequencies of keywords to %s, ...", out_filepath);
		stat = __create_file_internal(ks_lst, tgt_dir, fp_list, fp_out, n_args, args);
		ulc_free_kwstat_list(ks_lst);
	}

	uls_fp_close(fp_out);
	uls_mfree(g_hash_buckets);

	return stat;
}

int
main(int argc, char* argv[])
{
	FILE *fp_list = NULL;
	char *target_dir = NULL;
	const char *conf_fname;
	int i0, i, conf_fname_len, stat = 0;
	int cse_insen;

	srand(time(NULL));
	progname = THIS_PROGNAME;
	if (argc <= 1) {
		usage_brief();
		return 1;
	}

	if (ulc_prepend_searchpath_pwd() < 0) {
		err_log("InternalError: don't know about the program '%s'.", THIS_PROGNAME);
		return -1;
	}

	if ((i0 = parse_options(argc, argv)) <= 0) {
		if (i0 < 0) err_log("Incorrect use of command options.");
		return i0;
	}

	if (filelist != NULL) {
		if (i0 >= argc) {
			err_log("Specify the target-directory to apply the paths in %s\n", filelist);
			return -1;
		}

		target_dir = ult_strdup(argv[i0++]);
		uls_path_normalize(target_dir, target_dir);

		if ((fp_list = uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
			err_log("%s: fail to read '%s'", __func__, filelist);
			return -1;
		}
	}

	conf_fname = uls_filename(config_file, &conf_fname_len);

	if (out_file == NULL) {
		strcpy(out_file_buff, conf_fname);
		i = conf_fname_len;
		strcpy(out_file_buff+i, ".ulf");
		out_file = out_file_buff;
	}

	if (opt_verbose >= 2)
		ulc_list_searchpath(config_file);

	if ((sam_lex = uls_create(config_file)) == uls_nil) {
		err_log("can't create a uls-object for %s.", config_file);
		return -1;
	}

	cse_insen = sam_lex->flags & ULS_FL_CASE_INSENSITIVE;

	ulf_hashfunc = uls_get_hashfunc(ULS_HASH_ALGORITHM, cse_insen);
	if (ulf_hashfunc == uls_nil) {
		err_log("%s: unkown hash algorithm", __func__, ULS_HASH_ALGORITHM);
		return -1;
	}

	stat = main_proc(target_dir, fp_list, out_file, argc - i0, argv + i0);

	if (fp_list != NULL) {
		uls_fp_close(fp_list);
		fp_list = NULL;
	}

	uls_destroy(sam_lex);

	uls_mfree(filelist);
	uls_mfree(out_file);
	uls_mfree(target_dir);

	return stat;
}

void
init_weights_indices()
{
	i_weight_list = j_weight_list = k_weight_list = 0;
}

int
next_weights_indices(int space_size)
{

	if (++k_weight_list >= space_size) {
		if (++j_weight_list >= space_size) {
			if (++i_weight_list >= space_size) {
				init_weights_indices();
				return 0;
			}
			j_weight_list = 0;
		}
		k_weight_list = 0;
	}

	return 1; // true
}
