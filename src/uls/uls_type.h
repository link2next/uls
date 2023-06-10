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

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_const.h"
#if !defined(ULS_DOTNET) && !defined(USE_ULSNETJAVA)
#ifdef ULS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/types.h>
#endif
#endif
#endif

#ifdef _ULS_CPLUSPLUS
extern "C" {
#endif

#ifdef ULS_DECL_BASIC_TYPES
#ifdef ULS_DOTNET
typedef System::Int16    uls_int16;
typedef System::Int32    uls_int32;
typedef System::Int64    uls_int64;
typedef System::UInt16   uls_uint16;
typedef System::UInt32   uls_uint32;
typedef System::UInt64   uls_uint64;

typedef System::Object ^uls_voidptr_t;
#else
#ifdef ULS_WINDOWS
typedef INT16       uls_int16;
typedef INT32       uls_int32;
typedef INT64       uls_int64;
typedef UINT16      uls_uint16;
typedef UINT32      uls_uint32;
typedef UINT64      uls_uint64;
#else
typedef int16_t     uls_int16;
typedef int32_t     uls_int32;
typedef int64_t     uls_int64;
typedef u_int16_t   uls_uint16;
typedef u_int32_t   uls_uint32;
typedef u_int64_t   uls_uint64;
#endif
typedef void   *uls_voidptr_t;
#endif // ULS_DOTNET

typedef uls_uint32  uls_flags_t;
typedef uls_uint32  uls_uch_t;
#define ULS_UCH_EOS 0
typedef void *uls_native_vptr_t;

#endif // ULS_DECL_BASIC_TYPES

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
#define uls_streql(a,b)  (_uls_tool_(strcmp)((const char*)(a),(const char*)(b))==0)

#ifdef ULS_DOTNET
#define uls_nil  nullptr
#define uls_ptr(a) %(a)
#define __uls_def_ptrvar(avar) ^avar

#define _uls_type_this(ns,typnam) typnam##_t
#define _uls_ptrtype_this(ns,typnam) typnam##_ptr_t
#define _uls_type_this_(ns,typnam) uls_##typnam##_t
#define _uls_ptrtype_this_(ns,typnam) uls_##typnam##_ptr_t

#define _uls_type_ns(ns,typnam) ns::typnam##_t
#define _uls_ptrtype_ns(ns,typnam) ns::typnam##_ptr_t
#define _uls_type_ns_(ns,typnam) ns::uls_##typnam##_t
#define _uls_ptrtype_ns_(ns,typnam) ns::uls_##typnam##_ptr_t

#define ULS_DECLARE_STRUCT_BRIEF(typnam) ref struct typnam##_t
#define ULS_DEFINE_STRUCT_PTR(typnam) typedef ref struct typnam##_t ^typnam##_ptr_t
#define ULS_DECLARE_STRUCT(typnam) ULS_DECLARE_STRUCT_BRIEF(typnam); ULS_DEFINE_STRUCT_PTR(typnam)
#define ULS_DEFINE_STRUCT(typnam) ULS_DECLARE_STRUCT(typnam); ULS_DEFINE_STRUCT_BEGIN(typnam)
#define ULS_DEFINE_STRUCT_BEGIN(typnam) ULS_DECLARE_STRUCT_BRIEF(typnam)

#define __uls_initial_zerofy_object(obj)
#define uls_alloc_object(typ) gcnew typ()
#define uls_alloc_object_clear(typ) uls_alloc_object(typ)
#define uls_dealloc_object(obj) do { (obj) = nullptr; } while (0)

// delegate
#define _ULS_DELEGATE_NAME(typnam) _uls_delegate_##typnam
#define ULS_DELEGATE_NAME(typnam) uls_##typnam##_t
#define ULS_DEFINE_DELEGATE_BEGIN(typnam,rtype) delegate rtype _ULS_DELEGATE_NAME(typnam)
#define ULS_DEFINE_DELEGATE_END(typnam) typedef _ULS_DELEGATE_NAME(typnam) ^ULS_DELEGATE_NAME(typnam)

#define _uls_callback_type_(clsnam,typnam) clsnam::ULS_DELEGATE_NAME(typnam)
#define _uls_callback_type_this_(typnam) ULS_DELEGATE_NAME(typnam)

#define uls_alloc_callback_this(clsnam,typnam,mthnam) gcnew clsnam::_ULS_DELEGATE_NAME(typnam)(this,&ULS_QUALIFIED_METHOD(mthnam))
#define uls_dealloc_callback(cb_proc) delete cb_proc

#define _uls_nameof_cb_varnam(mthnam) _cb_##mthnam
#define _uls_alloc_callback(mthnam,typnam) _uls_nameof_cb_varnam(mthnam) = gcnew _ULS_DELEGATE_NAME(typnam)(this,&ULS_QUALIFIED_METHOD(mthnam))
#define _uls_alloc_ns_callback(mthnam,clsnam,typnam) _uls_nameof_cb_varnam(mthnam) = gcnew clsnam::_ULS_DELEGATE_NAME(typnam)(this,&ULS_QUALIFIED_METHOD(mthnam))
#define _uls_dealloc_callback(mthnam) delete _uls_nameof_cb_varnam(mthnam)

#define _uls_ref_callback(o,mthnam) (o)->_uls_nameof_cb_varnam(mthnam)
#define _uls_ref_callback_this(mthnam) _uls_nameof_cb_varnam(mthnam)

#define _uls_def_callback(mthnam,clsnam,typnam) _uls_callback_type_(clsnam,typnam) _uls_nameof_cb_varnam(mthnam)
#define _uls_def_callback_this(mthnam,typnam) _uls_callback_type_this_(typnam) _uls_nameof_cb_varnam(mthnam)

#define _uls_ref_callback_tool_(mthnam) _tool_->_uls_nameof_cb_varnam(mthnam)
#define _uls_callback_type_tool_(typnam) UlsToolbase::ULS_DELEGATE_NAME(typnam)
#define _uls_def_callback_tool_(mthnam,typnam) _uls_callback_type_(UlsToolbase,typnam) _uls_nameof_cb_varnam(mthnam)

// arraytype
#define _uls_type_array(ptrtyp) array<ptrtyp>^
#define _uls_decl_array(name,ptrtyp) array<ptrtyp>^name
#define _uls_cast_array_type(ptrtyp) (array<ptrtyp>^)
#define _uls_init_array(n,ptrtyp) gcnew array<ptrtyp>(n)
#define _uls_deinit_array(ary) do { delete ary; (ary) = nullptr; } while (0)
#define _uls_resize_array(ary,ptrtyp,n) Array::Resize(ary,n)

#define _uls_type_array_ns(ns,ptrtyp) _uls_type_array(ns::ptrtyp)
#define _uls_decl_array_ns(name,ns,ptrtyp) _uls_decl_array(name,ns::ptrtyp)
#define _uls_cast_array_type_ns(ns,ptrtyp) _uls_cast_array_type(ns::ptrtyp)
#define _uls_init_array_ns(n,ns,ptrtyp) _uls_init_array(n,ns::ptrtyp)
#define _uls_resize_array_ns(ary,ns,ptrtyp,n) _uls_resize_array(ary,ns::ptrtyp,n)

// ptrarray
#define _uls_type_ptrarray(typ) _uls_type_array(typ^)
#define _uls_decl_ptrarray(name,typ) _uls_decl_array(name,typ^)
#define _uls_cast_ptrarray_type(typ) _uls_cast_array_type(typ^)
#define _uls_ref_ptrarray(name,typ) _uls_decl_array(name,typ^)
#define _uls_ref_ptrarray_init(name,typ,val) _uls_ref_ptrarray(name,typ)=val
#define _uls_init_ptrarray(n,typ) _uls_init_array(n,typ^)
#define _uls_deinit_ptrarray(ary) _uls_deinit_array(ary)
#define _uls_resize_ptrarray(ary,typ,n)  _uls_resize_array(ary,typ,n)

#define _uls_type_ptrarray_ns(ns,typ) _uls_type_ptrarray(ns::typ)
#define _uls_decl_ptrarray_ns(name,ns,typ) _uls_decl_ptrarray(name,ns::typ)
#define _uls_cast_ptrarray_type_ns(ns,typ) _uls_cast_ptrarray_type(ns::typ)
#define _uls_ref_ptrarray_ns(name,ns,typ) _uls_decl_ptrarray(name,ns::typ)
#define _uls_ref_ptrarray_init_ns(name,ns,typ,val) _uls_ref_ptrarray(name,ns::typ)=val
#define _uls_init_ptrarray_ns(n,ns,typ) _uls_init_ptrarray(n,ns::typ)
#define _uls_resize_ptrarray_ns(ary,ns,typ,n) _uls_resize_ptrarray(ary,ns::typ,n)

#else // ULS_DOTNET

#define uls_nil  NULL
#define uls_ptr(a) &(a)
#define __uls_def_ptrvar(avar) *avar

#define _uls_type_this(ns,typnam) typnam##_t
#define _uls_ptrtype_this(ns,typnam) typnam##_ptr_t
#define _uls_type_this_(ns,typnam) uls_##typnam##_t
#define _uls_ptrtype_this_(ns,typnam) uls_##typnam##_ptr_t

#define _uls_type_ns(ns,typnam) _uls_type_this(ns,typnam)
#define _uls_ptrtype_ns(ns,typnam) _uls_ptrtype_this(ns,typnam)
#define _uls_type_ns_(ns,typnam) _uls_type_this_(ns,typnam)
#define _uls_ptrtype_ns_(ns,typnam) _uls_ptrtype_this_(ns,typnam)

#define _uls_tool_type(typ) typ##_t
#define _uls_tool_ptrtype(typ) typ##_ptr_t
#define _uls_tool_type_(typ) uls_##typ##_t
#define _uls_tool_ptrtype_(typ) uls_##typ##_ptr_t
#define _uls_tool_ptrtype_cast_(typ,var1,var2) _uls_tool_ptrtype_(typ) var1 = (_uls_tool_ptrtype_(typ)) (var2)

#define ULS_DECLARE_STRUCT_BRIEF(name) struct _ ## name
#define ULS_DEFINE_STRUCT_PTR(name) typedef struct _##name  *name##_ptr_t
#define ULS_DECLARE_STRUCT(name) typedef struct _ ## name name##_t; ULS_DEFINE_STRUCT_PTR(name)
#define ULS_DEFINE_STRUCT(name) ULS_DECLARE_STRUCT(name); ULS_DECLARE_STRUCT_BRIEF(name)
#define ULS_DEFINE_STRUCT_BEGIN(name) ULS_DECLARE_STRUCT_BRIEF(name)

#define __uls_initial_zerofy_object(obj) uls_bzero(obj,sizeof(*(obj)))
#define uls_alloc_object(typ) (typ*)uls_malloc(sizeof(typ))
#define uls_alloc_object_clear(typ) (typ*)uls_malloc_clear(sizeof(typ))
#define uls_dealloc_object(obj) uls_mfree(obj)

// delegate
#define ULS_DELEGATE_NAME(typnam) uls_##typnam##_t
#define ULS_DEFINE_DELEGATE_BEGIN(typnam,rtype) typedef rtype (*ULS_DELEGATE_NAME(typnam))
#define ULS_DEFINE_DELEGATE_END(typnam)

#define _uls_callback_type_(clsnam,typnam) ULS_DELEGATE_NAME(typnam)
#define _uls_callback_type_this_(typnam) ULS_DELEGATE_NAME(typnam)

#define uls_alloc_callback_this(clsnam,typnam,mthnam) mthnam
#define uls_dealloc_callback(cb_proc)

#define _uls_ref_callback(o,mthnam) mthnam
#define _uls_ref_callback_this(mthnam) mthnam

// arraytype
#define _uls_type_array(ptrtyp) ptrtyp*
#define _uls_decl_array(name,ptrtyp) _uls_type_array(ptrtyp)name
#define _uls_cast_array_type(ptrtyp) (_uls_type_array(ptrtyp))
#define _uls_init_array(n,ptrtyp) (ptrtyp*)uls_malloc_clear((n)*sizeof(ptrtyp))
#define _uls_deinit_array(ary) uls_mfree(ary)
#define _uls_resize_array(ary,ptrtyp,n) (ary)=(ptrtyp*)uls_mrealloc(ary,(n)*sizeof(ptrtyp))

#define _uls_type_array_ns(ns,ptrtyp) _uls_type_array(ptrtyp)
#define _uls_decl_array_ns(name,ns,ptrtyp) _uls_decl_array(name,ptrtyp)
#define _uls_cast_array_type_ns(ns,ptrtyp) _uls_cast_array_type(ptrtyp)
#define _uls_init_array_ns(n,ns,ptrtyp) _uls_init_array(n,ptrtyp)
#define _uls_resize_array_ns(ary,ns,ptrtyp,n) _uls_resize_array(ary,ptrtyp,n)

// ptrarray
#define _uls_type_ptrarray(typ) _uls_type_array(typ*)
#define _uls_decl_ptrarray(name,typ) _uls_decl_array(name,typ*)
#define _uls_cast_ptrarray_type(typ) _uls_cast_array_type(typ*)
#define _uls_ref_ptrarray(name,typ) _uls_decl_array(name,typ*)
#define _uls_ref_ptrarray_init(name,typ,val) _uls_ref_ptrarray(name,typ)=(val)
#define _uls_init_ptrarray(n,typ) _uls_init_array(n,typ*)
#define _uls_deinit_ptrarray(ary) _uls_deinit_array(ary)
#define _uls_resize_ptrarray(ary,typ,n) _uls_resize_array(ary,typ*,n)

#define _uls_type_ptrarray_ns(ns,typ) _uls_type_ptrarray(typ)
#define _uls_decl_ptrarray_ns(name,ns,typ) _uls_decl_ptrarray(name,typ)
#define _uls_cast_ptrarray_type_ns(ns,typ) _uls_cast_ptrarray_type(typ)
#define _uls_ref_ptrarray_ns(name,ns,typ) _uls_decl_ptrarray(name,typ)
#define _uls_ref_ptrarray_init_ns(name,ns,typ,val) _uls_ref_ptrarray_init(name,typ,val)
#define _uls_init_ptrarray_ns(n,ns,typ) _uls_init_ptrarray(n,typ)
#define _uls_resize_ptrarray_ns(ary,ns,typ,n) _uls_resize_ptrarray(ary,typ,n)

#define _uls_log_primitive(proc) proc##_primitive
#define _uls_log_static(proc) _uls_log_primitive(proc)

#define _uls_log(proc) proc
#define _uls_log_(proc) uls_##proc

#define _uls_tool(proc) proc
#define _uls_tool_(proc) uls_##proc
#define __uls_tool_(proc) _uls_##proc

#endif // ULS_DOTNET

#define ULS_DEF_PARRAY(typmac,ns,typnam) \
	ULS_DEFINE_STRUCT_BEGIN(uls_##typnam##_parray) { _uls_decl_ptrarray(slots,typmac(ns,typnam)); int n, n_alloc; }; \
	ULS_DECLARE_STRUCT(uls_##typnam##_parray)
#define ULS_DEF_PARRAY_THIS(typnam) ULS_DEF_PARRAY(_uls_type_this_,none,typnam)

#define uls_decl_parray(name,typnam) uls_##typnam##_parray_t name
#define uls_parray_ptrtype(typnam) uls_##typnam##_parray_ptr_t
#define uls_cast_parray_ptrtype(typnam) (uls_parray_ptrtype(typnam))
#define uls_move_parray(a,b) do { \
		(b)->slots = (a)->slots; (a)->slots = nilptr; \
		(b)->n = (a)->n; (a)->n = 0; \
		(b)->n_alloc = (a)->n_alloc; (a)->n_alloc = 0; \
	} while (0)
#define uls_ref_parray(name,typnam) uls_parray_ptrtype(typnam) name
#define uls_ref_parray_init(name,typnam,val) uls_ref_parray(name,typnam)=(val)
#define uls_init_parray(a,typnam,nn) do { \
		(a)->slots = _uls_init_ptrarray(nn, uls_##typnam##_t); (a)->n = 0; (a)->n_alloc = nn; \
	} while(0)
#define uls_deinit_parray(a) do { \
		_uls_deinit_ptrarray((a)->slots); (a)->slots = nilptr; \
		(a)->n = (a)->n_alloc = 0; \
	} while(0)

#define uls_decl_parray_ptr(a,typnam) uls_##typnam##_parray_ptr_t a;
#define uls_alloc_parray_ptr(a,typnam,nn) do { \
		(a)=uls_alloc_object(uls_##typnam##_parray_t); \
		uls_init_parray(a,typnam,nn); \
	} while(0)
#define uls_dealloc_parray_ptr(a) do { \
		uls_deinit_parray(a); \
		uls_dealloc_object(a); \
	} while(0)

