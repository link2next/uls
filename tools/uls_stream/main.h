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

#ifndef __ULS_STREAM_MAIN_H__
#define __ULS_STREAM_MAIN_H__

#include "ult_utils.h"

#ifndef  __ULS_WINDOWS__
#include <sys/types.h>
#include <sys/wait.h>
#endif

#define N_TMPLVAL_ARRAY 64

extern int uls_endian;
extern int  opt_verbose, opt_binary;
extern int  opt_no_numbering;

extern char home_dir[ULS_FILEPATH_MAX+1];
extern const char *ulc_config, *uld_config, *tag_name;
extern char config_pathbuff[ULS_FILEPATH_MAX+1];
extern char uls_specname[ULC_LONGNAME_MAXSIZ+1];
extern char *filelist, *target_dir, *output_file;
extern char *cmdline_filter;
extern int  out_ftype;

extern uls_tmpl_list_t tmpl_list;
extern int exist_name_val_ent(const char *name);
extern int add_name_val_ent(const char *name, const char *val);

extern uls_lex_ptr_t sam_lex;

#endif
