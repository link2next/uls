/**
 * This file automatically has been generated by ULS and
 *  contains an implementation of lexical analyzer
 *  for parsing the programming language 'tokseq'.
 *
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
#ifndef __TOKSEQ_LEX_H__
#define __TOKSEQ_LEX_H__

#include <uls/uls_conf.h>

#define         TOK_NONE  -7
#define         TOK_LINK  -6
#define         TOK_TMPL  -5
#define      TOK_LINENUM  -4
#define       TOK_NUMBER  -3
#define          TOK_EOF  -1
#define          TOK_EOI  128
#define          TOK_ERR  129

#define           TOK_ID  130
#define        TOK_IDENT  130
#define     TOK_ID_ALIAS  130

extern int uls_init_tokseq(uls_lex_ptr_t uls);
extern uls_lex_ptr_t uls_create_tokseq(void);
extern int uls_destroy_tokseq(uls_lex_ptr_t uls);

#endif