#define uls_resize_parray(a,typnam,nn) do { \
		_uls_resize_ptrarray((a)->slots,uls_##typnam##_t,nn); \
		if ((nn) < (a)->n) (a)->n = (nn); (a)->n_alloc = (nn); \
	} while (0)
#define uls_parray_slots(b) ((b)->slots)
#define uls_decl_parray_slots(a,typnam) _uls_ref_ptrarray(a,uls_##typnam##_t)
#define uls_decl_parray_slots_init(a,typnam,b) uls_decl_parray_slots(a,typnam)=uls_parray_slots(b)

#ifdef ULS_DOTNET
#define uls_decl_parray_ns(name,ns,typnam) ns::uls_##typnam##_parray_t name
#define uls_decl_parray_ptr_ns(a,ns,typnam) ns::uls_##typnam##_parray_ptr_t a
#define uls_parray_ptrtype_ns(ns,typnam) ns::uls_##typnam##_parray_ptr_t
#define uls_cast_parray_ptrtype_ns(ns,typnam) (uls_parray_ptrtype_ns(ns,typnam))
#define uls_ref_parray_ns(name,ns,typnam) uls_parray_ptrtype_ns(ns,typnam) name
#define uls_ref_parray_init_ns(name,ns,typnam,val) uls_ref_parray_ns(name,ns,typnam)=(val)
#define uls_init_parray_ns(a,ns,typnam,nn) do { \
		(a)->slots = _uls_init_ptrarray_ns(nn, ns, uls_##typnam##_t); (a)->n = 0; (a)->n_alloc = (nn); \
	} while(0)

