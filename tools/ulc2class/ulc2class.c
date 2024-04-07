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
	{ "enum",          required_argument, NULL, 'e' },
	{ "filename",      required_argument, NULL, 'f' },
	{ "group",         required_argument, NULL, 'g' }, // OBSOLETE:
	{ "lang",          required_argument, NULL, 'l' },
	{ "class-name",    required_argument, NULL, 'n' },
	{ "output",        required_argument, NULL, 'o' },
	{ "prefix",        required_argument, NULL, 'p' },
	{ "query",         no_argument,       NULL, 'q' },
	{ "uld-sample",    no_argument,       NULL, 's' },
	{ "uld",           no_argument,       NULL, 'S' },
	{ "silent",        no_argument,       NULL, 'y' },
	{ "dump",          required_argument, NULL, 'z' },
	{ "verbose",       no_argument,       NULL, 'v' },
	{ "version",       no_argument,       NULL, 'V' },
	{ "Help",          no_argument,       NULL, 'H' },
	{ "help",          no_argument,       NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
#endif

#define ULC2CLASS_OPTSTR "d:e:f:g:l:n:o:p:qsSvVHhyz:"

static void usage_synopsis()
{
	err_log("Usage: %s [OPTIONS] <file.ulc|file.uld>", progname);
	err_log("    %s generates the source files for lexical analysis from ulc file.", progname);
	err_log("       %s <ulc-filepath|lang-name>", progname);
	err_log("       %s -l {c|cpp|cppcli|cs|java} <ulc-file>", progname);
	err_log("       %s -lc -e <enum-name> <ulc-file>", progname);
	err_log("       %s -f <out-filename> -n <class-name> <ulc-file>", progname);
	err_log("       %s -d <out-dirpath> -f <out-filename> <ulc-file>", progname);
	err_log("       %s -S <lang-name>", progname);
	err_log("       %s -q [lang-name]", progname);
	err_log("       %s --dump=<category> <ulc-filepath>,", progname);
	err_log("          where category = [keyw|names|rsvd|hash|ch_ctx|quote|1char|2char|utf]");
}

static void usage_desc()
{
#ifdef ULS_WINDOWS
	err_log("  -d <dirpath>      specify the directory for output files.");
	err_log("  -e <enum-name>    specify the enum-name if you want token-name list with enum style.");
	err_log("  -f <filename>     specify the output file name without suffix.");
	err_log("  -l <lang-name>    specify the language name when generating source files.");
	err_log("  -n <name>         specifies the name of class(or enum-name).");
	err_log("  -o <filepath>     specify the output file path.");
	err_log("  -p <prefix>       prepend <prefix> at the front of token-name.");
	err_log("  -q                query the list of ulc names.");
	err_log("  -S                generates a sample uld-file.");
	err_log("  -v                verbose mode.");
	err_log("  -V                prints the version information.");
	err_log("  -h                displays the brief help.");
#else
	err_log("  -d, --dirpath <dirpath>    specify the directory for output files.");
	err_log("  -e, --enum <enum-name>     specify the enum-name if you want token-name list with enum style.");
	err_log("  -f, --filename <filename>  specify the output file name without suffix.");
	err_log("  -l, --lang=<lang-name>     specify the target language name.");
	err_log("  -n, --class-name=<name>    specify the name of the class(or enum-name).");
	err_log("  -o, --output <filepath>    specify the output file path.");
	err_log("  -p, --prefix <prefix>      prepend <prefix> at the front of token-name.");
	err_log("  -q, --query [lang-name]    query the list of ulc names.");
	err_log("  -S, --uld-sample           generates a sample uld-file.");
	err_log("  -v, --verbose              verbose mode.");
	err_log("  -V, --version              prints the version information.");
	err_log("  -h, --help                 displays the brief help.");
#endif
}

static void usage_brief()
{
	usage_synopsis();
	err_log("");

	usage_desc();
	err_log("");
}

static void usage()
{
	usage_brief();
}

static void usage_long(void)
{
	usage_brief();

	err_log("The tokens generated by %s consists of reserved and regular ones.", progname);
	err_log("The regular tokens are defined by user.");
	err_log("The reserved tokens are the basic tokens that the system initially defines.");
	err_log("");

	err_log("The below are the reserved tokens:");
	err_log(" * EOI(End of Input): The last token of all input streams.");
	err_log("       It returns EOI if the internal input stack is empty!");
	err_log(" * EOF: The internal input structure contains a stack of input streams.");
	err_log("       It returns EOF whenever top of the stacks is consumed.");
	err_log(" * ERR: It returns this err token in case of error.");
	err_log(" * ID: Identifer token, which should be defined by user in ulc-file.");
	err_log(" * NUMBER: It represents the current token is number whichever it is integer or floating number.");
	err_log(" * LINENUM: This informs users of the location of the input cursor.");
	err_log(" * TMPL: This allows the template variables in uls-file, which must be replaced with strings.");
	err_log(" * NONE: It returns NONE when the input stack is intial state.");
	err_log("");

	err_log("You can specify the qualified long class name with -n-option.");
	err_log("For instance, the class name may be 'AAA.BBB.SampleLex', or just class name 'SampleLex'.");
	err_log("");

	err_log("To generate c++ headers,");
	err_log("    %s sample.ulc", progname);
	err_log("    %s -lcpp -n AAA.BBB.SampleLex sample.ulc", progname);
	err_log("The default output of %s is a c++ header file.", progname);
	err_log("");

	err_log("To generate header files of other languages, use -l-option.");
	err_log("    %s -lc sample.ulc", progname);
	err_log("    %s -ljava -n sample sample.ulc", progname);
	err_log("");

	err_log("To generate C# wrapper class files,");
	err_log("    %s -lcs -n AAA.BBB.SampleLex sample.ulc", progname);
	err_log("    %s -d /topdir/AAA/BBB -f SampleLex -lcs -n AAA.BBB.SampleLex sample.ulc", progname);
	err_log("    The above line specifies the output directory with -d-option.");
	err_log("    The f-option is used for the common filename of output files.");
	err_log("");

	err_log("To generate java class files,");
	err_log("    %s -ljava -n AAA.BBB.SampleLex sample.ulc", progname);
	err_log("    %s -d /topdir/AAA/BBB -f SampleLex -l java -n AAA.BBB.SampleLex sample.ulc", progname);
	err_log("");

	err_log("To query the available names for ulc in the uls repository,");
	err_log("    %s -q", progname);
	err_log("  .....");
	err_log("  cpp c++ C++");
	err_log("  c_sharp C# c# cs csharp c-sharp");
	err_log("  go Go golang");
	err_log("  visual_basic visual-basic VisualBasic 'visual basic' 'Visual basic'");
	err_log("  .....");
	err_log(" Each line represents the supported names of a language.");
	err_log(" You may select the any name in same group for your preference.");
	err_log(" The name must be used as the argument of uls-object creator,");
	err_log("     such as uls_create(), subclasses of UlsLex(), for configuration name.");
	err_log("");

	err_log("If you want to know whether or not a language is supported by ULS,");
	err_log("    %s -q golang", progname);
	err_log("    %s -q c++", progname);
	err_log("");

	err_log("To dump the mapping of token name to token number, use -s-option with -q-option.");
	err_log("    %s -S golang", progname);
	err_log("#@go");
	err_log("");
	err_log("#ERR              ERR               -8");
	err_log("#NONE             NONE              -7");
	err_log(".....");
	err_log("");
	err_log("You can modify the token name and number after saving the above output as uld-file");
	err_log("  and specify the path of uld-file in the argument of uls-object creator for configuration name.");
	err_log("");

	err_log("Refer to the examples in 'ulc_exam' in the package to see how to write ulc-file.");
	err_log("Refer to the examples' or 'tests' to see how to use ulc-file.");
	err_log("");
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

	case 'S':
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
		err_panic("%s: fail to getcwd()", __func__);
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

	if (prn_flags & (ULS_FL_C_GEN | ULS_FL_CPP_GEN | ULS_FL_CPPCLI_GEN)) {
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
dump_tok_info(uls_lex_ptr_t uls)
{
	if (ult_streql(opt_dump, "keyw")) {
		uls_dump_list_tokdef_vx(uls);
	} else if (ult_streql(opt_dump, "names")) {
		uls_dump_tokdef_names(uls);
	} else if (ult_streql(opt_dump, "rsvd")) {
		uls_dump_tokdef_rsvd(uls);
	} else if (ult_streql(opt_dump, "hash")) {
		uls_dump_kwtable(uls);
	} else if (ult_streql(opt_dump, "ch_ctx")) {
		uls_dump_char_context(uls);
	} else if (ult_streql(opt_dump, "quote")) {
		uls_dump_quote(uls);
	} else if (ult_streql(opt_dump, "1char")) {
		uls_dump_1char(uls);
	} else if (ult_streql(opt_dump, "2char")) {
		uls_dump_2char(uls);
	} else if (ult_streql(opt_dump, "utf")) {
		uls_dump_utf8firstbyte(uls);
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

	if (opt_query) {
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
		err_log("%s: Failed to process the configuration file %s", progname, ulc_config);
		if (typ_fpath != ULS_NAME_FILEPATH_ULD) {
			ulc_list_searchpath(ulc_config);
		}
		return -1;
	}

	if (opt_dump != NULL) {
		dump_tok_info(sam_lex);
	} else if (opt_uld_gen) {
		uld_dump_sample(sam_lex);
	} else {
		rc = uls_init_parms_emit(&emit_parm, out_dirpath, out_filename, uld_file,
			specname, opt_class_name, opt_enum_name, opt_prefix,
			prn_flags | ULS_FL_WANT_RESERVED_TOKS);
		if (rc < 0) {
			err_log("%s: fail to create source files for %s", progname, ulc_config);
			stat = -1;
		} else {
			stat = ulc_generate_files(&emit_parm);
			uls_deinit_parms_emit(&emit_parm);
		}
	}

	uls_destroy(sam_lex);

	uls_mfree(ulc_config);
	uls_mfree(out_dirpath);
	uls_mfree(out_filepath);
	uls_mfree(out_filename0);
	uls_mfree(out_filename);

	return stat;
}
