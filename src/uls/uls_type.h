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

#ifndef ULS_EXCLUDE_HFILES
#include "uls/uls_const.h"
#if !defined(ULS_DOTNET) && !defined(USE_ULSNETJAVA)
#ifdef ULS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#else
#include <sys/types.h>
#endif
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
#define uls_streql(a,b)  (_uls_tool_(strcmp)((const char*)(a),(const char*)(b))==0)

#ifdef ULS_DOTNET
#define ULS_QUALIFIED_METHOD(mthname) ULS_CLASS_NAME::mthname
#define ULS_QUALIFIED_RETTYP(rettyp) ULS_CLASS_NAME::rettyp
#else
#define ULS_QUALIFIED_METHOD(mthname) mthname
#define ULS_QUALIFIED_RETTYP(rettyp) rettyp
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
typedef int16_t       uls_int16;
typedef int32_t       uls_int32;
typedef int64_t       uls_int64;
typedef uint16_t      uls_uint16;
typedef uint32_t      uls_uint32;
typedef uint64_t      uls_uint64;
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

#ifdef ULS_CLASSIFY_SOURCE
// name-buffer
#define uls_def_namebuf(nam,siz) uls_type_tool(nambuf) nam
#define uls_init_namebuf(nam,siz) _uls_tool_(init_nambuf)(uls_ptr(nam), siz)
#define uls_deinit_namebuf(nam) _uls_tool_(deinit_nambuf)(uls_ptr(nam))
#define uls_get_namebuf_value(nam) (nam.str)
#define uls_get_namebuf_length(nam) (nam.len)
#define uls_set_namebuf_value(nam,str) _uls_tool_(set_nambuf)(uls_ptr(nam),str,-1)
#define uls_set_namebuf_value_2(nam,str,len) _uls_tool_(set_nambuf)(uls_ptr(nam),str,len)

// name-buffer(this)
#define uls_def_namebuf_this(nam,siz) uls_nambuf_t nam
#define uls_init_namebuf_this(nam,siz) uls_init_nambuf(uls_ptr(nam), siz)
#define uls_deinit_namebuf_this(nam) uls_deinit_nambuf(uls_ptr(nam))
#define uls_get_namebuf_value_this(nam) (nam.str)
#define uls_get_namebuf_length_this(nam) (nam.len)
#define uls_set_namebuf_value_this(nam,str) uls_set_nambuf(uls_ptr(nam),str,-1)
#define uls_set_namebuf_value_2_this(nam,str,len) uls_set_nambuf(uls_ptr(nam),str,len)

// bytes pool
#define uls_def_bytespool(nam,siz) char*nam
#define uls_ref_bytespool(nam) char*nam
#define uls_init_bytespool(nam,siz,no_clear) (nam) = ((no_clear) ? uls_malloc_buffer(siz) : uls_malloc_buffer_clear(siz))
#define uls_deinit_bytespool(nam) uls_mfree(nam)

#else // ULS_CLASSIFY_SOURCE
// name-buffer
#define uls_def_namebuf(nam,siz) char  nam[siz+1]
#define uls_init_namebuf(nam,siz) nam[0]='\0'
#define uls_deinit_namebuf(nam)
#define uls_get_namebuf_value(nam) (nam)
#define uls_get_namebuf_length(nam) uls_strlen(nam)
#define uls_set_namebuf_value(nam,str) uls_set_nambuf_raw(nam,sizeof(nam),str,-1)
#define uls_set_namebuf_value_2(nam,str,len) uls_set_nambuf_raw(nam,sizeof(nam),str,len)

// name-buffer(this)
#define uls_def_namebuf_this uls_def_namebuf
#define uls_init_namebuf_this uls_init_namebuf
#define uls_deinit_namebuf_this uls_deinit_namebuf
#define uls_get_namebuf_value_this uls_get_namebuf_value
#define uls_get_namebuf_length_this uls_get_namebuf_length
#define uls_set_namebuf_value_this uls_set_namebuf_value
#define uls_set_namebuf_value_2_this uls_set_namebuf_value_2

// bytes pool
#define uls_def_bytespool(nam,siz) char nam[siz]
#define uls_ref_bytespool(nam) char*nam
#define uls_init_bytespool(nam,siz,no_clear) if (!no_clear) uls_bzero(nam,siz)
#define uls_deinit_bytespool(nam)
#endif // ULS_CLASSIFY_SOURCE

#ifdef ULS_DOTNET
#define uls_ptr(a) %(a)
#define uls_nil  nullptr
#define uls_ref_typ(typ) typ^
#define uls_def_ptrvar(avar) ^avar

#define _uls_type_this(ns,typnam) typnam##_t
#define _uls_ptrtype_this(ns,typnam) typnam##_ptr_t
#define uls_type_this(ns,typnam) uls_##typnam##_t
#define uls_ptrtype_this(ns,typnam) uls_##typnam##_ptr_t
#define uls_cast_ptrtype_this(ns,typnam,var1,var2) uls_ptrtype_this(ns,typnam) var1 = (uls_ptrtype_this(ns,typnam))(var2)

#define _uls_type_ns(ns,typnam) ns::typnam##_t
#define _uls_ptrtype_ns(ns,typnam) ns::typnam##_ptr_t
#define uls_type_ns(ns,typnam) ns::uls_##typnam##_t
#define uls_ptrtype_ns(ns,typnam) ns::uls_##typnam##_ptr_t
#define uls_cast_ptrtype_ns(ns,typnam,var1,var2) uls_ptrtype_ns(ns,typnam) var1 = (uls_ptrtype_ns(ns,typnam))(var2)

#define _uls_type_tool(typnam) _uls_type_ns(UlsToolbase,typnam)
#define _uls_ptrtype_tool(typnam) _uls_ptrtype_ns(UlsToolbase,typnam)
#define uls_type_tool(typnam) uls_type_ns(UlsToolbase,typnam)
#define uls_ptrtype_tool(typnam) uls_ptrtype_ns(UlsToolbase,typnam)
#define uls_cast_ptrtype_tool(typnam,var1,var2) uls_ptrtype_tool(typnam) var1 = (uls_ptrtype_tool(typnam))(var2)

