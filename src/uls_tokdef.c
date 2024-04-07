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
 * uls_tokdef.c -- token definitions of ULS --
 *     written by Stanley Hong <link2next@gmail.com>, April 2012.
 *
 *  This file is part of ULS, Unified Lexical Scheme.
 */
#ifndef ULS_EXCLUDE_HFILES
#define __ULS_TOKDEF__
#include "uls/uls_tokdef.h"
#include "uls/uls_log.h"
#endif

int
ULS_QUALIFIED_METHOD(__is_in_ilist)(int *ilst, int n_ilst, int val)
{
	int j;

	for (j=0; j<n_ilst; j++) {
		if (ilst[j] == val) return 1;
	}
	return 0;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_tokdef)(void)
{
	uls_tokdef_ptr_t e;

	e = uls_alloc_object_clear(uls_tokdef_t);
	uls_init_namebuf(e->keyword, ULS_TOKNAM_MAXSIZ);

	return e;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_tokdef)(uls_tokdef_ptr_t e)
{
	uls_deinit_namebuf(e->keyword);
	uls_dealloc_object(e);
}

void
ULS_QUALIFIED_METHOD(__init_tokdef_vx)(uls_tokdef_vx_ptr_t e_vx)
{
	e_vx->flags = 0;
	e_vx->tok_id = 0;

	uls_init_namebuf(e_vx->name, ULS_TOKNAM_MAXSIZ);
	e_vx->l_name = 0;

	e_vx->extra_tokdef = nilptr;
	e_vx->base = nilptr;
	e_vx->tokdef_names = nilptr;
}

void
ULS_QUALIFIED_METHOD(uls_init_tokdef_vx)(uls_tokdef_vx_ptr_t e_vx, int tok_id, const char* name, uls_tokdef_ptr_t e)
{
	__init_tokdef_vx(e_vx);

	e_vx->l_name = uls_set_namebuf_value(e_vx->name, name);
	e_vx->tok_id = tok_id;
	e_vx->base = e;

	if (e != nilptr) {
		e->view = e_vx;
	}
}

void
ULS_QUALIFIED_METHOD(uls_deinit_tokdef_vx)(uls_tokdef_vx_ptr_t e_vx)
{
	uls_tokdef_name_ptr_t e_nam, e_nam_next;

	for (e_nam=e_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam_next) {
		e_nam_next = e_nam->next;
		dealloc_tokdef_name(e_nam);
	}

	e_vx->tokdef_names = nilptr;
	uls_deinit_namebuf(e_vx->name);
}

ULS_QUALIFIED_RETTYP(uls_tokdef_vx_ptr_t)
ULS_QUALIFIED_METHOD(uls_create_tokdef_vx)(int tok_id, const char* name, uls_tokdef_ptr_t e)
{
	uls_tokdef_vx_ptr_t e_vx;

	if (name == NULL) name = "";
	e_vx = uls_alloc_object(uls_tokdef_vx_t);
	uls_init_tokdef_vx(e_vx, tok_id, name, e);

	return e_vx;
}

void
ULS_QUALIFIED_METHOD(uls_destroy_tokdef_vx)(uls_tokdef_vx_ptr_t e_vx)
{
	uls_deinit_tokdef_vx(e_vx);
	uls_dealloc_object(e_vx);
}

ULS_QUALIFIED_RETTYP(uls_tokdef_name_ptr_t)
ULS_QUALIFIED_METHOD(alloc_tokdef_name)(const char *name)
{
	uls_tokdef_name_ptr_t e_nam;

	e_nam = uls_alloc_object_clear(uls_tokdef_name_t);
	uls_init_namebuf(e_nam->name, ULS_TOKNAM_MAXSIZ);
	uls_set_namebuf_value(e_nam->name, name);

	e_nam->next = nilptr;
	return e_nam;
}

void
ULS_QUALIFIED_METHOD(dealloc_tokdef_name)(uls_tokdef_name_ptr_t e_nam)
{
	uls_deinit_namebuf(e_nam->name);
	uls_dealloc_object(e_nam);
}

