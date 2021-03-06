// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    common.h
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#ifndef ULIB_INTERNAL_COMMON_H
#define ULIB_INTERNAL_COMMON_H 1

// Manage file to include

#include <ulib/base/base.h>

#ifdef U_STDCPP_ENABLE
#  include <iostream>
#  ifdef HAVE_STRSTREAM_H
#     include <streambuf.h>
#     include <strstream.h>
#  else
#     include <streambuf>
#     include <ulib/replace/strstream.h>
#  endif
#  ifdef HAVE_OLD_IOSTREAM
#     define U_openmode ios::out
#  else
#     define U_openmode std::ios_base::out
#  endif
#  ifdef __clang__
#     define ios        std::ios
#     define istream    std::istream
#     define ostream    std::ostream
#     define streambuf  std::streambuf
#     define streamsize std::streamsize
#  endif
#else
typedef char istream;
typedef char ostream;
typedef char ostrstream;
typedef char istrstream;

extern U_EXPORT bool __cxa_guard_acquire();
extern U_EXPORT bool __cxa_guard_release();

extern U_EXPORT void* operator new(size_t);
extern U_EXPORT void* operator new[](size_t);
extern U_EXPORT void  operator delete(void*);
extern U_EXPORT void  operator delete[](void*);
#endif

// C++11 keywords and expressions

#ifdef U_COMPILER_NULLPTR
# define U_NULLPTR nullptr
#else
# define U_NULLPTR 0
#endif

#ifdef U_COMPILER_DEFAULT_MEMBERS
#  define U_DECL_EQ_DEFAULT = default
#else
#  define U_DECL_EQ_DEFAULT
#endif

#ifdef U_COMPILER_CONSTEXPR
# define U_DECL_CONSTEXPR constexpr
#else
# define U_DECL_CONSTEXPR
#endif

#ifdef U_COMPILER_EXPLICIT_OVERRIDES
# define U_DECL_OVERRIDE override
# define U_DECL_FINAL final
#else
# ifndef U_DECL_OVERRIDE
# define U_DECL_OVERRIDE
# endif
# ifndef U_DECL_FINAL
# define U_DECL_FINAL
# endif
#endif

#ifdef U_COMPILER_NOEXCEPT
# define U_DECL_NOEXCEPT noexcept
# define U_DECL_NOEXCEPT_EXPR(x) noexcept(x)
# ifdef U_DECL_NOTHROW
# undef U_DECL_NOTHROW // override with C++11 noexcept if available
# endif
#else
# define U_DECL_NOEXCEPT
# define U_DECL_NOEXCEPT_EXPR(x)
#endif
#ifndef U_DECL_NOTHROW
#define U_DECL_NOTHROW U_DECL_NOEXCEPT
#endif

#include <ulib/debug/macro.h>

#ifdef DEBUG
#  include <ulib/debug/trace.h>
#  include <ulib/debug/common.h>
#  include <ulib/debug/error_memory.h>
#  ifdef U_STDCPP_ENABLE
#     include <ulib/debug/objectDB.h>
#  endif
#endif

#include <ulib/internal/objectIO.h>
#include <ulib/internal/memory_pool.h>

// string representation

typedef struct ustringrep {
#ifdef DEBUG
   const void* _this;
#endif
#if defined(U_SUBSTR_INC_REF) || defined(DEBUG)
   struct ustringrep* parent; // manage substring for increment reference of source string
#  ifdef DEBUG
   int32_t child; // manage substring for capture event 'DEAD OF SOURCE STRING WITH CHILD ALIVE'...
#  endif
#endif
   uint32_t _length, _capacity, references;
   const char* str;
} ustringrep;

// for gcc compiler strict aliasing behaviour

class UStringRep;

union uustringrep {
   ustringrep* p1;
   UStringRep* p2;
   long*       p3;
};

// ======================================================================================
// NB: in this way we don't capture the event 'DEAD OF SOURCE STRING WITH CHILD ALIVE'...
// ======================================================================================
// #define U_SUBSTR_INC_REF
// ======================================================================================

#include <ulib/internal/macro.h>

// json value representation

union anyvalue {
   bool bool_;
   char char_;
   unsigned char uchar_;
   short short_;
   unsigned short ushort_;
   int int_;
   unsigned int uint_;
   long long_;
   unsigned long ulong_;
   long long llong_;
   unsigned long long ullong_;
   float float_;
   void* ptr_;
   double real_;
   long double lreal_;
};

typedef struct uvalue {
#ifdef DEBUG
   const void* _this;
#endif
   union anyvalue value;
   int type;
} uvalue;

struct null {}; // Special type to bind a NULL value to column using operator,() - syntactic sugar

// Type of the value held by a UValue object

typedef enum ValueType {
      NULL_VALUE =  0, // null value
   BOOLEAN_VALUE =  1, // bool value
      CHAR_VALUE =  2, //   signed char value
     UCHAR_VALUE =  3, // unsigned char value
     SHORT_VALUE =  4, //   signed short integer value
    USHORT_VALUE =  5, // unsigned short integer value
       INT_VALUE =  6, //   signed integer value
      UINT_VALUE =  7, // unsigned integer value
      LONG_VALUE =  8, //   signed long value
     ULONG_VALUE =  9, // unsigned long value
     LLONG_VALUE = 10, //   signed long long value
    ULLONG_VALUE = 11, // unsigned long long value
     FLOAT_VALUE = 12, // float value
      REAL_VALUE = 13, // double value
     LREAL_VALUE = 14, // long double value
    STRING_VALUE = 15, // UTF-8 string value
     ARRAY_VALUE = 16, // array value (ordered list)
    OBJECT_VALUE = 17  // object value (collection of name/value pairs)
} ValueType;

using namespace std;

// Init library

U_EXPORT void ULib_init();
#ifdef USE_LIBSSL
U_EXPORT void ULib_init_openssl();
#endif

#define U_ULIB_INIT(argv) U_SET_LOCATION_INFO, u_init_ulib(argv), ULib_init()

#endif