#define _ULS_DECLARE_STRUCT_BRIEF(typnam) ref struct typnam##_t
#define _ULS_DEFINE_STRUCT_PTR(typnam) typedef ref struct typnam##_t ^typnam##_ptr_t
#define _ULS_DECLARE_STRUCT(typnam) _ULS_DECLARE_STRUCT_BRIEF(typnam); _ULS_DEFINE_STRUCT_PTR(typnam)
#define _ULS_DEFINE_STRUCT_BEGIN(typnam) _ULS_DECLARE_STRUCT_BRIEF(typnam)
#define _ULS_DEFINE_STRUCT(typnam) _ULS_DECLARE_STRUCT(typnam); _ULS_DEFINE_STRUCT_BEGIN(typnam)

#define ULS_DECLARE_STRUCT_BRIEF(typnam) ref struct uls_##typnam##_t
#define ULS_DEFINE_STRUCT_PTR(typnam) typedef ref struct uls_##typnam##_t ^uls_##typnam##_ptr_t
#define ULS_DECLARE_STRUCT(typnam) ULS_DECLARE_STRUCT_BRIEF(typnam); ULS_DEFINE_STRUCT_PTR(typnam)
#define ULS_DEFINE_STRUCT_BEGIN(typnam) ULS_DECLARE_STRUCT_BRIEF(typnam)
#define ULS_DEFINE_STRUCT(typnam) ULS_DECLARE_STRUCT(typnam); ULS_DEFINE_STRUCT_BEGIN(typnam)

// delegate
#define _ULS_DELEGATE_NAME(typnam) _uls_delegate_##typnam
#define ULS_DELEGATE_NAME(typnam) uls_##typnam##_t
#define ULS_DEFINE_DELEGATE_BEGIN(typnam,rtype) delegate rtype _ULS_DELEGATE_NAME(typnam)
#define ULS_DEFINE_DELEGATE_END(typnam) typedef _ULS_DELEGATE_NAME(typnam) ^ULS_DELEGATE_NAME(typnam)

#define uls_nameof_cb_varnam(mthnam) _cb_##mthnam
#define uls_callback_type_ns(clsnam,typnam) clsnam::ULS_DELEGATE_NAME(typnam)
#define uls_callback_type_this(typnam) ULS_DELEGATE_NAME(typnam)

#define uls_ref_callback(o,mthnam) (o)->uls_nameof_cb_varnam(mthnam)
#define uls_ref_callback_this(mthnam) uls_nameof_cb_varnam(mthnam)

#define uls_def_callback(mthnam,typnam) uls_callback_type_this(typnam) uls_nameof_cb_varnam(mthnam)
#define uls_def_callback_ns(mthnam,clsnam,typnam) uls_callback_type_ns(clsnam,typnam) uls_nameof_cb_varnam(mthnam)
#define uls_def_callback_tool(mthnam,typnam) uls_def_callback_ns(mthnam,UlsToolbase,typnam)

#define uls_alloc_callback(mthnam,typnam) uls_nameof_cb_varnam(mthnam) = gcnew _ULS_DELEGATE_NAME(typnam)(this,&ULS_QUALIFIED_METHOD(mthnam))
#define uls_alloc_callback_ns(mthnam,clsnam,typnam) uls_nameof_cb_varnam(mthnam) = gcnew clsnam::_ULS_DELEGATE_NAME(typnam)(this,&ULS_QUALIFIED_METHOD(mthnam))
#define uls_alloc_callback_tool(mthnam,clsnam) uls_alloc_callback_ns(mthnam,clsnam,UlsToolbase)

#define uls_dealloc_callback(mthnam) delete uls_nameof_cb_varnam(mthnam)

#define _uls_parray_this_(ns,typnam) uls_##typnam##_parray_t
#define _uls_ptrparray_this_(ns,typnam) uls_##typnam##_parray_ptr_t
#define _uls_parray_ns_(ns,typnam) ns::_uls_parray_this_(ns,typnam)
#define _uls_ptrparray_ns_(ns,typnam) ns::_uls_ptrparray_this_(ns,typnam)

#define __uls_array00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_t
#define __uls_ptrarray00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_ptr_t
#define __uls_array00_ns_(ns,typnam,NN) ns::__uls_array00_this_(ns,typnam,NN)
#define __uls_ptrarray00_ns_(ns,typnam,NN) ns::__uls_ptrarray00_this_(ns,typnam,NN)

#define _uls_array00_this_(ns,typnam,NN) __uls_array00_this_(ns,typnam,NN)
#define _uls_ptrarray00_this_(ns,typnam,NN) __uls_ptrarray00_this_(ns,typnam,NN)
#define _uls_array00_ns_(ns,typnam,NN) __uls_array00_ns_(ns,typnam,NN)
#define _uls_ptrarray00_ns_(ns,typnam,NN) __uls_ptrarray00_ns_(ns,typnam,NN)

#define _uls_array01_this_(ns,typnam) uls_##typnam##_s01array_t
#define _uls_ptrarray01_this_(ns,typnam) uls_##typnam##_s01array_ptr_t
#define _uls_array01_ns_(ns,typnam) ns::_uls_array01_this_(ns,typnam)
#define _uls_ptrarray01_ns_(ns,typnam) ns::_uls_ptrarray01_this_(ns,typnam)

#define _uls_array10_this_(ns,typnam) uls_##typnam##_s10array_t
#define _uls_ptrarray10_this_(ns,typnam) uls_##typnam##_s10array_ptr_t
#define _uls_array10_ns_(ns,typnam) ns::_uls_array10_this_(ns,typnam)
#define _uls_ptrarray10_ns_(ns,typnam) ns::_uls_ptrarray10_this_(ns,typnam)

#else // ULS_DOTNET

#define uls_ptr(a) &(a)
#define uls_nil  NULL
#define uls_ref_typ(typ) typ*
#define uls_def_ptrvar(avar) *avar

#define _uls_type_this(ns,typnam) typnam##_t
#define _uls_ptrtype_this(ns,typnam) typnam##_ptr_t
#define uls_type_this(ns,typnam) uls_##typnam##_t
#define uls_ptrtype_this(ns,typnam) uls_##typnam##_ptr_t
#define uls_cast_ptrtype_this(ns,typnam,var1,var2) uls_ptrtype_this(ns,typnam) var1 = (uls_ptrtype_this(ns,typnam))(var2)

