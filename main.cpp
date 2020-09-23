/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ

kernel switch, what is that?

https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019

NOTE: this is MSVC only

This (as it is) does not build with _DEBUG defined. 
Why not? Because the linker has this problem?

main.obj : error LNK2019: unresolved external symbol _CrtDbgReport referenced in function 
"void * __cdecl std::_Allocate_manually_vector_aligned<struct std::_Default_allocate_traits>(unsigned __int64)" (??$_Allocate_manually_vector_aligned@U_Default_allocate_traits@std@@@std@@YAPEAX_K@Z)
*/
#include "top.h"

#include "crt_sampling.h"
#include "generate_dump.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <new.h>

// --------------------------------------------------------------------------
/*
Is this for C++ exceptions, for SEH or for both?

<vcruntime.h> #100

#ifndef _HAS_EXCEPTIONS // Predefine as 0 to disable exceptions
    #ifdef _KERNEL_MODE
        #define _HAS_EXCEPTIONS 0
    #else
        #define _HAS_EXCEPTIONS 1
    #endif // _KERNEL_MODE 
#endif // _HAS_EXCEPTIONS 

*/
static_assert( _HAS_EXCEPTIONS == 0 , __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : _HAS_EXCEPTIONS should be 0 as _KERNEL_MODE should be defined?" ) ;

// --------------------------------------------------------------------------
// optional
void my_handler() noexcept 
{
    puts("Memory allocation failed, terminating");
    std::terminate();
}
// --------------------------------------------------------------------------
// optional
// #ifdef __cpp_aligned_new
namespace my
{
    enum class align_val_t : size_t {};
}
// #endif // __cpp_aligned_new
// --------------------------------------------------------------------------
// optional
void* operator new  ( std::size_t count ){  return calloc(1, count) ;}
void* operator new  ( std::size_t count, my::align_val_t al ) { return calloc(1, count) ; }
void operator delete  ( void* ptr ) noexcept { free(ptr); }
void operator delete  ( void* ptr, my::align_val_t al ) noexcept {free(ptr); }
// --------------------------------------------------------------------------
static int program (int argc , char ** argv ) 
{
   crt_sample_one(argc,argv);

   std::vector< std::string > vs { argv + 1 , argv + argc } ;

   // provoke SEH in /kernel builds
   // vs.at(0xFF);

// lets try some legal usage
   std::cout << "\n cmd line arguments are" ;
   for ( auto const & str_  : vs )
   {
      dbg(str_);
   }
   return 0;
}

int main (int argc, char ** argv) 
{
     __try 
    { 
        std::set_new_handler(my_handler);
        errno = 0 ;
        program(argc,argv);
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are caught here
        // as now they are SEH
        perror( DBJ_APP_NAME " -- SEH Exception caught");
        
        puts( dump_last_run.finished_ok == TRUE 
        ? "minidump creation succeeded" 
        : "minidump creation failed" 
        );

        if ( dump_last_run.finished_ok ) {
            puts(dump_last_run.dump_folder_name);
            puts(dump_last_run.dump_file_name );
        }
    } 
        puts( "\n\n");
        puts( DBJ_APP_NAME " -- Leaving...");
        return 42;
}
/*
ISSUES
*/

/*
It seems Bjarne has expressed explicit dislike for MSFT SEH
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf

It also seems bellow is a grouping of all SEH raising 6 functions, existing in MS STL.

<xutility> #5817

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);

there is two more (declared/defined somewhere/elsewhere)

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_function_call();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xregex_error(const regex_constants::error_type _Code);

Implementation of all eight is in https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp

This is how that mechanism works (as an example ) :
*/
namespace my {
    inline namespace constants {
        enum error_type { 
        error_ctype,
        error_syntax
    };
    } // namespace constants

struct error final 
{ 
    error () = default ;

    const char * msg_ {"error"};

    explicit  error( constants::error_type ex_ ) 
    : err_(ex_) 
    { /* default ctor body */ }

    _NODISCARD constants::error_type code() const {
        return err_ ;
    }

#ifndef DBJ_RAISE 
#ifdef _DEBUG
#define DBJ_RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define DBJ_RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG    
#endif // !  DBJ_RAISE

    // we do not throw instance of this type
    // there is no throw in /kernel builds
    // we raise the SEH through calling _invoke_watson
    void raise() const noexcept { 
        DBJ_RAISE(*this) ;
    }

/*
 _RAISE is in yvals.h as :

#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG

That might mean c++ type of c++ exception is simply erased in /kernel builds?
Or not. MS STL _THROW macro is used for _NO_EXCEPTIONS bulds and that simply does

// <yvals> #482
_THROW(x) x._Raise()

with _Raise() in <exception> #186, on /kernel version of std::exception for _NO_EXCEPTION builds

*/  
    constants::error_type err_ ;
} ; // error

[[noreturn]] inline void __cdecl 
    my_error_throw (const constants::error_type code_) 
    {
      error(code_).raise() ;
    }
} // my
/*
The mythical (MS STL)"CORE". First mentioned here (by Billy O Neal) :
https://devblogs.microsoft.com/cppblog/stl-features-and-fixes-in-vs-2017-15-8/

<citation>
The header structure of the STL was changed to allow use of a subset of the library in 
conditions where the user can’t link with msvcp140.dll, such as driver development. 
(Previously the hard dependency on msvcp and pragma detect mismatch were injected by our 
base configuration header included by everything.) The following headers are now 
considered “core” and don’t inject our runtime dependencies (though we do still assume 
some form of CRT headers are present):

<cstddef>
<cstdlib>
<initializer_list>
<ratio>
<type_traits>

We aren’t actually driver developers ourselves and are interested in feedback
 in this area if there are things we can do to make these more usable in constrained environments.
 </citation>
*/
