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

#ifndef __ULT_UTILS_H__
#define __ULT_UTILS_H__

#include <stdlib.h>
#include <string.h>
#ifndef  ULS_WINDOWS
#include <limits.h>
#endif
#include "uls/uls_istream.h"
#include "uls/uls_util.h"

#define ult_streql(a,b) (strcmp(a,b)==0)

char* ult_strdup(const char* str);
int ult_chdir(const char* path);
int ult_getcwd(char* buf, int buf_siz);
int ult_is_absolute_path(const char* path);

int ult_guess_specname_from_istream(uls_istream_t *istr, uls_outparam_ptr_t parms);
int ult_guess_specname(const char* fpath, uls_outparam_ptr_t parms);
int ult_guess_specname_from_inputfiles(char* specname, FILE *fp_list, int only_first);
int ult_guess_specname_from_argvlist(char* specname, int n_args, const char** args, int only_first);
int ult_is_inputfiles_raw(const char *filelist);

int ult_guess_host_byteorder(void);

int ult_fd_create_wronly(const char* fpath);
int ult_fd_open_rdonly(const char* fpath);
void ult_fd_close(int fd);

const char* ult_get_suffix(const char *filepath);

typedef int (*ult_sort_func_t)(const uls_voidptr_t a, const uls_voidptr_t b);
void ult_simple_sort_vptr(uls_voidptr_t *list, int n_list, ult_sort_func_t cmp_func);

#endif // __ULT_UTILS_H__