#define _uls_type_ns(ns,typnam) _uls_type_this(ns,typnam)
#define _uls_ptrtype_ns(ns,typnam) _uls_ptrtype_this(ns,typnam)
#define uls_type_ns(ns,typnam) uls_type_this(ns,typnam)
#define uls_ptrtype_ns(ns,typnam) uls_ptrtype_this(ns,typnam)
#define uls_cast_ptrtype_ns(ns,typnam,var1,var2) uls_ptrtype_ns(ns,typnam) var1 = (uls_ptrtype_ns(ns,typnam))(var2)

#define _uls_type_tool(typnam) _uls_type_this(none,typnam)
#define _uls_ptrtype_tool(typnam) _uls_ptrtype_this(none,typnam)
#define uls_type_tool(typnam) uls_type_this(none,typnam)
#define uls_ptrtype_tool(typnam) uls_ptrtype_this(none,typnam)
#define uls_cast_ptrtype_tool(typnam,var1,var2) uls_ptrtype_tool(typnam) var1 = (uls_ptrtype_tool(typnam))(var2)

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

#define uls_callback_type_ns(clsnam,typnam) ULS_DELEGATE_NAME(typnam)
#define uls_callback_type_this(typnam) ULS_DELEGATE_NAME(typnam)

#define uls_ref_callback(o,mthnam) mthnam
#define uls_ref_callback_this(mthnam) mthnam

#define _uls_parray_this_(ns,typnam) uls_##typnam##_parray_t
#define _uls_ptrparray_this_(ns,typnam) uls_##typnam##_parray_ptr_t
#define _uls_parray_ns_(ns,typnam) _uls_parray_this_(ns,typnam)
#define _uls_ptrparray_ns_(ns,typnam) _uls_ptrparray_this_(ns,typnam)

#define __uls_array00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_t
#define __uls_ptrarray00_this_(ns,typnam,NN) uls_##typnam##_s00array##NN##_ptr_t
#define __uls_array00_ns_(ns,typnam,NN) __uls_array00_this_(ns,typnam,NN)
#define __uls_ptrarray00_ns_(ns,typnam,NN) __uls_ptrarray00_this_(ns,typnam,NN)

#define _uls_array00_this_(ns,typnam,NN) __uls_array00_this_(ns,typnam,NN)
#define _uls_ptrarray00_this_(ns,typnam,NN) __uls_ptrarray00_this_(ns,typnam,NN)
#define _uls_array00_ns_(ns,typnam,NN) __uls_array00_ns_(ns,typnam,NN)
#define _uls_ptrarray00_ns_(ns,typnam,NN) __uls_ptrarray00_ns_(ns,typnam,NN)

#define _uls_array01_this_(ns,typnam) uls_##typnam##_s01array_t
#define _uls_ptrarray01_this_(ns,typnam) uls_##typnam##_s01array_ptr_t
#define _uls_array01_ns_(ns,typnam) _uls_array01_this_(ns,typnam)
#define _uls_ptrarray01_ns_(ns,typnam) _uls_ptrarray01_this_(ns,typnam)

#define _uls_array10_this_(ns,typnam) uls_##typnam##_s10array_t
#define _uls_ptrarray10_this_(ns,typnam) uls_##typnam##_s10array_ptr_t
#define _uls_array10_ns_(ns,typnam) _uls_array10_this_(ns,typnam)
#define _uls_ptrarray10_ns_(ns,typnam) _uls_ptrarray10_this_(ns,typnam)

#endif // ULS_DOTNET

