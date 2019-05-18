
#include "main.h"

#include "read_uls.h"
#include "write_uls.h"
#include "filelist.h"
#include "utils.h"
#ifdef ULS_FDF_SUPPORT
#include "fdfilter.h"
#endif
#ifndef ULS_WINDOWS
#include <locale.h>
#endif

#ifdef ULS_WINDOWS
#define THIS_PROGNAME "UlsStream"
#else
#define THIS_PROGNAME "stream"
#endif

#ifdef ULS_FDF_SUPPORT
#define ULSSTREAM_OPTSTR "o:t:T:L:n:zxvsC:VHh"
#else
#define ULSSTREAM_OPTSTR "o:t:T:L:f:n:zxvsC:VHh"
#endif

char *progname;
int  opt_verbose, opt_write, opt_silent;
int  opt_want_eof;
int  opt_no_numbering;

char home_dir[1024];
const char *config_path;
char config_pathbuff[1024];
char specname[ULS_LEXSTR_MAXSIZ+1];
const char *filelist;

char *target_dir;
char *output_file;
const char *tag_name;
char *cmdline_filter;
int  out_ftype;

#ifdef HAVE_GETOPT_H
#include <getopt.h>

static const struct option longopts[] = {
	{ "output",  required_argument,        NULL, 'o' },
	{ "type",  required_argument,          NULL, 't' },
	{ "tmpl",  required_argument,          NULL, 'T' },
	{ "lang",  required_argument,          NULL, 'L' },
	{ "conglomerate", required_argument,   NULL, 'C' },
#ifdef ULS_FDF_SUPPORT
	{ "filter", required_argument,         NULL, 'f' },
#endif
	{ "name", required_argument,           NULL, 'n' },
	{ "no-linenum", no_argument,           NULL, 'x' },
	{ "want-eof", no_argument,             NULL, 'z' },
	{ "verbose", no_argument,              NULL, 'v' },
	{ "version", no_argument,              NULL, 'V' },
	{ "Help",    no_argument,              NULL, 'H' },
	{ "help",    no_argument,              NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
#endif

static void usage(void)
{
	printf("Usage: %s [options] [name1=value1 ...] <file|dir>\n", progname);
	printf(" options:\n");
	printf("  -L, --lang=<ulc-spec>      The lexical-configuration(*.ulc) about your language\n");
	printf("  -o, --output=<filepath>    specify the output filepath, a stream-file\n");
	printf("  -t, --type=<output-type>   specify the format of output-file.\n");
	printf("  -C, --conglomerate=<listfile>  output a conglomerate file from the multiple input-files\n");
#ifdef ULS_FDF_SUPPORT
	printf("  -f, --filter=<cmdline>     filter the input files with the -C-option\n");
#endif
	printf("  -v, --verbose              verbose mode\n");
	printf("  -V, --version              prints the version information\n");
	printf("  -h, --help                 display a short help.");
	printf("\n\n");

	printf("  To dump the token sequence,\n");
	printf("     %s -L <ulc-file> <source-file|(*.uls)>\n", progname);
	printf("     %s -L <ulc-file> <uls-file|(*.uls)>\n", progname);
	printf("\n");

	printf("  To create the token sequence file,\n");
	printf("     %s -L <ulc-file> -t <out-type> [-o <outfile>] <src-file|(*.uls)>\n", progname);
	printf("\n");

	printf("  To concatenate the all token sequence files in directories,\n");
	printf("     %s [-L <ulc-file>] -C <list-file> <target-dir>\n", progname);
	printf("     %s [-L <ulc-file>] -C <list-file> -f 'filter-path' <target-dir>\n", progname);
	printf("\n");

	printf("  To make a template-uls file from plain uls-file,\n");
	printf("     %s -L <ulc-file> -t {bin-le,bin-be,bin} -T <var1> [...] <uls-file>\n", progname);
	printf("\n");

	printf("  To instantiate the template uls-file,\n");
	printf("     %s -L <ulc-file> var1=value1 [ var2=value2 ] <tmpl-uls-file>\n", progname);
	printf("\n");
	printf(" For more information, type '%s -H'\n\n", progname);
}

static void usage_long(void)
{
	printf("Usage: %s [OPTIONS]  <file|dir>\n\n", progname);
	printf("       %s -L <ulc-file> <source-file|(*.uls)>\n\n", progname);
	printf("       %s -L <ulc-file> -t <out-type> [-o <output-file>] <source-file|(*.uls)>\n\n", progname);
	printf("       %s  <uls-file|(*.uls)>\n\n", progname);
	printf("       %s [-L <ulc-file>] -C <list-file> <target-dir>\n\n", progname);
	printf("       %s [-L <ulc-file>] -C <list-file> -f 'filter-path' <target-dir>\n\n", progname);

	printf("\n");
	printf("%s reads source file or lexical stream file(*.uls), a binary token stream file,\n", progname);
	printf("and displays it or converts it into uls-file.\n");
	printf("Before the process, it needs to read the lexical configuration from ulc-file(*.ulc) specified by -L-option.\n");
	printf("Then, the created lexical analyzer will read the input-files(*.uls) which can be source files.\n");
	printf("The output file is uls-file, a binary token sequence file or\n");
	printf("  a token list with its lexeme as below.\n\n");

	printf("    [     ID]  DDD\n");
	printf("    [     <=]  <=\n");
	printf("    [     ID]  EEE\n");
	printf("    [      {]  {\n");
	printf("    [     ID]  YYY\n");
	printf("    [      =]  \n");
	printf("\n");

	printf("If you specify the -t-option, %s will produce an uls-file(*.uls).\n", progname);
	printf("Then, again you can input the uls-file as an input file to '%s.\n", progname);
	printf("In short, the input file of %s can be source-code file or uls-file.\n", progname);
	printf("The available values for -t-option are 'bin-le'(little-endian), 'bin-be'(big-endian) or 'bin'(host byteorder).\n");
	printf("\n");
	printf("If you specify -C-option, you can get a conglomerate file of all the files in <list-file> ");
	printf(", where the file in each line starts from <target-dir>.\n");
	printf("The <list-file> is just a simple list of file paths in 'target-dir' that you want to process.\n");
	printf("Let the <list-file> be as follows.\n");
	printf("     # Comment here\n\n");
	printf("     # A filepath per line\n\n");
	printf("     input1.c\n\n");
	printf("     src/input2.c\n\n");
	printf("     srclib/input3.c\n\n");
	printf("\n");

	printf("The default output-file ('a.uls') will be a sequence of tokens of all the files.\n");
	printf("The -f-option can be used with -C-option to filter the files before lexical analyzing.\n");
	printf("The argument of -f-option should be a command line which inputs from stdin and outputs to stdout like 'gcc -E'.\n");
	printf("\n");

	printf("When you specify the -L-option, you must consider the search order of ulc-file.\n");
	printf("  Refer the user-guide in the directory 'doc' for details.\n");
	printf("  -L, --lang=<ulc-spec>      The lexical-configuration(*.ulc) about your language\n");
	printf("  -o, --output=<filepath>    specify the output filepath, a stream-file\n");
	printf("  -t, --type=<output-type>   specify the format of output-file.\n");
	printf("  -C, --conglomerate=<listfile>  output a conglomerate file from the multiple input-files\n");
#ifdef ULS_FDF_SUPPORT
	printf("  -f, --filter=<cmdline>     filter the input files with the -C-option\n");
#endif
	printf("  -v, --verbose              verbose mode\n");
	printf("  -V, --version              prints the version information\n");
	printf("  -h, --help                 display a short help.");
	printf("\n\n");

	printf("For example, you may use %s as follows.\n", progname);

	printf("  To read ULS-file,\n");
	printf("     %s -L sample.ulc input1.txt\n", progname);
	printf("     %s -L sample.ulc bin-le.uls\n", progname);
	printf("     %s -L sample.ulc -o a.txt bin-le.uls\n", progname);
	printf("\n");

	printf("  To write ULS-file,\n");
	printf("     %s -L sample.ulc -t bin-le bin-le.uls\n", progname);
	printf("     %s -L sample.ulc -t bin-be bin-le.uls\n", progname);
	printf("\n");

	printf("  To get the conglomerate ULS-file,\n");
	printf("     %s -t bin    -C ./a.list /package/home\n", progname);
#ifdef ULS_FDF_SUPPORT
	printf("     %s -t bin-le -C ./a.list -f 'gcc -E' ../package/home\n", progname);
#endif
	printf("\n");

	printf("   To make a template-uls file,\n");
	printf("      %s -L sample.ulc -t bin-le -o tmpl_ex.uls -TT tmpl_ex.txt\n", progname);
	printf("\n");

	printf("   To use the created template-uls file,\n");
	printf("      %s -L <ulc-file>  tmpl_var1=int tmpl_var2='unsigned int' a.uls\n", progname);
	printf("      %s -L sample.ulc T='unsigned int' tmpl_ex.uls\n", progname);
	printf("\n");

	printf("   To create a template uls-file,\n");
	printf("      %s -Lsample.ulc -t bin -TT tmpl_ex.txt,\n", progname);
	printf("         where the value of -T-option is the lexeme ID token.\n");
	printf("   The -T-option can be used to replace the ID-tokens with TMPL-tokens.\n");
	printf("\n");
}

static int _ULS_STDCALL
ulsstream_options(int opt, char* optarg)
{
	int stat = 0;
#ifdef ULS_FDF_SUPPORT
	char fpath_buff[ULS_FILEPATH_MAX+1];
	char *lptr, *cmdl, *argv0;
	int siz;
#endif

	switch (opt) {
	case 'o':
		output_file = strdup(optarg, -1);
		path_normalize(output_file, output_file);
		break;
#ifdef ULS_FDF_SUPPORT
	case 'f':
		cmdl = skip_blanks(optarg);

		siz = strlen(cmdl) + ULS_FILEPATH_MAX + 4;
		cmdline_filter = malloc(siz);
		strcpy(cmdline_filter, cmdl);

		lptr = cmdline_filter;
		argv0 = splitstr(&lptr);

		argv0 = strdup(argv0, -1);
		path_normalize(argv0, argv0);

		if (!is_absolute_path(argv0) && dirent_exist(argv0) == ST_MODE_REG) {
			getcwd(fpath_buff, ULS_FILEPATH_MAX);
			snprintf(cmdline_filter, siz, "%s/%s", fpath_buff, cmdl);
		} else {
			mfree(cmdline_filter);
			cmdline_filter = cmdl;
		}
		break;
#endif
	case 't':
		opt_write = 1;
		if (streql(optarg, "bin")) {
#ifdef ULS_BIG_ENDIAN
			out_ftype = ULS_STREAM_BIN_BE;
#else
			out_ftype = ULS_STREAM_BIN_LE;
#endif
		} else if (streql(optarg, "bin-le")) {
			out_ftype = ULS_STREAM_BIN_LE;
		} else if (streql(optarg, "bin-be")) {
			out_ftype = ULS_STREAM_BIN_BE;
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
		config_path = optarg;
		break;

	case 'C':
		opt_write = 1;
		filelist = optarg;
		break;

	case 's':
		++opt_silent;
		break;

	case 'n':
		tag_name = optarg;
		break;

	case 'z':
		opt_want_eof = 1;
		break;

	case 'x':
		opt_no_numbering = 1;
		break;

	case 'H':
		usage_long();
		break;

	case 'h':
		usage();
		break;

	case 'v':
		++opt_verbose;
		break;

	case 'V':
		printf("%s %s, written by %s,\n\tis provided under %s.\n",
			progname, ULF_GEN_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		break;

	default:
		err_log("undefined option -%c", opt);
		stat = -1;
		break;
	}

	return stat;
}

static int
parse_options(int* ptr_argc, char* argv[])
{
	int argc = *ptr_argc;
	int i, i0, i1;
	char *ptr, *name;

#ifdef HAVE_GETOPT_H
	int  rc, opt, longindex;
#endif

#ifdef ULS_BIG_ENDIAN
		out_ftype = ULS_STREAM_BIN_BE;
#else
		out_ftype = ULS_STREAM_BIN_LE;
#endif

#ifdef HAVE_GETOPT_H
	while ((opt=getopt_long(argc, argv, ULSSTREAM_OPTSTR, longopts, &longindex)) != -1) {
		if ((rc=ulsstream_options(opt, optarg)) < 0) {
			return rc;
		}
	}
	i0 = optind;
#else
	if ((i0=getopts(argc, argv, ULSSTREAM_OPTSTR, ulsstream_options)) <= 0) {
		return -1;
	}
#endif
	if (config_path != NULL) {
		strncpy(config_pathbuff, config_path, ULS_LEXSTR_MAXSIZ);
		config_path = config_pathbuff;
	}

	if (opt_write && output_file == NULL) {
		output_file = "a.uls";
	}

	if (filelist != NULL) {
		if (i0 >= argc) {
			err_log("Specify the target-directory to apply the paths in %s", filelist);
			return -1;
		}
		target_dir = strdup(argv[i0++], -1);
		path_normalize(target_dir, target_dir);
	}

	for (i1=i=i0; i<argc; i++) {
		if ((ptr=strchr(argv[i], '=')) != NULL) {
			*ptr++ = '\0';
			name = argv[i];
			if (!add_name_val_ent(name, (void *) ptr)) {
				err_log("duplicate template name '%s' or array overflow!", name);
				return -1;
			}

		} else {
			argv[i1++] = argv[i];
		}
	}

	*ptr_argc = argc = i1;
	return i0;
}

int
main(int argc, char* argv[])
{
	lex_t *uls;
	int i0, stat = 0;
	int i, ftype;
	char *input_file;
	FILE *fp_out;

	initialize_uls();
#ifndef ULS_WINDOWS
	setlocale(LC_CTYPE, "");
#endif
	progname = THIS_PROGNAME;
	getcwd(home_dir, sizeof(home_dir));

	if ((i0=parse_options(&argc, argv)) < 0) {
		err_log("i0=%d, argc=%d", i0, argc);
		err_log("Type '%s -h'", argv[0]);
		return i0;
	}

	if (i0 >= argc) {
		err_log("%s(%s), written by %s,\n\tis provided under %s.",
			progname, ULF_GEN_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		usage();
		return 1;
	}

	if (filelist != NULL) {
		if (config_path == NULL) {
			if ((ftype=guess_specname_from_inputfiles(config_pathbuff)) < 0) {
				err_log("Unknown spec-name, specify the ulc-file of input-files");
				return -1;
			}
			config_path = config_pathbuff;
		}

		get_specname(config_path, specname, NULL);

		if (opt_verbose)
			err_log("The output file depends on the lex-spec '%s'", config_path);

		chdir(home_dir);

		if ((uls=create(config_path)) == NULL) {
			err_log("can't create a uls-object for lexical analysis.");
			return -1;
		}

		if (opt_want_eof)
			want_eof(uls);

		stat = proc_filelist(ftype, uls);
		destroy(uls);

		return stat;
	}

	if (opt_write) {
		if (config_path == NULL) {
			if (guess_specname(argv[i0], config_pathbuff) < 0) {
				err_log("can't open or don't know spec of %s", argv[i0]);
				return -1;
			}

			config_path = config_pathbuff;
		}

		get_specname(config_path, specname, NULL);

		if (opt_verbose)
			err_log("Loading uls config file '%s', ...", config_path);

		if ((uls=create(config_path)) == NULL) {
			err_log("can't create a uls-object for lexical analysis.");
			return -1;
		}

		if (opt_want_eof)
			want_eof(uls);

		for (i=i0; i<argc; i++) {
			input_file = strdup(argv[i], -1);
			path_normalize(input_file, input_file);
			write_file(uls, input_file);
		}

		destroy(uls);
		return stat;
	}

	if (output_file != NULL) {
		if ((fp_out=fopen(output_file, "w")) == NULL) {
			err_log("can't create %s", output_file);
		}
	} else {
		fp_out = stdout;
	}

	for (i=i0; i<argc; i++) {
		input_file = strdup(argv[i], -1);
		path_normalize(input_file, input_file);
		dump_file(input_file, fp_out);
	}

	fileclose(fp_out);
	return 0;
}
