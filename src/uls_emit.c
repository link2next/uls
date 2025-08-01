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
 * uls_emit.c -- generating the output file of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, March 2014.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_EMIT__
#include "uls/uls_emit.h"
#include "uls/uls_core.h"
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
			_uls_log_(sysprn)("%s\n", prefix_stmt);
		}
	}
	_uls_log_(sysprn)(" */\n");
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(comp_by_tokid_vx)(uls_const_voidptr_t a, uls_const_voidptr_t b)
{
	const uls_tokdef_vx_ptr_t e1_vx = (const uls_tokdef_vx_ptr_t) a;
	const uls_tokdef_vx_ptr_t e2_vx = (const uls_tokdef_vx_ptr_t) b;
	return e1_vx->tok_id - e2_vx->tok_id;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(filter_to_print_tokdef)(uls_lex_ptr_t uls, uls_tokdef_ptr_t e, int flags)
{
	uls_tokdef_vx_ptr_t e_vx = e->view;
	int filtered = 0;

	if (e_vx->tok_id == uls->xcontext.toknum_LINENUM || uls_get_namebuf_value(e_vx->name)[0] == '\0') {
		filtered = 1; // filtered

	} else if (is_reserved_tok(uls, uls_get_namebuf_value(e_vx->name)) >= 0) {
		if ((flags & ULS_FL_WANT_RESERVED_TOKS) == 0) filtered = 1;

	} else if (e->keyw_type == ULS_KEYW_TYPE_LITERAL) {
		if ((flags & ULS_FL_WANT_QUOTE_TOKS) == 0) filtered = 1;

	} else {
		if ((flags & ULS_FL_WANT_REGULAR_TOKS) == 0) filtered = 1;
	}

	if (filtered) {
//		_uls_log(err_log)("'%s' filtered for printing header file.", e->view->name);
	}

	return filtered;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(print_tokdef_constants)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char *tok_pfx, int flags)
{
	uls_tokdef_vx_ptr_t e0_vx;
	uls_tokdef_name_ptr_t e_nam;
	char toknam_str[2*ULS_TOKNAM_MAXSIZ+1];
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

		_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, uls_get_namebuf_value(e0_vx->name));
		uls_sysprn_tabs(n_tabs, "%s %16s %s", cnst_symbol, toknam_str, asgn_symbol);
		_uls_log_(sysprn)(" %d%s\n", e0_vx->tok_id, end_symbol);

		for (e_nam = e0_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->next) {
			_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, uls_get_namebuf_value(e_nam->name));
			uls_sysprn_tabs(n_tabs, "%s %16s %s", cnst_symbol, toknam_str, asgn_symbol);
			_uls_log_(sysprn)(" %d%s\n", e0_vx->tok_id, end_symbol);
		}
	}
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(print_tokdef_enum_constants)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	int n_tabs, const char *enum_name, const char *tok_pfx, int flags)
{
	char toknam_str[2*ULS_TOKNAM_MAXSIZ+1];
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

		_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, uls_get_namebuf_value(e0_vx->name));

		sect_lead = (e0_vx->tok_id % 10 == 0);
		if (sect_lead || prev_tok_id + 1 != e0_vx->tok_id) {
			uls_sysprn_tabs(n_tabs + 1, "%s = %d", toknam_str, e0_vx->tok_id);
		} else {
			uls_sysprn_tabs(n_tabs + 1, "%s", toknam_str);
		}

		if (last_one && e0_vx->tokdef_names == nilptr) {
			_uls_log_(sysprn)("\n");
		} else {
			_uls_log_(sysprn)(",\n");
		}

		for (e_nam = e0_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->next) {
			_uls_log_(snprintf)(toknam_str, sizeof(toknam_str), "%s%s", tok_pfx, uls_get_namebuf_value(e_nam->name));
			uls_sysprn_tabs(n_tabs + 1, "%s = %d", toknam_str, e0_vx->tok_id);
			if (last_one && e_nam->next == nilptr) {
				_uls_log_(sysprn)("\n");
			} else {
				_uls_log_(sysprn)(",\n");
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
ULS_QUALIFIED_METHOD(__print_uld_lineproc_2)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	const char *cptr;
	const char *qmark;

	uls_sysprn_tabs(n_tabs, "tok_names.name = \"%s\";\n", tok_names->name);

	if ((cptr = tok_names->name2) == NULL) {
		qmark = "";
		cptr = "NULL";
	} else {
		qmark = "\"";
	}
	uls_sysprn_tabs(n_tabs, "tok_names.name2 = %s%s", qmark, cptr);
	_uls_log_(sysprn)("%s;\n", qmark);

	uls_sysprn_tabs(n_tabs, "tok_names.tokid_changed = %d;\n", tok_names->tokid_changed);
	uls_sysprn_tabs(n_tabs, "tok_names.tokid = %d;\n", tok_names->tokid);

	if ((cptr = tok_names->aliases) == NULL) {
		qmark = "";
		cptr = "NULL";
	} else {
		qmark = "\"";
		cptr = _uls_tool(skip_blanks)(cptr);
	}
	uls_sysprn_tabs(n_tabs, "tok_names.aliases = %s%s", qmark, cptr);
	_uls_log_(sysprn)("%s;\n", qmark);

	uls_sysprn_tabs(n_tabs, "uld_change_names(names_map, uls_ptr(tok_names));\n\n");

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3cpp)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	const char *nilmark = "NULL";
	const char *cptr1, *cptr2;
	const char *qmark1a, *qmark1b;
	const char *qmark2a, *qmark2b;

	if ((cptr1 = tok_names->name2) == NULL) {
		qmark1a = qmark1b ="";
		cptr1 = nilmark;
	} else {
		qmark1a = "_T(\"";
		qmark1b = "\")";
	}

	if ((cptr2 = tok_names->aliases) == NULL || *cptr2 == '\0') {
		qmark2a = qmark2b = "";
		cptr2 = nilmark;
	} else {
		qmark2a = "_T(\"";
		qmark2b = "\")";
		cptr2 = _uls_tool(skip_blanks)(cptr2);
	}

	uls_sysprn_tabs(n_tabs, "changeUldNames(_T(\"%s\"), %s", tok_names->name, qmark1a);
	_uls_log_(sysprn)("%s%s", cptr1, qmark1b);
	_uls_log_(sysprn)(", %d, %d", tok_names->tokid_changed, tok_names->tokid);
	_uls_log_(sysprn)(", %s%s", qmark2a, cptr2);
	_uls_log_(sysprn)("%s);\n", qmark2b);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3cs)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	const char *nilmark = "null";
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
		cptr2 = _uls_tool(skip_blanks)(cptr2);
	}

	uls_sysprn_tabs(n_tabs, "changeUldNames(\"%s\", %s", tok_names->name, qmark1);
	_uls_log_(sysprn)("%s%s", cptr1, qmark1);
	_uls_log_(sysprn)(", %d, %d", tok_names->tokid_changed, tok_names->tokid);
	_uls_log_(sysprn)(", %s%s", qmark2, cptr2);
	_uls_log_(sysprn)("%s);\n", qmark2);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_lineproc_3java)(uld_line_ptr_t tok_names, int n_tabs, const char *lptr)
{
	const char *nilmark = "null";
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
		cptr2 = _uls_tool(skip_blanks)(cptr2);
	}

	uls_sysprn_tabs(n_tabs, "changeUldNames(\"%s\", %s", tok_names->name, qmark1);
	_uls_log_(sysprn)("%s%s", cptr1, qmark1);
	_uls_log_(sysprn)(", %d, %d", tok_names->tokid_changed, tok_names->tokid);
	_uls_log_(sysprn)(", %s%s", qmark2, cptr2);
	_uls_log_(sysprn)("%s);\n", qmark2);

	return 0;
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__print_uld_c_source_2_fp)(int n_tabs, FILE *fin_uld)
{
	int   n_tok_names;

	uls_sysprn_tabs(n_tabs, " {\n\tuld_line_t tok_names;\n\n");
	n_tok_names = print_uld_source(fin_uld, n_tabs, uls_ref_callback_this(__print_uld_lineproc_2));
	uls_sysprn_tabs(n_tabs, " }\n");

	return n_tok_names;
}

