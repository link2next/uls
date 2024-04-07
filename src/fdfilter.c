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
  <file> fdfilter.c </file>
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
#include "uls/uls_log.h"

#include "uls/fdfilter.h"
#include "uls/uls_fileio.h"
#include "uls/uls_misc.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void
fdf_init(fdf_t *fdf, uls_fdf_iprovider_t i_provider, const char* cmdline)
{
	int i;

	if (i_provider == nilptr)
		i_provider = fdf_iprovider_simple;

	fdf->fd_org = -1;
	fdf->fd = -1;
	fdf->i_provider = i_provider;
	fdf->filter = cmdline;

	for (i=0; i<FDF_N_PROCS_POPEN; i++) {
		fdf->child_pid[i] = -1;
	}
}

void
fdf_reset(fdf_t *fdf, uls_fdf_iprovider_t i_provider, const char* cmdline)
{
	if (i_provider == nilptr)
		i_provider = fdf_iprovider_simple;

	fdf->i_provider = i_provider;
	fdf->filter = cmdline;
}

void
fdf_deinit(fdf_t *fdf)
{
	fdf_close(fdf);
}

ULS_DECL_STATIC int
fdf_open_0(int *pipe1, int *pipe2, fdf_t *fdf, int fd)
{
	int  r_pipe, w_pipe;
	int  rc;

	if ((fdf->child_pid[0]=fork()) < 0) {
		_uls_log(err_log)("fork error!!");
		return -1;
	}

	if (fdf->child_pid[0] == 0) {
		uls_fd_close(pipe1[1]); uls_fd_close(pipe2[0]);
		r_pipe = pipe1[0]; w_pipe = pipe2[1];
		rc = fdf->i_provider(fd, w_pipe);
		uls_fd_close(r_pipe); uls_fd_close(w_pipe);
		uls_appl_exit(rc);
	}

	fdf->child_pid[1] = -1;

	uls_fd_close(pipe1[0]); uls_fd_close(pipe2[1]);
	r_pipe = pipe2[0]; w_pipe = pipe1[1];

	uls_fd_close(w_pipe);
	fdf->fd_org = fd;
	fdf->fd = r_pipe; // r_pipe instead of fd

	return r_pipe;
}

int
fdf_open(fdf_t *fdf, int fd)
{
	const char* cmdline = fdf->filter;
	int  pipe1[2], pipe2[2];
	int  r_pipe, w_pipe, rc;
	int  fd_stdin = _uls_stdio_fd(0);
	int  fd_stdout = _uls_stdio_fd(1);

	if (fd < 0) return -1;

	if (pipe(pipe1) < 0) {
		_uls_log(err_log)("can't create pipes");
		return -1;
	}

	if (pipe(pipe2) < 0) {
		_uls_log(err_log)("can't create pipes");
		uls_fd_close(pipe1[0]); uls_fd_close(pipe1[1]);
		return -1;
	}

	if (cmdline == NULL) {
		if ((r_pipe=fdf_open_0(pipe1, pipe2, fdf, fd)) < 0) {
			uls_fd_close(pipe1[0]); uls_fd_close(pipe1[1]);
			uls_fd_close(pipe2[0]); uls_fd_close(pipe2[1]);
		}
		return r_pipe;
	}

	if ((fdf->child_pid[1]=fork()) < 0) {
		_uls_log(err_log)("fork error!!");
		uls_fd_close(pipe1[0]); uls_fd_close(pipe1[1]);
		uls_fd_close(pipe2[0]); uls_fd_close(pipe2[1]);
		return -1;
	}

	if (fdf->child_pid[1] == 0) {
		uls_fd_close(fd); // fd is unnecessary

		uls_fd_close(pipe1[1]); uls_fd_close(pipe2[0]);
		r_pipe = pipe1[0]; w_pipe = pipe2[1];

	 	if (r_pipe != fd_stdin) {
			if (dup2(r_pipe, fd_stdin) != fd_stdin) {
				_uls_log(err_log)("dup2 error (stdin)!!");
				uls_appl_exit(1);
			}
			uls_fd_close(r_pipe);
		}

		if (w_pipe != fd_stdout) {
			if (dup2(w_pipe, fd_stdout) != fd_stdout) {
				_uls_log(err_log)("dup2 error (stdout)!!");
				uls_appl_exit(1);
			}
			uls_fd_close(w_pipe);
		}

		if (uls_execv_cmdline(cmdline) < 0) {
			_uls_log(err_log)("execle error!");
			uls_appl_exit(1);
		}
		// NEVER REACHED
	}

	uls_fd_close(pipe1[0]); uls_fd_close(pipe2[1]);
	r_pipe = pipe2[0]; w_pipe = pipe1[1];

	if ((fdf->child_pid[0]=fork()) < 0) {
		_uls_log(err_log)("fork error!!");
		uls_fd_close(r_pipe); uls_fd_close(w_pipe);
		return -1;
	}

	if (fdf->child_pid[0] == 0) {
		uls_fd_close(r_pipe);
		rc = fdf->i_provider(fd, w_pipe);
		uls_fd_close(fd); uls_fd_close(w_pipe);
		uls_appl_exit(rc);
	}

	fdf->fd_org = fd; // fd was also transferred to child_pid[0]
	uls_fd_close(w_pipe);

	fdf->fd = r_pipe; // r_pipe instead of fd

	return r_pipe;
}