#define uls_alloc_parray_ptr_ns(a,ns,typnam,nn) do { \
		(a)=uls_alloc_object(ns::uls_##typnam##_parray_t); \
		uls_init_parray_ns(a,ns,typnam,nn); \
	} while(0)

#define uls_resize_parray_ns(a,ns,typnam,nn) do { \
		_uls_resize_ptrarray_ns((a)->slots,ns,uls_##typnam##_t,nn); \
		if ((nn) < (a)->n) (a)->n = (nn); (a)->n_alloc = (nn); \
	} while (0)

#define uls_decl_parray_slots_ns(a,ns,typnam) _uls_ref_ptrarray_ns(a,ns,uls_##typnam##_t)
#define uls_decl_parray_slots_init_ns(a,ns,typnam,b) uls_decl_parray_slots_ns(a,ns,typnam)=uls_parray_slots(b)

#define _uls_decl_parray_tool_(name,typnam)  uls_decl_parray_ns(name,UlsToolbase,typnam)
#define _uls_decl_parray_ptr_tool_(a,typnam)  uls_decl_parray_ptr_ns(a,UlsToolbase,typnam)
#define _uls_parray_ptrtype_tool_(typnam) uls_parray_ptrtype_ns(UlsToolbase,typnam)
#define _uls_cast_parray_ptrtype_tool_(typnam) uls_cast_parray_ptrtype_ns(UlsToolbase,typnam)
#define _uls_ref_parray_tool_(name,typnam) uls_ref_parray_ns(name,UlsToolbase,typnam)
#define _uls_ref_parray_init_tool_(name,typnam,val) uls_ref_parray_init_ns(name,UlsToolbase,typnam,val)
#define _uls_init_parray_tool_(a,typnam,nn) uls_init_parray_ns(a,UlsToolbase,typnam,nn)

