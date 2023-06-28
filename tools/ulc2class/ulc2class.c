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
  <file> ulc2class.c </file>
  <brief>
    Generating header file for token definitions.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, April 2011.
  </author>
*/

#include "uls.h"
#include "uls/uls_util.h"

#include "uls/uls_sysprops.h"
#include "uls/uls_langs.h"
#include "uls/uld_conf.h"
#include "uls/uls_emit.h"
#include "uls/uls_dump.h"

#include <ctype.h>

#define THIS_PROGNAME "ulc2class"
#include "ult_utils.h"

const char *progname;
char home_dir[ULS_FILEPATH_MAX+1];
const char *opt_class_name;
char *out_dirpath, *out_filepath, *out_filename, *out_filename0;
const char *opt_enum_name;
const char *opt_prefix;
const char *opt_dump;

int typ_fpath;
char specname[ULS_LEXSTR_MAXSIZ+1];

uls_flags_t prn_flags;
int opt_uld_gen, opt_query;
char *ulc_config;
uls_lex_ptr_t sam_lex;

#ifdef HAVE_GETOPT
#include <getopt.h>

static const struct option longopts[] = {
	{ "dirpath",       required_argument, NULL, 'd' },
	{ "filename",      required_argument, NULL, 'f' },
	{ "uld-sample",    no_argument,       NULL, 's' },
	{ "enum",          required_argument, NULL, 'e' },
	{ "lang",          required_argument, NULL, 'l' },
	{ "class-name",    required_argument, NULL, 'n' },
	{ "group",         required_argument, NULL, 'g' }, // OBSOLETE:
	{ "prefix",        required_argument, NULL, 'p' },
	{ "query",         no_argument,       NULL, 'q' },
	{ "output",        required_argument, NULL, 'o' },

	{ "silent",        no_argument,       NULL, 'y' },
	{ "dump",          required_argument, NULL, 'z' },
	{ "verbose",       no_argument,       NULL, 'v' },
	{ "version",       no_argument,       NULL, 'V' },
	{ "Help",          no_argument,       NULL, 'H' },
	{ "help",          no_argument,       NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
#endif

#define ULC2CLASS_OPTSTR "d:f:e:l:n:g:p:qo:svVHhyz:"

static void usage_synopsis()
{
	uls_printf("Usage: %s [OPTIONS] <ulc-file.ulc|uld-file.uld>\n", progname);
	uls_printf("    %s generates the source files for lexical analysis from ulc file.\n", progname);
	uls_printf("       %s <ulc-filepath|lang-name>\n", progname);
	uls_printf("       %s -l {c|cpp|cppcli|cs|java} <ulc-file>\n", progname);
	uls_printf("       %s -lc -e <enum-name> <ulc-file>\n", progname);
	uls_printf("       %s -f <out-filename> -n <class-name> <ulc-file>\n", progname);
	uls_printf("       %s -d <out-dirpath> -f <out-filename> <ulc-file>\n", progname);
	uls_printf("       %s -q [lang-name]\n", progname);
	uls_printf("       %s -q -s <lang-name>\n", progname);
	uls_printf("       %s --dump=<category> <ulc-filepath>,\n", progname);
	uls_printf("          where category = [keyw|names|rsvd|hash|ch_ctx|quote|1char|2char|utf]\n");
}

static void usage_desc()
{
#ifdef ULS_WINDOWS
	uls_printf("  -o <filepath>     specify the output file path.\n");
	uls_printf("  -f <filename>     specify the output file name without suffix.\n");
	uls_printf("  -d <dirpath>      specify the directory for output files.\n");
	uls_printf("  -l <lang-name>    specify the language name when generating source files.\n");
	uls_printf("  -n <name>         specifies the name of class(or enum-name).\n");
	uls_printf("  -q                query the list of ulc names.\n");
	uls_printf("  -s                generates a sample uld-file.\n");
//	uls_printf("  -g <token-group>  token-group = {'regular','reserved', 'quote'}.\n");
	uls_printf("  -v                verbose mode.\n");
	uls_printf("  -V                prints the version information.\n");
	uls_printf("  -h                displays the brief help.\n");
#else
	uls_printf("  -o, --output <filepath>    specify the output file path.\n");
	uls_printf("  -f, --filename <filename>  specify the output file name without suffix.\n");
	uls_printf("  -d, --dirpath <dirpath>    specify the directory for output files.\n");
	uls_printf("  -l, --lang=<lang-name>     specify the target language name.\n");
	uls_printf("  -n, --class-name=<name>    specify the name of the class(or enum-name).\n");

	uls_printf("  -q, --query [lang-name]    query the list of ulc names.\n");
	uls_printf("  -s, --uld-sample           generates a sample uld-file.\n");
//	uls_printf("  -g, --group=<token-group>  token-group = {'regular','reserved', 'quote'}.\n");
	uls_printf("  -v, --verbose              verbose mode.\n");
	uls_printf("  -V, --version              prints the version information.\n");
	uls_printf("  -h, --help                 displays the brief help.\n");
#endif
}

static void usage_brief()
{
	usage_synopsis();
	uls_printf("\n");

	usage_desc();
	uls_printf("\n");
}

static void usage()
{
	usage_brief();
}

static void usage_long(void)
{
	usage_brief();

	uls_printf("The tokens generated by %s consists of reserved and regular ones.\n");
	uls_printf("The regular tokens are defined by user.\n");
	uls_printf("The reserved tokens are the basic tokens that the system initially defines.\n");
	uls_printf("\n");

	uls_printf("The below are the reserved tokens:\n");
	uls_printf(" * EOI(End of Input): The last token of all input streams.\n");
	uls_printf("       It returns EOI if the internal input stack is empty!\n");
	uls_printf(" * EOF: The internal input structure contains a stack of input streams.\n");
	uls_printf("       It returns EOF whenever top of the stacks is consumed.\n");
	uls_printf(" * ERR: It returns this err token in case of error.\n");
	uls_printf(" * ID: Identifer token, which should be defined by user in ulc-file.\n");
	uls_printf(" * NUMBER: It represents the current token is number whichever it is integer or floating number.\n");
	uls_printf(" * LINENUM: This informs users of the location of the input cursor.\n");
	uls_printf(" * TMPL: This allows the template variables in uls-file, which must be replaced with strings.\n");
	uls_printf(" * NONE: It returns NONE when the input stack is intial state.\n");
	uls_printf("\n");

	uls_printf("You can specify the qualified long class name with -n-option.\n");
	uls_printf("For instance, the class name may be 'AAA.BBB.SampleLex', or just class name 'SampleLex'.\n");
	uls_printf("\n");

	uls_printf("To generate c++ headers,\n");
	uls_printf("    %s sample.ulc\n", progname);
	uls_printf("    %s -lcpp -n AAA.BBB.SampleLex sample.ulc\n", progname);
	uls_printf("The default output of %s is a c++ header file.\n", progname);
	uls_printf("\n");

	uls_printf("To generate header files of other languages, use -l-option.\n");
	uls_printf("    %s -lc sample.ulc\n", progname);
	uls_printf("    %s -ljava -n sample sample.ulc\n", progname);
	uls_printf("\n");

	uls_printf("To generate C# wrapper class files,\n");
	uls_printf("    %s -lcs -n AAA.BBB.SampleLex sample.ulc\n", progname);
	uls_printf("    %s -d /topdir/AAA/BBB -f SampleLex -lcs -n AAA.BBB.SampleLex sample.ulc\n", progname);
	uls_printf("    The above line specifies the output directory with -d-option.\n");
	uls_printf("    The f-option is used for the common filename of output files.\n");
	uls_printf("\n");

	uls_printf("To generate java class files,\n");
	uls_printf("    %s -ljava -n AAA.BBB.SampleLex sample.ulc\n", progname);
	uls_printf("    %s -d /topdir/AAA/BBB -f SampleLex -l java -n AAA.BBB.SampleLex sample.ulc\n", progname);
	uls_printf("\n");

	uls_printf("To query the available names for ulc in the uls repository,\n");
	uls_printf("    %s -q\n", progname);
	uls_printf("  .....\n");
	uls_printf("  cpp c++ C++\n");
	uls_printf("  c_sharp C# c# cs csharp c-sharp\n");
	uls_printf("  go Go golang\n");
	uls_printf("  visual_basic visual-basic VisualBasic 'visual basic' 'Visual basic'\n");
	uls_printf("  .....\n");
	uls_printf(" Each line represents the supported names of a language.\n");
	uls_printf(" You may select the any name in same group for your preference.\n");
	uls_printf(" The name must be used as the argument of uls-object creator,\n");
	uls_printf("     such as uls_create(), subclasses of UlsLex(), for configuration name.\n");
	uls_printf("\n");

	uls_printf("If you want to know whether or not a language is supported by ULS,\n");
	uls_printf("    %s -q golang\n", progname);
	uls_printf("    %s -q c++\n", progname);
	uls_printf("\n");

	uls_printf("To dump the mapping of token name to token number, use -s-option with -q-option.\n");
	uls_printf("    %s -q -s golang\n", progname);
	uls_printf("      #@golang\n");
	uls_printf("        ......\n");
	uls_printf("        NUMBER -3\n");
	uls_printf("            ID -2\n");
	uls_printf("           EOF -1\n");
	uls_printf("           EOI 0\n");
	uls_printf("   RUNE_LITSTR 128\n");
	uls_printf(" INTERP_LITSTR 129\n");
	uls_printf("    RAW_LITSTR 130\n");
	uls_printf("         .....\n");
	uls_printf("You can modify the token name and number after saving the above output as uld-file\n");
	uls_printf("  and specify the path of uld-file in the argument of uls-object creator for configuration name.\n");
	uls_printf("\n");

	uls_printf("Refer to the examples in 'ulc_exam' in the package to see how to write ulc-file.\n");
	uls_printf("Refer to the examples' or 'tests' to see how to use ulc-file.\n");
	uls_printf("\n");
}

static int
set_lang_generated(const char* lang_name)
{
	int stat = 0;

	prn_flags &= ~ULS_FL_LANG_GEN_MASK;

	if (ult_streql(lang_name, "c")) {
		prn_flags |= ULS_FL_C_GEN;

	} else if (ult_streql(lang_name, "cpp") || ult_streql(lang_name, "c++")) {
		prn_flags |= ULS_FL_CPP_GEN; // default value in Linux

	} else if (ult_streql(lang_name, "cppcli") || ult_streql(lang_name, "cpp/cli") ||
		ult_streql(lang_name, "c++.net")) {
		prn_flags |= ULS_FL_CPPCLI_GEN; // default value in Windows

	} else if (ult_streql(lang_name, "cs")) {
		prn_flags |= ULS_FL_CS_GEN;

	} else if (ult_streql(lang_name, "java")) {
		prn_flags |= ULS_FL_JAVA_GEN;

	} else {
		stat = -1;
	}

	return stat;
}

static const char*
get_standard_suffix(void)
{
	const char *cptr;

	if (prn_flags & ULS_FL_CS_GEN) {
		cptr = ".cs";
	} else if (prn_flags & ULS_FL_JAVA_GEN) {
		cptr = ".java";
	} else {
		cptr = ".h";
	}

	return cptr;
}

static int
ulc2class_options(int opt, char* optarg)
{
	int   stat = 0;

	switch (opt) {
	case 'e':
		opt_enum_name = optarg;
		break;

	case 'l':
		if (set_lang_generated(optarg) < 0) {
			err_log("%s: invalid lang-name!", optarg);
			stat = -1;
		}
		break;

	case 'g':
		if (ult_streql(optarg, "regular")) {
			prn_flags |= ULS_FL_WANT_REGULAR_TOKS;
		} else if (ult_streql(optarg, "reserved")) {
			prn_flags |= ULS_FL_WANT_RESERVED_TOKS;
		} else if (ult_streql(optarg, "quote")) {
			prn_flags |= ULS_FL_WANT_QUOTE_TOKS;
		} else {
			err_log("unknown group type '%s'", optarg);
			stat = -1;
		}
		break;

	case 'n':
		if (optarg == NULL || *optarg=='\0') {
			err_log("%s: Invalid (class) name", progname);
			stat = -1; break;
		}
		opt_class_name = optarg;
		break;

	case 'p':
		opt_prefix = optarg;
		if (strlen(opt_prefix) > ULS_LEXSTR_MAXSIZ) {
			err_log("%s: Too long token prefix %s selected", progname, opt_prefix);
			stat = -1;
		}
		break;

	case 'd':
		out_dirpath = ult_strdup(optarg);
		uls_path_normalize(out_dirpath, out_dirpath);
		break;

	case 'f':
		if (out_filepath != NULL) {
			err_log("%s: collided with -o-option!", progname);
			stat = -1; break;
		}
		prn_flags |= ULS_FL_WANT_WRAPPER;
		out_filename = ult_strdup(optarg);
		uls_path_normalize(out_filename, out_filename);
		break;

	case 'o':
		if (out_filename != NULL) {
			err_log("%s: -f-option already used", progname);
			stat = -1; break;
		}
		out_filepath = ult_strdup(optarg);
		uls_path_normalize(out_filepath, out_filepath);
		break;

	case 'q':
		opt_query = 1;
		break;

	case 's':
		opt_uld_gen = 1;
		break;

	case 'y':
		prn_flags &= ~ULS_FL_VERBOSE;
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
		prn_flags |= ULS_FL_VERBOSE;
		break;


	case 'V':
		uls_printf("%s %s, written by %s,\n\tis provided under %s.\n",
			progname, ULC2CLASS_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		stat = 1;
		break;

	case 'z':
		opt_dump = optarg;
		break;

	default:
		err_log("%s: undefined option -%c", progname, opt);
		stat = -1;
		break;
	}

	return stat;
}

static int
parse_options(int argc, char* argv[])
{
#ifdef HAVE_GETOPT
	int  opt, longindex;
#endif
	int  rc, len, i0, mask, len_fname, len_filepath, len_dirpath2;
	char the_rootdir[2] = { ULS_FILEPATH_DELIM, '\0' };
	char *fname, *tmp_buf;

	if (ult_getcwd(home_dir, ULS_FILEPATH_MAX) < 0) {
		err_panic("%s: fail to getcwd()", __FUNCTION__);
	}

	opt_prefix = "";

#ifdef HAVE_GETOPT
	while ((opt=getopt_long(argc, argv, ULC2CLASS_OPTSTR, longopts, &longindex)) != -1) {
		if ((rc=ulc2class_options(opt, optarg)) != 0) {
			if (rc > 0) rc = 0;
			return rc;
		}
	}
	i0 = optind;
#else
	if ((i0=uls_getopts(argc, argv, ULC2CLASS_OPTSTR, ulc2class_options)) <= 0) {
		return i0;
	}
#endif

	if (opt_query) {
		if (i0 < argc) {
			ulc_config = ult_strdup(argv[i0++]);
		}

		return i0;
	}

	if (i0 < argc) {
		uls_outparam_t parms;

		ulc_config = ult_strdup(argv[i0++]);
		parms.line = specname;
		if ((typ_fpath = uls_get_spectype(ulc_config, uls_ptr(parms))) < 0) {
			err_log("%s: Invalid name for spec-name", ulc_config);
			return -1;
		}

	} else {
		typ_fpath = -1;
	}

	if (out_filepath != NULL) {
		fname = uls_filename(out_filepath, &len_fname);
		if (len_fname <= 0) {
			err_log("%s: invalid filename '%s'", progname, out_filepath);
			return -1;
		}

		if ((len_filepath = (int) (fname - out_filepath)) > 0) {
			if (out_filepath[len_filepath-1] != ULS_FILEPATH_DELIM) {
				err_log("%s: invalid filepath '%s'", progname, out_filepath);
				return -1;
			}

			if (--len_filepath <= 0) {
				out_dirpath = uls_strdup(the_rootdir, -1);
			} else {
				out_dirpath = uls_strdup(out_filepath, len_filepath);
			}
		}

		out_filename = uls_strdup(fname, len_fname);
		if (!ult_streql(fname + len_fname,  get_standard_suffix())) {
			out_filename0 = uls_strdup(fname, -1);
		}

	} else if (out_filename != NULL) {
		fname = uls_filename(out_filename, &len_fname);
		if (len_fname <= 0) {
			err_log("%s: invalid filename '%s'", out_filename, fname);
			return -1;
		}

		if ((len_dirpath2 = (int) (fname - out_filename)) > 0) {
			if (out_filename[len_dirpath2-1] != ULS_FILEPATH_DELIM) {
				err_log("%s: invalid filepath '%s'", progname, out_filename);
				return -1;
			}

			if (--len_dirpath2 <= 0) {
				out_dirpath = uls_strdup(the_rootdir, -1);
			} else if (ult_is_absolute_path(out_filename)) {
				out_dirpath = uls_strdup(out_filename, len_dirpath2);
			} else if (out_dirpath != NULL) {
				len = uls_strlen(out_dirpath);
				out_dirpath = (char *) uls_mrealloc(out_dirpath, len + 1 + len_dirpath2 + 1);
				out_dirpath[len++] = ULS_FILEPATH_DELIM;
				len += uls_strncpy(out_dirpath + len, out_filename, len_dirpath2);
				uls_path_normalize(out_dirpath, out_dirpath);
			} else {
				out_dirpath = uls_strdup(out_filename, len_dirpath2);
			}
		}

		tmp_buf = out_filename;
		out_filename = uls_strdup(fname, -1);
		uls_mfree(tmp_buf);

	} else {
		prn_flags |= ULS_FL_WANT_WRAPPER;
	}

	if (out_dirpath != NULL) {
		if ((rc=ult_chdir(out_dirpath)) < 0 || ult_chdir(home_dir) < 0) {
			err_log("%s: invalid dirpath'%s'", progname, out_dirpath);
			return -1;
		}
	}

	mask = ULS_FL_WANT_REGULAR_TOKS | ULS_FL_WANT_QUOTE_TOKS | ULS_FL_WANT_RESERVED_TOKS;
	if (!(prn_flags & mask)) prn_flags |= mask;

	if ((prn_flags & ULS_FL_LANG_GEN_MASK) == 0) {
		prn_flags |= ULS_FL_CPP_GEN;
	}

	if (prn_flags & (ULS_FL_C_GEN | ULS_FL_CPP_GEN |ULS_FL_CPPCLI_GEN)) {
	} else {
		prn_flags &= ~ULS_FL_WANT_WRAPPER;
	}

	return i0;
}

int
ulc_query_var(const char *varnam)
{
	const char *strval;
	int stat = 0;

	if ((strval = uls_get_system_property(varnam)) == NULL) {
		err_log("%s: unknown variable %s.", progname, varnam);
		stat = -1;
	} else {
		uls_printf("%s\n", strval);
	}

	return stat;
}

int
ulc_query_process(void)
{
	if (opt_uld_gen) {
		uld_export_names(sam_lex);
	}

	return 0;
}

int
ulc_generate_files(uls_parms_emit_t *emit_parm)
{
	const char *cptr_suff;
	char *tmp_buf;
	int rc, stat = 0;

	if (uls_generate_tokdef_file(sam_lex, emit_parm) < 0) {
		err_log("%s: fail to make the header files for %s", progname, ulc_config);
		return -1;
	}

	if (out_filename0 == NULL) {
		return 0;
	}

	cptr_suff = get_standard_suffix();

	rc = uls_strlen(emit_parm->out_fname);
	tmp_buf= (char *) uls_malloc(rc + (int) strlen(cptr_suff) + 1);
	uls_strcpy(tmp_buf, emit_parm->out_fname);
	uls_strcpy(tmp_buf + rc, cptr_suff);

	if (out_dirpath != NULL) {
		ult_chdir(out_dirpath);
	}

	if (rename(tmp_buf, out_filename0) < 0) {
		err_log("%s: fail to rename %s to %s", progname, tmp_buf, out_filename0);
		stat = -1;
	}

	uls_mfree(tmp_buf);
	ult_chdir(home_dir);

	return stat;
}

void
dump_tok_info(uls_lex_t *uls)
{
	if (ult_streql(opt_dump, "keyw")) {
		uls_dump_tokdef_vx(uls);
	} else if (ult_streql(opt_dump, "names")) {
		uls_dump_tokdef_names(uls);
	} else if (ult_streql(opt_dump, "rsvd")) {
		uls_dump_tokdef_rsvd(uls);
	} else if (ult_streql(opt_dump, "hash")) {
		uls_dump_kwtable(&uls->idkeyw_table);
	} else if (ult_streql(opt_dump, "ch_ctx")) {
		uls_dump_char_context(uls);
	} else if (ult_streql(opt_dump, "quote")) {
		uls_dump_quote(uls);
	} else if (ult_streql(opt_dump, "1char")) {
		uls_dump_1char(uls);
	} else if (ult_streql(opt_dump, "2char")) {
		uls_dump_2char(uls);
	} else if (ult_streql(opt_dump, "utf")) {
		uls_dump_utf8(uls);
	}
}

int
main(int argc, char* argv[])
{
	const char *uld_file;
	int i0, rc, stat=0;
	uls_parms_emit_t emit_parm;

	progname = THIS_PROGNAME;
	if (argc <= 1) {
		usage_brief();
		return 1;
	}

	if ((i0=parse_options(argc, argv)) <= 0) {
		if (i0 < 0) err_log("%s: Incorrect use of command options.", progname);
		return i0;
	}

	if (opt_uld_gen && !opt_query) {
		err_log("%s: Use -s with -q-option.", progname);
		return -1;
	}

	if (opt_query && !opt_uld_gen) {
		if (opt_class_name != NULL) {
			return ulc_query_var(opt_class_name);
		}

		if (ulc_config == NULL) {
			uls_list_langs();
		} else {
			if (uls_list_names_of_lang(ulc_config) < 0) {
				err_log("%s: No information found!", ulc_config);
			}
		}

		return 0;
	}

	if (ulc_config == NULL) {
		err_log("%s: Need args!", progname);
		usage_brief();
		return -1;
	}

	if (typ_fpath == ULS_NAME_FILEPATH_ULC) {
		uls_path_normalize(ulc_config, ulc_config);
		uld_file = ulc_config;
	} else if (typ_fpath == ULS_NAME_FILEPATH_ULD) {
		prn_flags |= ULS_FL_ULD_FILE;
		uls_path_normalize(ulc_config, ulc_config);
		uld_file = ulc_config;
	} else { // ULS_NAME_SPECNAME
		uld_file = NULL;
	}

	if (prn_flags & ULS_FL_VERBOSE) {
		if (typ_fpath != ULS_NAME_FILEPATH_ULD) {
			ulc_list_searchpath(ulc_config);
		}
	}

	if ((sam_lex=uls_create(ulc_config)) == uls_nil) {
		err_log("%s: Failed to open the configuration file %s.", progname, ulc_config);
		if (typ_fpath != ULS_NAME_FILEPATH_ULD) {
			ulc_list_searchpath(ulc_config);
		}
		return -1;
	}

	rc = uls_init_parms_emit(&emit_parm, out_dirpath, out_filename, uld_file,
		specname, opt_class_name, opt_enum_name, opt_prefix,
		prn_flags | ULS_FL_WANT_RESERVED_TOKS);
	if (rc < 0) {
		err_log("%s: fail to create source files for %s", progname, ulc_config);
		stat = -1;
	}

	if (opt_dump != NULL) {
		dump_tok_info(sam_lex);
	} else if (opt_query) {
		stat = ulc_query_process();
	} else {
		stat = ulc_generate_files(&emit_parm);
	}

	uls_deinit_parms_emit(&emit_parm);
	uls_destroy(sam_lex);

	uls_mfree(ulc_config);
	uls_mfree(out_dirpath);
	uls_mfree(out_filepath);
	uls_mfree(out_filename0);
	uls_mfree(out_filename);

	return stat;
}