int
fdf_close(fdf_t* fdf)
{
	fdf->fd_org = -1;

	if (fdf->fd >= 0) {
		uls_fd_close(fdf->fd);
		fdf->fd = -1;
	}

	if (uls_proc_join(fdf->child_pid, FDF_N_PROCS_POPEN) < 0)
		return -1;

	return 0;
}

int
fdf_iprovider_simple(int fdin, int writefd)
{
	int	rc, stat = 1;
	char buff[1024];

	while (stat > 0) {
		if ((rc=uls_readn(fdin, buff, sizeof(buff))) < 0) {
			stat = -1;
			break;
		} else if (rc < sizeof(buff)) {
			stat = 0;
		}

		if (uls_writen(writefd, buff, rc) < rc) {
			stat = -2;
		}
	}

	return stat;
}

int
fdf_iprovider_filelist(int fd_list, int writefd)
{
	char linebuff[ULS_FILEPATH_MAX+1];
	char *lptr, *fpath;
	int  len, rc, stat = 0;
	int  fdin;
	FILE *fp_lst;

	if ((fp_lst=uls_fp_fdopen(fd_list, "r")) == NULL) {
		_uls_log(err_log)("%s: fail to fdopen!", __func__);
		return -1;
	}

	while (1) {
		if ((len=uls_fp_gets(fp_lst, linebuff, sizeof(linebuff), 0)) <= ULS_EOF) {
			if (len < ULS_EOF) stat = -1;
			break;
		}

		lptr = skip_blanks(linebuff);
		if (*lptr=='\0' || *lptr=='#') continue;

		len -= (int) (lptr - linebuff);
		fpath = lptr;
		fpath[len] = '\0';

		_uls_log(err_log)("%s:", fpath);

		if ((fdin=uls_fd_open(fpath, ULS_FIO_READ)) < 0) {
			_uls_log(err_log)("%s: not found!", fpath);
			continue;
		}

		rc = fdf_iprovider_simple(fdin, writefd);
		uls_fd_close(fdin);

		if (rc < 0) {
			stat = -1;
			_uls_log(err_log)("%s: fail to process!", fpath);
		}
	}

	return stat;
}

char**
uls_pars_cmdline(const char* cmdline, char** p_line, int* p_args)
{
	int n_alloc_args, k;
	char *lptr, *wrd, **args;
	uls_wrd_t wrdx;

	if (cmdline == NULL || *(cmdline=skip_blanks(cmdline)) == '\0') {
		*p_line = NULL;
		*p_args = 0;
		return NULL;
	}

	k = uls_strlen(cmdline);

	*p_line = lptr = uls_malloc(k+1);
	uls_strcpy(lptr, cmdline);

	n_alloc_args = 32;
	args = (char **) uls_malloc((n_alloc_args+1)*sizeof(char *));

	wrdx.lptr = lptr;
	for (k=0; ; k++) {
		if (*(wrd=_uls_splitstr(uls_ptr(wrdx))) == '\0')
			break;

		if (k + 1 >= n_alloc_args) {
			n_alloc_args = uls_ceil_log2(k + 1, 4);
			args = (char **) uls_mrealloc(args, n_alloc_args * sizeof(char *));
		}
		args[k] = wrd;
	}
	lptr = wrdx.lptr;

	args[k] = NULL;

	if (p_args != NULL) *p_args = k;
	return args;
}

int
uls_execv_cmdline(const char* cmdline)
{
	char *progpath, **args, *argsbuff;
	const char *ptr;
	int  n_args;
	uls_outparam_t parms;

	if ((args=uls_pars_cmdline(cmdline, uls_ptr(argsbuff), uls_ptr(n_args))) == nilptr) {
		return -1;
	}

	parms.lptr = progpath = args[0];
	ptr = _uls_filename(uls_ptr(parms));
	args[0] = progpath + (int) (ptr - progpath);

	if (uls_dirent_exist(progpath) <= 0 || execv(progpath, args) < 0) {
		uls_mfree(args);
		uls_mfree(argsbuff);
		return -1;
	}

	// NEVER REACHED
	return 0;
}

int
uls_proc_join_round_1(uls_pid_t *child_pid, int n_child_pid)
{
	int status, exit_code;
	int k, stat, n_procs_rem=0;
	uls_pid_t pid;

	for (k=0; k<n_child_pid; k++) {
		if (child_pid[k] <= 0) continue;

		if ((pid=waitpid(child_pid[k], uls_ptr(status), WNOHANG)) < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == ECHILD) {
				_uls_log(err_log)("%s: ECHILD!", __func__);
				for (k=0; k<n_child_pid; k++) {
					if (child_pid[k] > 0) child_pid[k] = 0;
				}
				n_procs_rem = 0;
				break;
			} else {
				_uls_log(err_log)("error to waitpid()");
				return -1;
			}

		} else if (pid == 0) {
			++n_procs_rem;
			continue;
		}

		stat = -1;
		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
			if (exit_code != 0) {
				_uls_log(err_log)("child %d exited with %d", pid, exit_code);
				child_pid[k] = -1;
			} else {
				child_pid[k] = 0;
				stat = 0;
			}
		} else {
			child_pid[k] = -1;
		}

		if (stat < 0) return -1;
	}

	return n_procs_rem;
}

int
uls_proc_join(uls_pid_t *child_pid, int n_child_pid)
{
	int rc, stat=0;

	for ( ; ; ) {
		if ((rc=uls_proc_join_round_1(child_pid, n_child_pid)) < 0) {
			stat = -1;
			break;
		} else if (rc == 0) {
			break;
		}

		usleep(200000); // sleeping after 1-rounding, ...
	}

	return stat;
}
