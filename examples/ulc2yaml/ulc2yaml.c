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
  <file> ulc2yaml.c </file>
  <brief>
    Generating header file for token definitions.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Feb. 2022.
  </author>
*/

#include <uls.h>
#include <uls/uls_util.h>
#include <uls/uls_sysprops.h>
#include <uls/uls_langs.h>
#include <uls/uls_conf.h>
#include <uls/uld_conf.h>
#include <uls/uls_emit.h>
#include <uls/uls_dump.h>

#include <string.h>
#include <ctype.h>

#define THIS_PROGNAME "ulc2yaml"

const char *progname;
char *out_filepath;
char specname[128];

int typ_fpath;
int prn_flags;

char *ulc_config;

#ifdef HAVE_GETOPT
#include <getopt.h>

static const struct option longopts[] = {
	{ "output",        required_argument, NULL, 'o' },
	{ "signed-yaml",   no_argument,       NULL, 's' },
	{ "verbose",       no_argument,       NULL, 'v' },
	{ "version",       no_argument,       NULL, 'V' },
	{ "help",          no_argument,       NULL, 'h' },
	{ "Help",          no_argument,       NULL, 'H' },
	{ NULL, 0, NULL, 0 }
};
#endif

#define ULC2YAML_OPTSTR "o:svVhH"

static void usage_synopsis()
{
	uls_printf("Usage: %s [OPTIONS] <ulc-filepath>\n", progname);
	uls_printf("    %s generates yaml-file from ulc-file\n", progname);
}

static void usage_desc()
{
	uls_printf("      --signed-yaml           Prints the yaml format signature\n");
	uls_printf("  -o, --output <output-path>  Specify the output file path.\n");
	uls_printf("  -v, --verbose               Verbose mode.\n");
	uls_printf("  -V, --version               Prints the version information.\n");
	uls_printf("  -h, --help                  Displays the brief help.\n");
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

	uls_printf("The program dumps yaml format from ulc filepath.\n");
	uls_printf("For example, it dumps yaml content as following format:\n");

	uls_printf("\t%%YAML 1.2\n");
	uls_printf("\t---\n");
	uls_printf("\tinherits: ansi_c\n");
	uls_printf("\tulc-format-ver: 2.9\n");
	uls_printf("\tcase-sensitive: true\n");
	uls_printf("\tnumber-prefixes:\n");
	uls_printf("\t  - prefix: 0X\n");
	uls_printf("\t    radix: 16\n");
	uls_printf("\t  - prefix: 0x\n");
	uls_printf("\t    radix: 16\n");
	uls_printf("\t  - prefix: 0b\n");
	uls_printf("\t    radix: 2\n");
	uls_printf("\t  - prefix: 0\n");
	uls_printf("\t    radix: 8\n");
	uls_printf("\t............\n");
	uls_printf("\t............\n");
}

static int
ulc2yaml_options(int opt, char* optarg)
{
	int   stat = 0;

	switch (opt) {
	case 'o':
		out_filepath = uls_strdup(optarg, -1);
		uls_path_normalize(out_filepath, out_filepath);
		break;

	case 's':
		prn_flags |= 0x01;
		break;

	case 'v':
		break;

	case 'V':
		uls_printf("%s %s, written by %s,\n\tis provided under %s.\n",
			progname, ULC2YAML_PROGVER, ULS_AUTHOR, ULS_LICENSE_NAME);
		stat = 1;
		break;

	case 'h':
		usage();
		stat = 2;
		break;

	case 'H':
		usage_long();
		stat = 3;
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
	int  i0;
#ifdef HAVE_GETOPT
	int  rc, opt, longindex;

	while ((opt=getopt_long(argc, argv, ULC2YAML_OPTSTR, longopts, &longindex)) != -1) {
		if ((rc=ulc2yaml_options(opt, optarg)) != 0) {
			if (rc > 0) rc = 0;
			return rc;
		}
	}
	i0 = optind;
#else
	if ((i0=uls_getopts(argc, argv, ULC2YAML_OPTSTR, ulc2yaml_options)) <= 0) {
		return i0;
	}
#endif

	if (i0 < argc) {
		uls_outparam_t parms;

		ulc_config = uls_strdup(argv[i0++], -1);
		parms.line = specname;
		if ((typ_fpath = uls_get_spectype(ulc_config, uls_ptr(parms))) < 0) {
			err_log("%s: Invalid name for spec-name", ulc_config);
			return -1;
		}
	} else {
		typ_fpath = -1;
	}

	return i0;
}

int
main(int argc, char* argv[])
{
	int i0, stat=0;
	FILE *fout;

	progname = THIS_PROGNAME;
	if (argc <= 1) {
		usage_brief();
		return 1;
	}

	if ((i0=parse_options(argc, argv)) <= 0) {
		if (i0 < 0) err_log("%s: Incorrect use of command options.", progname);
		return i0;
	}

	if (ulc_config == NULL) {
		err_log("%s: Need args!", progname);
		usage_brief();
		return -1;
	}

	if (typ_fpath == ULS_NAME_FILEPATH_ULC) {
		uls_path_normalize(ulc_config, ulc_config);
	}

	if (out_filepath != NULL) {
		if ((fout = fopen(out_filepath, "wb")) == NULL) {
			err_log("%s: can't open '%s'", __func__, out_filepath);
			return -1;
		}
	} else {
		fout = stdout;
	}

	if (uls_dump_tokdef__yaml(ulc_config, fout, prn_flags) < 0) {
		stat = -1;
	}

	if (out_filepath != NULL) fclose(fout);
	uls_mfree(ulc_config);
	uls_mfree(out_filepath);

	return stat;
}

