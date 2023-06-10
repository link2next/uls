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
  <file> uls_stream.c </file>
  <brief>
    Read from input file and dump tokens in binary/text stream.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/

#include "main.h"

#include "read_uls.h"
#include "write_uls.h"
#include "filelist.h"

#ifdef ULS_FDF_SUPPORT
#include "uls/fdfilter.h"
#endif

#define THIS_PROGNAME "uls_stream"

#ifdef ULS_FDF_SUPPORT
#define ULSSTREAM_OPTSTR "bo:t:T:L:n:vsC:VHhf:"
#else
#define ULSSTREAM_OPTSTR "bo:t:T:L:n:vsC:VHh"
#endif

int uls_endian;
char *progname;
int  opt_verbose, opt_binary;
int  opt_no_numbering;

char home_dir[ULS_FILEPATH_MAX+1];
const char *ulc_config, *tag_name;
char config_pathbuff[ULS_FILEPATH_MAX+1];
char uls_specname[ULC_LONGNAME_MAXSIZ+1];
char *filelist, *target_dir, *output_file, *cmdline_filter;
int  out_ftype;

uls_tmpl_list_t tmpl_list;

uls_lex_ptr_t sam_lex;

#ifdef HAVE_GETOPT
#include <getopt.h>

static const struct option longopts[] = {
	{ "binary", no_argument,               NULL, 'b' },
	{ "output",  required_argument,        NULL, 'o' },
	{ "type",  required_argument,          NULL, 't' },
	{ "tmpl",  required_argument,          NULL, 'T' },
	{ "lang",  required_argument,          NULL, 'L' },
	{ "conglomerate", required_argument,   NULL, 'C' },
#ifdef ULS_FDF_SUPPORT
	{ "filter", required_argument,         NULL, 'f' },
#endif
	{ "name", required_argument,           NULL, 'n' },
	{ "short", no_argument,                NULL, 's' },
	{ "verbose", no_argument,              NULL, 'v' },
	{ "version", no_argument,              NULL, 'V' },
	{ "Help",    no_argument,              NULL, 'H' },
	{ "help",    no_argument,              NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
#endif

static void usage_synopsis(void)
{
	uls_printf("Usage: %s [options] [name1=value1 ...] <file|dir>\n", progname);
	uls_printf("    %s generates token sequence file from (plain text) input files.\n", progname);
	uls_printf("       %s <uls-file|(*.uls)>\n", progname);
	uls_printf("       %s -L <ulc-file> [-b] [-o <output-file>] <text-file|(*.uls)>\n", progname);
	uls_printf("       %s [-L <ulc-file>] -C <listing-of-files> <target-dir>\n", progname);
	uls_printf("       %s [-L <ulc-file>] -C <listing-of-files> -f 'filter-path' <target-dir>\n", progname);
}

static void usage_desc(void)
{
#ifdef ULS_WINDOWS
	uls_printf("  -L <ulc-spec>    Specify the lexical-configuration for your own language.\n");
	uls_printf("  -o <filepath>    Specify the output filepath.\n");
	uls_printf("  -b               The output file will be binary file.\n");
	uls_printf("  -C <listfile>    This outputs a conglomerate file from the multiple input-files.\n");
	uls_printf("  -v               verbose mode.\n");
	uls_printf("  -V               Prints the version information.\n");
	uls_printf("  -h               Display a short help.");
#else
	uls_printf("  -L, --lang=<ulc-spec>         Specify lexical-configuration(*.ulc) for your own language.\n");
	uls_printf("  -o, --output=<filepath>       specify the output filepath.\n");
	uls_printf("  -b, --binary                  The output file will be binary file.\n");
	uls_printf("  -C, --conglomerate=<listfile> This outputs a conglomerate file from the multiple input-files.\n");
#ifdef ULS_FDF_SUPPORT
	uls_printf("  -f, --filter=<cmdline>        Specify the filter for the input files with the -C-option.\n");
#endif
	uls_printf("  -v, --verbose                 verbose mode.\n");
	uls_printf("  -V, --version                 Print the version information.\n");
	uls_printf("  -h, --help                    Display a short help.\n");
#endif
}

static void usage_brief(void)
{
	usage_synopsis();
	uls_printf("\n");

	usage_desc();
	uls_printf("\n");
}

static void usage(void)
{
	usage_brief();

	uls_printf("  To dump the token sequence,\n");
	uls_printf("    %s -L <ulc-file> <source-file|(*.uls)>\n", progname);
	uls_printf("    %s -L <ulc-file> <uls-file|(*.uls)>\n", progname);
	uls_printf("\n");

	uls_printf("  To create the token sequence file,\n");
	uls_printf("    %s -L <ulc-file> [-o <outfile>] <src-file|(*.uls)>\n", progname);
	uls_printf("\n");

	uls_printf("  To concatenate the all token sequence files in directories,\n");
	uls_printf("    %s [-L <ulc-file>] -C <list-file> <target-dir>\n", progname);
	uls_printf("    %s [-L <ulc-file>] -C <list-file> -f 'filter-path' <target-dir>\n", progname);
	uls_printf("\n");

	uls_printf("  To make a template-uls file from plain uls-file,\n");
	uls_printf("    %s -L <ulc-file> -T <var1> [...] <uls-file>\n", progname);
	uls_printf("\n");

	uls_printf("  To instantiate the template uls-file,\n");
	uls_printf("    %s -L <ulc-file> var1=value1 [ var2=value2 ] <tmpl-uls-file>\n", progname);
	uls_printf("\n");
}

static void usage_long(void)
{
	usage_brief();

	uls_printf("The '%s' reads source code files or lexical stream files(*.uls) as input files.\n", progname);
	uls_printf("It displays or converts and save it again into uls-format-file.\n");
	uls_printf("It's necessary to specify a lexical configuration from ulc-file(*.ulc) with -L-option.\n");
	uls_printf("The created lexical analyzer from the ulc-file will read the input-files.\n");
	uls_printf("The output file is an uls-file, which contains a token sequence file.\n");
	uls_printf("Below is illustrated a token sequence with its lexeme.\n");

	uls_printf("  [     ID]  DDD\n");
	uls_printf("  [     <=]  <=\n");
	uls_printf("  [     ID]  EEE\n");
	uls_printf("  [      {]  {\n");
	uls_printf("  [     ID]  YYY\n");
	uls_printf("  [      =]  \n");
	uls_printf("\n");

	uls_printf("You can use the uls-file as an input file to '%s.\n", progname);
	uls_printf("\n");
	uls_printf("If you specify -C-option with list of files, you can get a conglomerate all the input files into one file.\n");
	uls_printf("The list file is just a simple list of file paths that you want to process.\n");
	uls_printf("The each line in the file represents the filepath which starts from <target-dir>.\n");
	uls_printf("Let a listing-file 'a.list' be as follows.\n");
	uls_printf("  # Comment here\n");
	uls_printf("  # A filepath per line\n");
	uls_printf("  input1.c\n");
	uls_printf("  src/input2.c\n");
	uls_printf("  srclib/input3.c\n");
	uls_printf("\n");

	uls_printf("You can use -C-option as below.\n");
	uls_printf("  %s -C a.list /package/home/target-dir\n\n", progname);
	uls_printf("The output file is specified with -o-option.");
	uls_printf(" The default output-file in binary mode('-b') is the 'a.uls'.\n");

	uls_printf("The -f-option can be used with -C-option to filter the files before the input to be passed to lexical analyzer.\n");
	uls_printf("The argument of -f-option should be a command line which inputs from stdin and outputs to stdout like 'gcc -E'.\n");
	uls_printf("\n");

	uls_printf("To read ULS-file,\n");
	uls_printf("  %s -L sample.ulc input1.txt\n", progname);
	uls_printf("  %s -L sample.ulc input1.uls\n", progname);
	uls_printf("  %s -L sample.ulc -o a.txt input1.uls\n", progname); uls_printf("\n");

	uls_printf("You can use the -t-option to specify the format of output file.\n");
	uls_printf("  %s -L sample.ulc -t bin-be input1.uls\n", progname);
	uls_printf("      This generates the binary file of big-endian type.\n");
	uls_printf("  %s -L sample.ulc -t bin-le input1.uls\n", progname);
	uls_printf("      This generates the binary file of little-endian type.\n");
	uls_printf("  %s -L sample.ulc input1.uls\n", progname);
	uls_printf("      This generates the binary file of host-type.\n");
	uls_printf("  %s -L sample.ulc -t txt input1.uls\n", progname);
	uls_printf("      This save a text formatted file from 'input1.uls'.\n");
	uls_printf("\n");

	uls_printf("To get the conglomerate ULS-file,\n");
	uls_printf("  %s -b -C ./a.list /package/home\n", progname);
#ifdef ULS_FDF_SUPPORT
	uls_printf("  %s -b -C ./a.list -f 'gcc -E' ../package/home\n", progname);
#endif
	uls_printf("\n");

	uls_printf("To make a template-uls file from a source file tmpl_ex.cc,\n");
	uls_printf("  %s -L sample.ulc -b -o tmpl_ex.uls -TT1 tmpl_ex.cc\n", progname);
	uls_printf("  This make the identifier 'T1' in 'tmpl_ex.cc' a template variable.\n");
	uls_printf("\n");

	uls_printf("To make a uls file from the 'tmpl_ex.uls' with the template variables substituted,\n");
	uls_printf("  %s -L sample.ulc T1='unsigned int' tmpl_ex.uls\n", progname);
	uls_printf("\n");
}

static int
ulsstream_options(int opt, char* optarg)
{
	int stat = 0;
#ifdef ULS_FDF_SUPPORT
	char fpath_buff[ULS_FILEPATH_MAX+1];
	char *lptr, *cmdl, *argv0;
	int siz;
#endif

	switch (opt) {
	case 'b':
		opt_binary =  1;
		break;

	case 'o':
		output_file = ult_strdup(optarg);
		uls_path_normalize(output_file, output_file);

		if (strcmp(ult_get_suffix(output_file), "uls") == 0) {
			opt_binary = 1;
		}
		break;


	case 'f':
#ifdef ULS_FDF_SUPPORT
		uls_mfree(cmdline_filter);
		cmdl = skip_blanks(optarg);

		siz = strlen(cmdl) + ULS_FILEPATH_MAX + 4;
		cmdline_filter = uls_malloc(siz);
		strcpy(cmdline_filter, cmdl);

		if (uls_streql(cmdline_filter, "pass")) {
			break;
		}

		lptr = cmdline_filter;
		argv0 = uls_splitstr(&lptr);

		argv0 = ult_strdup(argv0);
		uls_path_normalize(argv0, argv0);

		if (!ult_is_absolute_path(argv0) && uls_dirent_exist(argv0) == ST_MODE_REG) {
			if (ult_getcwd(fpath_buff, ULS_FILEPATH_MAX) < 0) {
				err_panic("%s: fail to getcwd()", __FUNCTION__);
			}
			uls_snprintf(cmdline_filter, siz, "%s/%s", fpath_buff, cmdl);
		} else {
			uls_mfree(cmdline_filter);
			cmdline_filter = cmdl;
		}
		uls_mfree(argv0);
#else
		err_log("%s: fdf not supported!", __FUNCTION__);
		stat = -1;
#endif
		break;

	case 't':
		opt_binary = 1;
		if (ult_streql(optarg, "bin")) {
		} else if (ult_streql(optarg, "bin-le")) {
			out_ftype = ULS_STREAM_BIN_LE;
		} else if (ult_streql(optarg, "bin-be")) {
			out_ftype = ULS_STREAM_BIN_BE;
		} else if (ult_streql(optarg, "txt")) {
			out_ftype = ULS_STREAM_TXT;
		} else {
			err_log("%s: invalid option value for -t", optarg);
			stat = -1;
		}
		break;

	case 'T':
		if (!add_name_val_ent(optarg, "")) {
			err_log("can't add tmpl-name", optarg);
			stat = -1;
		}
		break;

	case 'L':
		ulc_config = optarg;
		break;

	case 'C':
		opt_binary = 1;
		filelist = ult_strdup(optarg);
		uls_path_normalize(filelist, filelist);
		break;

	case 's': // short
		opt_no_numbering = 1;
		break;

	case 'n':
		tag_name = optarg;
		break;

	case 'H':
		usage_long();
		stat = 3;
		break;

	case 'h':
		usage();
		stat = 2;
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'V':
		uls_printf("%s(%s), written by %s,\n\tis provided under %s.\n",
			progname, ULS_STREAM_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
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
	int i0;
	char *ptr, *name;
#ifdef HAVE_GETOPT
	int  rc, opt, longindex;
#endif

	uls_endian = ult_guess_host_byteorder();
	if (ult_getcwd(home_dir, ULS_FILEPATH_MAX) < 0) {
		err_panic("%s: fail to getcwd()", __FUNCTION__);
	}

	target_dir = ult_strdup(".");

	if (uls_endian == ULS_BIG_ENDIAN) {
		out_ftype = ULS_STREAM_BIN_BE;
	} else {
		out_ftype = ULS_STREAM_BIN_LE;
	}

#ifdef HAVE_GETOPT
	while ((opt=getopt_long(argc, argv, ULSSTREAM_OPTSTR, longopts, uls_ptr(longindex))) != -1) {
		if ((rc=ulsstream_options(opt, optarg)) != 0) {
			if (rc > 0) rc = 0;
			return rc;
		}
	}
	i0 = optind;
#else
	if ((i0=uls_getopts(argc, argv, ULSSTREAM_OPTSTR, ulsstream_options)) <= 0) {
		return i0;
	}
#endif

	if (opt_binary && output_file == NULL) {
		output_file = ult_strdup("a.uls");
	}

	if (tag_name == NULL) {
		tag_name = "<tag_none>";
	}

	for ( ; i0 < argc; ) {
		if ((ptr=strchr(argv[i0], '=')) != NULL) {
			*ptr++ = '\0';
			name = argv[i0++];
			if (!add_name_val_ent(name, (void *) ptr)) {
				err_log("duplicate template name '%s' or array overflow!", name);
				return -1;
			}
		} else {
			break;
		}
	}

	if (filelist != NULL) {
		if (i0 >= argc) {
			err_log("Specify the target-directory to apply the paths in %s", filelist);
			return -1;
		}

		if (!opt_binary) {
			err_log("For now, the -C-option is supported only when -b-option is on.");
			return -1;
		}

		uls_mfree(target_dir);
		target_dir = ult_strdup(argv[i0++]);
		uls_path_normalize(target_dir, target_dir);
	}

	return i0;
}

int
exist_name_val_ent(const char* name)
{
	return uls_get_tmpl_value(uls_ptr(tmpl_list), name) != NULL ? 1 : 0;
}

int
add_name_val_ent(const char *name, const char *val)
{
	if (uls_add_tmpl(uls_ptr(tmpl_list), name, val) < 0)
		return 0;

	return 1;
}

int
main(int argc, char* argv[])
{
	int i0, ftype, stat = 0;
	FILE *fp_out, *fp_list;
	uls_outparam_t parms;

	initialize_uls();

	progname = THIS_PROGNAME;
	if (argc <= 1) {
		usage_brief();
		return 1;
	}

	if (ulc_prepend_searchpath_pwd() < 0) {
		err_log("InternalError: don't know about the program '%s'.", THIS_PROGNAME);
		return -1;
	}

	uls_init_tmpls(uls_ptr(tmpl_list), N_TMPLVAL_ARRAY, 0);

	if ((i0=parse_options(argc, argv)) <= 0) {
		if (i0 < 0) err_log("%s: Need args!", progname);
		stat = -1; goto end_1;
	}

	if (ulc_config == NULL) {
		if (filelist != NULL) {
			if ((fp_list=uls_fp_open(filelist, ULS_FIO_READ)) == NULL) {
				err_log("%s: fail to read '%s'", __FUNCTION__, filelist);
				return -1;
			}

			if (ult_chdir(target_dir) < 0) {
				err_log("%s: fail to chdir %s", __FUNCTION__, target_dir);
				uls_fp_close(fp_list);
				return -1;
			}

			ftype = ult_guess_specname_from_inputfiles(config_pathbuff, fp_list, 1);

			uls_fp_close(fp_list);
			ult_chdir(home_dir);

		} else {
			if (ult_chdir(target_dir) < 0) {
				err_log("%s: fail to chdir %s", __FUNCTION__, target_dir);
				return -1;
			}

			ftype = ult_guess_specname_from_argvlist(config_pathbuff, argc-i0, (const char **) argv+i0, 1);
			ult_chdir(home_dir);
		}

		if (ftype == ULS_STREAM_RAW) {
			strcpy(config_pathbuff, "simple");
		} else if (ftype < 0) {
			err_log("Unknown spec-name:");
			err_log("Specify the ulc-config-name of input-file(s) using the -L-option.");
			stat = -1; goto end_1;
		}

		ulc_config = config_pathbuff;
		strcpy(uls_specname, ulc_config);

	} else {
		parms.line = uls_specname;
		uls_get_spectype(ulc_config, uls_ptr(parms));
	}

	if (opt_verbose)
		err_log("The output file depends on the lex-spec '%s'(%s)",
			ulc_config, uls_specname);

	if ((sam_lex=uls_create(ulc_config)) == uls_nil) {
		err_log("Failed to create the uls lexical analyzer for '%s'.", ulc_config);
		stat = -1; goto end_1;
	}

	if (filelist != NULL) {
		stat = proc_filelist(target_dir);

	} else {
		if (opt_binary) {
			stat = write_uls_files(argc-i0, (const char **) argv+i0);
		} else {
			fp_out = _uls_stdio_fp(1);

			if (output_file != NULL && (fp_out=uls_fp_open(output_file, ULS_FIO_CREAT)) == NULL) {
				err_log("can't create %s", output_file);
				stat = -1;
			} else {
				stat = dump_uls_files(argc-i0, (const char **) argv+i0, fp_out);
			}

			if (fp_out != _uls_stdio_fp(1) && fp_out != NULL) {
				uls_fp_close(fp_out);
			}
			fp_out = NULL;
		}
	}

	uls_destroy(sam_lex);

end_1:
	uls_mfree(filelist);
	uls_mfree(target_dir);
	uls_mfree(output_file);
	uls_deinit_tmpls(uls_ptr(tmpl_list));

	return stat;
}