#define ULS_DEF_PARRAY(typnam) \
	ULS_DECLARE_STRUCT(typnam##_parray); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_parray) { uls_decl_ptrarray(slots,typnam); int n, n_alloc; }

#define uls_decl_parray_ns(name,ns,typnam,typmac) typmac(ns,typnam) name
#define uls_parray_ptrtype_ns(ns,typnam,typmac) uls_ref_typ(typmac(ns,typnam))

#define uls_decl_parray(name,typnam) uls_decl_parray_ns(name,none,typnam,_uls_parray_this_)
#define uls_ref_parray(name,typnam) uls_ref_parray_ns(name,none,typnam,_uls_parray_this_)
#define uls_ref_parray_init(name,typnam,val) uls_ref_parray(name,typnam)=(val)

#ifdef ULS_CLASSIFY_SOURCE

// type00
#define ULS_DEF_ARRAY_TYPE00(typnam,arynam,NN) ULS_DEF_PARRAY(typnam)
#define uls_decl_array_ns_type00(ary,ns,typnam,typmac,NN) uls_decl_parray_ns(ary,ns,typnam,typmac)

// type01
#define ULS_DEF_ARRAY_TYPE01(typnam) ULS_DEF_PARRAY(typnam)
#define uls_decl_array_ns_type01(ary,ns,typnam,typmac) uls_decl_parray_ns(ary,ns,typnam,typmac)
#define uls_ref_array_ns_type01(ary,ns,typnam,typmac) uls_ref_parray_ns(ary,ns,typnam,typmac)

// type10
#define ULS_DEF_ARRAY_TYPE10(typnam) ULS_DEF_PARRAY(typnam)
#define uls_decl_array_ns_type10(ary,ns,typnam,typmac) uls_decl_parray_ns(ary,ns,typnam,typmac)
#define uls_ref_array_ns_type10(ary,ns,typnam,typmac) uls_ref_parray_ns(ary,ns,typnam,typmac)

// type00(this,tool)
#define uls_decl_array_type00(ary,typnam,NN) uls_decl_array_ns_type00(ary,none,typnam,_uls_parray_this_,NN)

#define uls_decl_array_tool_type00(ary,typnam,NN) uls_decl_array_ns_type00(ary,UlsToolbase,typnam,_uls_parray_ns_,NN)

// type01(this,tool)
#define uls_decl_array_type01(ary,typnam) uls_decl_array_ns_type01(ary,none,typnam,_uls_parray_this_)
#define uls_ref_array_type01(ary,typnam) uls_ref_array_ns_type01(ary,none,typnam,_uls_parray_this_)

#define uls_decl_array_tool_type01(ary,typnam) uls_decl_array_ns_type01(ary,UlsToolbase,typnam,_uls_parray_ns_)
#define uls_ref_array_tool_type01(ary,typnam) uls_ref_array_ns_type01(ary,UlsToolbase,typnam,_uls_parray_ns_)

// type10(this,tool)
#define uls_decl_array_type10(ary,typnam) uls_decl_array_ns_type10(ary,none,typnam,_uls_parray_this_)
#define uls_ref_array_type10(ary,typnam) uls_ref_array_ns_type10(ary,none,typnam,_uls_parray_this_)

#define uls_decl_array_tool_type10(ary,typnam) uls_decl_array_ns_type10(ary,UlsToolbase,typnam,_uls_parray_ns_)
#define uls_ref_array_tool_type10(ary,typnam) uls_ref_array_ns_type10(ary,UlsToolbase,typnam,_uls_parray_ns_)

#else // ULS_CLASSIFY_SOURCE

// type00
#define ULS_DEF_ARRAY_TYPE00(typnam,arynam,NN)  \
	ULS_DECLARE_STRUCT(arynam); \
	ULS_DEFINE_STRUCT_BEGIN(arynam) { uls_type_this(ns,typnam) slots[NN]; int n; }

#define uls_decl_array_ns_type00(ary,ns,typnam,typmac,NN) typmac(ns,typnam,NN)ary

// type01
#define ULS_DEF_ARRAY_TYPE01(typnam) \
	ULS_DECLARE_STRUCT(typnam##_s01array); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_s01array) { uls_type_this(none,typnam) uls_def_ptrvar(slots); int n; }

#define uls_decl_array_ns_type01(ary,ns,typnam,typmac) typmac(ns,typnam)ary
#define uls_ref_array_ns_type01(ary,ns,typnam,typmac) typmac(ns,typnam)*ary

// type10
#define ULS_DEF_ARRAY_TYPE10(typnam) \
	ULS_DECLARE_STRUCT(typnam##_s10array); \
	ULS_DEFINE_STRUCT_BEGIN(typnam##_s10array) { uls_type_this(none,typnam) uls_def_ptrvar(slots); int n, n_alloc; }

#define uls_decl_array_ns_type10(ary,ns,typnam,typmac) typmac(ns,typnam)ary
#define uls_ref_array_ns_type10(ary,ns,typnam,typmac) typmac(ns,typnam)*ary

// type00(this,tool)
#define uls_decl_array_type00(ary,typnam,NN) uls_decl_array_ns_type00(ary,none,typnam,_uls_array00_this_,NN)

#define uls_decl_array_tool_type00(ary,typnam,NN) uls_decl_array_ns_type00(ary,UlsToolbase,typnam,_uls_array00_ns_,NN)

// type01(this,tool)
#define uls_decl_array_type01(ary,typnam) uls_decl_array_ns_type01(ary,none,typnam,_uls_array01_this_)
#define uls_ref_array_type01(ary,typnam) uls_ref_array_ns_type01(ary,none,typnam,_uls_array01_this_)

#define uls_decl_array_tool_type01(ary,typnam) uls_decl_array_ns_type01(ary,UlsToolbase,typnam,_uls_array01_ns_)
#define uls_ref_array_tool_type01(ary,typnam) uls_ref_array_ns_type01(ary,UlsToolbase,typnam,_uls_array01_ns_)

// type10(this,tool)
#define uls_decl_array_type10(ary,typnam) uls_decl_array_ns_type10(ary,none,typnam,_uls_array10_this_)
#define uls_ref_array_type10(ary,typnam) uls_ref_array_ns_type10(ary,none,typnam,_uls_array10_this_)

#define uls_decl_array_tool_type10(ary,typnam) uls_decl_array_ns_type10(ary,UlsToolbase,typnam,_uls_array10_ns_)
#define uls_ref_array_tool_type10(ary,typnam) uls_ref_array_ns_type10(ary,UlsToolbase,typnam,_uls_array10_ns_)

#endif // ULS_CLASSIFY_SOURCE

#ifndef ULS_DOTNET
ULS_DECLARE_STRUCT(lex);

#define _uls_log_primitive(proc) proc##_primitive
#define _uls_log_static(proc) _uls_log_primitive(proc)

#define _uls_log(proc) proc
#define _uls_log_(proc) uls_##proc

#define _uls_tool(proc) proc
#define _uls_tool_(proc) uls_##proc
#define __uls_tool_(proc) _uls_##proc
#endif // ULS_DOTNET


#ifdef ULS_DOTNET
#define uls_initial_zerofy_object(obj)
#define uls_alloc_object(typ) gcnew typ()
#define uls_alloc_object_clear(typ) uls_alloc_object(typ)
#define uls_dealloc_object(obj) do { (obj) = nullptr; } while (0)

// arraytype
#define _uls_type_array(typ) array<typ>^
#define _uls_decl_array(name,typ) _uls_type_array(typ) name
#define _uls_cast_array_type(typ) (array<typ>^)
#define _uls_init_array(n,typ) gcnew array<typ>(n)
#define _uls_deinit_array(ary) do { delete ary; (ary) = nullptr; } while (0)
#define _uls_resize_array(ary,typ,n) Array::Resize(ary,n)

// arraytype(ptr)
#define uls_type_ptrarray_ns(ns,typnam,typmac) array<typmac(ns,typnam)^>^
#define uls_decl_ptrarray_ns(name,ns,typnam,typmac) uls_type_ptrarray_ns(ns,typnam,typmac)name
#define uls_cast_ptrarray_ns(ns,typnam,typmac) (uls_type_ptrarray_ns(ns,typnam,typmac))
#define uls_ref_ptrarray_ns(name,ns,typnam,typmac) uls_decl_ptrarray_ns(name,ns,typnam,typmac)
#define uls_ref_ptrarray_init_ns(name,ns,typnam,typmac,val) uls_ref_ptrarray_ns(name,ns,typnam,typmac)=(val)
#define uls_init_ptrarray_ns(n,ns,typnam,typmac) gcnew array<typmac(ns,typnam)^>(n)
#define uls_deinit_ptrarray_ns(ary) do { delete ary; (ary) = nullptr; } while (0)
#define uls_resize_ptrarray_ns(ary,ns,typnam,typmac,n) Array::Resize(ary,n)

#else // ULS_DOTNET

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

// arraytype(ptr)
#define uls_type_ptrarray_ns(ns,typnam,typmac) typmac(ns,typnam)**
#define uls_decl_ptrarray_ns(name,ns,typnam,typmac) uls_type_ptrarray_ns(ns,typnam,typmac)name
#define uls_cast_ptrarray_ns(ns,typnam,typmac) (uls_type_ptrarray_ns(ns,typnam,typmac))
#define uls_ref_ptrarray_ns(name,ns,typnam,typmac) uls_decl_ptrarray_ns(name,ns,typnam,typmac)
#define uls_ref_ptrarray_init_ns(name,ns,typnam,typmac,val) uls_ref_ptrarray_ns(name,ns,typnam,typmac)=(val)
#define uls_init_ptrarray_ns(n,ns,typnam,typmac) (typmac(ns,typnam)**)uls_malloc_clear((n)*sizeof(typmac(ns,typnam)*))
#define uls_deinit_ptrarray_ns(ary) uls_mfree(ary)
#define uls_resize_ptrarray_ns(ary,ns,typnam,typmac,n) (ary)=(typmac(ns,typnam)**)uls_mrealloc(ary,(n)*sizeof(typmac(ns,typnam)*))

#endif // ULS_DOTNET

#define uls_type_ptrarray(typnam) uls_type_ptrarray_ns(none,typnam,uls_type_this)
#define uls_decl_ptrarray(name,typnam) uls_decl_ptrarray_ns(name,none,typnam,uls_type_this)
#define uls_cast_ptrarray(typnam) uls_cast_ptrarray_ns(none,typnam,uls_type_this)
#define uls_ref_ptrarray(name,typnam) uls_ref_ptrarray_ns(name,none,typnam,uls_type_this)

#define uls_init_ptrarray(n,typnam) uls_init_ptrarray_ns(n,none,typnam,uls_type_this)
#define uls_deinit_ptrarray(ary) uls_deinit_ptrarray_ns(ary)
#define uls_resize_ptrarray(ary,typnam,n) uls_resize_ptrarray_ns(ary,none,typnam,uls_type_this,n)

#define uls_type_ptrarray_tool(typnam) uls_type_ptrarray_ns(UlsToolbase,typnam,uls_type_ns)
#define uls_decl_ptrarray_tool(name,typnam) uls_decl_ptrarray_ns(name,UlsToolbase,typnam,uls_type_ns)
#define uls_cast_ptrarray_tool(typnam) uls_cast_ptrarray_ns(UlsToolbase,typnam,uls_type_ns)
#define uls_ref_ptrarray_tool(name,typnam) uls_ref_ptrarray_ns(name,UlsToolbase,typnam,uls_type_ns)

#define uls_init_ptrarray_tool(n,typnam) uls_init_ptrarray_ns(n,UlsToolbase,typnam,uls_type_ns)
#define uls_deinit_ptrarray_tool(ary) uls_deinit_ptrarray_ns(ary)
#define uls_resize_ptrarray_tool(ary,typnam,n) uls_resize_ptrarray_ns(ary,UlsToolbase,typnam,uls_type_ns,n)

#define uls_decl_parray_ptrtype_ns(a,ns,typnam,typmac) uls_parray_ptrtype_ns(ns,typnam,typmac) a
#define uls_cast_parray_ptrtype_ns(ns,typnam,typmac) (uls_parray_ptrtype_ns(ns,typnam,typmac))
#define uls_ref_parray_ns(name,ns,typnam,typmac) uls_decl_parray_ptrtype_ns(name,ns,typnam,typmac)
#define uls_ref_parray_init_ns(name,ns,typnam,typmac,val) uls_ref_parray_ns(name,ns,typnam,typmac)=(val)

#define uls_init_parray_ns(a,ns,typnam,typmac,nn) do { \
		(a)->slots = uls_init_ptrarray_ns(nn,ns,typnam,typmac); (a)->n = 0; (a)->n_alloc = nn; \
	} while(0)
#define uls_deinit_parray_ns(a) do { \
		uls_deinit_ptrarray_ns((a)->slots); \
		(a)->n = (a)->n_alloc = 0; \
	} while(0)

#define uls_alloc_parray_ptrtype_ns(a,ns,typnam,typmac,typmac0,nn) do { \
		(a)=uls_alloc_object(typmac(ns,typmac)); \
		uls_init_parray_ns(a,ns,typnam,typmac0,nn); \
	} while(0)
#define uls_dealloc_parray_ptrtype_ns(a) do { \
		uls_deinit_parray_ns(a); \
		uls_dealloc_object(a); \
	} while(0)

