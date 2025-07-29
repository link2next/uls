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
  <file> uls_init.c </file>
  <brief>
    The initialization routines for ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_INIT__
#include "uls/uls_init.h"
#include "uls/uls_lex.h"
#include "uls/uls_conf.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_langs.h"
#include "uls/utf_file.h"
#include "uls/litesc.h"
#include "uls/uls_util.h"
#include "uls/uls_log.h"
#endif

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(__finalize_uls)(void)
{
	finalize_uls_litesc();

	if (uls_langs != nilptr) {
		uls_destroy_lang_list(uls_langs);
		uls_langs = nilptr;
	}

	unload_uch_ranges_list();

	ulc_set_searchpath(NULL);
#ifndef ULS_DOTNET
	finalize_uls_util();
#endif
}

ULS_DECL_STATIC int
ULS_QUALIFIED_METHOD(__initialize_uls)(void)
{
	char pathbuff[ULS_FILEPATH_MAX+1];

#ifndef ULS_DOTNET
	if (_uls_tool(initialize_uls_util)() < 0) {
		return -1;
	}
#endif
	if (load_uch_ranges_list() < 0) {
		_uls_log(err_log)("ULS: can't find the file for unicode id ranges!");
		return -1;
	}

	if (uls_langs != nilptr) uls_destroy_lang_list(uls_langs);
	_uls_log_(snprintf)(pathbuff, ULS_FILEPATH_MAX, "%s%c%s", _uls_sysinfo_(etc_dir), ULS_FILEPATH_DELIM, ULS_LANGS_FNAME);

	if ((uls_langs = uls_load_langdb(pathbuff)) == nilptr) {
		_uls_log_(snprintf)(pathbuff, ULS_FILEPATH_MAX, "%s%c%s", _uls_sysinfo_(etc_dir), ULS_FILEPATH_DELIM, TMP_LANGS_FNAME);

		if ((uls_langs = uls_load_langdb(pathbuff)) == nilptr) {
			_uls_log(err_log)("can't load lang-db '%s'!", ULS_LANGS_FNAME);
			_uls_log(err_log)("  etc_dir = '%s'", _uls_sysinfo_(etc_dir));
			return -1;
		}
	}

	initialize_uls_litesc();

	_uls_sysinfo_(initialized) = 1;
	return 0;
}

void
#ifdef __GNUC__
__attribute__((constructor))
#endif
ULS_QUALIFIED_METHOD(_initialize_uls)(void)
{
	if (_uls_tool_(sysinfo) != nilptr && _uls_sysinfo_(initialized)) {
		return;
	}

	if (__initialize_uls() < 0) {
		_uls_tool_(appl_exit)(1);
	}
}

void
#ifdef __GNUC__
__attribute__((destructor))
#endif
ULS_QUALIFIED_METHOD(_finalize_uls)(void)
{
	if (_uls_tool_(sysinfo) == nilptr || !_uls_sysinfo_(initialized)) {
		return;
	}

	__finalize_uls();
}

void
ULS_QUALIFIED_METHOD(initialize_uls)(void)
{
	_initialize_uls();
}

void
ULS_QUALIFIED_METHOD(finalize_uls)(void)
{
	_finalize_uls();
}