#define _uls_alloc_parray_ptr_tool_(a,typnam,nn) uls_alloc_parray_ptr_ns(a,UlsToolbase,typnam,nn)
#define _uls_resize_parray_tool_(a,typnam,nn) uls_resize_parray_ns(a,UlsToolbase,typnam,nn)

#define _uls_decl_parray_slots_tool_(a,typnam) uls_decl_parray_slots_ns(a,UlsToolbase,typnam)
#define _uls_decl_parray_slots_init_tool_(a,typnam,b) uls_decl_parray_slots_init_ns(a,UlsToolbase,typnam,b)

#else

#define uls_decl_parray_ns(name,ns,typnam) uls_decl_parray(name,typnam)
#define uls_decl_parray_ptr_ns(a,ns,typnam) uls_decl_parray_ptr(a,typnam)
#define uls_parray_ptrtype_ns(ns,typnam) uls_parray_ptrtype(typnam)
#define uls_cast_parray_ptrtype_ns(ns,typnam) uls_cast_parray_ptrtype(typnam)
#define uls_ref_parray_ns(name,ns,typnam) uls_ref_parray(name,typnam)
#define uls_ref_parray_init_ns(name,ns,typnam,val) uls_ref_parray_init(name,typnam,val)
#define uls_init_parray_ns(a,ns,typnam,nn) uls_init_parray(a,typnam,nn)

#define uls_alloc_parray_ptr_ns(a,ns,typnam,nn) uls_alloc_parray_ptr(a,typnam,nn)
#define uls_resize_parray_ns(a,ns,typnam,nn) uls_resize_parray(a,typnam,nn)

#define uls_decl_parray_slots_ns(a,ns,typnam) uls_decl_parray_slots(a,typnam)
#define uls_decl_parray_slots_init_ns(a,ns,typnam,b) uls_decl_parray_slots_init(a,typnam,b)

#define _uls_decl_parray_tool_(name,typnam)  uls_decl_parray(name,typnam)
#define _uls_decl_parray_ptr_tool_(a,typnam)  uls_decl_parray_ptr(a,typnam)
#define _uls_parray_ptrtype_tool_(typnam) _uls_parray_ptrtype(typnam)
#define _uls_cast_parray_ptrtype_tool_(typnam) uls_cast_parray_ptrtype(typnam)
#define _uls_ref_parray_tool_(name,typnam) uls_ref_parray(name,typnam)
#define _uls_ref_parray_init_tool_(name,typnam,val) uls_ref_parray_init(name,typnam,val)
#define _uls_init_parray_tool_(a,typnam,nn) uls_init_parray(a,typnam,nn)

#define _uls_alloc_parray_ptr_tool_(a,typnam,nn) uls_alloc_parray_ptr(a,typnam,nn)
#define _uls_resize_parray_tool_(a,typnam,nn) uls_resize_parray(a,typnam,nn)

#define _uls_decl_parray_slots_tool_(a,typnam) uls_decl_parray_slots(a,typnam)
#define _uls_decl_parray_slots_init_tool_(a,typnam,b) uls_decl_parray_slots_init(a,typnam,b)

#endif // ULS_DOTNET


#ifdef _ULS_IMPLDLL
#define nilptr  uls_nil
#endif