#define uls_resize_parray_ns(a,ns,typnam,typmac,nn) do { \
		uls_resize_ptrarray_ns((a)->slots,ns,typnam,typmac,nn); \
		if ((nn) < (a)->n) (a)->n = (nn); (a)->n_alloc = (nn); \
	} while (0)

#define uls_decl_parray_slots_ns(a,ns,typnam,typmac) uls_ref_ptrarray_ns(a,ns,typnam,typmac)
#define uls_decl_parray_slots_init_ns(a,ns,typnam,typmac,b) uls_decl_parray_slots_ns(a,ns,typnam,typmac)=(b)->slots

// this
#define uls_init_parray(a,typnam,nn) uls_init_parray_ns(a,none,typnam,uls_type_this,nn)
#define uls_deinit_parray(a) uls_deinit_parray_ns(a)
#define uls_resize_parray(a,typnam,nn) uls_resize_parray_ns(a,none,typnam,uls_type_this,nn)
#define uls_parray_slots(b) ((b)->slots)
#define uls_decl_parray_slots(a,typnam) uls_decl_parray_slots_ns(a,none,typnam,uls_type_this)
#define uls_decl_parray_slots_init(a,typnam,b) uls_decl_parray_slots_init_ns(a,none,typnam,uls_type_this,b)

// tool
#define uls_decl_parray_tool(name,typnam) uls_decl_parray_ns(name,UlsToolbase,typnam,_uls_parray_ns_)
#define uls_ref_parray_tool(name,typnam) uls_ref_parray_ns(name,UlsToolbase,typnam,_uls_parray_ns_)
#define uls_init_parray_tool(a,typnam,nn) uls_init_parray_ns(a,UlsToolbase,typnam,uls_type_ns,nn)
#define uls_deinit_parray_tool(a) uls_deinit_parray_ns(a)
#define uls_resize_parray_tool(a,typnam,nn) uls_resize_parray_ns(a,UlsToolbase,typnam,uls_type_ns,nn)
#define uls_decl_parray_slots_tool(a,typnam) uls_decl_parray_slots_ns(a,UlsToolbase,typnam,uls_type_ns)
#define uls_decl_parray_slots_init_tool(a,typnam,b) uls_decl_parray_slots_init_ns(a,UlsToolbase,typnam,uls_type_ns,b)


