/**
 * This file automatically has been generated by ULS and
 *  contains an implementation of lexical analyzer
 *  for parsing the programming language 'gcc'.
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
#include <uls/uls_core.h>

int uls_init_gcc(uls_lex_ptr_t uls)
{
	const char *confname = "gcc";

	if (uls_init(uls, confname) <  0) {
		return -1;
	}

	return 0;
}

uls_lex_ptr_t uls_create_gcc(void)
{
	const char *confname = "gcc";
	uls_lex_ptr_t uls;

	if ((uls = uls_create(confname)) == NULL) {
		return NULL;
	}

	return uls;
}

int uls_destroy_gcc(uls_lex_ptr_t uls)
{
	return uls_destroy(uls);
}

