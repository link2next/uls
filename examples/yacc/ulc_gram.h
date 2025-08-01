/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_BLKSEP = 258,
    TOK_TRUE = 259,
    TOK_FALSE = 260,
    ATTR_WANT_LF_TOK = 261,
    ATTR_WANT_TAB_TOK = 262,
    ATTR_WANT_EOF_TOK = 263,
    ATTR_CASE_SENSITIVE = 264,
    ATTR_COMMENT_TYPE = 265,
    ATTR_QUOTE_TYPE = 266,
    ATTR_ID_FIRST_CHARS = 267,
    ATTR_ID_CHARS = 268,
    ATTR_RENAME = 269,
    ATTR_FILE_VERSION = 270,
    TOK_ERR = 273,
    TOK_EOI = 274,
    TOK_EOF = 275,
    TOK_NUM = 276,
    TOK_ID = 277,
    TOK_WORD = 278
  };
#endif
/* Tokens.  */
#define TOK_BLKSEP 258
#define TOK_TRUE 259
#define TOK_FALSE 260
#define ATTR_WANT_LF_TOK 261
#define ATTR_WANT_TAB_TOK 262
#define ATTR_WANT_EOF_TOK 263
#define ATTR_CASE_SENSITIVE 264
#define ATTR_COMMENT_TYPE 265
#define ATTR_QUOTE_TYPE 266
#define ATTR_ID_FIRST_CHARS 267
#define ATTR_ID_CHARS 268
#define ATTR_RENAME 269
#define ATTR_FILE_VERSION 270
#define TOK_ERR 273
#define TOK_EOI 274
#define TOK_EOF 275
#define TOK_NUM 276
#define TOK_ID 277
#define TOK_WORD 278

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 50 "ulc_gram.y"

	char   *s_val;
	int	    i_val;
	struct _ulc_wordlist *wrdlst;

#line 105 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
