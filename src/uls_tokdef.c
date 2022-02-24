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
#include "uls/uls_tokdef.h"
#include "uls/uls_log.h"

void
print_tokdef_vx_char(uls_uch_t uch, uls_tokdef_vx_ptr_t e_vx)
{
	int tokid = e_vx->tok_id;

	if (uls_isgraph(uch)) {
		uls_printf("\t'%c' (%3u)", uch, uch);
		if (uch != tokid) {
			uls_printf("  --> %d\n", tokid);
		} else {
			uls_printf("\n");
		}
	} else {
		uls_printf("\t    (%3u)  --> %d\n", uch, tokid);
	}
}

int
__is_in_ilist(int *ilst, int n_ilst, int val)
{
	int j;

	for (j=0; j<n_ilst; j++) {
		if (ilst[j] == val) return 1;
	}
	return 0;
}

uls_tokdef_ptr_t
uls_create_tokdef(void)
{
	uls_tokdef_ptr_t e;

	e = uls_alloc_object(uls_tokdef_t);
	uls_initial_zerofy_object(e);
	uls_init_namebuf(e->keyword, ULS_LEXSTR_MAXSIZ);

	return e;
}

void
uls_destroy_tokdef(uls_tokdef_ptr_t e)
{
	uls_deinit_namebuf(e->keyword);
	uls_dealloc_object(e);
}

void
__init_tokdef_vx(uls_tokdef_vx_ptr_t e_vx)
{
	e_vx->flags = 0;
	e_vx->tok_id = 0;

	uls_init_namebuf(e_vx->name, ULS_LEXSTR_MAXSIZ);
	e_vx->l_name = 0;

	e_vx->extra_tokdef = nilptr;
	e_vx->base = nilptr;
	e_vx->tokdef_names = nilptr;
}

void
uls_init_tokdef_vx(uls_tokdef_vx_ptr_t e_vx, int tok_id, const char* name, uls_tokdef_ptr_t e)
{
	__init_tokdef_vx(e_vx);

	e_vx->l_name = uls_set_namebuf_value(e_vx->name, name);
	e_vx->tok_id = tok_id;
	e_vx->base = e;

	if (e != nilptr) {
		e->view = e_vx;
		e->name = uls_get_namebuf_value(e_vx->name);
	}
}

void
uls_deinit_tokdef_vx(uls_tokdef_vx_ptr_t e_vx)
{
	uls_tokdef_name_ptr_t e_nam, e_nam_prev;

	for (e_nam=e_vx->tokdef_names; e_nam != nilptr; e_nam = e_nam_prev) {
		e_nam_prev = e_nam->prev;
		dealloc_tokdef_name(e_nam);
	}

	e_vx->tokdef_names = nilptr;
	uls_deinit_namebuf(e_vx->name);
}

uls_tokdef_vx_ptr_t
uls_create_tokdef_vx(int tok_id, const char* name, uls_tokdef_ptr_t e)
{
	uls_tokdef_vx_ptr_t e_vx;

	e_vx = uls_alloc_object(uls_tokdef_vx_t);
	uls_init_tokdef_vx(e_vx, tok_id, name, e);

	return e_vx;
}

void
uls_destroy_tokdef_vx(uls_tokdef_vx_ptr_t e_vx)
{
	uls_deinit_tokdef_vx(e_vx);
	uls_dealloc_object(e_vx);
}

uls_tokdef_name_ptr_t
alloc_tokdef_name(const char *name, uls_tokdef_vx_ptr_t view)
{
	uls_tokdef_name_ptr_t e_nam;

	e_nam = uls_alloc_object(uls_tokdef_name_t);
	uls_initial_zerofy_object(e_nam);

	uls_init_namebuf(e_nam->name, ULS_LEXSTR_MAXSIZ);
	uls_set_namebuf_value(e_nam->name, name);

	e_nam->view = view;
	e_nam->prev = nilptr;

	return e_nam;
}

void
dealloc_tokdef_name(uls_tokdef_name_ptr_t e_nam)
{
	uls_deinit_namebuf(e_nam->name);
	uls_dealloc_object(e_nam);
}

uls_tokdef_name_ptr_t
find_tokdef_name(uls_tokdef_vx_ptr_t e_vx_leader, const char* name, uls_outparam_ptr_t parms)
{
	uls_tokdef_name_ptr_t e, e_prev=nilptr;

	for (e=e_vx_leader->tokdef_names; e != nilptr; e = e->prev) {
		if (uls_streql(uls_get_namebuf_value(e->name), name)) {
			break;
		}
		e_prev = e;
	}

	if (parms != nilptr) {
		parms->data = e_prev;
	}

	return e;
}

void
insert_tokdef_name_to_group(uls_tokdef_vx_ptr_t e_vx_leader,
	uls_tokdef_name_ptr_t e_nam_prev, uls_tokdef_name_ptr_t e_nam)
{
	if (e_nam_prev != nilptr) {
		e_nam->prev = e_nam_prev->prev;
		e_nam_prev->prev = e_nam;
	} else {
		e_nam->prev = e_vx_leader->tokdef_names;
		e_vx_leader->tokdef_names = e_nam;
	}
}

int
append_tokdef_name_to_group(uls_tokdef_vx_ptr_t e_vx_leader, uls_tokdef_name_ptr_t e_nam)
{
	uls_tokdef_name_ptr_t e, e_prev=nilptr;

	for (e=e_vx_leader->tokdef_names; e != nilptr; e = e->prev) {
		if (uls_streql(uls_get_namebuf_value(e->name), uls_get_namebuf_value(e_nam->name))) {
			return 0;
		}
		e_prev = e;
	}

	insert_tokdef_name_to_group(e_vx_leader, e_prev, e_nam);
	return 1;
}

uls_tokdef_ptr_t
search_tokdef_group(uls_tokdef_vx_ptr_t e_vx_leader, const char* keyw)
{
	uls_tokdef_ptr_t e;

	for (e=e_vx_leader->base; e != nilptr; e = e->next) {
		if (uls_streql(uls_get_namebuf_value(e->keyword), keyw)) {
			return e;
		}
	}

	return nilptr;
}

void
append_tokdef_to_group(uls_tokdef_vx_ptr_t e_vx_leader, uls_tokdef_ptr_t e_target)
{
	uls_tokdef_ptr_t e, e_prev=nilptr;

	for (e=e_vx_leader->base; e != nilptr; e = e->next) {
		e_prev = e;
	}

	if (e_prev != nilptr) {
		e_target->next = e_prev->next;
		e_prev->next = e_target;
	} else {
		e_target->next = e_vx_leader->base;
		e_vx_leader->base = e_target;
	}
}
