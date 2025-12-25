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
 * ult_log.c -- logging for internal use --
 *     written by Stanley Hong <link2next@gmail.com>, Dec. 2025.
 */
#define __ULT_LOG__ 
#include "ult_log.h"

#include <stdlib.h>
#include <stdio.h>

static void _syserrNULL(t_errlvl level, const char* fmt, va_list args);
static void _syserrFILE(t_errlvl level, const char* fmt, va_list args);

static  void        *syserr_stream = NULL;
static  t_errproc    syserr_proc = (t_errproc) _syserrFILE;

void ult_log_init(void* data, t_errproc proc1)
{
	syserr_stream = data;	
	if (proc1==(t_errproc)0) {
		if (data) {
			syserr_stream = data;
			syserr_proc   = _syserrFILE;
		} else {
			syserr_proc   = _syserrNULL;
		}
	} else {
		syserr_proc = proc1;
		syserr_stream = data;
	}
}

void ult_log(const char* fmt, ...)
{
	va_list	args;
	va_start(args, fmt);
	(*syserr_proc)(ERRLEVEL_MSG, fmt, args);
	va_end(args);
}

void ult_panic(const char* fmt, ...)
{
	va_list	args;
	va_start(args, fmt);
	(*syserr_proc)(ERRLEVEL_SYS, fmt, args);
	va_end(args);

	exit(ERRLEVEL_SYS);
}

static void _syserrNULL(t_errlvl level, const char* fmt, va_list args)
{
}

static void _syserrFILE(t_errlvl level, const char* fmt, va_list args)
{
	FILE *fp_log = (FILE*) syserr_stream;

	if (syserr_stream == NULL) {
		fp_log = stderr;
	} else { 
		fp_log = (FILE*) syserr_stream;
	}
	vfprintf(fp_log, fmt, args);
	fprintf(fp_log, "\n");
}
