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
 * uls_emit.c -- generating the output file of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, March 2014.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_EMIT__
#include "uls/uls_emit.h"
#include "uls/uld_conf.h"
#include "uls/uls_misc.h"
#include "uls/uls_fileio.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(emit_source_head)(const char *name)
{
	static char *file_head[] = {
		"Permission is hereby granted, free of charge, to any person",
		"obtaining a copy of this software and associated documentation",
		"files (the \"Software\"), to deal in the Software without",
		"restriction, including without limitation the rights to use,",
		"copy, modify, merge, publish, distribute, sublicense, and/or sell",
		"copies of the Software, and to permit persons to whom the",
		"Software is furnished to do so, subject to the following",
		"conditions:",
		"",
		"The above copyright notice and this permission notice shall be",
		"included in all copies or substantial portions of the Software.",
		"",
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,",
		"EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES",
		"OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND",
		"NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT",
		"HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,",
		"WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING",
		"FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR",
		"OTHER DEALINGS IN THE SOFTWARE.",
		NULL
	};

	const char *prefix_stmt = " *";
	const char *cstr;
	int i;

	_uls_log_(sysprn)("/**\n");
	_uls_log_(sysprn)("%s This file automatically has been generated by ULS and\n", prefix_stmt);
	_uls_log_(sysprn)("%s  contains an implementation of lexical analyzer\n", prefix_stmt);
	_uls_log_(sysprn)("%s  for parsing the programming language '%s'.\n", prefix_stmt, name);
	_uls_log_(sysprn)("%s\n", prefix_stmt);

	for (i=0; (cstr=file_head[i]) != NULL; i++) {
		if (*cstr != '\0') {
			_uls_log_(sysprn)("%s %s\n", prefix_stmt, cstr);
		} else {
			_uls_log_(sysprn)("%s \n", prefix_stmt);
		}
	}
	_uls_log_(sysprn)(" */\n");
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(comp_by_tokid_vx)(const uls_voidptr_t a, const uls_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1_vx = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2_vx = (const uls_tokdef_vx_ptr_t) b;
	int stat;

	if (e1_vx->tok_id > e2_vx->tok_id) stat = 1;
	else if (e1_vx->tok_id < e2_vx->tok_id) stat = -1;
	else stat = 0;

	return stat;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(filter_to_print_tokdef)(uls_lex_ptr_t uls, uls_tokdef_ptr_t e, int flags)
{
	uls_tokdef_vx_ptr_t e_vx = e->view;
	int stat = 1;

	if (e_vx->tok_id == uls->xcontext.toknum_LINENUM || _uls_get_namebuf_value(e_vx->name)[0] == '\0') {
		stat = 0; // filtered

	} else if (is_reserved_tok(uls, _uls_get_namebuf_value(e_vx->name)) >= 0) {
		if ((flags & ULS_FL_WANT_RESERVED_TOKS) == 0) stat = 0;

	} else if (e->keyw_type == ULS_KEYW_TYPE_LITERAL) {
		if ((flags & ULS_FL_WANT_QUOTE_TOKS) == 0) stat = 0;

	} else {
		if ((flags & ULS_FL_WANT_REGULAR_TOKS) == 0) stat = 0;
	}

	if (!stat) {
//		_uls_log(err_log)("'%s' filtered for printing header file.", e->view->name);
	}

	return stat;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(print_tokdef_constants)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char* tok_pfx, int flags)
{
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	char toknam_str[2*ULS_LEXSTR_MAXSIZ+1];
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, tokdef_ary_prn);
	const char *cnst_symbol, *asgn_symbol = "=", *end_symbol = ";";
	int i;

	if (flags & ULS_FL_C_GEN) {
		cnst_symbol = "#define";
		asgn_symbol = end_symbol = "";

	} else if ((flags & ULS_FL_CPP_GEN) || (flags & ULS_FL_CPPCLI_GEN)) {
		cnst_symbol = "static const int";

	} else if (flags & ULS_FL_CS_GEN) {
		cnst_symbol = "public const int";

	} else if (flags & ULS_FL_JAVA_GEN) {
		cnst_symbol = "public final int";

	} else {
		cnst_symbol = "unknown";
		asgn_symbol = end_symbol = "";
	}

	for (i=0; i<n_tokdef_ary_prn; i++) {
		e0_vx = slots_vx[i];

		if (e0_vx->tok_id % 10 == 0 || i == N_RESERVED_TOKS) _uls_log_(sysprn)("\n");

		_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, _uls_get_namebuf_value(e0_vx->name));

		uls_sysprn_tabs(n_tabs, "%s %16s %s", cnst_symbol, toknam_str, asgn_symbol);
		_uls_log_(sysprn)(" %d%s\n", e0_vx->tok_id, end_symbol);

		for (e_nam = e0_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->prev) {
			_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, _uls_get_namebuf_value(e_nam->name));
			uls_sysprn_tabs(n_tabs, "%s %16s %s", cnst_symbol, toknam_str, asgn_symbol);
			_uls_log_(sysprn)(" %d%s\n", e0_vx->tok_id, end_symbol);
		}
	}
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(print_tokdef_enum_constants)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char* enum_name, const char* tok_pfx, int flags)
{
	char toknam_str[2*ULS_LEXSTR_MAXSIZ+1];
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, tokdef_ary_prn);
	int i, prev_tok_id, sect_lead, last_one;
	const char *cptr;

	if (n_tokdef_ary_prn <= 0) return;

	if (n_tokdef_ary_prn > 0) prev_tok_id = slots_vx[0]->tok_id;
	else prev_tok_id = -1;

	if (flags & ULS_FL_CPPCLI_GEN) {
		cptr = "enum class";
	} else if (flags & (ULS_FL_CS_GEN | ULS_FL_JAVA_GEN)) {
		cptr = "public enum";
	} else {
		cptr = "enum";
	}

	uls_sysprn_tabs(n_tabs, "%s %s %c\n", cptr, enum_name, '{');

	for (i=0; i < n_tokdef_ary_prn; i++) {
		if (i + 1 >= n_tokdef_ary_prn) {
			last_one = 1;
		} else {
			last_one = 0;
		}

		e0_vx = slots_vx[i];

		_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, _uls_get_namebuf_value(e0_vx->name));

		sect_lead = (e0_vx->tok_id % 10 == 0);
		if (sect_lead || prev_tok_id + 1 != e0_vx->tok_id) {
			uls_sysprn_tabs(n_tabs, "%16s = %d", toknam_str, e0_vx->tok_id);
		} else {
			uls_sysprn_tabs(n_tabs, "%16s", toknam_str);
		}

		if (!last_one || e0_vx->tokdef_names != nilptr) {
			_uls_log_(sysprn)(",\n");
		} else {
			_uls_log_(sysprn)("\n");
			break;
		}

		for (e_nam = e0_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->prev) {
			_uls_log_(sysprn)("\t%24s = %d", _uls_get_namebuf_value(e_nam->name), e0_vx->tok_id);
			if (e_nam->prev != nilptr) {
				_uls_log_(sysprn)(",\n");
			} else { // last one
				_uls_log_(sysprn)("\n");
			}
		}

		prev_tok_id = e0_vx->tok_id;
	}

	if (flags & (ULS_FL_CS_GEN | ULS_FL_JAVA_GEN)) {
		cptr = "}";
	} else {
		cptr = "};";
	}

	uls_sysprn_tabs(n_tabs, "%s\n", cptr);
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_1)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	char  linebuff[ULS_LINEBUFF_SIZ__ULD+1];
	const char *cptr;
	int len;

	if ((cptr = tok_names->name2) == NULL) {
		cptr = "NULL";
		len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "\"%s\", %s",
			tok_names->name, cptr);
	} else {
		len = _uls_log_(snprintf)(linebuff, sizeof(linebuff), "\"%s\", \"%s\"",
			tok_names->name, cptr);
	}

	len += _uls_log_(snprintf)(linebuff+len, sizeof(linebuff)-len, ", %d, %d",
		tok_names->tokid_changed, tok_names->tokid);

	if (*(cptr = tok_names->aliases) != '\0') {
		len += _uls_log_(snprintf)(linebuff+len, sizeof(linebuff)-len, ", \"%s %s\"", cptr, lptr);
	} else {
		len += _uls_log_(snprintf)(linebuff+len, sizeof(linebuff)-len, ", NULL");
	}

	uls_sysprn_tabs(n_tabs, "{ %s },\n", linebuff);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_2)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	const char *cptr;

	uls_sysprn_tabs(n_tabs, "tok_names.name = \"%s\";\n", tok_names->name);

	if ((cptr = tok_names->name2) == NULL) cptr = "NULL";
	uls_sysprn_tabs(n_tabs, "tok_names.name2 = \"%s\";\n", cptr);

	uls_sysprn_tabs(n_tabs, "tok_names.tokid_changed = %d;\n", tok_names->tokid_changed);
	uls_sysprn_tabs(n_tabs, "tok_names.tokid = %d;\n", tok_names->tokid);

	if ((cptr = tok_names->aliases) == NULL) cptr = "NULL";
	uls_sysprn_tabs(n_tabs, "tok_names.aliases = \"%s\";\n", cptr);

	uls_sysprn_tabs(n_tabs, "uld_change_names(names_map, uls_ptr(tok_names));\n\n");

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr, const char *nilmark)
{
	const char *cptr1, *cptr2;
	const char *qmark1, *qmark2;

	if ((cptr1 = tok_names->name2) == NULL) {
		qmark1 = "";
		cptr1 = nilmark;
	} else {
		qmark1 = "\"";
	}

	if ((cptr2 = tok_names->aliases) == NULL || *cptr2 == '\0') {
		qmark2 = "";
		cptr2 = nilmark;
	} else {
		qmark2 = "\"";
	}

	uls_sysprn_tabs(n_tabs, "changeUldNames(\"%s\", %s", tok_names->name, qmark1);
	_uls_log_(sysprn)("%s%s", cptr1, qmark1);
	_uls_log_(sysprn)(", %d, %d", tok_names->tokid_changed, tok_names->tokid);
	_uls_log_(sysprn)(", %s%s", qmark2, cptr2);
	_uls_log_(sysprn)("%s);\n", qmark2);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3cpp)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	return __print_uld_lineproc_3(tok_names, n_tabs, lptr, "NULL");
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3cs)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	return __print_uld_lineproc_3(tok_names, n_tabs, lptr, "null");
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_c_source_1_fp)(int n_tabs, FILE *fin_uld)
{
	int n_tok_names;

	uls_sysprn_tabs(n_tabs, "static uld_line_t toknam_defs[] = {\n");
	n_tok_names = print_uld_source(fin_uld, n_tabs + 1, _uls_ref_callback_this(__print_uld_lineproc_1));
	uls_sysprn_tabs(n_tabs + 1, "{ NULL, }\n");
	uls_sysprn_tabs(n_tabs, "};\n");

	uls_sysprn_tabs(n_tabs, "static const int N_toknam_defs = %d;\n\n", n_tok_names);

	return n_tok_names;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_c_source_2_fp)(int n_tabs, FILE *fin_uld)
{
	int   n_tok_names;

	uls_sysprn_tabs(n_tabs, " {\n\tuld_line_t tok_names;\n\n");
	n_tok_names = print_uld_source(fin_uld, n_tabs, _uls_ref_callback_this(__print_uld_lineproc_2));
	uls_sysprn_tabs(n_tabs, " }\n");

	return n_tok_names;
}

