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
    Generating xml-version of ulc-file reading the structure from it.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#include "globals.h"
#include "ulc2xml.h"

ulc_file_t ulc_file;

static int
print(const char* fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = uls_vsysprn(fmt, args);
	va_end(args);

	return len;
}

ulc_wordlist_t*
new_wordlist_element(char *text)
{
	ulc_wordlist_t *e;

	e = (ulc_wordlist_t *) malloc(sizeof(ulc_wordlist_t));
	if (e == NULL)
		err_panic("malloc error!");

	e->wordtext = text;
	e->next = NULL;

	return e;
}

void
release_wordlist(ulc_wordlist_t* wrdlst)
{
	ulc_wordlist_t *e, *e_next;

	for (e=wrdlst; e!=NULL; e=e_next) {
		e_next = e->next;
		free(e->wordtext);
		free(e);
	}
}

ulc_token_def_t*
new_tokdef(char *name, char* keyw, int tok_id)
{
	ulc_token_def_t*  e;

	if (ulc_file.n_tokdef_list >= ulc_file.n_alloc_tokdef_list) {
		ulc_file.n_alloc_tokdef_list += 32;
		ulc_file.tokdef_list = (ulc_token_def_t*) realloc(ulc_file.tokdef_list,
			ulc_file.n_alloc_tokdef_list * sizeof(ulc_token_def_t));
		if (ulc_file.tokdef_list == NULL) {
			err_panic("%s: malloc error!", __FUNCTION__);
		}
	}

	e = ulc_file.tokdef_list + ulc_file.n_tokdef_list;

	e->tok_id = tok_id;
	e->name = name;
	e->keyword = keyw;

	++ulc_file.n_tokdef_list;
	return e;
}

void
release_tokdef(ulc_token_def_t *e)
{
	if (e->name) free(e->name);
	if (e->keyword) free(e->keyword);
}

void
release_tokdef_list(void)
{
	ulc_token_def_t*  e;
	int i;

	for (i=0; i<ulc_file.n_tokdef_list; i++) {
		e = ulc_file.tokdef_list + i;
		release_tokdef(e);
	}

	if (ulc_file.tokdef_list) free(ulc_file.tokdef_list);
	ulc_file.n_alloc_tokdef_list = ulc_file.n_tokdef_list = 0;
}

void
init_ulc_file(void)
{
	memset(&ulc_file, 0x00, sizeof(ulc_file_t));
}

void
deinit_ulc_file(void)
{
	release_tokdef_list();
	memset(&ulc_file, 0x00, sizeof(ulc_file_t));
}

static const char*
onoff_str(int a)
{
	return a!=0 ? "on" : "off";
}

void dump_ulc_file(ulc_file_t *ulc, FILE* fout)
{
	ulc_commtype_t   *cmt;
	ulc_quotetype_t  *qmt;
	ulc_id_range_t   *id_range;
	ulc_token_def_t  *tdef;
	int i;

	uls_sysprn_open(fout, NULL);

	print("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	print("<ulc-file>\n");
	print("  <file-version>%s</file-version>\n", ulc->filever);
	print("  <ulc-header>\n");
	print("    <ulc-attr name=\"want_eof_tok\">%s</ulc-attr>\n", onoff_str(ulc->flags & ULC2XML_WANT_EOF_TOK));
	print("    <ulc-attr name=\"case_insensitive\">%s</ulc-attr>\n", onoff_str(ulc->flags & ULC2XML_CASE_INSENSITIVE));
	print("    <ulc-attr name=\"want_lf_char\">%s</ulc-attr>\n", onoff_str(ulc->flags & ULC2XML_WANT_LF_CHAR));
	print("    <ulc-attr name=\"want_tab_char\">%s</ulc-attr>\n", onoff_str(ulc->flags & ULC2XML_WANT_TAB_CHAR));

	for (i=0; i<ulc_file.n_commtypes; i++) {
		cmt = ulc_file.commtypes + i;
		print("    <comment-mark>\n");
		print("       <parm name=\"start_str\"><![CDATA[%s]]></parm>\n", cmt->start_mark);
		print("       <parm name=\"end_str\"><![CDATA[%s]]></parm>\n", cmt->end_mark);
		print("    </comment-mark>\n");
	}

	for (i=0; i<ulc_file.n_quotetypes; i++) {
		qmt = ulc_file.quotetypes + i;
		print("    <quote-mark>\n");
		print("       <parm name=\"start_str\"><![CDATA[%s]]></parm>\n", qmt->start_mark);
		print("       <parm name=\"end_str\"><![CDATA[%s]]></parm>\n", qmt->end_mark);
		print("    </quote-mark>\n");
	}

	print("    <idfirst_charsets>\n");
	for (i=0; i<ulc_file.n_idfirst_charsets; i++) {
		id_range = ulc_file.idfirst_charsets + i;
		print("      <charset><![CDATA[%s]]></charset>\n", id_range->charset);
	}
	print("    </idfirst_charsets>\n");

	print("    <id_charsets>\n");
	for (i=0; i<ulc_file.n_id_charsets; i++) {
		id_range = ulc_file.id_charsets + i;
		print("      <charset><![CDATA[%s]]></charset>\n", id_range->charset);
	}
	print("    </id_charsets>\n");

	print("  </ulc-header>\n");

	print("  <ulc-body>\n");

	for (i=0; i<ulc_file.n_tokdef_list; i++) {
		tdef = ulc_file.tokdef_list + i;
		print("    <tokdef>\n");
		print("      <tok-id>%d</tok-id>\n", tdef->tok_id);
		print("      <tok-name>%s</tok-name>\n", tdef->name);
		print("      <keyword><![CDATA[%s]]></keyword>\n", tdef->keyword);
		print("    </tokdef>\n");
	}

	print("  </ulc-body>\n");
	print("</ulc-file>\n");

	uls_sysprn_close();
}