#ifdef ULS_CLASSIFY_SOURCE

// standard array
#define _uls_get_stdary_slot(ary,idx) (ary)[idx]
#define _uls_set_stdary_slot(ary,idx,obj) (ary)[idx]=obj

#define _uls_alloc_iary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot); \
		(ary)[idx]=slot=uls_alloc_object_clear(typmac(ns,typnam)); \
		uls_init_##typnam(slot); \
	} while (0)
#define _uls_dealloc_iary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		if ((slot) != nilptr) { uls_deinit_##typnam(slot); uls_dealloc_object(slot); _uls_set_stdary_slot(ary,idx,nilptr); } \
	} while (0)

#define _uls_alloc_zary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot); \
		(ary)[idx]=slot=uls_alloc_object_clear(typmac(ns,typnam)); \
	} while (0)
#define _uls_dealloc_zary_slot(ary,idx,ns,typnam,typmac) do { \
		typmac(ns,typnam) uls_def_ptrvar(slot)=_uls_get_stdary_slot(ary,idx); \
		if ((slot) != nilptr) { uls_dealloc_object(slot); _uls_set_stdary_slot(ary,idx,nilptr); } \
	} while (0)

// type00
#define uls_init_array_ns_type00(ary,ns,typnam,typmac,NN) do { int i; \
		(ary)->slots = uls_init_ptrarray_ns(NN, ns,typnam,typmac); (ary)->n = (ary)->n_alloc = (NN); \
		for (i=0; i<(NN); i++) { (ary)->slots[i]=uls_alloc_object_clear(typmac(ns,typnam)); } \
	} while (0)
#define uls_deinit_array_ns_type00(ary,ns,typnam,typmac) do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_dealloc_object((ary)->slots[i]); } \
		uls_deinit_ptrarray_ns((ary)->slots); \
	} while (0)

#define uls_array_ref_slots_ns_type00(slots,ns,typnam,typmac) uls_decl_ptrarray_ns(slots,ns,typnam,typmac)
#define uls_array_slots_type00(ary) ((ary)->slots)
#define uls_get_array_slot_type00(ary,idx) _uls_get_stdary_slot((ary)->slots,idx)

// type01
#define uls_init_array_ns_type01(ary,ns,typnam,typmac,nn) do { int i; \
		(ary)->slots = uls_init_ptrarray_ns(nn, ns,typnam,typmac); (ary)->n = (ary)->n_alloc = (nn); \
		for (i=0; i<(nn); i++) { _uls_alloc_zary_slot((ary)->slots,i,ns,typnam,typmac); } \
	} while (0)
#define uls_deinit_array_ns_type01(ary,ns,typnam,typmac) do { int i; \
		for (i=0; i<(ary)->n; i++) { _uls_dealloc_zary_slot((ary)->slots,i,ns,typnam,typmac); } \
		uls_deinit_ptrarray_ns((ary)->slots); \
	} while (0)

#define uls_init_array_ns_type01a(ary,ns,typnam,typmac,nn) do { int i; \
		uls_init_array_ns_type01(ary,ns,typnam,typmac,nn); \
		for (i=0; i<(nn); i++) { uls_init_##typnam(uls_get_array_slot_type01(ary,i)); } \
	} while (0)
#define uls_deinit_array_ns_type01a(ary,ns,typnam,typmac) do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_deinit_##typnam(uls_get_array_slot_type01(ary,i)); } \
		uls_deinit_array_ns_type01(ary,ns,typnam,typmac); \
	} while (0)

#define uls_decl_array_slots_ns_type01(a,ns,typnam,typmac) uls_decl_ptrarray_ns(a,ns,typnam,typmac)
#define uls_array_slots_type01(ary) ((ary)->slots)
#define uls_get_array_slot_type01(ary,idx) _uls_get_stdary_slot(uls_array_slots_type01(ary),idx)

// type10
#define uls_cast_array_ns_type10(ns,typnam,typmac) uls_cast_parray_ptrtype_ns(ns,typnam,typmac)
#define uls_ref_array_init_ns_type10(ary,ns,typnam,typmac,ary2) uls_ref_array_ns_type10(ary,ns,typnam,typmac)=ary2
#define uls_decl_array_alloc_ns_type10(ary,ns,typnam,typmac) uls_ref_array_ns_type10(ary,ns,typnam,typmac)=uls_alloc_object_clear(typmac(ns,typnam))

#define uls_alloc_array_slot_ns_type10(ary,ns,typnam,typmac,idx) _uls_alloc_iary_slot((ary)->slots,idx,ns,typnam,typmac)
#define uls_dealloc_array_slot_ns_type10(ary,ns,typnam,typmac,idx) _uls_dealloc_iary_slot((ary)->slots,idx,ns,typnam,typmac)

#define uls_init_array_ns_type10(ary,ns,typnam,typmac,nn) uls_init_parray_ns(ary,ns,typnam,typmac,nn)
#define uls_deinit_array_ns_type10(ary,ns,typnam,typmac) do { \
		uls_resize_array_ns_type10(ary,ns,typnam,typmac,0); \
		uls_deinit_parray(ary); \
	} while (0)
#define uls_resize_array_ns_type10(ary,ns,typnam,typmac,nn_alloc) do { \
		int i; for (i=(nn_alloc); i<(ary)->n; i++) { _uls_dealloc_iary_slot((ary)->slots,i,ns,typnam,typmac); } \
		uls_resize_parray(ary,typnam,nn_alloc); \
	} while (0)

#define uls_decl_array_slots_ns_type10(a,ns,typnam,typmac) uls_decl_ptrarray_ns(a,ns,typnam,typmac)
// type00(this,tool)
#define uls_init_array_type00(ary,typnam,NN) uls_init_array_ns_type00(ary,none,typnam,uls_type_this,NN)
#define uls_deinit_array_type00(ary,typnam) uls_deinit_array_ns_type00(ary,none,typnam,uls_type_this)
#define uls_decl_array_slots_type00(a,typnam) uls_decl_array_slots_ns_type00(a,none,typnam,uls_type_this)
#define uls_decl_array_slots_init_type00(a,typnam,b) uls_decl_array_slots_init_ns_type00(a,none,typnam,uls_type_this,b)
#define uls_array_ref_slots_type00(slots,typnam) uls_array_ref_slots_ns_type00(slots,none,typnam,uls_type_this)

