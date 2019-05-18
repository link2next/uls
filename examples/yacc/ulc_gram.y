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
 * ulc_gram.y -- generating xml-version of ulc-file reading the structure from it --
 *     written by Stanley Hong (link2next@gmail.com), 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
%{

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "ulc_gram.h"
#include "ulc2xml.h"

extern char *yytext;
extern int currentline(void);
extern int firstfile(char *filename);

void yyerror(const char *);
extern int yylex(void);
int yyparse(void);

%}

%union {
	char   *s_val;
	int	    i_val;
	struct _ulc_wordlist *wrdlst;
}

%token TOK_BLKSEP 258 TOK_TRUE 259 TOK_FALSE 260
%token ATTR_WANT_LF_TOK 261 ATTR_WANT_TAB_TOK 262 ATTR_WANT_EOF_TOK 263
%token ATTR_CASE_SENSITIVE 264 ATTR_COMMENT_TYPE 265 ATTR_QUOTE_TYPE 266
%token ATTR_ID_FIRST_CHARS 267 ATTR_ID_CHARS 268 ATTR_RENAME 269
%token ATTR_FILE_VERSION 270
%token TOK_ERR 273 TOK_EOI 274 TOK_EOF 275
%token<i_val> TOK_NUM 276
%token<s_val> TOK_ID  277 TOK_WORD 278

%type<s_val> ulc_word
%type<i_val> ulc_bool
%type<wrdlst> ulc_word_list

%%

ulc_file:
	ulc_header TOK_BLKSEP '\n' ulc_body
	;

ulc_header: attr_list
	;

attr_list :
	/* empty */ |
	attr_list ulc_attr '\n'
	;

ulc_word :
	TOK_ID {
		$$ = $1;
	} |
	TOK_WORD {
		$$ = $1;
	}
	;

ulc_word_list :
	/* empty */  {
		$$ = NULL;
	} |
	ulc_word_list ulc_word {
		ulc_wordlist_t *e;
		e = new_wordlist_element($2);
		e->next = $1;
		$$ = e;
	}
	;

ulc_bool:
	TOK_FALSE {
		$$ = 0;
	} |
	TOK_TRUE {
		$$ = 1;
	}
	;

ulc_attr:
	/* empty */ |
	ATTR_WANT_LF_TOK {
		ulc_file.flags |= ULC2XML_WANT_LF_CHAR;
	} |
	ATTR_WANT_TAB_TOK {
		ulc_file.flags |= ULC2XML_WANT_TAB_CHAR;
	} |
	ATTR_WANT_EOF_TOK {
		ulc_file.flags |= ULC2XML_WANT_EOF_TOK;
	} |
	ATTR_CASE_SENSITIVE ':' ulc_bool {
		if ($3) {
			ulc_file.flags &= ~ULC2XML_CASE_INSENSITIVE;
		} else {
			ulc_file.flags |= ULC2XML_CASE_INSENSITIVE;
		}
	} |
	ATTR_COMMENT_TYPE ':' TOK_WORD TOK_WORD {
		ulc_commtype_t  *cmt;

		if (ulc_file.n_commtypes >= ULS_N_MAX_QUOTETYPES)
			yyerror("Too many comment types");

		if (strlen($3) > ULS_COMM_MARK_MAXSIZ || strlen($4) > ULS_COMM_MARK_MAXSIZ)
			yyerror("Too long comment types");

		cmt = ulc_file.commtypes + ulc_file.n_commtypes;
		strcpy(cmt->start_mark, $3);
		strcpy(cmt->end_mark, $4);
		++ulc_file.n_commtypes;
		free($3);
		free($4);
	} |
	ATTR_QUOTE_TYPE ':' TOK_WORD TOK_WORD {
		ulc_quotetype_t  *qmt;

		if (ulc_file.n_quotetypes >= ULS_N_MAX_QUOTETYPES)
			yyerror("Too many quote types");

		if (strlen($3) > ULS_QUOTE_MARK_MAXSIZ || strlen($4) > ULS_QUOTE_MARK_MAXSIZ)
			yyerror("Too long quote types");

		qmt = ulc_file.quotetypes + ulc_file.n_quotetypes;
		strcpy(qmt->start_mark, $3);
		strcpy(qmt->end_mark, $4);
		++ulc_file.n_quotetypes;
		free($3);
		free($4);
	} |
	ATTR_ID_FIRST_CHARS ':' ulc_word_list {
		ulc_wordlist_t *wrdlst=$3, *e;
		ulc_id_range_t *id_range;
		char *parm;

		for (e=wrdlst; e!=NULL; e=e->next) {
			parm = e->wordtext;

			if (ulc_file.n_idfirst_charsets >= ULS_N_CHARSET_RANGES)
				yyerror("overflow of idfirst ranges");

			id_range = ulc_file.idfirst_charsets + ulc_file.n_idfirst_charsets;
	
			/* TESTCASE: $_. 120 0x221-226 */
			if (isdigit(*parm)) {
				id_range->type = ULC_ID_RANGE_TYPE_RANGE;
				strcpy(id_range->charset, parm);

			} else {
				id_range->type = ULC_ID_RANGE_TYPE_CHARSET;
				strcpy(id_range->charset, parm);
			}

			++ulc_file.n_idfirst_charsets;
		}
		
		release_wordlist(wrdlst);
	} |
	ATTR_ID_CHARS  ':' ulc_word_list {
		ulc_wordlist_t *wrdlst=$3, *e;
		ulc_id_range_t *id_range;
		char *parm;

		for (e=wrdlst; e!=NULL; e=e->next) {
			parm = e->wordtext;

			if (ulc_file.n_id_charsets >= ULS_N_CHARSET_RANGES)
				yyerror("overflow of id ranges");

			id_range = ulc_file.id_charsets + ulc_file.n_id_charsets;

			/* TESTCASE: $_. 120 0x221-226 */
			if (isdigit(*parm)) {
				id_range->type = ULC_ID_RANGE_TYPE_RANGE;
				strcpy(id_range->charset, parm);

			} else {
				id_range->type = ULC_ID_RANGE_TYPE_CHARSET;
				strcpy(id_range->charset, parm);
			}

			++ulc_file.n_id_charsets;
		}
		
		release_wordlist(wrdlst);
	} |
	ATTR_RENAME ':' TOK_ID TOK_ID {
		free($3);
		free($4);
	} | ATTR_FILE_VERSION ':' TOK_NUM  {
		sprintf(ulc_file.filever, "%u", $3);
	} |
	error
	;

ulc_body : statement_list;

statement_list:
	/* empty */ |
	statement_list statement '\n'
	;

statement:
	/* empty */ |
	TOK_ID ulc_word {
		new_tokdef($1, $2, ulc_file.tokid_seqnum++);
	} |
	TOK_ID ulc_word TOK_NUM {
		new_tokdef($1, $2, $3);
		ulc_file.tokid_seqnum = $3 + 1;
	}
	;

%%

void
yyerror(const char *s)
{
	fprintf(stderr, "%s at %d", s, currentline());
	exit(1);
}