#ifdef ULS_CLASSIFY_SOURCE
// name-buffer
#define _uls_def_namebuf(nam,siz) _uls_tool_type_(nambuf) nam
#define _uls_init_namebuf(nam,siz) _uls_tool_(init_nambuf)(uls_ptr(nam), siz)
#define _uls_deinit_namebuf(nam) _uls_tool_(deinit_nambuf)(uls_ptr(nam))
#define _uls_get_namebuf_value(nam) (nam.str)
#define _uls_get_namebuf_length(nam) (nam.len)
#define _uls_set_namebuf_value(nam,str) _uls_tool_(set_nambuf)(uls_ptr(nam),str,-1)
#define _uls_set_namebuf_value_2(nam,str,len) _uls_tool_(set_nambuf)(uls_ptr(nam),str,len)

// bytes pool
#define _uls_def_bytespool(nam,siz) char*nam
#define _uls_ref_bytespool(nam) char*nam
#define _uls_init_bytespool(nam,siz,clear) (nam) = clear ? uls_malloc_buffer_clear(siz) : uls_malloc_buffer(siz)
#define _uls_deinit_bytespool(nam) uls_mfree(nam)

// standard array
#define _uls_get_stdary_slot(ary,idx) (ary)[idx]
#define _uls_set_stdary_slot(ary,idx,obj) (ary)[idx]=obj
#define _uls_alloc_stdary_slot(ary,idx,typmac,ns,typnam) do { \
		typmac(ns,typnam) __uls_def_ptrvar(slot); \
		(ary)[idx]=slot=uls_alloc_object_clear(typmac(ns,typnam)); \
		uls_init_##typnam(slot); \
	} while (0)

#define _uls_dealloc_stdary_slot(ary,idx,typmac,ns,typnam) do { \
		typmac(ns,typnam) __uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		if ((slot) != nilptr) { uls_deinit_##typnam(slot); uls_dealloc_object(slot); _uls_set_stdary_slot(ary,idx,nilptr); } \
	} while (0)

#define _uls_alloc_stdary_slot_tool_(ary,idx,typmac,typnam) do { \
		typmac(UlsToolbase,typnam) __uls_def_ptrvar(slot); \
		(ary)[idx]=slot=uls_alloc_object(typmac(UlsToolbase,typnam)); \
		_tool_->uls_init_##typnam(slot); \
	} while (0)

#define _uls_dealloc_stdary_slot_tool_(ary,idx,typmac,typnam) do { \
		typmac(UlsToolbase,typnam) __uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		_tool_->uls_deinit_##typnam(slot); uls_dealloc_object(slot); \
	} while (0)

// type01
#define _ULS_DEF_ARRAY_TYPE01(typmac,ns,typnam,NN) ULS_DEF_PARRAY(typmac,ns,typnam)
#define _uls_decl_array_type01(ary,typmac,ns,typnam,NN) uls_decl_parray(ary,typnam)
#define _uls_init_array_type01(ary,typmac,ns,typnam,NN) do { int i; \
		(ary)->slots = _uls_init_ptrarray(NN, typmac(ns,typnam)); (ary)->n = (ary)->n_alloc = (NN); \
		for (i=0; i<(NN); i++) { (ary)->slots[i]=uls_alloc_object_clear(typmac(ns,typnam)); } \
	} while (0)
#define _uls_deinit_array_type01(ary,typmac,ns,typnam) do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_dealloc_object((ary)->slots[i]); } \
		_uls_deinit_ptrarray((ary)->slots); \
	} while (0)
#define _uls_array_ref_slots_type01(slots,typmac,ns,typnam) _uls_decl_ptrarray(slots,typmac(ns,typnam))
#define uls_array_slots_type01(ary) ((ary)->slots)
#define uls_get_array_slot_type01(ary,idx) _uls_get_stdary_slot((ary)->slots,idx)

// type02
#define _ULS_DEF_ARRAY_TYPE02(typmac,ns,typnam) ULS_DEF_PARRAY(typmac,ns,typnam)
#define _uls_decl_array_type02(ary,typmac,ns,typnam) uls_decl_parray(ary,typnam)
#define _uls_cast_array_type02(typmac,ns,typnam) uls_cast_parray_ptrtype(typnam)
#define _uls_ref_array_type02(ary,typmac,ns,typnam) uls_ref_parray(ary,typnam)
#define _uls_ref_array_init_type02(ary,typmac,ns,typnam,ary2) _uls_ref_array_type02(ary,typmac,ns,typnam)=ary2
#define _uls_init_array_type02(ary,typmac,ns,typnam,nn) do { int i; \
		(ary)->slots = _uls_init_ptrarray(nn, typmac(ns,typnam)); (ary)->n = (ary)->n_alloc = (nn); \
		for (i=0; i<(nn); i++) { _uls_alloc_stdary_slot((ary)->slots,i,typmac,ns,typnam); } \
	} while (0)
#define _uls_deinit_array_type02(ary,typmac,ns,typnam) do { int i; \
		for (i=0; i<(ary)->n; i++) { _uls_dealloc_stdary_slot((ary)->slots,i,typmac,ns,typnam); } \
		_uls_deinit_ptrarray((ary)->slots); (ary)->slots = nilptr; \
	} while (0)
#define uls_array_slots_type02(ary) ((ary)->slots)
#define uls_get_array_slot_type02(ary,idx) _uls_get_stdary_slot(uls_array_slots_type02(ary),idx)

// type10
#define _ULS_DEF_ARRAY_TYPE10(typmac,ns,typnam) ULS_DEF_PARRAY(typmac,ns,typnam)
#define _uls_decl_array_type10(ary,ns,typnam) uls_decl_parray(ary,typnam)
#define _uls_cast_array_type10(ns,typnam) uls_cast_parray_ptrtype(typnam)
#define _uls_ref_array_type10(ary,ns,typnam) uls_ref_parray(ary,typnam)
#define _uls_ref_array_init_type10(ary,ns,typnam,ary2) _uls_ref_array_type10(ary,ns,typnam)=ary2
#define _uls_decl_array_type10_alloc(ary,ns,typnam) _uls_ref_array_type10(ary,ns,typnam)=uls_alloc_object_clear(uls_##typnam##_parray_t)
#define _uls_move_array_type10(a,b) uls_move_parray(a,b)

