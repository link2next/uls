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
  <file> uls_type.h </file>
  <brief>
    The utility routines in ULS.
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011
  </author>
*/

#ifndef __ULS_TYPE_H__
#define __ULS_TYPE_H__

#include "uls/uls_const.h"
#if !defined(USE_ULSNETJAVA)
#ifdef ULS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#else
#include <sys/types.h>
#endif
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define uls_dim(A) (sizeof(A)/sizeof(A[0]))

/* LOG_B, B ==2^LOG_B for some n */
#define uls_floor_log2(A,LOG_B) ((A) &  ~((1 << (LOG_B)) - 1))
#define uls_ceil_log2(A,LOG_B) (((A) + (1 << (LOG_B)) - 1) & (~((1<<(LOG_B)) - 1)))
#define uls_ceil(A,B) (((A)+(B)-1)/(B)*(B))
#define uls_roundup uls_ceil

#define uls_set_bit(nr,addr)   ((*((char *) (addr) + ((nr)>>3) )) |=  (0x80 >> ((nr) & 0x07)))
#define uls_clear_bit(nr,addr) ((*((char *) (addr) + ((nr)>>3) )) &= ~(0x80 >> ((nr) & 0x07)))
#define uls_test_bit(nr,addr)  ((*((char *) (addr) + ((nr)>>3) )) &   (0x80 >> ((nr) & 0x07)))

#define uls_num2char_hex(val) (((val) >= 10) ?  (val) - 10 + 'A' : (val) + '0')
#define uls_streql(a,b)  (uls_strcmp((const char*)(a),(const char*)(b))==0)

#ifdef ULS_WINDOWS
typedef int16_t     uls_int16;
typedef int32_t     uls_int32;
typedef int64_t     uls_int64;
typedef uint16_t    uls_uint16;
typedef uint32_t    uls_uint32;
typedef uint64_t    uls_uint64;
#else
typedef int16_t     uls_int16;
typedef int32_t     uls_int32;
typedef int64_t     uls_int64;
typedef u_int16_t   uls_uint16;
typedef u_int32_t   uls_uint32;
typedef u_int64_t   uls_uint64;
#endif

typedef void        *uls_voidptr_t;
typedef uls_uint32  uls_flags_t;
typedef uls_uint32  uls_uch_t;
#define ULS_UCH_EOS 0
typedef void        *uls_native_vptr_t;

#define uls_nil  NULL
#define uls_ptr(a) &(a)
#define uls_ref_typ(typ) typ*
#define uls_def_ptrvar(avar) *avar

#define uls_initial_zerofy_object(obj) uls_bzero(obj,sizeof(*(obj)))
#define uls_alloc_object(typ) (typ*)uls_malloc(sizeof(typ))
#define uls_alloc_object_clear(typ) (typ*)uls_malloc_clear(sizeof(typ))
#define uls_dealloc_object(obj) uls_mfree(obj)

// arraytype
#define _uls_type_array(typ) typ*
#define _uls_decl_array(name,typ) _uls_type_array(typ) name
#define _uls_cast_array_type(typ) (_uls_type_array(typ))
#define _uls_init_array(n,typ) (typ*)uls_malloc_clear((n)*sizeof(typ))
#define _uls_deinit_array(ary) uls_mfree(ary)
#define _uls_resize_array(ary,typ,n) (ary)=(typ*)uls_mrealloc(ary,(n)*sizeof(typ))

// name-buffer
#define uls_def_namebuf(nam,siz) char  nam[siz+1]
#define uls_init_namebuf(nam,siz) nam[0]='\0'
#define uls_deinit_namebuf(nam)
#define uls_get_namebuf_value(nam) (nam)
#define uls_get_namebuf_length(nam) uls_strlen(nam)
#define uls_set_namebuf_value(nam,str) uls_set_nambuf_raw(nam,sizeof(nam),str,-1)
#define uls_set_namebuf_value_2(nam,str,len) uls_set_nambuf_raw(nam,sizeof(nam),str,len)

