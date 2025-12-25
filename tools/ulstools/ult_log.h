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
 * ult_log.h -- logging for internal use --
 *     written by Stanley Hong <link2next@gmail.com>, Dec. 2025.
 */
#ifndef _ULT_LOG_H_
#define _ULT_LOG_H_

#include <stdarg.h>

#ifdef __ULT_LOG__ 
#define EXTERNAL
#else
#define EXTERNAL extern
#endif

typedef enum {
	ERRLEVEL_MSG=1, ERRLEVEL_WARN, ERRLEVEL_SYS, ERRLEVEL_QUIT, ERRLEVEL_ABORT
} t_errlvl;

typedef void (*t_errproc)(t_errlvl level, const char* fmt, va_list args);

EXTERNAL void ult_log_init(void* data, t_errproc proc1);
EXTERNAL void ult_log(const char* fmt, ...);
EXTERNAL void ult_panic(const char* fmt, ...);

#undef EXTERNAL
#endif /* _ULT_LOG_H_ */


