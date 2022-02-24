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
#define __ULS_INIT__
#include "uls/uls_init.h"
#include "uls/uls_lex.h"
#include "uls/uls_conf.h"
#include "uls/uls_sysprops.h"
#include "uls/uls_langs.h"
#include "uls/utf_file.h"
#include "uls/uls_misc.h"
#include "uls/uls_util.h"
#include "uls/litesc.h"
#include "uls/uls_log.h"
#ifndef ULS_WINDOWS
#include <locale.h>
#endif

ULS_DECL_STATIC void
__finalize_uls(void)
{
	finalize_uls_litesc();
	finalize_ulc_lexattr();

	if (uls_langs != nilptr) {
		uls_destroy_lang_list(uls_langs);
		uls_langs = nilptr;
	}

	unload_uch_ranges_list();

	ulc_set_searchpath(NULL);
	finalize_uls_util();
}

ULS_DECL_STATIC int
__initialize_uls(void)
{
	char pathbuff[ULS_FILEPATH_MAX+1];

	if (initialize_uls_util() < 0) {
		return -1;
	}

	if (load_uch_ranges_list() < 0) {
		err_log("ULS: can't find the file for unicode id ranges!");
		return -1;
	}

	if (uls_langs != nilptr) uls_destroy_lang_list(uls_langs);
	uls_snprintf(pathbuff, ULS_FILEPATH_MAX, "%s/%s", _uls_sysinfo_(etc_dir), ULS_LANGS_FNAME);
	if ((uls_langs = uls_load_langdb(pathbuff)) == nilptr) {
		uls_snprintf(pathbuff, ULS_FILEPATH_MAX, "%s/%s", _uls_sysinfo_(etc_dir), TMP_LANGS_FNAME);
		if ((uls_langs = uls_load_langdb(pathbuff)) == nilptr) {
			err_log("can't load lang-db '%s'!", ULS_LANGS_FNAME);
			err_log("  etc_dir = '%s'", _uls_sysinfo_(etc_dir));
			return -1;
		}
	}

	initialize_ulc_lexattr();
	initialize_uls_litesc();

	_uls_sysinfo_(initialized) = 1;
	return 0;
}

#ifndef ULS_WINDOWS
ULS_DECL_STATIC int
set_uls_locale(void)
{
	const char *cptr0, *cptr;
	char lang_entry[16], lang_buff[16];
	char *locale_list[] = { "C", "en_US" };
	char *encoding_suffs[] = { "utf8", "UTF-8" };
	int i, j, len, stat=0;

	if ((cptr0=getenv("LANG")) != NULL) {
		if ((cptr = uls_strchr(cptr0, '.')) != NULL && (len=(int)(cptr-cptr0)) > 0 && len < 8) {
			for (j=0; j<len; j++) lang_entry[j] = cptr0[j];
			lang_entry[len] = '\0';
			locale_list[0] = lang_entry;
		}
	}

	for (i=0; i<uls_dim(locale_list); i++) {
		cptr0 = locale_list[i];
		len = uls_strlen(cptr0);

		uls_strcpy(lang_buff, cptr0);

		for (j=0; j<uls_dim(encoding_suffs); j++) {
			lang_buff[len] = '.';
			uls_strcpy(lang_buff+len+1, encoding_suffs[j]);
			if (setlocale(LC_ALL, lang_buff) != NULL) {
				stat = 1;
				break;
			}
		}
		if (stat > 0) break;
	}

	return stat;
}
#endif

void
#ifdef __GNUC__
__attribute__((constructor))
#endif
_initialize_uls(void)
{
	if (uls_sysinfo != nilptr && _uls_sysinfo_(initialized)) {
		return;
	}

	if (__initialize_uls() < 0) {
		uls_appl_exit(1);
	}
#ifndef ULS_WINDOWS
	if (!set_uls_locale()) {
		err_log("Fail to set locale utf8!");
	}
#endif
}

void
#ifdef __GNUC__
__attribute__((destructor))
#endif
_finalize_uls(void)
{
	if (uls_sysinfo == nilptr || !_uls_sysinfo_(initialized)) {
		return;
	}

	__finalize_uls();
}

void
initialize_uls_static(void)
{
	_initialize_uls();
}

void
initialize_uls(void)
{
	initialize_uls_static();
#ifdef ULS_NO_SUPPORT_FINALCALL
	atexit(__finalize_uls);
#endif
}

void
finalize_uls(void)
{
	_finalize_uls();
}