int
ULS_QUALIFIED_METHOD(canbe_tokname)(const char *str)
{
	int i, val;
	char ch;

	if (str == NULL || *str == '\0') return 0;

	for (i=0; (ch=str[i])!='\0'; i++) {
		if (i > 0) {
			val = _uls_tool_(isalnum)(ch) || (ch == '_');
		} else {
			val = _uls_tool_(isalpha)(ch) || (ch == '_');
		}
		if (val == 0) return 0;
	}

	if (i > ULS_TOKNAM_MAXSIZ)
		return 0;

	return i;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_name_ptr_t)
ULS_QUALIFIED_METHOD(find_tokdef_alias)(uls_tokdef_vx_ptr_t e_vx, const char* name)
{
	uls_tokdef_name_ptr_t e_nam;

	for (e_nam = e_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam->next) {
		if (uls_streql(uls_get_namebuf_value(e_nam->name), name)) {
			break;
		}
	}

	return e_nam;
}

ULS_QUALIFIED_RETTYP(uls_tokdef_ptr_t)
ULS_QUALIFIED_METHOD(find_tokdef_by_keyw)(uls_tokdef_vx_ptr_t e_vx, const char* keyw)
{
	uls_tokdef_ptr_t e;

	for (e=e_vx->base; e != nilptr; e = e->next) {
		if (uls_streql(uls_get_namebuf_value(e->keyword), keyw)) {
			return e;
		}
	}

	return nilptr;
}

ULS_DECL_STATIC void
ULS_QUALIFIED_METHOD(insert_tokdef_name_to_group)(uls_tokdef_vx_ptr_t e_vx, uls_tokdef_name_ptr_t e_nam)
{
	// insert e_nam into the head.
	e_nam->next = e_vx->tokdef_names;
	e_vx->tokdef_names = e_nam;
}

int
ULS_QUALIFIED_METHOD(uls_change_tokdef_vx_name)(uls_tokdef_vx_ptr_t e_vx, const char* name, const char* name2)
{
	int stat = 0;
	uls_tokdef_name_ptr_t e_nam;

	if (name != NULL && uls_streql(name, name2)) {
		return 0;
	}

	if (canbe_tokname(name2) <= 0) {
		_uls_log(err_log)("%s: not token name!", name2);
		return -1;
	}

	if (name == NULL || uls_streql(uls_get_namebuf_value(e_vx->name), name)) {
		uls_set_namebuf_value(e_vx->name, name2);
		stat = 1; // found & changed!

	} else if ((e_nam = find_tokdef_alias(e_vx, name)) != nilptr) {
		uls_set_namebuf_value(e_nam->name, name2);
		stat = 1; // found & changed!
	}

	return stat;
}

int
ULS_QUALIFIED_METHOD(uls_add_tokdef_vx_name)(uls_tokdef_vx_ptr_t e_vx, const char* name)
{
	int stat = 0;
	uls_tokdef_name_ptr_t e_nam;

	if (uls_get_namebuf_value(e_vx->name)[0] == '\0') {
		uls_set_namebuf_value(e_vx->name, name);
		stat = 1;
	} else if ((e_nam = find_tokdef_alias(e_vx, name)) == nilptr) {
		e_nam = alloc_tokdef_name(name);
		insert_tokdef_name_to_group(e_vx, e_nam);
		stat = 1;
	}

	return stat;
}

void
ULS_QUALIFIED_METHOD(append_tokdef_to_group)(uls_tokdef_vx_ptr_t e_vx, uls_tokdef_ptr_t e_target)
{
	uls_tokdef_ptr_t e, e_prev = nilptr;

	e_target->view = e_vx;
	e_target->next = nilptr;

	for (e = e_vx->base; e != nilptr; e = e->next) {
		e_prev = e;
	}

	if (e_prev != nilptr) {
		e_prev->next = e_target;
	} else {
		e_vx->base = e_target;
	}
}
