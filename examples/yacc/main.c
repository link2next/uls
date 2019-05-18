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
  <file> ulc2xml.c </file>
  <brief>
    generating xml-version of ulc-file reading the structure from it.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "globals.h"
#include "ulc_gram.h"

const char *progname;

int  opt_mode;
int  opt_verbose;

uls_lex_t *ulc_lex;
char *outfile_xml;
FILE *fout_xml;

static void usage(void)
{
	err_log("%s v1.0", progname);
	err_log("  ULS can be used as front end of yacc/bison");
	err_log("  '%s' demonstrates how ULS is used with yacc/bison", progname);
	err_log("");
	err_log(" Usage:");
	err_log("  %s [-o output-file] <configfile-1>", progname);
	err_log("");
	err_log("  For example,");
	err_log("      %s input1.txt", progname);
}

static int
globals_init(int argc, char** argv)
{
	progname = uls_filename(argv[0], NULL);
	fout_xml = NULL;
	outfile_xml = NULL;
	init_ulc_file();
	return 0;
}

static int
options(int opt, char* optarg)
{
	int stat = 0;

	switch (opt) {
	case 'm':
		opt_mode = atoi(optarg);
		break;

	case 'o':
		outfile_xml = optarg;
		break;

	case 'h':
		usage();
		stat = 1;
		break;

	case 'v':
		opt_verbose = 1;
		break;

	default:
		err_log("undefined option -%c", opt);
		usage();
		stat = -1;
		break;
	}

	return stat;
}

static int
parse_options(int argc, char* argv[])
{
	int   i0;

	globals_init(argc, argv);

	if ((i0=uls_getopts(argc, argv, "m:o:hv", options)) <= 0) {
		exit(i0);
	}

	if (outfile_xml == NULL) {
		fout_xml = stdout;
	} else if ((fout_xml=uls_fp_open(outfile_xml, ULS_FIO_CREAT)) == NULL) {
		err_log("can't open output file '%s' for writing", outfile_xml);
		return -1;
	}

	return i0;
}

extern int yyparse(void);
extern YYSTYPE yylval;

#define ULC2XML_ID_MAXLEN 63
static char yy_lexeme_str[ULC2XML_ID_MAXLEN+1];
char *yytext;
int  yyline;

int
currentline(void)
{
	return yyline;
}

static uls_uch_t
ulc2xml_peek_ch(int *ptr_is_quote)
{
	uls_uch_t uch;
	uls_nextch_detail_t detail;
	int is_quote = 0;

	if ((uch = uls_peek_uch(ulc_lex, &detail)) == ULS_UCH_NONE && detail.qmt != NULL) {
		is_quote = 1;
	}

	if (ptr_is_quote != NULL) {
		*ptr_is_quote = is_quote;
	}

	return uch;
}

static uls_uch_t
ulc2xml_get_ch(int *ptr_is_quote)
{
	uls_uch_t uch;
	uls_nextch_detail_t detail;
	int is_quote = 0;

	if ((uch = uls_get_uch(ulc_lex, &detail)) == ULS_UCH_NONE && detail.qmt != NULL) {
		is_quote = 1;
	}

	if (ptr_is_quote != NULL) {
		*ptr_is_quote = is_quote;
	}

	return uch;
}

int yylex(void)
{
	int i, tok, is_quote;
	uls_uch_t uch;

	tok = uls_get_tok(ulc_lex);
	if (tok == TOK_EOI) return 0;

	if (tok == ':' || tok == '\n') {
		if (tok == '\n') ++yyline;
		yytext = NULL;
		return tok;
	}

	if (tok == TOK_NUM) {
		yylval.i_val = uls_lexeme_int(ulc_lex);

	} else if (tok == TOK_ID) {
		if (uls_lexeme_len(ulc_lex) > ULC2XML_ID_MAXLEN) {
			err_panic("%s: Too Long Identifier!", uls_lexeme(ulc_lex));
		}

		yylval.s_val = yytext = strdup(uls_lexeme(ulc_lex));

	} else if (tok < 128) {
		yy_lexeme_str[0] = tok;
		for (i=1; i<ULC2XML_ID_MAXLEN; ) {
			if ((uch = ulc2xml_peek_ch(&is_quote)) == ULS_UCH_NONE || is_quote ||
				uch == ' ' || uch == '\n') {
				break;
			}
			uch = ulc2xml_get_ch(&is_quote);
			yy_lexeme_str[i++] = (char) uch;
		}
		yy_lexeme_str[i] = '\0';

		yylval.s_val = yytext = strdup(yy_lexeme_str);
		tok = TOK_WORD;

	} else {
		yytext = NULL;
	}

	return tok;
}

int parse_ulc_file(char *file, ulc_file_t *ulc_file)
{
	if (uls_set_file(ulc_lex, file, 0) < 0) {
		err_log("can't open input-file file %s", file);
		return -1;
	}

	yyline = 1;
	yyparse();

	return 0;
}

int
main(int argc, char* argv[])
{
	char *input_file;
	int i0;
#ifdef _ULS_WANT_STATIC_LIBS
	initialize_uls();
#endif
	if ((i0=parse_options(argc, argv)) <= 0) {
		return i0;
	}

	input_file = "sample.ulc";

	if ((ulc_lex = uls_create("ulc.ulc")) == NULL) {
		err_log("can't init uls-object");
		return -1;
	}

	if (opt_verbose) err_log("Processing '%s', ...", input_file);
	parse_ulc_file(input_file, &ulc_file);

	if (opt_verbose) err_log("\tSyntax checking done.");
	uls_destroy(ulc_lex);

	if (opt_verbose) err_log("Generating output into stdout, ...");
	dump_ulc_file(&ulc_file, fout_xml);

	uls_fp_close(fout_xml);
	deinit_ulc_file();

	if (opt_verbose) err_log("\tDone.");

	return 0;
}