// bytes pool
#define uls_def_bytespool(nam,siz) char nam[siz]
#define uls_ref_bytespool(nam) char*nam
#define uls_init_bytespool(nam,siz,no_clear) if (!no_clear) uls_bzero(nam,siz)
#define uls_deinit_bytespool(nam)

#define uls_type_this(ns,typnam) uls_##typnam##_t
#define uls_ref_callback(mthnam) mthnam

#define _ULS_DECLARE_STRUCT_BRIEF(typnam) struct _ ## typnam
#define _ULS_DEFINE_STRUCT_PTR(typnam) typedef struct _##typnam  *typnam##_ptr_t
#define _ULS_DECLARE_STRUCT(typnam) typedef struct _ ## typnam typnam##_t; _ULS_DEFINE_STRUCT_PTR(typnam)
#define _ULS_DEFINE_STRUCT_BEGIN(typnam) _ULS_DECLARE_STRUCT_BRIEF(typnam)
#define _ULS_DEFINE_STRUCT(typnam) _ULS_DECLARE_STRUCT(typnam); _ULS_DEFINE_STRUCT_BEGIN(typnam)

#define ULS_DECLARE_STRUCT_BRIEF(typnam) struct _uls_##typnam
#define ULS_DEFINE_STRUCT_PTR(typnam) typedef struct _uls_##typnam  *uls_##typnam##_ptr_t
#define ULS_DECLARE_STRUCT(typnam) typedef struct _uls_##typnam uls_##typnam##_t; ULS_DEFINE_STRUCT_PTR(typnam)
#define ULS_DEFINE_STRUCT_BEGIN(typnam) ULS_DECLARE_STRUCT_BRIEF(typnam)
#define ULS_DEFINE_STRUCT(typnam) ULS_DECLARE_STRUCT(typnam); ULS_DEFINE_STRUCT_BEGIN(typnam)

// delegate
#define ULS_DELEGATE_NAME(typnam) uls_##typnam##_t
#define ULS_DEFINE_DELEGATE_BEGIN(typnam,rtype) typedef rtype (*ULS_DELEGATE_NAME(typnam))
#define ULS_DEFINE_DELEGATE_END(typnam)
#define uls_callback_type(typnam) ULS_DELEGATE_NAME(typnam)

// standard array
#define _uls_get_stdary_slot(ary,idx) ((ary)+(idx))
#define _uls_set_stdary_slot(ary,idx,obj) (ary)[idx]=*(obj);

#define uls_get_array_slot(ary,idx) _uls_get_stdary_slot(ary,idx)
#define uls_set_array_slot(ary,idx,obj) _uls_set_stdary_slot(ary,idx,obj)

#define _uls_alloc_iary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		uls_init_##typnam(slot); \
	} while (0)
#define _uls_dealloc_iary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		uls_deinit_##typnam(slot); \
	} while (0)

#define _uls_alloc_zary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		uls_initial_zerofy_object(slot); \
	} while (0)
#define _uls_dealloc_zary_slot(ary,idx,ns,typnam,typmac)

// typmac
#define _uls_parray_this_(ns,typnam) uls_##typnam##_parray_t
#define _uls_ptrparray_this_(ns,typnam) uls_##typnam##_parray_ptr_t

#define __uls_array00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_t
#define __uls_ptrarray00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_ptr_t

#define _uls_array00_this_(ns,typnam,NN) __uls_array00_this_(ns,typnam,NN)
#define _uls_ptrarray00_this_(ns,typnam,NN) __uls_ptrarray00_this_(ns,typnam,NN)

#define _uls_array01_this_(ns,typnam) uls_##typnam##_s01array_t
#define _uls_ptrarray01_this_(ns,typnam) uls_##typnam##_s01array_ptr_t

#define _uls_array10_this_(ns,typnam) uls_##typnam##_s10array_t
#define _uls_ptrarray10_this_(ns,typnam) uls_##typnam##_s10array_ptr_t