int
ULS_QUALIFIED_METHOD(print_uld_source)(FILE *fin_uld, int n_tabs, uls_proc_uld_line_t lineproc)
{
	char  linebuff[ULS_LINEBUFF_SIZ__ULD+1], *lptr;
	int   linelen, lno=0, stat=0, n_tok_names=0;
	uld_line_t tok_names;
	_uls_tool_type_(wrd) wrdx;

	while (1) {
		if ((linelen=_uls_tool_(fp_gets)(fin_uld, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				_uls_log(err_log)("%s: ulc file i/o error at %d", __FUNCTION__, lno);
				stat = -1;
			} else {
				stat = n_tok_names;
			}
			break;
		}
		++lno;

		if (*(lptr = _uls_tool(skip_blanks)(linebuff)) == '\0' ||
			*lptr == '#' || (lptr[0]=='/' && lptr[1]=='/'))
			continue;

		wrdx.lptr = lptr;

		if (uld_pars_line(lno, uls_ptr(wrdx), uls_ptr(tok_names)) < 0) {
			stat = -1;
			break;
		}

		if (lineproc(uls_ptr(tok_names), n_tabs, wrdx.lptr) < 0) {
			stat = -1;
			break;
		}

		++n_tok_names;
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(print_tokdef_c_header)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn, uls_parms_emit_ptr_t emit_parm)
{
	char  fnameAZ[ULS_LEXSTR_MAXSIZ+1], ch;
	uls_flags_t flags = emit_parm->flags;
	const char *lex_name = emit_parm->class_name;
	int i;

	_uls_tool_(str_toupper)(emit_parm->out_fname, fnameAZ, -1);
	for (i=0; (ch=fnameAZ[i]) != '\0'; i++) {
		if (!_uls_tool_(isalnum)(ch)) {
			fnameAZ[i] = '_';
		}
	}

	_uls_log_(sysprn)("#ifndef __%s_H__\n", fnameAZ);
	_uls_log_(sysprn)("#define __%s_H__\n\n", fnameAZ);

	if (flags & ULS_FL_WANT_WRAPPER) {
		_uls_log_(sysprn)("#include <uls/uls_conf.h>\n\n");
	}

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			0, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			0, emit_parm->tok_pfx, flags);
	}

	if (flags & ULS_FL_WANT_WRAPPER) {
		_uls_log_(sysprn)("\n");
		_uls_log_(sysprn)("extern int uls_init_%s(uls_lex_ptr_t uls);\n", lex_name);
		_uls_log_(sysprn)("extern uls_lex_ptr_t uls_create_%s(void);\n", lex_name);
		_uls_log_(sysprn)("extern int uls_destroy_%s(uls_lex_ptr_t uls);\n", lex_name);
	}

	_uls_log_(sysprn)("\n#endif\n\n");
}

