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
  <file> uls_const.h </file>
  <brief>
    A common file for all the ULS sources.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, August 2011.
  </author>
*/

#ifndef __ULS_CONST_H__
#define __ULS_CONST_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_config.h"
#endif

#define TMP_SYSPROPS_FNAME   "uls_sysprops.txt"
#define TMP_ID_RANGES_FNAME  "uls_id_ranges.txt"
#define TMP_LANGS_FNAME      "uls_langs.txt"
#define ULS_ULCNAME_DELIM    '/'

#define ULS_VERSION_STR      "1.9.0"
#define ULC2CLASS_PROGVER    "v2.7.0"
#define ULF_GEN_PROGVER      "v1.6.0"
#define ULS_STREAM_PROGVER   "v2.6.0"

#define ULS_INITIAL_NAME     "ULS(Unified Lexical Scheme)"
#define ULS_AUTHOR           "Stanley Hong (link2next@gmail.com)"
#define ULS_GREETING         ULS_INITIAL_NAME " " ULS_VERSION_STR ", written by " ULS_AUTHOR "."
#define ULS_URL              "https://sourceforge.net/projects/uls"
#define ULS_LICENSE_NAME     "The MIT License (MIT)"
#define ULS_COPYRIGHT_YEAR_START    2011
#define ULS_COPYRIGHT_YEAR_CURRENT  2024

#ifdef ULS_WINDOWS
#define ULS_REG_HOME         L"HKLM:SOFTWARE\\UlsWin"
#define ULS_REG_INSTDIR_NAME L"UlsHome"
#define ULS_SYSPROPS_FNAME   TMP_SYSPROPS_FNAME
#define ULS_ID_RANGES_FNAME  TMP_ID_RANGES_FNAME
#define ULS_LANGS_FNAME      TMP_LANGS_FNAME
#define ULS_FILEPATH_DELIM   '\\'
#define ULS_FILEPATH_DELIM_WCH L'\\'
#define ULS_DIRLIST_DELIM    ';'
#define ULS_DIRLIST_DELIM_WCH L';'
#define ULS_OS_TEMP_DIR      "C:\\Windows\\Temp"
#define ULS_SHARE_DFLDIR     "C:\\Program Files\\Common Files\\UlsWin"
#define ULS_ETC_DIR          "C:\\Program Files\\Common Files\\UlsWin"
#define ULS_SYSPROPS_FPATH  ULS_ETC_DIR "\\" ULS_SYSPROPS_FNAME
#define ULS_OS_SHARE_DFLDIR ULS_SHARE_DFLDIR
#else
#define ULS_SYSPROPS_FNAME   "uls.sysprops"
#define ULS_ID_RANGES_FNAME  "uls.id_ranges"
#define ULS_LANGS_FNAME      "uls.langs"
#define ULS_FILEPATH_DELIM   '/'
#define ULS_FILEPATH_DELIM_WCH L'/'
#define ULS_DIRLIST_DELIM    ':'
#define ULS_DIRLIST_DELIM_WCH L':'
#define ULS_OS_TEMP_DIR      "/tmp"
#define ULS_SHARE_DFLDIR     "/usr/share/uls"
#define ULS_ETC_DFLDIR       "/usr/local/etc/uls"
#ifdef _ULS_IMPLDLL
#ifndef ULS_ETC_DIR
#error "ULS_ETC_DIR: not defined!"
#endif
#define ULS_SYSPROPS_FPATH  ULS_ETC_DIR "/" ULS_SYSPROPS_FNAME
#endif // _ULS_IMPLDLL
#define ULS_OS_SHARE_DFLDIR "/usr/local/share:/usr/share"
#endif

#define ULC_REPO_DOMAIN "uls.link2next.io://season-1"

// # of maximum value of 'int'
#define N_MAX_DIGITS_INT       20

#define ULS_BIG_ENDIAN         0
#define ULS_LITTLE_ENDIAN      1

#define ULS_IEEE754_BINARY16   0
#define ULS_IEEE754_BINARY32   1
#define ULS_IEEE754_BINARY64   2
#define ULS_IEEE754_BINARY80   3
#define ULS_IEEE754_BINARY128  4

#define ULS_FL_STATIC  0x80000000
#define ULS_FL_NOCLEAR 0x40000000
#define ULS_FL_VERBOSE 0x20000000

#define EOI_TOK_IDX       0
#define EOF_TOK_IDX       1
#define ID_TOK_IDX        2
#define NUM_TOK_IDX       3
#define LINENUM_TOK_IDX   4
#define TMPL_TOK_IDX      5
#define LINK_TOK_IDX      6
#define NONE_TOK_IDX      7
#define ERR_TOK_IDX       8
#define N_RESERVED_TOKS   9

#define ULS_COMM_MARK_MAXSIZ       30
#define ULS_N_MAX_COMMTYPES        4
#define ULS_QUOTE_MARK_MAXSIZ      7
#define ULS_N_MAX_QUOTETYPES       8

#define N_ULC_SEARCH_PATHS         8
#define ULS_MAGICCODE_SIZE         80

#define ULS_LINEBUFF_SIZ           255
#define ULS_FILEPATH_MAX           1023