// type00
#define ULS_DEF_ARRAY_TYPE00(typnam,arynam,NN)  \
	ULS_DECLARE_STRUCT(arynam); \
	ULS_DEFINE_STRUCT_BEGIN(arynam) { uls_type_this(ns,typnam) slots[NN]; int n; }

#define uls_decl_array_type00(ary,typnam,NN) _uls_array00_this_(none,typnam,NN) ary
#define uls_init_array_type00(ary,typnam,NN) do { \
	uls_bzero((ary)->slots,(NN)*sizeof(uls_type_this(none,typnam))); (ary)->n = NN; \
	} while (0)
#define uls_deinit_array_type00(ary,typnam)

#define uls_array_slots_type00(ary) ((ary)->slots)
#define uls_array_get_slot_type00(ary,idx) ((ary)->slots + (idx))
#define uls_array_decl_slots_type00(slots,typnam) uls_##typnam##_ptr_t slots
#define uls_array_ref_slots_type00(slots,typnam) uls_array_decl_slots_type00(slots,typnam)

// type01
#define ULS_DEF_ARRAY_TYPE01(typnam) \
	ULS_DECLARE_STRUCT(typnam##_s01array); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_s01array) { uls_type_this(none,typnam) uls_def_ptrvar(slots); int n; }

#define uls_decl_array_type01(ary,typnam) _uls_array01_this_(none,typnam) ary
#define uls_ref_array_type01(ary,typnam) _uls_array01_this_(none,typnam) *ary

#define uls_init_array_type01(ary,typnam,nn) do { \
		(ary)->slots=(uls_type_this(none,typnam)*)uls_malloc_clear((nn)*sizeof(uls_type_this(none,typnam))); \
		(ary)->n = nn; \
	} while (0)

#define uls_deinit_array_type01(ary,typnam) do { \
		uls_mfree((ary)->slots); (ary)->n = 0; \
	} while (0)

#define uls_init_array_type01a(ary,typnam,nn) do { int i; \
        	uls_init_array_type01(ary,typnam,nn); \
		for (i=0; i<(nn); i++) { uls_init_##typnam(uls_array_get_slot_type01(ary,i)); } \
	} while (0)

#define uls_deinit_array_type01a(ary,typnam)  do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_deinit_##typnam(uls_array_get_slot_type01(ary,i)); } \
		uls_deinit_array_type01(ary,typnam); \
	} while (0)
#define uls_array_slots_type01(ary) ((ary)->slots)
#define uls_array_get_slot_type01(ary,idx) ((ary)->slots + (idx))
#define uls_decl_array_slots_type01(a,typnam) uls_type_this(none,typnam) uls_def_ptrvar(a)

// type10
#define ULS_DEF_ARRAY_TYPE10(typnam) \
	ULS_DECLARE_STRUCT(typnam##_s10array); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_s10array) { uls_type_this(none,typnam) uls_def_ptrvar(slots); int n, n_alloc; }

#define uls_decl_array_type10(ary,typnam) _uls_array10_this_(none,typnam) ary
#define uls_decl_array_alloc_type10(ary,typnam) \
	_uls_array10_this_(none,typnam) *ary = uls_alloc_object_clear(_uls_array10_this_(none,typnam))
#define uls_ref_array_type10(ary,typnam) _uls_array10_this_(none,typnam) *ary

#define uls_cast_array_type10(typnam) (_uls_array10_this_(none,typnam)*)
#define uls_ref_array_init_type10(ary,typnam,ary2) _uls_array10_this_(none,typnam) *ary = ary2
#define uls_init_array_type10(ary,typnam,nn) do { \
		(ary)->slots=(uls_type_this(none,typnam)*)uls_malloc_clear((nn)*sizeof(uls_type_this(none,typnam))); \
		(ary)->n = 0; (ary)->n_alloc = (nn); \
	} while (0)
#define uls_deinit_array_type10(ary,typnam) do { \
		uls_resize_array_type10(ary,typnam,0); \
		uls_mfree((ary)->slots); (ary)->n = (ary)->n_alloc = 0; \
	} while (0)
