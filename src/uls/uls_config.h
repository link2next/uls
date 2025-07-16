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
  <file> uls_config.h </file>
  <brief>
    The configuration header for building the ULS library
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, August 2011.
  </author>
*/

#ifndef __ULS_CONFIG_H__
#define __ULS_CONFIG_H__

//#define _ULS_DEBUG
//#define _ULS_BUILD_STATIC_LIBS
#define NO_CSZ_POOL

#if defined(_WIN32)
#define ULS_WINDOWS
#define __WINDOWS__
#define ULS_CLASSIFY_SOURCE

#ifdef _WIN64
#define ARCH_64BIT
#endif

#define SIZEOF_INT       4
#define SIZEOF_LONG      sizeof(long)
#define SIZEOF_LONG_LONG 8

#define SIZEOF_DOUBLE sizeof(double)
#define SIZEOF_LONG_DOUBLE sizeof(long double)

#elif defined(__linux__) || defined(unix)
#ifdef __linux__
#define __LINUX__
#else
#define ULS_UNIX
#endif

#if SIZEOF_VOID_P == 8
#define ARCH_64BIT
#endif

//#define ULS_CLASSIFY_SOURCE
#define HAVE_GETOPT
#define HAVE_PTHREAD
#define ULS_FDF_SUPPORT

#elif defined(__APPLE__) && defined(__MACH__)
#define __APPLE_OSX__

#if SIZEOF_VOID_P == 8
#define ARCH_64BIT
#endif

//#define ULS_CLASSIFY_SOURCE
#define HAVE_GETOPT
#define HAVE_PTHREAD
#define ULS_FDF_SUPPORT

#else // ~_WIN32
#error "ULS: Unrecognized arch!"
#endif // _WIN32

#ifdef ULS_DOTNET

#define ULS_DLL_EXTERN
#define ULSCPP_DLL_EXTERN
#define _ULS_INLINE
#define ULS_DECL_STATIC
#define ULS_DECL_EXTERN_STATIC static
#ifndef ULS_CLASSIFY_SOURCE
#define ULS_CLASSIFY_SOURCE
#endif
#define ULS_USE_USTR
#define _MANAGE_ULS_OBJECTS

#else // ULS_DOTNET

#if defined(ULS_WINDOWS)
#ifdef _ULS_IMPLDLL
#define ULS_DLL_EXTERN __declspec(dllexport)
#else
#define ULS_DLL_EXTERN __declspec(dllimport)
#endif
#ifdef _ULSCPP_IMPLDLL
#define ULSCPP_DLL_EXTERN __declspec(dllexport)
#else
#define ULSCPP_DLL_EXTERN __declspec(dllimport)
#endif
#define _ULS_INLINE __inline
#define ULS_DECL_STATIC static
#define ULS_DECL_EXTERN_STATIC
#else // !ULS_WINDOWS
#define ULS_DLL_EXTERN
#define ULSCPP_DLL_EXTERN
#define ULS_DECL_STATIC static
#define _ULS_INLINE inline
#define ULS_DECL_EXTERN_STATIC
#endif // ULS_WINDOWS

#if defined(_ULS_IMPLDLL) || defined(_ULSCPP_IMPLDLL)
#define _ULS_INTERNAL_USE_ONLY
#else
#define _ULS_USEDLL
#define _ULS_USE_ULSCOMPAT
#endif

#ifdef _ULS_USEDLL
#ifdef _UNICODE
#define ULS_USE_WSTR
#endif
#endif

#define ULS_DECL_BASIC_TYPES
#define ULS_DECL_GLOBAL_TYPES

#define ULS_DECL_PRIVATE_TYPE
#define ULS_DECL_PROTECTED_TYPE
#define ULS_DECL_PUBLIC_TYPE

#define ULS_DEF_PRIVATE_TYPE
#define ULS_DEF_PROTECTED_TYPE
#define ULS_DEF_PUBLIC_TYPE

//#define ULS_DEF_PRIVATE_DATA
//#define ULS_DEF_PROTECTED_DATA
//#define ULS_DEF_PUBLIC_DATA

//#define ULS_DECL_PRIVATE_PROC
#ifdef _ULS_INTERNAL_USE_ONLY
#define ULS_DECL_PROTECTED_PROC
#endif
#define ULS_DECL_PUBLIC_PROC

#if defined(__cplusplus)
#define _ULS_CPLUSPLUS
#endif

#endif // ULS_DOTNET

#ifdef _ULS_DEBUG
#define ULS_DO_ASSERT
#endif

#endif // __ULS_CONFIG_H__