int
ULS_QUALIFIED_METHOD(print_uld_source)(FILE *fin_uld, int n_tabs, uls_proc_uld_line_t lineproc)
{
	char  linebuff[ULS_LINEBUFF_SIZ__ULD+1], *lptr;
	int   linelen, lno=0, stat=0, n_tok_names=0;
	uld_line_t tok_names;
	uls_type_tool(wrd) wrdx;

	while (1) {
		if ((linelen=_uls_tool_(fp_gets)(fin_uld, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (linelen < ULS_EOF) {
				_uls_log(err_log)("%s: ulc file i/o error at %d", __func__, lno);
				stat = -1;
			} else {
				stat = n_tok_names;
			}
			break;
		}
		++lno;

		linelen = _uls_tool(str_trim_end)(linebuff, linelen);
		if (linebuff[0] == '#' || *(lptr = _uls_tool(skip_blanks)(linebuff)) == '\0')
			continue;

		wrdx.lptr = lptr;
		if (uld_pars_line(uls_ptr(wrdx), uls_ptr(tok_names)) < 0) {
			_uls_log(err_log)("#%d: Failed to parse line in uld-file", lno);
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
ULS_QUALIFIED_METHOD(print_tokdef_c_header)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm)
{
	char  fnameAZ[ULS_FILENAME_MAX+1], ch;
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
		_uls_log_(sysprn)("#include <uls/uls_auw.h>\n\n");
	}

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			0, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(tokdef_ary_prn, n_tokdef_ary_prn,
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
ULS_QUALIFIED_METHOD(__print_tokdef_c_source_fp)(FILE *fin_uld)
{
	static char *load_pairs_proc_prolog[] = {
		"static int load_pairs(uls_lex_ptr_t uls)",
		"{",
		"	uld_names_map_ptr_t names_map;",
		"	int   stat = 0;",
		"",
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

	const char *cstr;
	int i, siz_uld_filebuff, rval;

	if ((siz_uld_filebuff = _uls_tool_(fp_filesize)(fin_uld)) < 0) {
		_uls_log(err_log)("invalid uld-file!");
		return -1;
	}
	siz_uld_filebuff = uld_calc_filebuff_size(siz_uld_filebuff);

	for (i=0; (cstr=load_pairs_proc_prolog[i]) != NULL; i++) {
		_uls_log_(sysprn)("%s\n", cstr);
	}

	_uls_log_(sysprn)("	names_map = uld_prepare_names(uls, %d);\n", siz_uld_filebuff);

	if ((rval = __print_uld_c_source_2_fp(1, fin_uld)) < 0) {
		return -1;
	}

	for (i=0; (cstr=load_pairs_proc_epilog[i]) != NULL; i++) {
		_uls_log_(sysprn)("%s\n", cstr);
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(__print_tokdef_c_source_file)(const char *filepath)
{
	FILE   *fin_uld;
	int rval;

	if ((fin_uld = _uls_tool_(fp_open)(filepath, ULS_FIO_READ)) == NULL) {
		_uls_log(err_log)("can't open file '%s'", filepath);
		return -1;
	}

	_uls_log_(sysprn)("#include <uls/uld_conf.h>\n\n");
	rval = __print_tokdef_c_source_fp(fin_uld);
	_uls_tool_(fp_close)(fin_uld);

	return rval;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_c_source)(uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	const char *lex_name = emit_parm->class_name;
	const char *config_path;

	_uls_log_(sysprn)("#include <uls/uls_lex.h>\n");
	_uls_log_(sysprn)("#include <uls/uls_auw.h>\n");
	_uls_log_(sysprn)("#include <uls/uls_util.h>\n");

	if (emit_parm->flags & ULS_FL_ULD_FILE) {
		if (__print_tokdef_c_source_file(emit_parm->filepath_conf) < 0) {
			_uls_log(err_log)("fail to write uld-struct to '%s'", emit_parm->filepath_conf);
			return -1;
		}
		config_path = base_ulc;
	} else {
		_uls_log_(sysprn)("\n");
		if ((config_path = emit_parm->filepath_conf) == NULL) {
			config_path = base_ulc;
		}
	}

	_uls_log_(sysprn)("int uls_init_%s(uls_lex_ptr_t uls)\n", lex_name);
	_uls_log_(sysprn)("{\n");
	_uls_log_(sysprn)("	 LPCTSTR confname = _T(\"%s\");\n", config_path);
	_uls_log_(sysprn)("\n");
	_uls_log_(sysprn)("	if (uls_init(uls, confname) <  0) {\n");
	_uls_log_(sysprn)("		return -1;\n");
	_uls_log_(sysprn)("	}\n");
	_uls_log_(sysprn)("\n");

	if (emit_parm->flags & ULS_FL_ULD_FILE) {
		_uls_log_(sysprn)("	if (load_pairs(uls) < 0) {\n");
		_uls_log_(sysprn)("		return -1;\n");
		_uls_log_(sysprn)("	}\n");
		_uls_log_(sysprn)("\n");
	}

	_uls_log_(sysprn)("	return 0;\n");
	_uls_log_(sysprn)("}\n\n");

	_uls_log_(sysprn)("uls_lex_ptr_t uls_create_%s(void)\n", lex_name);
	_uls_log_(sysprn)("{\n");
	_uls_log_(sysprn)("	LPCTSTR confname = _T(\"%s\");\n", config_path);
	_uls_log_(sysprn)("	uls_lex_ptr_t uls;\n");
	_uls_log_(sysprn)("\n");
	_uls_log_(sysprn)("	if ((uls = uls_create(confname)) == NULL) {\n");
	_uls_log_(sysprn)("		return NULL;\n");
	_uls_log_(sysprn)("	}\n");
	_uls_log_(sysprn)("\n");

	if (emit_parm->flags & ULS_FL_ULD_FILE) {
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
ULS_QUALIFIED_METHOD(print_tokdef_cpp_header)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	uls_decl_parray_slots_tool(al, argstr);

	const char *cptr1, *cptr2, *ns_uls;
	char ch_lbrace='{';
	int  n_tabs;

	if (n_tokdef_ary_prn <= 0) {
		return 0;
	}

	_uls_log_(sysprn)("#pragma once\n\n");

	if (flags & ULS_FL_CPPCLI_GEN) {
		ns_uls="uls::polaris";
	} else {
		ns_uls="uls::crux";
		_uls_log_(sysprn)("#include <uls/UlsLex.h>\n");
		_uls_log_(sysprn)("#include <uls/uls_auw.h>\n");
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	for (n_tabs=0; n_tabs<emit_parm->n_name_components; n_tabs++) {
		cptr1 = al[n_tabs]->str;
		uls_sysprn_tabs(n_tabs, "namespace %s %c\n", cptr1, ch_lbrace);
	}

	if (flags & ULS_FL_CPPCLI_GEN) {
		cptr1 = "public ref class";
	} else {
		cptr1 = "class";
	}

	uls_sysprn_tabs(n_tabs, "%s %s :", cptr1, emit_parm->class_name);
	_uls_log_(sysprn)(" public %s::UlsLex %c\n", ns_uls, ch_lbrace);

	uls_sysprn_tabs(n_tabs, "  public:\n");

	++n_tabs;

	if (emit_parm->enum_name != NULL) {
		print_tokdef_enum_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	_uls_log_(sysprn)("\n");

	if (flags & ULS_FL_CPPCLI_GEN) {
		cptr2 = "System::String^";
	} else {
		cptr2 = "uls::tstring";
	}

	if (flags & ULS_FL_WANT_WRAPPER) {
		if (emit_parm->filepath_conf != NULL && !(emit_parm->flags & ULS_FL_ULD_FILE)) {
			if (flags & ULS_FL_CPPCLI_GEN) {
				uls_sysprn_tabs(n_tabs, "%s();\n", emit_parm->class_name);
				uls_sysprn_tabs(n_tabs, "%s(%s filepath);\n", emit_parm->class_name, cptr2);
			} else {
				uls_sysprn_tabs(n_tabs, "%s(%s filepath = _T(\"%s\"));\n",
					emit_parm->class_name, cptr2, emit_parm->ulc_filepath_enc);
			}
		} else {
			uls_sysprn_tabs(n_tabs, "%s();\n", emit_parm->class_name);
		}
	} else {
		if (emit_parm->filepath_conf != NULL) {
			if (flags & ULS_FL_CPPCLI_GEN) {
				uls_sysprn_tabs(n_tabs, "%s()\n", emit_parm->class_name);
				uls_sysprn_tabs(n_tabs+1, " : %s::UlsLex(\"%s\") %c}\n\n", ns_uls, emit_parm->ulc_filepath_enc, ch_lbrace);
				uls_sysprn_tabs(n_tabs, "%s(%s filepath)\n", emit_parm->class_name, cptr2);
				uls_sysprn_tabs(n_tabs+1, " : %s::UlsLex(filepath) %c}\n", ns_uls, ch_lbrace);
			} else {
				uls_sysprn_tabs(n_tabs, "%s(%s filepath = _T(\"%s\"))\n", emit_parm->class_name, cptr2, emit_parm->ulc_filepath_enc);
				uls_sysprn_tabs(n_tabs+1, " : %s::UlsLex(filepath) %c}\n", ns_uls, ch_lbrace);
			}
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
ULS_QUALIFIED_METHOD(print_tokdef_cpp_source)(
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	uls_decl_parray_slots_tool(al, argstr);
	FILE *fin_uld;
	const char *cptr, *cptr2, *ns_uls;
	char ch_lbrace='{';
	int  i, bufsiz_uldfile, n_tabs=0;

	_uls_log_(sysprn)("#include \"%s.h\"\n\n", emit_parm->out_fname);

	if (flags & ULS_FL_CPPCLI_GEN) {
		ns_uls = "uls::polaris";
		cptr2 = "System::String^";
		_uls_log_(sysprn)("using namespace %s;\n\n", ns_uls);
	} else {
		ns_uls = "uls::crux";
		cptr2 = "tstring";
		_uls_log_(sysprn)("using tstring = uls::tstring;\n\n");
	}

	al = uls_parray_slots(uls_ptr(emit_parm->name_components.args));
	for (i=0; i<emit_parm->n_name_components; i++) {
		cptr = al[i]->str;
		_uls_log_(sysprn)("namespace %s %c\n", cptr, ch_lbrace);
	}

	if (emit_parm->n_name_components > 0) ++n_tabs;

	if (emit_parm->filepath_conf != NULL && !(flags & ULS_FL_ULD_FILE)) {
		if (flags & ULS_FL_CPPCLI_GEN) {
			uls_sysprn_tabs(n_tabs, "%s::%s()\n", emit_parm->class_name, emit_parm->class_name);
			uls_sysprn_tabs(n_tabs+1, ": %s::UlsLex(\"%s\") %c}\n\n", ns_uls, emit_parm->ulc_filepath_enc, ch_lbrace);
		}

		uls_sysprn_tabs(n_tabs, "%s::%s(%s filepath)\n", emit_parm->class_name, emit_parm->class_name, cptr2);
		uls_sysprn_tabs(n_tabs+1, ": %s::UlsLex(filepath) %c}\n", ns_uls, ch_lbrace);

	} else {
		uls_sysprn_tabs(n_tabs, "%s::%s()\n", emit_parm->class_name, emit_parm->class_name);
		uls_sysprn_tabs(n_tabs+1, ": %s::UlsLex(_T(\"%s\")) %c\n", ns_uls, base_ulc, ch_lbrace);

		++n_tabs;
		if (emit_parm->flags & ULS_FL_ULD_FILE) {
			if ((fin_uld = _uls_tool_(fp_open)(emit_parm->filepath_conf, ULS_FIO_READ)) == NULL) {
				_uls_log(err_log)("can't open file '%s'", emit_parm->filepath_conf);
				return -1;
			}

			if ((bufsiz_uldfile = _uls_tool_(fp_filesize)(fin_uld)) < 0) {
				_uls_log(err_log)("invalid file: %s", emit_parm->filepath_conf);
				return -1;
			}
			bufsiz_uldfile = uld_calc_filebuff_size(bufsiz_uldfile);

			uls_sysprn_tabs(n_tabs, "prepareUldMap(%d);\n", bufsiz_uldfile);
			_uls_log_(sysprn)("\n");

			print_uld_source(fin_uld, n_tabs, uls_ref_callback_this(__print_uld_lineproc_3cpp));
			_uls_tool_(fp_close)(fin_uld);

			_uls_log_(sysprn)("\n");
			uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
			uls_sysprn_tabs(n_tabs, "update_rsvdtoks();\n");
		}
		--n_tabs;

		uls_sysprn_tabs(n_tabs, "}\n");
	}

	for (i = emit_parm->n_name_components-1; i >= 0; i--) {
		_uls_log_(sysprn)("}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_cs)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	uls_decl_parray_slots_tool(al, argstr);
	FILE *fin_uld;
	const char *cptr, *ns_uls = "uls.polaris";
	char ch_lbrace='{';
	int  bufsiz_uldfile, n_tabs;

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
		print_tokdef_enum_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	uls_sysprn_tabs(0, "\n");

	if (emit_parm->filepath_conf != NULL && !(emit_parm->flags & ULS_FL_ULD_FILE)) {
		uls_sysprn_tabs(n_tabs, "public %s(String filepath = \"%s\")\n",
			emit_parm->class_name, emit_parm->ulc_filepath_enc);
		uls_sysprn_tabs(n_tabs+1, ": base(filepath) %c\n", ch_lbrace);
		uls_sysprn_tabs(n_tabs, "}\n");

	} else {
		uls_sysprn_tabs(n_tabs, "public %s()\n", emit_parm->class_name);
		uls_sysprn_tabs(n_tabs+1, ": base(\"%s\") %c\n", base_ulc, ch_lbrace);

		++n_tabs;
		if (emit_parm->flags & ULS_FL_ULD_FILE) {
			if ((fin_uld = _uls_tool_(fp_open)(emit_parm->filepath_conf, ULS_FIO_READ)) == NULL) {
				_uls_log(err_log)("can't open file '%s'", emit_parm->filepath_conf);
				return -1;
			}

			if ((bufsiz_uldfile = _uls_tool_(fp_filesize)(fin_uld)) < 0) {
				_uls_log(err_log)("invalid file: %s", emit_parm->filepath_conf);
				return -1;
			}
			bufsiz_uldfile = uld_calc_filebuff_size(bufsiz_uldfile);

			uls_sysprn_tabs(n_tabs, "prepareUldMap(%d);\n", bufsiz_uldfile);
			_uls_log_(sysprn)("\n");

			print_uld_source(fin_uld, n_tabs, uls_ref_callback_this(__print_uld_lineproc_3cs));
			_uls_tool_(fp_close)(fin_uld);

			_uls_log_(sysprn)("\n");
			uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
			uls_sysprn_tabs(n_tabs, "update_rsvdtoks();\n");
		}
		--n_tabs;
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "} // End of %s\n", emit_parm->class_name);

	for (--n_tabs; n_tabs >= 0; n_tabs--) {
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(print_tokdef_java)(
	uls_ref_parray(tokdef_ary_prn,tokdef_vx), int n_tokdef_ary_prn,
	uls_parms_emit_ptr_t emit_parm, const char *base_ulc)
{
	uls_flags_t flags = emit_parm->flags;
	uls_decl_parray_slots_tool(al, argstr);
	FILE *fin_uld;
	const char *cptr, *ns_uls = "uls.polaris";
	char ch_lbrace='{';
	int  bufsiz_uldfile, n_tabs;

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
		print_tokdef_enum_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->enum_name, emit_parm->tok_pfx, flags);
	} else {
		print_tokdef_constants(tokdef_ary_prn, n_tokdef_ary_prn,
			n_tabs, emit_parm->tok_pfx, flags);
	}

	_uls_log_(sysprn)("\n");

	if (emit_parm->filepath_conf != NULL && !(emit_parm->flags & ULS_FL_ULD_FILE)) {
		uls_sysprn_tabs(n_tabs, "public %s() %c\n", emit_parm->class_name, ch_lbrace);
		uls_sysprn_tabs(n_tabs+1, "super(\"%s\");\n", emit_parm->ulc_filepath_enc);
		uls_sysprn_tabs(n_tabs, "}\n\n");

		uls_sysprn_tabs(n_tabs, "public %s(String filepath) %c\n", emit_parm->class_name, ch_lbrace);
		uls_sysprn_tabs(n_tabs+1, "super(filepath);\n");
		uls_sysprn_tabs(n_tabs, "}\n");

	} else {
		uls_sysprn_tabs(n_tabs, "public %s() %c\n", emit_parm->class_name, ch_lbrace);
		uls_sysprn_tabs(n_tabs+1, "super(\"%s\");\n", base_ulc);

		++n_tabs;
		if (emit_parm->flags & ULS_FL_ULD_FILE) {
			if ((fin_uld = _uls_tool_(fp_open)(emit_parm->filepath_conf, ULS_FIO_READ)) == NULL) {
				_uls_log(err_log)("can't open file '%s'", emit_parm->filepath_conf);
				return -1;
			}

			if ((bufsiz_uldfile = _uls_tool_(fp_filesize)(fin_uld)) < 0) {
				_uls_log(err_log)("invalid file: %s", emit_parm->filepath_conf);
				return -1;
			}
			bufsiz_uldfile = uld_calc_filebuff_size(bufsiz_uldfile);

			uls_sysprn_tabs(n_tabs, "prepareUldMap(%d);\n", bufsiz_uldfile);
			_uls_log_(sysprn)("\n");

			print_uld_source(fin_uld, n_tabs, uls_ref_callback_this(__print_uld_lineproc_3java));
			_uls_tool_(fp_close)(fin_uld);

			_uls_log_(sysprn)("\n");
			uls_sysprn_tabs(n_tabs, "finishUldMap();\n");
			uls_sysprn_tabs(n_tabs, "update_rsvdtoks();\n");
		}
		--n_tabs;
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	--n_tabs;
	uls_sysprn_tabs(n_tabs, "}\n");

	for (--n_tabs; n_tabs >= 0; n_tabs--) {
		uls_sysprn_tabs(n_tabs, "}\n");
	}

	return 0;
}

int
ULS_QUALIFIED_METHOD(uls_init_parms_emit)(uls_parms_emit_ptr_t emit_parm,
	const char *out_dpath, const char *out_fname,
	const char *filepath_cfg, const char *ulc_name,
	const char *class_path, const char *enum_name,
	const char *tok_pfx, const char *ulc_filepath, int flags)
{
	char *fname_buff, *ename_buff;

	int i, rc, len_clsnam_dfl;
	uls_decl_parray_slots_tool(al, argstr);
	uls_ptrtype_tool(argstr) arg0;
	uls_type_tool(wrd) wrdx;
	char *ptr, ch, namebuff[512];

	emit_parm->out_filepath = (char *) _uls_tool_(malloc)(ULS_FILEPATH_MAX + 1);
	emit_parm->fname_buff = fname_buff = (char *) _uls_tool_(malloc)(ULS_FILENAME_MAX + 1);
	emit_parm->ename_buff = ename_buff = (char *) _uls_tool_(malloc)(ULS_FILENAME_MAX + 1);
	emit_parm->n_name_components = -1;

	len_clsnam_dfl = _uls_tool_(strcpy)(fname_buff, ulc_name);
	if (flags & ULS_FL_C_GEN) {

	} else {
		ch = fname_buff[0];
		fname_buff[0] = _uls_tool_(toupper)(ch);
	}

	if (enum_name != NULL) {
		if (*enum_name == '\0') {
			i = _uls_tool_(strcpy)(ename_buff, fname_buff);
			_uls_tool_(strcpy)(ename_buff+i, "Token");

			ch = ename_buff[0];
			ename_buff[0] = _uls_tool_(toupper)(ch);
		} else {
			_uls_tool_(strcpy)(ename_buff, enum_name);
		}

		enum_name = ename_buff;

	} else if ((flags & ULS_FL_C_GEN) && class_path != NULL) {
		_uls_tool_(strcpy)(ename_buff, emit_parm->class_name);
		enum_name = ename_buff;
	}

	if (enum_name != NULL) {
		if (_uls_tool(is_pure_word)(enum_name, 1) <= 0) {
			_uls_log(err_log)("%s: invalid enum name!", enum_name);
			return -1;
		}
	}

	_uls_tool_(init_arglst)(uls_ptr(emit_parm->name_components), ULS_CLASS_DEPTH);

	if (flags & ULS_FL_C_GEN) {
		emit_parm->class_name = ulc_name;

	} else {
		if (class_path == NULL) {
			emit_parm->n_name_components = 0;

			arg0 = _uls_tool_(create_argstr)();
			emit_parm->class_name = _uls_tool_(copy_argstr)(arg0, fname_buff, len_clsnam_dfl);

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
	}

	if (out_fname == NULL) {
		if (flags & ULS_FL_C_GEN) {
			_uls_tool_(strcpy)(fname_buff + len_clsnam_dfl, "_lex");
		} else {
			_uls_tool_(strcpy)(fname_buff + len_clsnam_dfl, "Lex");
		}

		out_fname = fname_buff;
	}

	if (out_dpath != NULL) {
		emit_parm->len_out_filepath = _uls_tool_(strcpy)(emit_parm->out_filepath, out_dpath);
		emit_parm->out_filepath[emit_parm->len_out_filepath++] = ULS_FILEPATH_DELIM;
	} else {
		emit_parm->len_out_filepath = 0;
	}

	emit_parm->len_out_filepath += _uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, out_fname);

	if (flags & ULS_FL_CS_GEN) {
		_uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, ".cs");

	} else if (flags & ULS_FL_JAVA_GEN) {
		_uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, ".java");

	} else {
		_uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, ".h");
	}

	emit_parm->flags = flags;
	emit_parm->ulc_name = ulc_name;
	emit_parm->filepath_conf = filepath_cfg;

	emit_parm->enum_name = enum_name;
	emit_parm->class_path = class_path;

	emit_parm->out_dpath = out_dpath;
	emit_parm->out_fname = out_fname;

	if (tok_pfx == NULL) tok_pfx = "";
	emit_parm->tok_pfx = tok_pfx;

	if (ulc_filepath == NULL) ulc_filepath = emit_parm->filepath_conf;
	emit_parm->ulc_filepath_enc = _uls_tool_(strdup)(ulc_filepath, -1);
	for (ptr = emit_parm->ulc_filepath_enc; (ch = *ptr) != '\0'; ptr++) {
		if (ch == '\\') *ptr = '/';
	}

	return 0;
}

void
ULS_QUALIFIED_METHOD(uls_deinit_parms_emit)(uls_parms_emit_ptr_t emit_parm)
{
	uls_mfree(emit_parm->out_filepath);
	uls_mfree(emit_parm->ulc_filepath_enc);

	uls_mfree(emit_parm->fname_buff);
	uls_mfree(emit_parm->ename_buff);
	_uls_tool_(deinit_arglst)(uls_ptr(emit_parm->name_components));
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(collect_printable_list_of_tokdef_vx)(uls_lex_ptr_t uls, uls_parms_emit_ptr_t emit_parm,
	uls_ref_parray(tokdef_ary_prn,tokdef_vx))
{
	int i, j, n, n_ary_prn = 0;
	uls_decl_parray_slots(slots_prn, tokdef_vx);
	uls_decl_parray_slots(slots_vx, tokdef_vx);

	uls_onechar_tokdef_etc_ptr_t  e_etc;
	uls_onechar_tokgrp_ptr_t tokgrp;
	uls_tokdef_vx_ptr_t e_vx;
	uls_tokdef_ptr_t e;

	slots_prn = uls_parray_slots(tokdef_ary_prn);
	slots_vx = uls_parray_slots(uls_ptr(uls->tokdef_vx_array));

	for (i = 0; i < uls->tokdef_vx_array.n; i++) {
		e_vx = slots_vx[i];
		if (uls_get_namebuf_value(e_vx->name)[0] != '\0') {
			if ((e = e_vx->base) == nilptr || !filter_to_print_tokdef(uls, e, emit_parm->flags)) {
				slots_prn[n_ary_prn++] = e_vx;
			}
		}
	}

	for (e_etc = uls->onechar_table.tokdefs_etc_list; e_etc != nilptr; e_etc = e_etc->next) {
		e_vx = e_etc->tokdef_vx;

		if ((e_vx->flags & ULS_VX_CHRMAP) && uls_get_namebuf_value(e_vx->name)[0] != '\0') {
			if (n_ary_prn >= tokdef_ary_prn->n_alloc) {
				uls_resize_parray(tokdef_ary_prn, tokdef_vx, n_ary_prn + 64);
				slots_prn = uls_parray_slots(tokdef_ary_prn);
			}
			slots_prn[n_ary_prn++] = e_vx;
		}
	}

	for (i = 0; i < ULS_N_ONECHAR_TOKGRPS; i++) {
		tokgrp = uls_get_array_slot_type00(uls_ptr(uls->onechar_table.tokgrps), i);
		slots_vx = uls_parray_slots(uls_ptr(tokgrp->tokdef_vx_1char));
		n = tokgrp->tokdef_vx_1char.n;

		if (n_ary_prn + n > tokdef_ary_prn->n_alloc) {
			uls_resize_parray(tokdef_ary_prn, tokdef_vx, n_ary_prn + n);
			slots_prn = uls_parray_slots(tokdef_ary_prn);
		}

		for (j = 0; j < n; j++) {
			if ((e_vx = slots_vx[j]) != nilptr) {
				if ((e_vx->flags & ULS_VX_CHRMAP) && uls_get_namebuf_value(e_vx->name)[0] != '\0') {
					slots_prn[n_ary_prn++] = e_vx;
				}
			}
		}
	}

	tokdef_ary_prn->n = n_ary_prn;
	return n_ary_prn;
}

int
ULS_QUALIFIED_METHOD(uls_generate_tokdef_file)(uls_lex_ptr_t uls, uls_parms_emit_ptr_t emit_parm)
{
	int stat = 0;
	uls_decl_parray(tokdef_ary_prn, tokdef_vx);
	uls_decl_parray_slots(slots_prn, tokdef_vx);
	FILE     *fout;

	uls_init_parray(uls_ptr(tokdef_ary_prn), tokdef_vx, 3 * uls->tokdef_vx_array.n);
	collect_printable_list_of_tokdef_vx(uls, emit_parm, uls_ptr(tokdef_ary_prn));
	slots_prn = uls_parray_slots(uls_ptr(tokdef_ary_prn));
	_uls_quicksort_vptr(slots_prn, tokdef_ary_prn.n, comp_by_tokid_vx);

	if ((fout = _uls_tool_(fp_open)(emit_parm->out_filepath, ULS_FIO_WRITE | ULS_FIO_NO_UTF8BOM)) == NULL) {
		_uls_log(err_log)("%s: fail to create file '%s'", __func__, emit_parm->out_filepath);
		uls_deinit_parray(uls_ptr(tokdef_ary_prn));
		stat = -1;

	} else {
		if (!(emit_parm->flags & ULS_FL_SILENT)) {
			_uls_log(err_log)("Writing the class definition of %s to %s ...",
				emit_parm->class_name, emit_parm->out_filepath);
		}

		if (_uls_log_(sysprn_open)(fout, nilptr) < 0) {
			_uls_log(err_log)("%s: create an output file", __func__);
			stat = -1;

		} else {
			emit_source_head(uls_get_namebuf_value(uls->ulc_name));

			if (emit_parm->flags & ULS_FL_C_GEN) {
				print_tokdef_c_header(uls_ptr(tokdef_ary_prn), tokdef_ary_prn.n, emit_parm);
			} else if (emit_parm->flags & (ULS_FL_CPP_GEN|ULS_FL_CPPCLI_GEN)) { // C++ class
				print_tokdef_cpp_header(uls_ptr(tokdef_ary_prn), tokdef_ary_prn.n, emit_parm,
				uls_get_namebuf_value(uls->ulc_name));

			} else if (emit_parm->flags & ULS_FL_CS_GEN) { // C# class
				print_tokdef_cs(uls_ptr(tokdef_ary_prn), tokdef_ary_prn.n, emit_parm,
					uls_get_namebuf_value(uls->ulc_name));

			} else if (emit_parm->flags & ULS_FL_JAVA_GEN) { // Java class
				print_tokdef_java(uls_ptr(tokdef_ary_prn), tokdef_ary_prn.n, emit_parm,
					uls_get_namebuf_value(uls->ulc_name));
			}

			_uls_log_(sysprn_close)();
		}

		_uls_tool_(fp_close)(fout);
	}

	if (stat < 0 || (emit_parm->flags & ULS_FL_WANT_WRAPPER) == 0) {
		uls_deinit_parray(uls_ptr(tokdef_ary_prn));
		return stat;
	}

	if (emit_parm->flags & ULS_FL_C_GEN) {
		_uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, ".c");
	} else if (emit_parm->flags & (ULS_FL_CPP_GEN | ULS_FL_CPPCLI_GEN)) {
		_uls_tool_(strcpy)(emit_parm->out_filepath + emit_parm->len_out_filepath, ".cpp");
	}

	if ((fout = _uls_tool_(fp_open)(emit_parm->out_filepath, ULS_FIO_WRITE | ULS_FIO_NO_UTF8BOM)) == NULL) {
		_uls_log(err_log)("%s: fail to create file '%s'", __func__, emit_parm->out_filepath);
		stat = -1;

	} else {
		if (_uls_log_(sysprn_open)(fout, nilptr) < 0) {
			_uls_log(err_log)("%s: create an output file", __func__);
			stat = -1;

		} else {
			_uls_log(err_log)("Writing the class implementation of '%s' to %s...",
				emit_parm->class_name, emit_parm->out_filepath);

			emit_source_head(uls_get_namebuf_value(uls->ulc_name));

			if (emit_parm->flags & ULS_FL_C_GEN) {
				print_tokdef_c_source(emit_parm, uls_get_namebuf_value(uls->ulc_name));
			} else if (emit_parm->flags & (ULS_FL_CPP_GEN | ULS_FL_CPPCLI_GEN)) {
				if (tokdef_ary_prn.n > 0) {
					print_tokdef_cpp_source(emit_parm, uls_get_namebuf_value(uls->ulc_name));
				}
			}

			_uls_log_(sysprn_close)();
		}

		_uls_tool_(fp_close)(fout);
	}

	uls_deinit_parray(uls_ptr(tokdef_ary_prn));
	return stat;
}