#define uls_resize_array_type10(ary,typnam,nn_alloc) do { \
		int i; for (i=(nn_alloc); i<(ary)->n; i++) { _uls_dealloc_iary_slot((ary)->slots,i,none,typnam,uls_type_this); } \
		(ary)->slots=(uls_type_this(none,typnam)*)uls_mrealloc((ary)->slots,(nn_alloc)*sizeof(uls_type_this(none,typnam))); \
		if ((nn_alloc) < (ary)->n) (ary)->n = (nn_alloc); (ary)->n_alloc = (nn_alloc); \
	} while (0)

#define uls_decl_array_slots_type10(a,typnam) uls_type_this(none,typnam) uls_def_ptrvar(a)
#define uls_array_slots_type10(ary) ((ary)->slots)
#define uls_array_get_slot_type10(ary,idx) ((ary)->slots + (idx))

#define uls_array_alloc_slot_type10(ary,typnam,idx) \
	_uls_alloc_iary_slot((ary)->slots,idx,none,typnam,uls_type_this)
#define uls_array_dealloc_slot_type10(ary,typnam,idx) \
	_uls_dealloc_iary_slot((ary)->slots,idx,none,typnam,uls_type_this)

// type11
#define ULS_DEF_PARRAY(typnam) \
	ULS_DECLARE_STRUCT(typnam##_parray); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_parray) { \
		uls_type_this(none,typnam)**slots; int n, n_alloc; }

#define uls_decl_parray(name,typnam) _uls_parray_this_(none,typnam) name
#define uls_ref_parray(name,typnam) uls_ref_typ(_uls_parray_this_(none,typnam)) name
#define uls_ref_parray_init(name,typnam,val) uls_ref_parray(name,typnam)=(val)
#define uls_init_parray(a,typnam,nn) do { \
		(a)->slots = (uls_type_this(none,typnam)**)uls_malloc_clear((nn)*sizeof(uls_type_this(none,typnam)*)); \
		(a)->n = 0; (a)->n_alloc = nn; \
	} while(0)
#define uls_deinit_parray(a) do { \
		uls_mfree((a)->slots); \
		(a)->n = (a)->n_alloc = 0; \
	} while(0)
#define uls_resize_parray(a,typnam,nn)  do { \
		(a)->slots = (uls_type_this(none,typnam)**)uls_mrealloc((a)->slots,(nn)*sizeof(uls_type_this(none,typnam)*)); \
		if ((nn) < (a)->n) (a)->n = (nn); (a)->n_alloc = (nn); \
	} while (0)

#define uls_parray_slots(b) ((b)->slots)
#define uls_decl_parray_slots(a,typnam) uls_type_this(none,typnam)**a
#define uls_decl_parray_slots_init(a,typnam,b) uls_type_this(none,typnam)**a=(b)->slots

#ifdef _ULSCPP_IMPLDLL
#if defined(ULS_WINDOWS)
#define _ULSCPP_AUWCVT_LEN(slot_no) auwcvt->get_slot_len(slot_no)
#define _ULSCPP_USTR2NSTR(ustr, astr, slot_no) \
	((astr) = auwcvt->mbstr2mbstr(ustr, UlsAuw::CVT_MBSTR_ASTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))
#define _ULSCPP_NSTR2USTR(astr, ustr, slot_no) \
	((ustr) = auwcvt->mbstr2mbstr(astr, UlsAuw::CVT_MBSTR_USTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))
#else
#define _ULSCPP_AUWCVT_LEN(slot_no) auwcvt->get_slot_len(slot_no)
#define _ULSCPP_USTR2NSTR(ustr, astr, slot_no) ((astr) = (ustr),uls_strlen(astr))
#define _ULSCPP_NSTR2USTR(astr, ustr, slot_no) ((ustr) = (astr),uls_strlen(ustr))
#endif
#endif // _ULSCPP_IMPLDLL

#define nilptr  uls_nil

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_TYPE_H__