#define _uls_alloc_array_slot_type10(ary,typmac,ns,typnam,idx) _uls_alloc_stdary_slot((ary)->slots,idx,typmac,ns,typnam)
#define _uls_dealloc_array_slot_type10(ary,typmac,ns,typnam,idx) _uls_dealloc_stdary_slot((ary)->slots,idx,typmac,ns,typnam)

#define _uls_init_array_type10(ary,ns,typnam,nn) uls_init_parray(ary,typnam,nn)
#define _uls_deinit_array_type10(ary,typmac,ns,typnam) do { \
		_uls_resize_array_type10(ary,typmac,ns,typnam,0); \
		uls_deinit_parray(ary); \
	} while (0)
#define _uls_resize_array_type10(ary,typmac,ns,typnam,nn_alloc) do { \
		int i; for (i=(nn_alloc); i<(ary)->n; i++) { _uls_dealloc_stdary_slot((ary)->slots,i,typmac,ns,typnam); } \
		uls_resize_parray(ary,typnam,nn_alloc); \
	} while (0)

#define uls_array_slots_type10(a) (a)->slots
#define uls_decl_array_slots_type10(a,typnam) _uls_decl_ptrarray(a,uls_##typnam##_t)
#define uls_decl_array_slots_init_type10(a,typnam,b) uls_decl_array_slots_type10(a,typnam)=uls_array_slots_type10(b)
#define uls_get_array_slot_type10(a,idx) uls_get_array_this_slot(uls_array_slots_type10(a),idx)

#else // ~ULS_CLASSIFY_SOURCE

// name-buffer
#define _uls_def_namebuf(nam,siz) char  nam[siz+1]
#define _uls_init_namebuf(nam,siz) nam[0]='\0'
#define _uls_deinit_namebuf(nam)
#define _uls_get_namebuf_value(nam) (nam)
#define _uls_get_namebuf_length(nam) uls_strlen(nam)
#define _uls_set_namebuf_value(nam,str) uls_set_nambuf_raw(nam,sizeof(nam),str,-1)
#define _uls_set_namebuf_value_2(nam,str,len) uls_set_nambuf_raw(nam,sizeof(nam),str,len)

// bytes pool
#define _uls_def_bytespool(nam,siz) char nam[siz]
#define _uls_ref_bytespool(nam) char*nam
#define _uls_init_bytespool(nam,siz,clear) do { if (clear) { uls_bzero(nam,siz); } } while (0)
#define _uls_deinit_bytespool(nam)

// standard array
#define _uls_get_stdary_slot(ary,idx) ((ary)+(idx))
#define _uls_set_stdary_slot(ary,idx,obj) (ary)[idx]=*(obj);
#define _uls_alloc_stdary_slot(ary,idx,typmac,ns,typnam) uls_init_##typnam(_uls_get_stdary_slot(ary,idx))
#define _uls_dealloc_stdary_slot(ary,idx,typmac,ns,typnam) uls_deinit_##typnam(_uls_get_stdary_slot(ary,idx))
#define _uls_alloc_stdary_slot_tool_(ary,idx,typmac,typnam) uls_init_##typnam(_uls_get_stdary_slot(ary,idx))
#define _uls_dealloc_stdary_slot_tool_(ary,idx,typmac,typnam) uls_deinit_##typnam(_uls_get_stdary_slot(ary,idx))