#define ULS_LINEBUFF_SIZ__ULC      255
#define ULS_LINEBUFF_SIZ__ULD      127
#define ULS_LINEBUFF_SIZ__ULF      127
#define ULS_LINEBUFF_SIZ__ULS      127

#define TOKDEF_LINES_DELTA         64
#define TOKDEF_LINES_DELTA2        64

#define ULS_LEXSEGS_MAX            63
#define ULS_FDBUF_INITSIZE         512
#define ULS_FDBUF_SIZE             1024

#define ULS_N_ONECHAR_TOKGRPS      4

#define ULS_TXT_RECHDR_SZ          32
#define ULS_BIN_BLKSIZ_LOG2        9
#define ULS_BIN_BLKSIZ             (1<<ULS_BIN_BLKSIZ_LOG2)
#define ULS_BIN_HDR_SZ             512
#define ULS_BIN_RECHDR_NUM_INT32   2
#define ULS_BIN_RECHDR_SZ (ULS_BIN_RECHDR_NUM_INT32*sizeof(uls_int32))
#define ULS_BIN_REC_SZ(txtlen) (ULS_BIN_RECHDR_SZ+uls_roundup((txtlen)+1,4))

#define ULC_N_LEXATTRS             13
#define ULS_STREAM_SUBNAME_SIZE    31
#define ULS_STREAM_CTIME_SIZE      20

#define ULC_COLUMNS_MAXSIZ         16
#define ULC_LONGNAME_MAXSIZ        63

#define ULS_FDBUF_INITSIZE_STREAM ((ULS_FDBUF_INITSIZE)<<1)
#define ULS_FDZBUF_INITSIZE        (3*ULS_FDBUF_SIZE/2)

#define ULS_UNGETS_BUFSIZ          256
#define ULS_LEXSTR_MAXSIZ          31
#define ULS_NAME_MAXSIZ            31

#define ULS_CARDINAL_TOKNAM_SIZ    15
#define ULS_CARDINAL_LXMPFX_MAXSIZ 15

#define ULS_KWTABLE_TWOPLUS_SIZE   14
#define ULS_TWOPLUS_MAXLEN         (ULS_KWTABLE_TWOPLUS_SIZE+1)

#define ULS_CNST_NILSTR_SIZE       4
#define ULS_SYNTAX_TABLE_SIZE      128
#define ULS_TOKTOWER_DFLSIZ        16
#define ULF_HASH_TABLE_SIZE        111

#define ULC_VERSION_MAJOR          2
#define ULC_VERSION_MINOR          3
#define ULC_VERSION_DEBUG          0

#define ULS_VERSION_STREAM_MAJOR   2
#define ULS_VERSION_STREAM_MINOR   2
#define ULS_VERSION_STREAM_DEBUG   0

#define ULF_VERSION_MAJOR          2
#define ULF_VERSION_MINOR          1
#define ULF_VERSION_DEBUG          0

#define ULF_VERSION_HASHFUNC_MAJOR 1
#define ULF_VERSION_HASHFUNC_MINOR 0
#define ULF_VERSION_HASHFUNC_DEBUG 0

#define ULS_HASH_ALGORITHM "ULF-HASH"

// '-', '.', 0 1 2 3 4 5 6 7 8 9
#define ULS_CH_DIGIT   0x01

/* one-char token candidate */
#define ULS_CH_1       0x02

/* the tokens consiting of multi-chars. */
#define ULS_CH_2PLUS   0x04

/* The char-number will be used for literal-string token. */
#define ULS_CH_QUOTE   0x08

/* The char having this flag may be the first char of ID */
#define ULS_CH_IDFIRST 0x10

/* The char having this flag may be a char of ID */
#define ULS_CH_ID      0x20

/* The char-number was reserverd as token-id. */
#define ULS_CH_COMM    0x80

#define ULS_FALSE       0
#define ULS_TRUE        1

#define ULS_FAIL        0
#define ULS_SUCCESS     1

// Encoding
#define ULS_UTF8_CH_MAXLEN    4

#define ULS_MBCS_UNKNOWN  -1
#define ULS_MBCS_UTF8      0
#define ULS_MBCS_MS_MBCS   1

#define UTF8_CODEPOINT_END          0x1FFFFF
#define UTF16_CODEPOINT_END         0x10FFFF
#define UTF16_CODEPOINT_RSVD_START  0xD800
#define UTF16_CODEPOINT_RSVD_END    0xDFFF
#define UTF32_CODEPOINT_END         UTF8_CODEPOINT_END

#define DOUBLE_EXPO_SIZE_BITS  11
#define DOUBLE_EXPO_BIAS      1023

#define DOUBLE_SIZE_BYTES  SIZEOF_DOUBLE
#define DOUBLE_SIZE_BITS   (DOUBLE_SIZE_BYTES*8)
#define DOUBLE_MENTISA_STARTBIT  (1+DOUBLE_EXPO_SIZE_BITS)

#define ULS_DECIMAL_SEPARATOR_DFL  0x01
#define ULS_MAXLEN_NUMBER_PREFIX  7
#define ULS_N_MAX_NUMBER_PREFIXES 8

#define ULS_N_BOXLST 3
#define ULS_N_SYSPROPS  16

#ifndef ULS_WINDOWS
#include "uls_const_linux.h"
#endif

#endif // __ULS_CONST_H__