int
ULS_QUALIFIED_METHOD(__print_tokdef_c_source_fp)(FILE *fin_uld, int typ)
{
	static char *load_pairs_proc_prolog[] = {
		"static int load_pairs(uls_lex_ptr_t uls)",
		"{",
		"	uld_names_map_ptr_t names_map;",
		"	int   i, stat = 0;",
		"",
		"	names_map = uld_prepare_names(uls);",
		NULL
	};

	static char *load_pairs_proc_epilog[] = {
		"",
		"	if (uld_post_names(names_map) < 0) {",
		"		stat = -1;",
		"	}",
		"",
		"	return stat;",
		"}",
		"",
		NULL
	};

	static char *load_pairs_proc_typ1[] = {
		"",
		"	for (i=0; i<N_toknam_defs; i++) {",
		"		if (uld_change_names(names_map, toknam_defs + i) < 0) {",
		"			stat = -1;",
		"			break;",
		"		}",
		"	}",
		NULL
	};

	const char *cstr;
	int i, rval;

	if (typ == 1) {
		if ((rval = __print_uld_c_source_1_fp(0, fin_uld)) < 0) {
			return -1;
		}
	}

	for (i=0; (cstr=load_pairs_proc_prolog[i]) != NULL; i++) {
		_uls_log_(sysprn)("%s\n", cstr);
	}

	if (typ == 1) {
		for (i=0; (cstr=load_pairs_proc_typ1[i]) != NULL; i++) {
			_uls_log_(sysprn)("%s\n", cstr);
		}
	} else if (typ == 2) {
		if ((rval = __print_uld_c_source_2_fp(1, fin_uld)) < 0) {
			return -1;
		}
	}

	for (i=0; (cstr=load_pairs_proc_epilog[i]) != NULL; i++) {
		_uls_log_(sysprn)("%s\n", cstr);
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(__print_tokdef_c_source_file)(const char *filepath, int typ)
{
	FILE   *fin_uld;
	int rval;

	if ((fin_uld = _uls_tool_(fp_open)(filepath, ULS_FIO_READ)) == NULL) {
		_uls_log(err_log)("can't open file '%s'", filepath);
		return -1;
	}

	_uls_log_(sysprn)("#include <uls/uld_conf.h>\n\n");
	rval = __print_tokdef_c_source_fp(fin_uld, typ);
	_uls_tool_(fp_close)(fin_uld);

	return rval;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_c_source)(uls_parms_emit_ptr_t emit_parm, const char *base_ulc, int typ)
{
	uls_flags_t flags = emit_parm->flags;
	const char *lex_name = emit_parm->class_name;
	const char *strfmt;

	_uls_log_(sysprn)("#include <uls/uls_core.h>\n");

	if (emit_parm->fpath_uld != NULL) {
		if (__print_tokdef_c_source_file(emit_parm->fpath_uld, typ) < 0) {
			_uls_log(err_log)("fail to write uld-struct to '%s'", emit_parm->fpath_uld);
			return -1;
		}
	} else {
		_uls_log_(sysprn)("\n");
	}


	if (flags & ULS_FL_STRFMT_ASTR) {
		strfmt = "_astr";
	} else if (flags & ULS_FL_STRFMT_WSTR) {
		strfmt = "_wstr";
	} else {
		strfmt = "";
	}

	_uls_log_(sysprn)("int uls_init_%s(uls_lex_ptr_t uls)\n", lex_name);
	_uls_log_(sysprn)("{\n");
	_uls_log_(sysprn)("	const char *confname = \"%s\";\n", base_ulc);
	_uls_log_(sysprn)("\n");
	_uls_log_(sysprn)("	if (uls_init%s(uls, confname) <  0) {\n", strfmt);
	_uls_log_(sysprn)("		return -1;\n");
	_uls_log_(sysprn)("	}\n");
	_uls_log_(sysprn)("\n");

	if (emit_parm->fpath_uld != NULL) {
		_uls_log_(sysprn)("	if (load_pairs(uls) < 0) {\n");
		_uls_log_(sysprn)("		return -1;\n");
		_uls_log_(sysprn)("	}\n");
		_uls_log_(sysprn)("\n");
	}

	_uls_log_(sysprn)("	return 0;\n");
	_uls_log_(sysprn)("}\n\n");

	_uls_log_(sysprn)("uls_lex_ptr_t uls_create_%s(void)\n", lex_name);
	_uls_log_(sysprn)("{\n");
	_uls_log_(sysprn)("	const char *confname = \"%s\";\n", base_ulc);
	_uls_log_(sysprn)("	uls_lex_ptr_t uls;\n");
	_uls_log_(sysprn)("\n");
	_uls_log_(sysprn)("	if ((uls = uls_create%s(confname)) == NULL) {\n", strfmt);
	_uls_log_(sysprn)("		return NULL;\n");
	_uls_log_(sysprn)("	}\n");
	_uls_log_(sysprn)("\n");

	if (emit_parm->fpath_uld != NULL) {
		_uls_log_(sysprn)("	if (load_pairs(uls) < 0) {\n");
		_uls_log_(sysprn)("		return NULL;\n");
		_uls_log_(sysprn)("	}\n");
		_uls_log_(sysprn)("\n");
	}

	_uls_log_(sysprn)("	return uls;\n");
	_uls_log_(sysprn)("}\n\n");

	_uls_log_(sysprn)("int uls_destroy_%s(uls_lex_ptr_t uls)\n", lex_name);
	_uls_log_(sysprn)("{\n");
	_uls_log_(sysprn)("	return uls_destroy(uls);\n");
	_uls_log_(sysprn)("}\n\n");

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_cpp_header)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	_uls_decl_parray_slots_tool_(al, argstr);

	const char *cptr1, *cptr2, *ns_uls;
	char ch_lbrace='{';
	int  n_tabs;

	if (n_tokdef_ary_prn <= 0) {
		return 0;
	}

	_uls_log_(sysprn)("#pragma once\n\n");

	if (flags & ULS_FL_CPPCLI_GEN) {
		ns_uls="uls::polaris";
	} else { // flags & ULS_FL_CPP_GEN
		_uls_log_(sysprn)("#include <uls/UlsLex.h>\n");
		_uls_log_(sysprn)("#include <string>\n\n");
		ns_uls="uls::crux";
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	for (n_tabs=0; n_tabs<emit_parm->n_name_components; n_tabs++) {
		cptr1 = al[n_tabs]->str;
		uls_sysprn_tabs(n_tabs, "namespace %s %c\n", cptr1, ch_lbrace);
	}

	if (flags & ULS_FL_CPPCLI_GEN) {
		cptr1 = "public ref class";
	} else { // flags & ULS_FL_CPP_GEN
		cptr1 = "class";
	}

	uls_sysprn_tabs(n_tabs, "%s %s :", cptr1, emit_parm->class_name);
	_uls_log_(sysprn)(" public %s::UlsLex %c\n", ns_uls, ch_lbrace);

	uls_sysprn_tabs(n_tabs, "  public:\n");

	++n_tabs;

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	_uls_log_(sysprn)("\n");

	if (flags & ULS_FL_CPPCLI_GEN) {
		cptr2 = "System::String ^";
	} else {
		cptr2 = "std::tstring &";
	}

	if (flags & ULS_FL_WANT_WRAPPER) {
		if (emit_parm->fpath_ulc != NULL) {
			uls_sysprn_tabs(n_tabs, "%s(%sulc_file);\n", emit_parm->class_name, cptr2);
		} else {
			uls_sysprn_tabs(n_tabs, "%s();\n", emit_parm->class_name);
		}
	} else {
		if (emit_parm->fpath_ulc != NULL || emit_parm->fpath_uld != NULL) {
			uls_sysprn_tabs(n_tabs, "%s(%sulc_fpath)\n", emit_parm->class_name, cptr2);
			uls_sysprn_tabs(n_tabs+1, " : %s::UlsLex(ulc_fpath) %c}\n", ns_uls, ch_lbrace);
		} else {
			uls_sysprn_tabs(n_tabs, "%s()\n", emit_parm->class_name);
			uls_sysprn_tabs(n_tabs+1, " : %s::UlsLex(\"%s\") %c}\n", ns_uls, base_ulc, ch_lbrace);
		}
	}

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "};\n");

	for (--n_tabs; n_tabs >= 0; n_tabs--) {
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_cpp_source)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	_uls_decl_parray_slots_tool_(al, argstr);
	FILE *fin_uld;
	const char *cptr, *cptr2, *ns_uls;
	char ch_lbrace='{';
	int  i, n_tabs=0;

	if (n_tokdef_ary_prn <= 0) {
		return 0;
	}
#ifdef ULS_WINDOWS
	_uls_log_(sysprn)("#include \"stdafx.h\"\n");
#endif
	_uls_log_(sysprn)("#include \"%s.h\"\n", emit_parm->out_fname);
	_uls_log_(sysprn)("#include <string>\n\n");

	if (flags & ULS_FL_CPPCLI_GEN) {
		ns_uls = "uls::polaris";
		cptr2 = "System::String^";
		_uls_log_(sysprn)("using namespace %s;\n\n", ns_uls);
	} else {
		ns_uls = "uls::crux";
		cptr2 = "std::tstring&";
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	for (i=0; i<emit_parm->n_name_components; i++) {
		cptr = al[i]->str;
		_uls_log_(sysprn)("namespace %s %c\n", cptr, ch_lbrace);
	}

	if (emit_parm->n_name_components > 0) ++n_tabs;

	if (emit_parm->fpath_ulc != NULL) {
		uls_sysprn_tabs(n_tabs, "%s::%s(%s ulc_file)\n", emit_parm->class_name, emit_parm->class_name, cptr2);
		uls_sysprn_tabs(n_tabs+1, ": %s::UlsLex(ulc_file) %c\n", ns_uls, ch_lbrace);
	} else {
		uls_sysprn_tabs(n_tabs, "%s::%s()\n", emit_parm->class_name, emit_parm->class_name);
		uls_sysprn_tabs(n_tabs+1, ": %s::UlsLex(\"%s\") %c\n", ns_uls, base_ulc, ch_lbrace);
	}

	++n_tabs;

	if (emit_parm->fpath_uld != NULL) {
		uls_sysprn_tabs(n_tabs, "prepareUldMap();\n");
		_uls_log_(sysprn)("\n");

		if ((fin_uld = _uls_tool_(fp_open)(emit_parm->fpath_uld, ULS_FIO_READ)) == NULL) {
			_uls_log(err_log)("can't open file '%s'", emit_parm->fpath_uld);
			return -1;
		}

		print_uld_source(fin_uld, n_tabs, _uls_ref_callback_this(__print_uld_lineproc_3cpp));
		_uls_tool_(fp_close)(fin_uld);

		_uls_log_(sysprn)("\n");
		uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
	}

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "}\n");

	for (i=emit_parm->n_name_components-1; i >= 0; i--) {
		_uls_log_(sysprn)("}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_cs)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	_uls_decl_parray_slots_tool_(al, argstr);
	FILE *fin_uld;
	const char *cptr, *ns_uls = "uls.polaris";
	char ch_lbrace='{';
	int  n_tabs;

	if (n_tokdef_ary_prn <= 0) {
		return 0;
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));

	_uls_log_(sysprn)("using System;\n\n");
	for (n_tabs=0; n_tabs<emit_parm->n_name_components; n_tabs++) {
		cptr = al[n_tabs]->str;
		uls_sysprn_tabs(n_tabs, "namespace %s\n", cptr);
		uls_sysprn_tabs(n_tabs, "%c\n", ch_lbrace);
	}

	uls_sysprn_tabs(n_tabs, "public class %s : %s.UlsLex", emit_parm->class_name, ns_uls);
	_uls_log_(sysprn)(" %c\n", ch_lbrace);

	++n_tabs;

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	uls_sysprn_tabs(0, "\n");

	if (emit_parm->fpath_ulc != NULL) {
		uls_sysprn_tabs(n_tabs, "public %s(String ulc_file)\n", emit_parm->class_name);
		uls_sysprn_tabs(n_tabs+1, ": base(ulc_file) %c\n", ch_lbrace);
	} else {
		uls_sysprn_tabs(n_tabs, "public %s()\n", emit_parm->class_name);
		uls_sysprn_tabs(n_tabs+1, ": base(\"%s\") %c\n", base_ulc, ch_lbrace);
	}

	++n_tabs;
	if (emit_parm->fpath_uld != NULL) {
		uls_sysprn_tabs(n_tabs, "prepareUldMap();\n");
		_uls_log_(sysprn)("\n");

		if ((fin_uld = _uls_tool_(fp_open)(emit_parm->fpath_uld, ULS_FIO_READ)) == NULL) {
			_uls_log(err_log)("can't open file '%s'", emit_parm->fpath_uld);
			return -1;
		}

		print_uld_source(fin_uld, n_tabs, _uls_ref_callback_this(__print_uld_lineproc_3cs));
		_uls_tool_(fp_close)(fin_uld);

		_uls_log_(sysprn)("\n");
		uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
	}
	--n_tabs;
	uls_sysprn_tabs(n_tabs, "}\n");

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "} // End of %s\n", emit_parm->class_name);

	for (--n_tabs; n_tabs >= 0; n_tabs--) {
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_java)(uls_lex_ptr_t uls,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	_uls_decl_parray_slots_tool_(al, argstr);
	FILE *fin_uld;
	const char *cptr, *ns_uls = "uls.polaris";
	char ch_lbrace='{';
	int  n_tabs;

	if (n_tokdef_ary_prn <= 0) {
		return 0;
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	if (emit_parm->n_name_components > 0) {
		_uls_log_(sysprn)("package ");

		for (n_tabs=0; n_tabs<emit_parm->n_name_components; n_tabs++) {
			cptr = al[n_tabs]->str;
			if (n_tabs != 0) _uls_log_(sysprn)(".%s", cptr);
			else _uls_log_(sysprn)("%s", cptr);
		}
		_uls_log_(sysprn)(";\n\n");
	}

	n_tabs = 0;
	uls_sysprn_tabs(n_tabs, "public class %s extends %s.UlsLex %c\n", emit_parm->class_name, ns_uls, ch_lbrace);
	++n_tabs;

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(uls, tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	_uls_log_(sysprn)("\n");

	if (emit_parm->fpath_ulc != NULL) {
		uls_sysprn_tabs(n_tabs, "public %s(String ulc_file) %c\n", emit_parm->class_name, ch_lbrace);
		uls_sysprn_tabs(n_tabs+1, "super(ulc_file);\n");
	} else {
		uls_sysprn_tabs(n_tabs, "public %s() %c\n", emit_parm->class_name, ch_lbrace);
		uls_sysprn_tabs(n_tabs+1, "super(\"%s\");\n", base_ulc);
	}

	++n_tabs;
	if (emit_parm->fpath_uld != NULL) {
		uls_sysprn_tabs(n_tabs, "prepareUldMap();\n");
		_uls_log_(sysprn)("\n");

		if ((fin_uld = _uls_tool_(fp_open)(emit_parm->fpath_uld, ULS_FIO_READ)) == NULL) {
			_uls_log(err_log)("can't open file '%s'", emit_parm->fpath_uld);
			return -1;
		}

		print_uld_source(fin_uld, n_tabs, _uls_ref_callback_this(__print_uld_lineproc_3cs));
		_uls_tool_(fp_close)(fin_uld);

		_uls_log_(sysprn)("\n");
		uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
	}
	--n_tabs;
	uls_sysprn_tabs(n_tabs, "}\n\n");

	uls_sysprn_tabs(n_tabs, "protected void finalize() %c\n", ch_lbrace);
	uls_sysprn_tabs(n_tabs, "\tsuper.finalize();\n");
	uls_sysprn_tabs(n_tabs, "}\n");

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "}\n");

	for (--n_tabs; n_tabs >= 0; n_tabs--) {
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_init_parms_emit)(uls_parms_emit_ptr_t emit_parm,
	const char *out_dpath, const char *out_fname, const char *fpath_config,
	const char* ulc_name, const char* class_path, const char *enum_name,
	const char *tok_pfx, int flags)
{
	int i, rc, len_clsnam_dfl;
	_uls_decl_parray_slots_tool_(al, argstr);
	_uls_tool_ptrtype_(argstr) arg0;
	_uls_tool_type_(wrd) wrdx;
	char ch, namebuff[512];

	if (tok_pfx == NULL) tok_pfx = "";

	emit_parm->pathbuff = (char *) _uls_tool_(malloc)(ULS_FILEPATH_MAX + 1);
	emit_parm->fname_buff =  (char *) _uls_tool_(malloc)(ULS_LEXSTR_MAXSIZ + 1);
	emit_parm->ename_buff =  (char *) _uls_tool_(malloc)(ULS_LEXSTR_MAXSIZ + 1);
	emit_parm->n_name_components = -1;

	len_clsnam_dfl = _uls_tool_(strcpy)(emit_parm->fname_buff, ulc_name);
	if (!(flags & ULS_FL_C_GEN)) {
		ch = emit_parm->fname_buff[0];
		emit_parm->fname_buff[0] = _uls_tool_(toupper)(ch);
	}

	_uls_tool_(init_arglst)(uls_ptr(emit_parm->name_components), ULS_CLASS_DEPTH);

	if (class_path == NULL) {
		emit_parm->n_name_components = 0;

		arg0 = _uls_tool_(create_argstr)();
		emit_parm->class_name = _uls_tool_(copy_argstr)(arg0, emit_parm->fname_buff, len_clsnam_dfl);

		_uls_tool_(append_arglst)(uls_ptr(emit_parm->name_components), arg0);

	} else {
		_uls_tool_(strcpy)(namebuff, class_path);
		wrdx.lptr = namebuff;
		if ((rc = __uls_tool_(explode_str)(uls_ptr(wrdx), '.', 1, uls_ptr(emit_parm->name_components))) <= 0) {
			return -1;
		}

		al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
		emit_parm->class_name = al[--rc]->str;
		emit_parm->n_name_components = rc;
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	for (i=0; i<=emit_parm->n_name_components; i++) {
		if (_uls_tool(is_pure_word)(al[i]->str, 0) <= 0) {
			_uls_log(err_log)("class-path '%s' contains null component!", class_path);
			return -1;
		}
	}

	if (enum_name != NULL) {
		if (*enum_name == '\0') {
			i = _uls_tool_(strcpy)(emit_parm->ename_buff, emit_parm->fname_buff);
			_uls_tool_(strcpy)(emit_parm->ename_buff+i, "Token");

			ch = emit_parm->ename_buff[0];
			emit_parm->ename_buff[0] = _uls_tool_(toupper)(ch);
		} else {
			_uls_tool_(strcpy)(emit_parm->ename_buff, enum_name);
		}

		enum_name = emit_parm->ename_buff;

	} else if ((flags & ULS_FL_C_GEN) && class_path != NULL) {
		_uls_tool_(strcpy)(emit_parm->ename_buff, emit_parm->class_name);
		enum_name = emit_parm->ename_buff;
	}

	if (enum_name != NULL) {
		if (_uls_tool(is_pure_word)(enum_name, 1) <= 0) {
			_uls_log(err_log)("%s: invalid enum name!", enum_name);
			return -1;
		}
	}

	if (out_fname == NULL) {
		if (flags & ULS_FL_C_GEN) {
			_uls_tool_(strcpy)(emit_parm->fname_buff + len_clsnam_dfl, "_lex");
		} else {
			_uls_tool_(strcpy)(emit_parm->fname_buff + len_clsnam_dfl, "Lex");
		}

		out_fname = emit_parm->fname_buff;
	}

	if (out_dpath != NULL) {
		emit_parm->len_fpath = _uls_tool_(strcpy)(emit_parm->pathbuff, out_dpath);
		emit_parm->pathbuff[emit_parm->len_fpath++] = ULS_FILEPATH_DELIM;
	} else {
		emit_parm->len_fpath = 0;
	}

	emit_parm->len_fpath += _uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, out_fname);

	if (flags & ULS_FL_CS_GEN) {
		_uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, ".cs");

	} else if (flags & ULS_FL_JAVA_GEN) {
		_uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, ".java");

	} else {
		_uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, ".h");
	}

	emit_parm->fpath = emit_parm->pathbuff;
	emit_parm->flags = flags;
	emit_parm->out_dpath = out_dpath;
	emit_parm->out_fname = out_fname;

	if (flags & ULS_FL_ULD_FILE) {
		emit_parm->fpath_ulc = NULL;
		emit_parm->fpath_uld = fpath_config;
	} else {
		emit_parm->fpath_ulc = fpath_config;
		emit_parm->fpath_uld = NULL;
	}

	emit_parm->ulc_name = ulc_name;
	emit_parm->class_path = class_path;
	emit_parm->enum_name = enum_name;
	emit_parm->tok_pfx = tok_pfx;

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_deinit_parms_emit)(uls_parms_emit_ptr_t emit_parm)
{
	uls_mfree(emit_parm->pathbuff);
	uls_mfree(emit_parm->fname_buff);
	uls_mfree(emit_parm->ename_buff);

	if (emit_parm->n_name_components >= 0) {
		_uls_tool_(deinit_arglst)(uls_ptr(emit_parm->name_components));
	}

	return 0;
}

ULS_DLL_EXTERN int
ULS_QUALIFIED_METHOD(uls_generate_tokdef_file)(uls_lex_ptr_t uls, uls_parms_emit_ptr_t emit_parm)
{
	int stat = 0;
	uls_decl_parray_slots_init(slots_vx, tokdef_vx, uls_ptr(uls->tokdef_vx_array));
	FILE     *fout;

	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;

	int   i, n_tokdef_ary_prn, n_alloc_tokdef_ary_prn;
	uls_decl_parray(tokdef_ary_prn, tokdef_vx);
	uls_decl_parray_slots(slots_prn, tokdef_vx);

	n_alloc_tokdef_ary_prn = uls->tokdef_vx_array.n;
	uls_init_parray(uls_ptr(tokdef_ary_prn), tokdef_vx, n_alloc_tokdef_ary_prn);
	slots_prn = uls_parray_slots(uls_ptr(tokdef_ary_prn));
	n_tokdef_ary_prn = 0;

	for (i=0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];

		if (_uls_get_namebuf_value(e_vx->name)[0] == '\0') {
			continue;
		}

		if ((e = e_vx->base) == nilptr || filter_to_print_tokdef(uls, e, emit_parm->flags)) {
				slots_prn[n_tokdef_ary_prn++] = e_vx;
		}
	}

	_uls_quicksort_vptr(slots_prn, n_tokdef_ary_prn, comp_by_tokid_vx);

	if ((fout = _uls_tool_(fp_open)(emit_parm->fpath, ULS_FIO_CREAT|ULS_FIO_WRITE)) == NULL) {
		_uls_log(err_log)("%s: fail to create file '%s'", __FUNCTION__, emit_parm->fpath);
		uls_deinit_parray(uls_ptr(tokdef_ary_prn));
		stat = -1;

	} else {
		if (emit_parm->flags & ULS_FL_VERBOSE)
			_uls_log(err_log)("Writing the class definition of '%s' to %s, ...",
				emit_parm->class_name, emit_parm->fpath);

		if (_uls_log_(sysprn_open)(fout, nilptr) < 0) {
			_uls_log(err_log)("%s: create an output file", __FUNCTION__);
			stat = -1;

		} else {
			emit_source_head(emit_parm->ulc_name);

			if (emit_parm->flags & ULS_FL_C_GEN) {
				print_tokdef_c_header(uls, uls_ptr(tokdef_ary_prn), n_tokdef_ary_prn, emit_parm);

			} else if (emit_parm->flags & (ULS_FL_CPP_GEN|ULS_FL_CPPCLI_GEN)) { // C++ class
				print_tokdef_cpp_header(uls, uls_ptr(tokdef_ary_prn), n_tokdef_ary_prn, emit_parm,
				_uls_get_namebuf_value(uls->ulc_name));

			} else if (emit_parm->flags & ULS_FL_CS_GEN) { // C# class
				print_tokdef_cs(uls, uls_ptr(tokdef_ary_prn), n_tokdef_ary_prn, emit_parm,
					_uls_get_namebuf_value(uls->ulc_name));

			} else if (emit_parm->flags & ULS_FL_JAVA_GEN) { // Java class
				print_tokdef_java(uls, uls_ptr(tokdef_ary_prn), n_tokdef_ary_prn, emit_parm,
					_uls_get_namebuf_value(uls->ulc_name));
			}

			_uls_log_(sysprn_close)();
		}

		_uls_tool_(fp_close)(fout);
	}

	uls_deinit_parray(uls_ptr(tokdef_ary_prn));

	if (stat < 0 || (emit_parm->flags & ULS_FL_WANT_WRAPPER) == 0) {
		return stat;
	}

	if (emit_parm->flags & ULS_FL_C_GEN) {
		_uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, ".c");
	} else if (emit_parm->flags & (ULS_FL_CPP_GEN | ULS_FL_CPPCLI_GEN)) {
		_uls_tool_(strcpy)(emit_parm->pathbuff + emit_parm->len_fpath, ".cpp");
	}
	emit_parm->fpath = emit_parm->pathbuff;

	if ((fout = _uls_tool_(fp_open)(emit_parm->fpath, ULS_FIO_CREAT|ULS_FIO_WRITE)) == NULL) {
		_uls_log(err_log)("%s: fail to create file '%s'", __FUNCTION__, emit_parm->fpath);
		stat = -1;

	} else {
		if (_uls_log_(sysprn_open)(fout, nilptr) < 0) {
			_uls_log(err_log)("%s: create an output file", __FUNCTION__);
			stat = -1;

		} else {
			if (emit_parm->flags & ULS_FL_VERBOSE)
				_uls_log(err_log)("Writing the class implementation of '%s' to %s...",
					emit_parm->class_name, emit_parm->fpath);

			emit_source_head(emit_parm->ulc_name);

			if (emit_parm->flags & ULS_FL_C_GEN) {
				print_tokdef_c_source(emit_parm, _uls_get_namebuf_value(uls->ulc_name), 2);
			} else if (emit_parm->flags & (ULS_FL_CPP_GEN | ULS_FL_CPPCLI_GEN)) {
				print_tokdef_cpp_source(uls, uls_ptr(tokdef_ary_prn), n_tokdef_ary_prn, emit_parm,
					_uls_get_namebuf_value(uls->ulc_name));
			}

			_uls_log_(sysprn_close)();
		}

		_uls_tool_(fp_close)(fout);
	}

	return stat;
}
