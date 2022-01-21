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
  <file> fdfilter.h </file>
  <brief>
    fdfilter is a collection of API for manipulating raw input.
    Given input by fd, fdfilter reads from it and oupput to another fd, filtering the stream by 'filter-prog'
    The 'filter-prog' is any executable of which input is from stdin and of output is to stdout.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#ifndef __FDFILTER_H__
#define __FDFILTER_H__

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_type.h"
#ifndef ULS_WINDOWS
#include <unistd.h>
#endif
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_PROTECTED_TYPE
#define FDF_N_PROCS_POPEN  2
#ifdef ULS_WINDOWS
typedef uls_int32 uls_pid_t;
#else
typedef pid_t uls_pid_t;
#endif
#endif

#ifdef ULS_DECL_PUBLIC_TYPE
ULS_DEFINE_DELEGATE_BEGIN(fdf_iprovider, int)(int fd, int writefd);
ULS_DEFINE_DELEGATE_END(fdf_iprovider);
#endif

#ifdef ULS_DEF_PROTECTED_TYPE
_ULS_DEFINE_STRUCT(fdf)
{
	uls_fdf_iprovider_t i_provider;
	const char *filter;

	int fd, fd_org;
	uls_pid_t child_pid[FDF_N_PROCS_POPEN];
};
#endif // ULS_DEF_PROTECTED_TYPE

#ifdef ULS_DECL_PROTECTED_PROC
char** uls_pars_cmdline(const char* cmdline, char** p_line, int* ptr_n_args);
int uls_execv_cmdline(const char* cmdline);
int uls_proc_join(uls_pid_t *child_pid, int n_child_pid);
#endif

#ifdef ULS_DECL_PUBLIC_PROC
void fdf_init(fdf_t *fdflt, uls_fdf_iprovider_t i_provider, const char* cmdline);
void fdf_reset(fdf_t *fdflt, uls_fdf_iprovider_t i_provider, const char* cmdline);
void fdf_deinit(fdf_t *fdflt);
int fdf_open(fdf_t *fdflt, int fd);
int fdf_close(fdf_t* fd_flt);
int fdf_iprovider_simple(int fdin, int writefd);
int fdf_iprovider_filelist(int fd_list, int writefd);
#endif

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __FDFILTER_H__