// type01(this,tool)
#define uls_init_array_type01(ary,typnam,n) uls_init_array_ns_type01(ary,none,typnam,uls_type_this,n)
#define uls_deinit_array_type01(ary,typnam) uls_deinit_array_ns_type01(ary,none,typnam,uls_type_this)

#define uls_init_array_type01a(ary,typnam,n) uls_init_array_ns_type01a(ary,none,typnam,uls_type_this,n)
#define uls_deinit_array_type01a(ary,typnam) uls_deinit_array_ns_type01a(ary,none,typnam,uls_type_this)

#define uls_decl_array_slots_type01(a,typnam) uls_decl_array_slots_ns_type01(a,none,typnam,uls_type_this)
#define uls_decl_array_slots_init_type01(a,typnam,b) uls_decl_array_slots_init_ns_type01(a,none,typnam,uls_type_this,b)
#define uls_init_array_tool_type01(ary,typnam,n) uls_init_array_ns_type01(ary,UlsToolbase,typnam,uls_type_ns,n)
#define uls_init_array_tool_type01a(ary,typnam,n) uls_init_array_ns_type01a(ary,UlsToolbase,typnam,uls_type_ns,n)
#define uls_deinit_array_tool_type01(ary,typnam) uls_deinit_array_ns_type01(ary,UlsToolbase,typnam,uls_type_ns)
#define uls_deinit_array_tool_type01a(ary,typnam) uls_deinit_array_ns_type01a(ary,UlsToolbase,typnam,uls_type_ns)
#define uls_decl_array_slots_tool_type01(a,typnam) uls_decl_array_slots_ns_type01(a,UlsToolbase,typnam,uls_type_ns)

// type10(this,tool)
#define uls_decl_array_alloc_type10(ary,typnam) uls_decl_array_alloc_ns_type10(ary,none,typnam,_uls_parray_this_)
#define uls_cast_array_type10(typnam) uls_cast_array_ns_type10(none,typnam,_uls_parray_this_)
#define uls_ref_array_init_type10(ary,typnam,ary2) uls_ref_array_init_ns_type10(ary,none,typnam,_uls_parray_this_,ary2)
#define uls_init_array_type10(ary,typnam,nn) uls_init_array_ns_type10(ary,none,typnam,uls_type_this,nn)
#define uls_deinit_array_type10(ary,typnam) uls_deinit_array_ns_type10(ary,none,typnam,uls_type_this)
#define uls_resize_array_type10(ary,typnam,nn_alloc) uls_resize_array_ns_type10(ary,none,typnam,uls_type_this,nn_alloc)
#define uls_decl_array_slots_type10(a,typnam) uls_decl_array_slots_ns_type10(a,none,typnam,uls_type_this)

#define uls_alloc_array_slot_type10(ary,typnam,idx) uls_alloc_array_slot_ns_type10(ary,none,typnam,uls_type_this,idx)
#define uls_dealloc_array_slot_type10(ary,typnam,idx) uls_dealloc_array_slot_ns_type10(ary,none,typnam,uls_type_this,idx)

#define uls_array_slots_type10(a) ((a)->slots)
#define uls_get_array_slot_type10(a,idx) uls_get_array_slot(uls_array_slots_type10(a),idx)

#else // ULS_CLASSIFY_SOURCE

// standard array
#define _uls_get_stdary_slot(ary,idx) ((ary)+(idx))
#define _uls_set_stdary_slot(ary,idx,obj) (ary)[idx]=*(obj);

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

// type00
#define uls_init_array_ns_type00(ary,ns,typnam,typmac,NN) do { \
	uls_bzero((ary)->slots,(NN)*sizeof(typmac(ns,typnam))); (ary)->n = NN; \
	} while (0)
#define uls_deinit_array_ns_type00(ary,ns,typnam,typmac)

#define uls_array_slots_type00(ary) ((ary)->slots)
#define uls_array_ref_slots_ns_type00(slots,ns,typnam,typmac) uls_##typnam##_ptr_t slots
#define uls_get_array_slot_type00(ary,idx) ((ary)->slots + (idx))

// type01
#define uls_init_array_ns_type01(ary,ns,typnam,typmac,nn) do { \
                (ary)->slots=(typmac(ns,typnam)*)uls_malloc_clear((nn)*sizeof(typmac(ns,typnam))); \
                (ary)->n = nn; \
        } while (0)
#define uls_deinit_array_ns_type01(ary,ns,typnam,typmac) do { \
		uls_mfree((ary)->slots); (ary)->n = 0; \
	} while (0)

#define uls_init_array_ns_type01a(ary,ns,typnam,typmac,nn) do { int i; \
        	uls_init_array_ns_type01(ary,ns,typnam,typmac,nn); \
		for (i=0; i<(nn); i++) { uls_init_##typnam(uls_get_array_slot_type01(ary,i)); } \
	} while (0)
#define uls_deinit_array_ns_type01a(ary,ns,typnam,typmac) do { int i; \
		for (i=0; i<(ary)->n; i++) { uls_deinit_##typnam(uls_get_array_slot_type01(ary,i)); } \
		uls_deinit_array_ns_type01(ary,ns,typnam,typmac); \
	} while (0)

#define uls_decl_array_slots_ns_type01(a,ns,typnam,typmac) typmac(ns,typnam) uls_def_ptrvar(a)
#define uls_array_slots_type01(ary) ((ary)->slots)
#define uls_get_array_slot_type01(ary,idx) ((ary)->slots + (idx))

// type10
#define uls_cast_array_ns_type10(ns,typnam,typmac) (typmac(ns,typnam)*)
#define uls_ref_array_init_ns_type10(ary,ns,typnam,typmac,ary2) uls_ref_array_ns_type10(ary,ns,typnam,typmac)=ary2
#define uls_decl_array_alloc_ns_type10(ary,ns,typnam,typmac) uls_ref_array_ns_type10(ary,ns,typnam,typmac)=uls_alloc_object_clear(typmac(ns,typnam))

