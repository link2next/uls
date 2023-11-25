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

#define THIS_PROGNAME "ulf_gen"
#define DFL_N_SAMPLES 0xFFFF

_ULS_DEFINE_STRUCT(stat_of_round)
{
	uls_dflhash_state_t dflhash_stat0;
	int   n; // # of buckets
	double avg, sigma2;
	double gamma;
};

char *progname;
char home_dir[ULS_FILEPATH_MAX+1];
const char *config_file;
char *target_dir;
char *filelist;
FILE *fp_list;
char *out_file;
char out_file_buff[ULS_FILEPATH_MAX+1];
int  opt_verbose;

uls_lex_ptr_t sam_lex;
uls_hashfunc_t ulf_hashfunc;

int target_hashtable_size;
int n_samples;

#define ULFGEN_OPTSTR "L:l:o:n:s:vVHh"

#ifdef HAVE_GETOPT
#include <getopt.h>

static const struct option longopts[] = {
	{ "lang",  required_argument,      NULL, 'L' },
	{ "list",  required_argument,      NULL, 'l' },
	{ "output",  required_argument,    NULL, 'o' },
	{ "num-iter",  required_argument,  NULL, 'n' },
	{ "size-hash",  required_argument, NULL, 's' },
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
	err_log("  -v, --verbose      verbose mode");
	err_log("  -V, --version      Print the version information");
	err_log("  -h, --help         Display the short help");
#else
	err_log("  -L, --lang=<ulc-spec>   Specify the lexical-spec(*.ulc) of the language");
	err_log("  -l, --list=<list-file>  Specify the list of data source files");
	err_log("  -o, --output=<a-file>   Specify the the output filepath(*.ulf)");
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
	int   siz;

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
		if (n_samples <= 0) n_samples = 1;
		break;

	case 'o':
		out_file = ult_strdup(optarg);
		uls_path_normalize(out_file, out_file);
		break;

	case 's':
		if ((siz=atoi(optarg)) > 0) {
			target_hashtable_size =  siz;
		} else {
			err_log("non-positive hash-size(%d) specified!", siz);
			stat = -1;
		}
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'H':
		usage_long();
		stat = 3;
		break;

	case 'h':
		usage();
		stat = 2;
		break;

	case 'V':
		uls_printf("%s %s, written by %s,\n\tis provided under %s.\n",
			progname, ULF_GEN_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		stat = 1;
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

	target_hashtable_size = ULF_HASH_TABLE_SIZE;

	filelist = NULL;
	out_file = NULL;
	target_dir = NULL;
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

	if (filelist != NULL) {
		if (i0 >= argc) {
			err_log("Specify the target-directory to apply the paths in %s\n", filelist);
			return -1;
		}

		if ((fp_list=uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
			err_log("%s: fail to read '%s'", __func__, filelist);
			return -1;
		}

		target_dir = ult_strdup(argv[i0++]);
		uls_path_normalize(target_dir, target_dir);
	}

	return i0;
}

int
incl_if_keyw(uls_keyw_stat_list_t *ks_lst, const char* keyw)
{
	uls_keyw_stat_ptr_t kwstat;
	int stat = 0;

	kwstat = ulc_search_kwstat_list(ks_lst, keyw);

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

int
proc_file(char *fpath, uls_keyw_stat_list_t *ks_lst)
{
	if (fpath == NULL) return 0;

	if (uls_push_file(sam_lex, fpath, 0) < 0) {
		return -1;
	}

	if (opt_verbose >= 1)
		err_log("processing %s, ...", fpath);

	for ( ; ; ) {
		uls_get_tok(sam_lex);
		if (uls_is_eoi(sam_lex)) break;

		incl_if_keyw(ks_lst, uls_lexeme(sam_lex));
	}

	return 1;
}

int
proc_filelist(FILE *fin, uls_keyw_stat_list_t *ks_lst)
{
	char  linebuff[ULS_FILEPATH_MAX+1], *lptr, *fpath;
	int   i, len, lno = 0, stat=0;

	while (1) {
		if ((len=uls_fp_gets(fin, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
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

		if (proc_file(fpath, ks_lst) < 0) {
			err_log("fail to process '%s'", fpath);
			stat = -1;
			break;
		}
	}

	return stat;
}

static void
uls_hashfunc_set_params(stat_of_round_ptr_t p_stat,
	int n_slots, int n_shifts, uls_uint32 hcodes0)
{
	p_stat->dflhash_stat0.n_slots = n_slots;
	p_stat->dflhash_stat0.n_shifts = n_shifts;
	p_stat->dflhash_stat0.init_hcode = hcodes0;

	p_stat->n = 0;
	p_stat->gamma = 0.;
}

void
go_round(uls_keyw_stat_list_t *ks_lst,
	uls_uint32 hcodes0, int* hash_range, int n_hash_range, stat_of_round_ptr_t p_stat)
{
	uls_ref_parray(lst, keyw_stat);
	uls_decl_parray_slots(slots_lst, keyw_stat);
	uls_keyw_stat_ptr_t e;
	int i, hash_id, n;
	double sum, sum1, sum2;

	memset(hash_range, 0x00, n_hash_range * sizeof(int));

	uls_hashfunc_set_params(p_stat, n_hash_range, -1, hcodes0);

	sum = 0.;
	lst = uls_ptr(ks_lst->lst);
	slots_lst = uls_parray_slots(lst);
	for (i=0; i < ks_lst->lst.n; i++) {
		e = slots_lst[i];

		hash_id = ulf_hashfunc(&p_stat->dflhash_stat0, e->keyw);
		if (hash_range[hash_id] < INT_MAX)
			++hash_range[hash_id];

		if (opt_verbose >= 3)
			uls_printf(" '%s' --> %d\n", e->keyw, hash_id);

		sum += hash_range[hash_id] * e->freq;
	}

	sum1 = sum2 = 0.;
	n = 0;
	for (hash_id=0; hash_id < n_hash_range; hash_id++) {
		if ((i=hash_range[hash_id]) > 0) {
			sum1 += i;
			sum2 += i*i;
			++n;
		}
	}

	p_stat->avg = sum1 / n;
	p_stat->sigma2 = sum2 / n - p_stat->avg * p_stat->avg;
	p_stat->n = n;
	p_stat->gamma = sum / n;
}

void
get_random_hcodes(int n_hcodes0, uls_uint32* hcodes0)
{
	uls_uint32 hcode;
	int i, j;

	for (i=0; i<n_hcodes0; i++) {
		hcode = 0;
		for (j=0; j<4; j++) {
			hcode ^= rand();
			hcode <<= 8;
		}
		hcodes0[i] = hcode;
	}
}

void
calc_good_hcode0(uls_keyw_stat_list_t *ks_lst, stat_of_round_ptr_t p_stat)
{
	int *bucket_size_array;
	stat_of_round_t  round_stat;
	int     i;

	bucket_size_array = (int *) uls_malloc(target_hashtable_size * sizeof(int));

	for (i=0; i<n_samples; i++) {
		get_random_hcodes(1, &p_stat->dflhash_stat0.init_hcode);

		go_round(ks_lst, p_stat->dflhash_stat0.init_hcode,
			bucket_size_array, target_hashtable_size, uls_ptr(round_stat));

		if (opt_verbose >= 2 && i % 1000 == 0) {
			uls_printf("\tn=%d: avg = %.3f, sigma2 = %.3f, gamma=%.3f\n",
				round_stat.n, round_stat.avg, round_stat.sigma2, round_stat.gamma);
		}

		if (i > 0) {
			if (round_stat.gamma < p_stat->gamma) {
				*p_stat = round_stat;
			}
		} else {
			*p_stat = round_stat;
		}
	}

	if (opt_verbose >= 1) {
		uls_printf("\tavg = %.3f, sigma2 = %.3f, n=%d, gamma=%.3f\n",
			p_stat->avg, p_stat->sigma2, p_stat->n, p_stat->gamma);
	}

	uls_mfree(bucket_size_array);

	if (opt_verbose >= 2) {
		uls_printf(" hcode[] = ");
		uls_printf(" 0x%08X", p_stat->dflhash_stat0.init_hcode);
		uls_printf("\n");
	}
}

static int keyw_stat_comp_by_freq_reverse(const uls_voidptr_t a, const uls_voidptr_t b)
{
	uls_keyw_stat_ptr_t a1 = (uls_keyw_stat_ptr_t) a;
	uls_keyw_stat_ptr_t b1 = (uls_keyw_stat_ptr_t) b;

	return b1->freq - a1->freq;
}

int
ulf_create_file_internal(FILE *fp_list, FILE *fp_out, int i0, int n_args, char **args)
{
	uls_keyw_stat_list_t *keyw_stat_list;
	stat_of_round_t best_round_stat;
	int i;

	keyw_stat_list = ulc_export_kwtable(uls_ptr(sam_lex->idkeyw_table));
	if (keyw_stat_list == uls_nil) {
		err_log("No keyword information!");
		return -1;
	}

	if (fp_list != NULL) {
		if (ult_chdir(target_dir) < 0) {
			err_log("can't change to %s", target_dir);
			ulc_free_kwstat_list(keyw_stat_list);
			return -1;
		}

		if (proc_filelist(fp_list, keyw_stat_list) < 0) {
			err_log("Failed to process files in %s", target_dir);
			return -1;
		}

		if (ult_chdir(home_dir) < 0) {
			err_log("fail to chdir(%s)", home_dir);
			ulc_free_kwstat_list(keyw_stat_list);
			return -1;
		}

	} else {
		for (i=i0; i<n_args; i++) {
			proc_file(args[i], keyw_stat_list);
		}
	}

	_uls_quicksort_vptr(uls_parray_slots(uls_ptr(keyw_stat_list->lst)), keyw_stat_list->lst.n, keyw_stat_comp_by_freq_reverse);

	calc_good_hcode0(keyw_stat_list, uls_ptr(best_round_stat));

	ulf_create_file(1, &best_round_stat.dflhash_stat0.init_hcode,
		target_hashtable_size, keyw_stat_list, fp_out);

	ulc_free_kwstat_list(keyw_stat_list);

	return 0;
}

int
main(int argc, char* argv[])
{
	FILE *fp_out;
	const char *conf_fname;
	int i0, i, conf_fname_len, stat = 0;
	int cse_insen;

	progname = THIS_PROGNAME;
	if (argc <= 1) {
		usage_brief();
		return 1;
	}

	if (ulc_prepend_searchpath_pwd() < 0) {
		err_log("InternalError: don't know about the program '%s'.", THIS_PROGNAME);
		return -1;
	}

	if ((i0=parse_options(argc, argv)) <= 0) {
		if (i0 < 0) err_log("Incorrect use of command options.");
		return i0;
	}

	conf_fname = uls_filename(config_file, &conf_fname_len);

	if (out_file == NULL) {
		strcpy(out_file_buff, conf_fname);
		i = conf_fname_len;
		strcpy(out_file_buff+i, ".ulf");
		out_file = out_file_buff;
	}

	if (opt_verbose)
		ulc_list_searchpath(config_file);

	if ((sam_lex=uls_create(config_file)) == uls_nil) {
		err_log("can't create a uls-object for %s.", config_file);
		return -1;
	}

	cse_insen = sam_lex->flags & ULS_FL_CASE_INSENSITIVE;
	ulf_hashfunc = uls_get_hashfunc(ULS_HASH_ALGORITHM, cse_insen);

	if ((fp_out=uls_fp_open(out_file, ULS_FIO_CREAT)) == NULL) {
		err_log("%s: fail to create '%s'", __func__, out_file);
		stat = -1;
	} else {
		err_log("Gathering the statistics of keywords usage, ...");
		if (ulf_create_file_internal(fp_list, fp_out, i0, argc, argv) < 0) {
			err_log("%s: internal error!'", __func__);
			stat = -1;
		} else {
			err_log("Writing the frequencies of keywords to %s, ...", out_file);
		}
		uls_fp_close(fp_out);
	}

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
