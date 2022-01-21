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
 * ulc2xml.h -- generating xml-version of ulc-file reading the structure from it --
 *     written by Stanley Hong <link2next@gmail.com>, 2011.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */

#ifndef __ULC2XML_H__
#define __ULC2XML_H__

#include <stdio.h>
#include <uls/uls_const.h>

#define ULS_N_CHARSET_RANGES      8

#define ULC2XML_WANT_EOF_TOK      0x0002
#define ULC2XML_CASE_INSENSITIVE  0x0010
#define ULC2XML_WANT_LF_CHAR      0x0020
#define ULC2XML_WANT_TAB_CHAR     0x0040

typedef struct _ulc_wordlist ulc_wordlist_t;
struct _ulc_wordlist {
	char *wordtext;
	struct _ulc_wordlist *next;
};

typedef struct _ulc_commtype ulc_commtype_t;
struct _ulc_commtype {
	char start_mark[ULS_COMM_MARK_MAXSIZ+1];
	char end_mark[ULS_COMM_MARK_MAXSIZ+1];
};

typedef struct _ulc_quotetype ulc_quotetype_t;
struct _ulc_quotetype {
	int  tok_id;
	char start_mark[ULS_QUOTE_MARK_MAXSIZ+1];
	char end_mark[ULS_QUOTE_MARK_MAXSIZ+1];
};

typedef struct _ulc_token_def ulc_token_def_t;
struct _ulc_token_def {
	int  tok_id;
	char *name;
	char *keyword;
};

#define ULC_ID_RANGE_TYPE_CHARSET  0
#define ULC_ID_RANGE_TYPE_RANGE    1

typedef struct _ulc_id_range ulc_id_range_t;
struct _ulc_id_range {
	int  type;
	char charset[ULS_LEXSTR_MAXSIZ+1];
};

typedef struct _ulc_file ulc_file_t;
struct _ulc_file {
	char filever[16];
	unsigned int flags;

	ulc_commtype_t commtypes[ULS_N_MAX_COMMTYPES];
	int n_commtypes;

	ulc_quotetype_t quotetypes[ULS_N_MAX_COMMTYPES];
	int n_quotetypes;

	ulc_id_range_t idfirst_charsets[ULS_N_CHARSET_RANGES];
	int n_idfirst_charsets;

	ulc_id_range_t id_charsets[ULS_N_CHARSET_RANGES];
	int n_id_charsets;

	ulc_token_def_t  *tokdef_list;
	int  n_tokdef_list, n_alloc_tokdef_list;
	int  tokid_seqnum;
};

extern ulc_file_t ulc_file;

extern ulc_wordlist_t* new_wordlist_element(char *text);
extern void release_wordlist(ulc_wordlist_t* wrdlst);

extern ulc_token_def_t* new_tokdef(char *name, char* keyw, int tok_id);
extern void release_tokdef(ulc_token_def_t *e);

extern void release_tokdef_list(void);

extern void init_ulc_file(void);
extern void deinit_ulc_file(void);

extern void dump_ulc_file(ulc_file_t *ulc, FILE* fout);

#endif