#define uls_alloc_array_slot_ns_type10(ary,ns,typnam,typmac,idx) _uls_alloc_iary_slot((ary)->slots,idx,ns,typnam,typmac)
#define uls_dealloc_array_slot_ns_type10(ary,ns,typnam,typmac,idx) _uls_dealloc_iary_slot((ary)->slots,idx,ns,typnam,typmac)
#define uls_init_array_ns_type10(ary,ns,typnam,typmac,nn) do { \
		(ary)->slots=(typmac(ns,typnam)*)uls_malloc_clear((nn)*sizeof(typmac(ns,typnam))); \
		(ary)->n = 0; (ary)->n_alloc = (nn); \
	} while (0)
#define uls_deinit_array_ns_type10(ary,ns,typnam,typmac) do { \
		uls_resize_array_ns_type10(ary,ns,typnam,typmac,0); \
		uls_mfree((ary)->slots); (ary)->n = (ary)->n_alloc = 0; \
	} while (0)
#define uls_resize_array_ns_type10(ary,ns,typnam,typmac,nn_alloc) do { \
		int i; for (i=(nn_alloc); i<(ary)->n; i++) { _uls_dealloc_iary_slot((ary)->slots,i,ns,typnam,typmac); } \
		(ary)->slots=(typmac(ns,typnam)*)uls_mrealloc((ary)->slots,(nn_alloc)*sizeof(typmac(ns,typnam))); \
		if ((nn_alloc) < (ary)->n) (ary)->n = (nn_alloc); (ary)->n_alloc = (nn_alloc); \
	} while (0)

#define uls_decl_array_slots_ns_type10(a,ns,typnam,typmac) typmac(ns,typnam) uls_def_ptrvar(a)

// type00(this,tool)
#define uls_init_array_type00(ary,typnam,NN) uls_init_array_ns_type00(ary,none,typnam,uls_type_this,NN)
#define uls_deinit_array_type00(ary,typnam) uls_deinit_array_ns_type00(ary,none,typnam,uls_type_this)
#define uls_decl_array_slots_type00(a,typnam) uls_decl_array_slots_ns_type00(a,none,typnam,uls_type_this)
#define uls_decl_array_slots_init_type00(a,typnam,b) uls_decl_array_slots_init_ns_type00(a,none,typnam,uls_type_this,b)
#define uls_array_ref_slots_type00(slots,typnam) uls_array_ref_slots_ns_type00(slots,none,typnam,uls_type_this)

// type01(this,tool)
#define uls_init_array_type01(ary,typnam,n) uls_init_array_ns_type01(ary,none,typnam,uls_type_this,n)
#define uls_deinit_array_type01(ary,typnam) uls_deinit_array_ns_type01(ary,none,typnam,uls_type_this)

#define uls_init_array_type01a(ary,typnam,n) uls_init_array_ns_type01a(ary,none,typnam,uls_type_this,n)
#define uls_deinit_array_type01a(ary,typnam) uls_deinit_array_ns_type01a(ary,none,typnam,uls_type_this)

#define uls_decl_array_slots_type01(a,typnam) uls_decl_array_slots_ns_type01(a,none,typnam,uls_type_this)
#define uls_decl_array_slots_init_type01(a,typnam,b) uls_decl_array_slots_init_ns_type01(a,none,typnam,uls_type_this,b)
#define uls_init_array_tool_type01(ary,typnam,n) uls_init_array_ns_type01(ary,UlsToolbase,typnam,uls_type_ns,n)
#define uls_init_array_tool_type01a(ary,typnam,n) uls_init_array_ns_type01a(ary,UlsToolbase,typnam,uls_type_ns,n)
#define uls_deinit_array_tool_type01(ary,typnam) uls_deinit_array_ns_type01(ary,UlsToolbase,typnam,uls_type_ns)
#define uls_deinit_array_tool_type01a(ary,typnam) uls_deinit_array_ns_type01a(ary,UlsToolbase,typnam,uls_type_ns)
#define uls_decl_array_slots_tool_type01(a,typnam) uls_decl_array_slots_ns_type01(a,UlsToolbase,typnam,uls_type_ns)

// type10(this,tool)
#define uls_decl_array_alloc_type10(ary,typnam) uls_decl_array_alloc_ns_type10(ary,none,typnam,_uls_array10_this_)
#define uls_cast_array_type10(typnam) uls_cast_array_ns_type10(none,typnam,_uls_array10_this_)
#define uls_ref_array_init_type10(ary,typnam,ary2) uls_ref_array_init_ns_type10(ary,none,typnam,_uls_array10_this_,ary2)
#define uls_init_array_type10(ary,typnam,nn) uls_init_array_ns_type10(ary,none,typnam,uls_type_this,nn)
#define uls_deinit_array_type10(ary,typnam) uls_deinit_array_ns_type10(ary,none,typnam,uls_type_this)
#define uls_resize_array_type10(ary,typnam,nn_alloc) uls_resize_array_ns_type10(ary,none,typnam,uls_type_this,nn_alloc)

#define uls_decl_array_slots_type10(a,typnam) uls_decl_array_slots_ns_type10(a,none,typnam,uls_type_this)
#define uls_decl_array_slots_init_type10(a,typnam,b) uls_decl_array_slots_init_ns_type10(a,none,typnam,uls_type_this,b)
#define uls_array_slots_type10(ary) ((ary)->slots)
#define uls_get_array_slot_type10(ary,idx) ((ary)->slots + (idx))

#define uls_alloc_array_slot_type10(ary,typnam,idx) uls_alloc_array_slot_ns_type10(ary,none,typnam,uls_type_this,idx)
#define uls_dealloc_array_slot_type10(ary,typnam,idx) uls_dealloc_array_slot_ns_type10(ary,none,typnam,uls_type_this,idx)

#endif // ULS_CLASSIFY_SOURCE

// standard array
#define uls_get_array_slot(ary,idx) _uls_get_stdary_slot(ary,idx)
#define uls_set_array_slot(ary,idx,obj) _uls_set_stdary_slot(ary,idx,obj)
#define uls_alloc_array_slot(ary,idx,typnam) _uls_alloc_iary_slot(ary,idx,none,typnam,uls_type_this)
#define uls_dealloc_array_slot(ary,idx,typnam) _uls_dealloc_iary_slot(ary,idx,none,typnam,uls_type_this)

#ifndef ULS_DOTNET
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

#define nilptr  uls_nil

#ifdef _ULS_CPLUSPLUS
}
#endif

#endif // __ULS_TYPE_H__