// type01
#define _ULS_DEF_ARRAY_TYPE01(typmac,ns,typnam,NN)  \
	ULS_DEFINE_STRUCT_BEGIN(uls_##typnam##_s01array##NN) { typmac(ns,typnam) slots[NN]; int n; }; \
	ULS_DECLARE_STRUCT(uls_##typnam##_s01array##NN)
#define _uls_decl_array_type01(ary,typmac,ns,typnam,NN) uls_##typnam##_s01array##NN##_t ary
#define _uls_init_array_type01(ary,typmac,ns,typnam,NN) do { \
	uls_bzero((ary)->slots,(NN)*sizeof(typmac(ns,typnam))); (ary)->n = NN; \
	} while (0)
#define _uls_deinit_array_type01(ary,typmac,ns,typnam)
#define _uls_array_ref_slots_type01(slots,typmac,ns,typnam) uls_##typnam##_ptr_t slots
#define uls_array_slots_type01(ary) ((ary)->slots)
#define uls_get_array_slot_type01(ary,idx) ((ary)->slots + (idx))

// type02
#define _ULS_DEF_ARRAY_TYPE02(typmac,ns,typnam)  \
	ULS_DEFINE_STRUCT_BEGIN(uls_##typnam##_s02array) { typmac(ns,typnam) __uls_def_ptrvar(slots); int n; }; \
	ULS_DECLARE_STRUCT(uls_##typnam##_s02array)
#define _uls_decl_array_type02(ary,typmac,ns,typnam) uls_##typnam##_s02array_t ary
#define _uls_cast_array_type02(typmac,ns,typnam) (uls_##typnam##_s02array_ptr_t)
#define _uls_ref_array_type02(ary,typmac,ns,typnam) uls_##typnam##_s02array_ptr_t ary
#define _uls_ref_array_init_type02(ary,typmac,ns,typnam,ary2) _uls_ref_array_type02(ary,typmac,ns,typnam)=ary2
#define _uls_init_array_type02(ary,typmac,ns,typnam,nn) do { int i; \
		(ary)->slots=(uls_##typnam##_ptr_t)uls_malloc((nn)*sizeof(uls_##typnam##_t)); \
		(ary)->n = nn; for (i=0; i<(nn); i++) { uls_init_##typnam(uls_get_array_slot_type02(ary,i)); } \
	} while (0)
#define _uls_deinit_array_type02(ary,typmac,ns,typnam) do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_deinit_##typnam(uls_get_array_slot_type02(ary,i)); } \
		uls_mfree((ary)->slots); (ary)->n = 0; \
	} while (0)

#define uls_array_slots_type02(ary) ((ary)->slots)
#define uls_get_array_slot_type02(ary,idx) ((ary)->slots + (idx))

// type10
#define _ULS_DEF_ARRAY_TYPE10(typmac,ns,typnam)  \
	ULS_DEFINE_STRUCT_BEGIN(uls_##typnam##_s10array) { typmac(ns,typnam) __uls_def_ptrvar(slots); int n, n_alloc; }; \
	ULS_DECLARE_STRUCT(uls_##typnam##_s10array)
#define _uls_decl_array_type10(ary,ns,typnam) uls_##typnam##_s10array_t ary
#define _uls_cast_array_type10(ns,typnam) (uls_##typnam##_s10array_ptr_t)
#define _uls_ref_array_type10(ary,ns,typnam) uls_##typnam##_s10array_ptr_t ary
#define _uls_ref_array_init_type10(ary,ns,typnam,ary2) _uls_ref_array_type10(ary,ns,typnam)=ary2
#define _uls_decl_array_type10_alloc(ary,ns,typnam) _uls_ref_array_type10(ary,ns,typnam)=uls_alloc_object_clear(uls_##typnam##_s10array_t)
#define _uls_move_array_type10(a,b) do { \
		(b)->slots = (a)->slots; (a)->slots = nilptr; \
		(b)->n = (a)->n; (a)->n = 0; \
		(b)->n_alloc = (a)->n_alloc; (a)->n_alloc= 0; \
	} while (0)

#define _uls_alloc_array_slot_type10(ary,typmac,ns,typnam,idx) _uls_alloc_stdary_slot((ary)->slots,idx,typmac,ns,typnam)
#define _uls_dealloc_array_slot_type10(ary,typmac,ns,typnam,idx) _uls_dealloc_stdary_slot((ary)->slots,idx,typmac,ns,typnam)

#define _uls_init_array_type10(ary,ns,typnam,nn) do { \
		(ary)->slots=(uls_##typnam##_ptr_t)uls_malloc_clear((nn)*sizeof(uls_##typnam##_t)); \
		(ary)->n = 0; (ary)->n_alloc = (nn); \
	} while (0)
#define _uls_deinit_array_type10(ary,typmac,ns,typnam) do { \
		_uls_resize_array_type10(ary,typmac,ns,typnam,0); \
		uls_mfree((ary)->slots); (ary)->n = (ary)->n_alloc = 0; \
	} while (0)
#define _uls_resize_array_type10(ary,typmac,ns,typnam,nn_alloc) do { \
		int i; for (i=(nn_alloc); i<(ary)->n; i++) { _uls_dealloc_stdary_slot((ary)->slots,i,none,ns,typnam); } \
		(ary)->slots=(uls_##typnam##_ptr_t)uls_mrealloc((ary)->slots,(nn_alloc)*sizeof(uls_##typnam##_t)); \
		if ((nn_alloc) < (ary)->n) (ary)->n = (nn_alloc); (ary)->n_alloc = (nn_alloc); \
	} while (0)

#define uls_array_slots_type10(a) (a)->slots
#define uls_decl_array_slots_type10(a,typnam) uls_##typnam##_ptr_t a
#define uls_decl_array_slots_init_type10(a,typnam,b) uls_decl_array_slots_type10(a,typnam)=uls_array_slots_type10(b)
#define uls_get_array_slot_type10(a,idx) uls_get_array_this_slot(uls_array_slots_type10(a),idx)

#endif // ULS_CLASSIFY_SOURCE

// standard array
#define uls_get_array_this_slot(ary,idx) _uls_get_stdary_slot(ary,idx)
#define uls_set_array_this_slot(ary,idx,obj) _uls_set_stdary_slot(ary,idx,obj)
#define uls_alloc_array_this_slot(ary,idx,typnam) _uls_alloc_stdary_slot(ary,idx,_uls_type_this_,none,typnam)
#define uls_dealloc_array_this_slot(ary,idx,typnam) _uls_dealloc_stdary_slot(ary,idx,_uls_type_this_,none,typnam)

// type01
#define ULS_DEF_ARRAY_THIS_TYPE01(typnam,NN) _ULS_DEF_ARRAY_TYPE01(_uls_type_this_,none,typnam,NN)
#define uls_decl_array_this_type01(ary,typnam,NN) _uls_decl_array_type01(ary,_uls_type_this_,none,typnam,NN)
#define uls_init_array_this_type01(ary,typnam,NN) _uls_init_array_type01(ary,_uls_type_this_,none,typnam,NN)
#define uls_deinit_array_this_type01(ary,typnam) _uls_deinit_array_type01(ary,_uls_type_this_,none,typnam)
#define uls_array_ref_slots_this_type01(slots,typnam) _uls_array_ref_slots_type01(slots,_uls_type_this_,none,typnam)

#define ULS_DEF_ARRAY_TYPE01(ns,typnam,NN) _ULS_DEF_ARRAY_TYPE01(_uls_type_ns_,ns,typnam,NN)
#define uls_decl_array_type01(ary,ns,typnam,NN) _uls_decl_array_type01(ary,_uls_type_ns_,ns,typnam,NN)
#define uls_init_array_type01(ary,ns,typnam,NN) _uls_init_array_type01(ary,_uls_type_ns_,ns,typnam,NN)
#define uls_deinit_array_type01(ary,ns,typnam) _uls_deinit_array_type01(ary,_uls_type_ns_,ns,typnam)
#define uls_array_ref_slots_type01(slots,ns,typnam) _uls_array_ref_slots_type01(slots,_uls_type_ns_,ns,typnam)

// type02
#define ULS_DEF_ARRAY_THIS_TYPE02(typnam) _ULS_DEF_ARRAY_TYPE02(_uls_type_this_,none,typnam)
#define uls_decl_array_this_type02(ary,typnam) _uls_decl_array_type02(ary,_uls_type_this_,none,typnam)
#define uls_init_array_this_type02(ary,typnam,n) _uls_init_array_type02(ary,_uls_type_this_,none,typnam,n)
#define uls_deinit_array_this_type02(ary,typnam) _uls_deinit_array_type02(ary,_uls_type_this_,none,typnam)
#define uls_cast_array_this_type02(typnam) _uls_cast_array_type02(_uls_type_this_,none,typnam)
#define uls_ref_array_this_type02(ary,typnam) _uls_ref_array_type02(ary,_uls_type_this_,none,typnam)
#define uls_ref_array_this_init_type02(ary,typnam,ary2) _uls_ref_array_init_type02(ary,_uls_type_this_,none,typnam,ary2)

#define ULS_DEF_ARRAY_TYPE02(typnam) _ULS_DEF_ARRAY_TYPE02(_uls_type_ns_,ns,typnam)
#define uls_decl_array_type02(ary,ns,typnam,n) _uls_decl_array_type02(ary,_uls_type_ns_,ns,typnam,n)
#define uls_init_array_type02(ary,ns,typnam,n) _uls_init_array_type02(ary,_uls_type_ns_,ns,typnam,n)
#define uls_deinit_array_type02(ary,ns,typnam) _uls_deinit_array_type02(ary,_uls_type_ns_,ns,typnam)
#define uls_cast_array_type02(ns,typnam) _uls_cast_array_type02(_uls_type_ns_,ns,typnam)
#define uls_ref_array_type02(ary,ns,typnam) _uls_ref_array_type02(ary,_uls_type_ns_,ns,typnam)
#define uls_ref_array_init_type02(ary,ns,typnam,ary2) _uls_ref_array_init_type02(ary,none,typnam,ary2)

// type10
#define ULS_DEF_ARRAY_THIS_TYPE10(typnam) _ULS_DEF_ARRAY_TYPE10(_uls_type_this_,none,typnam)
#define uls_decl_array_this_type10(ary,typnam) _uls_decl_array_type10(ary,none,typnam)
#define uls_alloc_array_this_slot_type10(ary,typnam,idx) _uls_alloc_array_slot_type10(ary,_uls_type_this_,none,typnam,idx)
#define uls_dealloc_array_this_slot_type10(ary,typnam,idx) _uls_dealloc_array_slot_type10(ary,_uls_type_this_,none,typnam,idx)
#define uls_init_array_this_type10(ary,typnam,nn) _uls_init_array_type10(ary,none,typnam,nn)
#define uls_deinit_array_this_type10(ary,typnam) _uls_deinit_array_type10(ary,_uls_type_this_,none,typnam)
#define uls_cast_array_this_type10(typnam) _uls_cast_array_type10(none,typnam)
#define uls_ref_array_this_type10(ary,typnam) _uls_ref_array_type10(ary,none,typnam)
#define uls_ref_array_this_init_type10(ary,typnam,ary2) _uls_ref_array_init_type10(ary,none,typnam,ary2)
#define uls_decl_array_this_type10_alloc(ary,typnam) _uls_decl_array_type10_alloc(ary,none,typnam)
#define uls_move_array_this_type10(a,b) _uls_move_array_type10(a,b)
#define uls_resize_array_this_type10(ary,typnam,nn_alloc) _uls_resize_array_type10(ary,_uls_type_this_,none,typnam,nn_alloc)

#define ULS_DEF_ARRAY_TYPE10(ns,typnam) _ULS_DEF_ARRAY_TYPE10(_uls_type_ns_,ns,typnam)
#define uls_decl_array_type10(ary,ns,typnam) _uls_decl_array_type10(ary,ns,typnam)
#define uls_alloc_array_slot_type10(ary,ns,typnam,idx) _uls_alloc_array_slot_type10(ary,_uls_type_ns_,ns,typnam,idx)
#define uls_dealloc_array_slot_type10(ary,ns,typnam,idx) _uls_dealloc_array_slot_type10(ary,_uls_type_ns_,ns,typnam,idx)
#define uls_init_array_type10(ary,ns,typnam,nn) _uls_init_array_type10(ary,ns,typnam,nn)
#define uls_deinit_array_type10(ary,ns,typnam) _uls_deinit_array_type10(ary,_uls_type_ns_,ns,typnam)
#define uls_cast_array_type10(ns,typnam) _uls_cast_array_type10(ns,typnam)
#define uls_ref_array_type10(ary,ns,typnam) _uls_ref_array_type10(ary,ns,typnam)
#define uls_ref_array_init_type10(ary,ns,typnam,ary2) _uls_ref_array_init_type10(ary,ns,typnam,ary2)
#define uls_decl_array_type10_alloc(ary,ns,typnam) _uls_decl_array_type10_alloc(ary,ns,typnam)
#define uls_move_array_type10(a,b) _uls_move_array_type10(a,b)
#define uls_resize_array_type10(ary,ns,typnam,nn_alloc) _uls_resize_array_type10(ary,_uls_type_ns_,ns,typnam,nn_alloc)

#ifndef ULS_DOTNET
ULS_DECLARE_STRUCT(uls_lex);

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

#define _ULSCPP_USTR2WSTR(ustr, wstr, slot_no) \
	((wstr) = auwcvt->mbstr2wstr(ustr, UlsAuw::CVT_MBSTR_USTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))

#define _ULSCPP_WSTR2USTR(wstr, ustr, slot_no) \
	((ustr) = auwcvt->wstr2mbstr(wstr, UlsAuw::CVT_MBSTR_USTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))

#define _ULSCPP_NSTR2WSTR(ustr, wstr, slot_no) \
	((wstr) = auwcvt->mbstr2wstr(ustr, UlsAuw::CVT_MBSTR_USTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))

#define _ULSCPP_WSTR2NSTR(wstr, ustr, slot_no) \
	((ustr) = auwcvt->wstr2mbstr(wstr, UlsAuw::CVT_MBSTR_USTR, slot_no),_ULSCPP_AUWCVT_LEN(slot_no))

#endif // _ULSCPP_IMPLDLL

#endif // ULS_DOTNET

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_TYPE_H__
